#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QJsonObject>
#include <QNetworkInterface>
#include <algorithm>
#include <deque>
#include <queue>
#include <iostream>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>
#include <QVariant>
#include <math.h>
#include <map>
#include "configreader.h"
#include "logger.h"

constexpr size_t frame_size = 2000;
constexpr double VALUE_SIZE = 3e6;
//constexpr double VALUE_SIZE = 10;


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    showMaximized();
    plot = ui->plot;
    finalPlot = ui->finalPlot;
    connect(plot, &QCustomPlot::mouseWheel, this, &MainWindow::mouseWheel);
    setupPlot(plot);
    plot->yAxis->setRange(-VALUE_SIZE, 5*VALUE_SIZE);
    plot->xAxis->setRange(0, frame_size);

    plot->addGraph();
    plot->addGraph();
    plot->addGraph();

    plot->graph(0)->setPen(QPen(QColor(82, 247, 79), 2));
    plot->graph(1)->setPen(QPen(QColor(242, 65, 65), 2));
    plot->graph(2)->setPen(QPen(QColor(255, 255, 75), 2));
    plot->graph(0)->setName("brightness");
    plot->graph(1)->setName("filtered");
    plot->graph(2)->setName("temprature");

    setupPlot(finalPlot);

    videoReceiver_ = std::make_unique<VideoReceiver>(ui->video_frame);

    videoReceiver_->moveToThread(&videoReceiverThread_);
    videoReceiverThread_.start();

    connect(videoReceiver_.get(), &VideoReceiver::sendImage, this, &MainWindow::receiveImage, Qt::QueuedConnection);

    network_ = std::make_unique<Network>();
    network_->moveToThread(&networkThread_);
    networkThread_.start();

    connect(network_.get(), &Network::sendData, this, &MainWindow::receiveData, Qt::QueuedConnection);
    connect(network_.get(), &Network::tcpIsConnected, this, &MainWindow::tcpIsConnected, Qt::QueuedConnection);
    connect(network_.get(), &Network::tcpIsDisconnected, this, &MainWindow::tcpIsDisconnected, Qt::QueuedConnection);
    connect(network_.get(), &Network::sendFuncCoeffs, this, &MainWindow::drawFunc, Qt::QueuedConnection);

    connect(this, &MainWindow::sendData, network_.get(), &Network::receiveData, Qt::QueuedConnection);
    connect(this, &MainWindow::sendFuncParameters, this, &MainWindow::drawFunc);


    plot->graph(0)->rescaleAxes(); // Adjusts primary x and y axes
    plot->graph(1)->rescaleAxes(); // Adjusts primary x and y axes
    plot->graph(2)->rescaleAxes(true); // Only expands existing ranges for secondary axes
    ui->temperatureRate->setText(QString::number(0.0));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receiveData(const QJsonDocument& json)
{
    double brightness = json["brightness"].toDouble();
    double filtered = json["filtered"].toDouble();
    double temperature = json["temperature"].toDouble();
    int bleStatus = json["bleStatus"].isNull() ? json["bleStatus"].toBool() : 3;
    modeEval(static_cast<EventType>(json["mode"].toInt()));
    ui->temperature_label->setText(QString::number(temperature));
    if(bleStatus == 1) {
        ui->ble_status->setStyleSheet(QString("QLabel")+normal_state);
    }
    if(bleStatus == 0) {
        ui->ble_status->setStyleSheet(QString("QLabel")+crictical_state);
    }
    plot->graph(0)->addData(sample_, brightness);
    plot->graph(1)->addData(sample_, filtered);
    plot->graph(2)->addData(sample_, temperature*VALUE_SIZE/60);
    data_.push_back(filtered);
    temp_data_.push_back(temperature);
    sample_++;
    if(commands_ == Commands::work) {
        getFuncParameters(json);
    }
    else if(commands_ == Commands::halt) {
        sample_ = 0;
    }
    plot->replot();
}

void MainWindow::modeEval(EventType mode)
{
    switch (mode) {
        case EventType::IDLE:
            ui->status->setText("IDLE");
        break;
        case EventType::CALIBRATION:
            ui->status->setText("CALIBRATION");
        break;
        case EventType::MEASHUREMENT:
            ui->status->setText("MEASHUREMENT");
        break;
        case EventType::CONDENSATION:
            ui->status->setText("CONDENSATION");
        break;
        case EventType::END:
            ui->status->setText("END");
        break;
        default:
            ui->status->setText("IDLE");
        break;
    }
}

void MainWindow::getFuncParameters(const QJsonDocument& json)
{
    if(json["isFuncParametersReady"].toBool() && firstCallEvaluateParameters_) {
        firstCallEvaluateParameters_ = false;

        auto jsonParameters = json["funcParameters"].toArray();
        QVector<double> parameters;
        parameters.reserve(jsonParameters.size());

        for (const QJsonValue& value : jsonParameters) {
            if (value.isDouble()) {
                parameters.append(value.toDouble());
            } else {
                qWarning() << "Non-double value found in QJsonArray, skipping...";
            }
        }
        emit sendFuncParameters(parameters);
    }
    emit sendFuncParameters({});
}

size_t MainWindow::findEndPoint(const QVector<double>& data)
{
    auto posMax = std::max_element(std::begin(data), std::end(data));
    auto max = *posMax;
    return std::distance(std::begin(data), std::find_if(posMax, std::end(data), [max](auto val){
        if(val < 0.95*max) return true;
        return false;
    }));
}

