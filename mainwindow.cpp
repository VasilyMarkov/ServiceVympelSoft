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


constexpr size_t frame_size = 2000;
constexpr double VALUE_SIZE = 3e6;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    showMaximized();
    plot = ui->plot;
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
    plot->graph(2)->setName("setTemperature");

    videoReceiver_ = std::make_unique<VideoReceiver>(ui->video_frame);
    connect(videoReceiver_.get(), &VideoReceiver::sendImage, this, &MainWindow::receiveImage);

    network_ = std::make_unique<Network>();

    connect(network_.get(), &Network::sendData, this, &MainWindow::receiveData);
    connect(network_.get(), &Network::tcpIsConnected, this, &MainWindow::tcpIsConnected);
    connect(network_.get(), &Network::tcpIsDisconnected, this, &MainWindow::tcpIsDisconnected);
    connect(this, &MainWindow::sendData, network_.get(), &Network::receiveData);
    connect(this, &MainWindow::sendFuncParameters, this, &MainWindow::drawFunc);


    ui->temperatureRate->setText(QString::number(0.0));
//    auto data = applyFunc({2, 4, 0.5, 1, 0.0, 0.0, 0.3, 0.0}, 0, 8, 1000, gaussPolyVal);

//    auto x_data = QVector<double>::fromStdVector(data.first);
//    auto y_data = QVector<double>::fromStdVector(data.second);

//    plot->graph(2)->addData(x_data, y_data);
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
    if(commands_ == Commands::work) {
        plot->graph(0)->addData(sample_, brightness);
        plot->graph(1)->addData(sample_, filtered);
        sample_++;

        getFuncParameters(json);
    }
    else {
        sample_ = 0;
        plot->graph(0)->setData(QVector<double>(), QVector<double>());
        plot->graph(1)->setData(QVector<double>(), QVector<double>());
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

void MainWindow::mouseWheel()
{
    if (plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        plot->axisRect()->setRangeZoom(plot->xAxis->orientation());
    }
    else if (plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        plot->axisRect()->setRangeZoom(plot->yAxis->orientation());
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
    plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

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
    plot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(100);

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
}

void MainWindow::on_startCV_clicked()
{
    QJsonObject json;
    json["commands"] = static_cast<int>(Commands::work);
    commands_ = Commands::work;
    emit sendData(QJsonDocument(json));
}

void MainWindow::on_setRate_button_clicked()
{
    QJsonObject json;
    json["commands"] = static_cast<int>(Commands::setRateTemprature);
    json["tempratureRate"] = QString(ui->temperatureRate->text()).toDouble();
    emit sendData(QJsonDocument(json));
}

void MainWindow::drawFunc(const QVector<double>& parameters)
{
    if(parameters.isEmpty()) return;

    auto data = applyFunc(
        std::vector<double>(std::begin(parameters), std::end(parameters)),
        0,
        sample_,
        sample_*4,
        gaussPolyVal
    );

    auto x_data = QVector<double>(std::begin(data.first), std::end(data.first));
    auto y_data = QVector<double>(std::begin(data.first), std::end(data.second));

    plot->graph(2)->addData(x_data, y_data);
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