void MainWindow::mouseWheel()
{
    if (plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        plot->axisRect()->setRangeZoom(plot->xAxis->orientation());
    }
    else if (plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        plot->axisRect()->setRangeZoom(plot->yAxis->orientation());
    }
    else if (plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis)) {
        plot->axisRect()->setRangeZoom(plot->yAxis2->orientation());
    }
    else
      plot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::setupPlot(QCustomPlot* plot)
{
    plot->xAxis->setBasePen(QPen(Qt::white, 1));
    plot->yAxis->setBasePen(QPen(Qt::white, 1));
    plot->xAxis->setTickPen(QPen(Qt::white, 1));
    plot->yAxis->setTickPen(QPen(Qt::white, 1));
    plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    plot->xAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setTickLabelColor(Qt::white);
    plot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    plot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    plot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridVisible(true);
    plot->yAxis->grid()->setSubGridVisible(true);
    plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    plot->yAxis2->setVisible(true);
    plot->yAxis2->setBasePen(QPen(Qt::white, 1));
    plot->yAxis2->setTickPen(QPen(Qt::white, 1));
    plot->yAxis2->setSubTickPen(QPen(Qt::white, 1));
    plot->yAxis2->setTickLabelColor(Qt::white);
    plot->yAxis2->setRange(-60, 60);
//    plot->yAxis2->setTickLabels(true);

    plot->rescaleAxes();

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    plot->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    plot->axisRect()->setBackground(axisRectGradient);
//    plot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(100);
//    plot->axisRect()->axis(QCPAxis::atLeft, 0)->setPadding(100);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    plot->legend->setFont(legendFont);

}

void MainWindow::on_stopCV_clicked()
{
    QJsonObject json;
    json["commands"] = static_cast<int>(Commands::halt);
    commands_ = Commands::halt;
    emit sendData(QJsonDocument(json));
//    Logger::getInstance().log(data_);
    data_.clear();

}

void MainWindow::on_startCV_clicked()
{
    QJsonObject json;
    json["commands"] = static_cast<int>(Commands::work);
    commands_ = Commands::work;
//    Logger::getInstance().createLog();
    emit sendData(QJsonDocument(json));
}

void MainWindow::on_setRate_button_clicked()
{
    QJsonObject json;
    json["commands"] = static_cast<int>(Commands::setRateTemprature);
    json["tempratureRate"] = QString(ui->temperatureRate->text()).toDouble();
    emit sendData(QJsonDocument(json));
}

QVector<double> derivative(const QVector<double>& data) {
    QVector<double> result(data);
    for (int i = 0; i < data.size()-1; ++i) {
        result[i] = (data[i+1] - data[i])*60;
    }
    return result;
}

void MainWindow::drawFunc(const QVector<double>& parameters)
{
    if(parameters.isEmpty()) return;
    qDebug() << parameters;
    auto normalize = [](QVector<double>& data){
        auto maxVal = *std::max_element(std::begin(data), std::end(data));
        auto minVal = *std::min_element(std::begin(data), std::end(data));
        std::transform(std::begin(data), std::end(data), std::begin(data), [minVal](auto val){return val - minVal;});
        std::transform(std::begin(data), std::end(data), std::begin(data), [maxVal](auto val){return val / maxVal;});
        return data;
    };

    auto data = applyFunc(
        std::vector<double>(std::begin(parameters), std::end(parameters)),
        0,
        data_.size(),
        data_.size(),
        gaussPolyVal
    );

    auto x_data = QVector<double>(std::begin(data.first), std::end(data.first));
    auto y_data = QVector<double>(std::begin(data.second), std::end(data.second));

    auto refData = QVector<double>(std::begin(data_), std::end(data_));
    auto normalizeRefData = normalize(refData);

    auto der = derivative(y_data);

    finalPlot->yAxis->setRange(-1, 1);
    finalPlot->xAxis->setRange(0, data_.size());
    finalPlot->addGraph();
    finalPlot->addGraph();
    finalPlot->addGraph();

    finalPlot->graph(0)->setPen(QPen(QColor(93, 149, 246), 2));
    finalPlot->graph(1)->setPen(QPen(QColor(255, 255, 75), 2));
    finalPlot->graph(2)->setPen(QPen(QColor(253, 114, 114), 2));

    finalPlot->graph(0)->addData(x_data, normalizeRefData);
    finalPlot->graph(1)->addData(x_data, y_data);
    finalPlot->graph(2)->addData(x_data, der);

    finalPlot->replot();
}

void MainWindow::receiveImage(const QPixmap& image)
{
    static bool firstCall = true;
    if(firstCall) {
        ui->video_frame->resize(image.width(), image.height());
    }
    ui->video_frame->setPixmap(image);
}

void MainWindow::tcpIsConnected()
{
    ui->camera_connection_status->setStyleSheet(QString("QLabel")+normal_state);
    ui->camera_connection_status->setText("Camera is connected");
}

void MainWindow::tcpIsDisconnected()
{
    ui->camera_connection_status->setStyleSheet(QString("QLabel")+crictical_state);
    ui->camera_connection_status->setText("Camera is disconnected");
}

void MainWindow::on_closeAppButton_clicked()
{
    QJsonObject json;
    json["commands"] = static_cast<int>(Commands::close);
    emit sendData(QJsonDocument(json));
}

