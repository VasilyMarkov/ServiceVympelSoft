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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , modes_({
        {EventType::IDLE, "idle"},
        {EventType::CALIBRATION, "calibration"},
        {EventType::MEASHUREMENT, "meashurement"}
    })
{
    ui->setupUi(this);

    auto hostIp = getOwnIp();

    if(hostIp.isNull()) {
        hostIp = QHostAddress(ConfigReader::getInstance().get("network", "hostIp").toString());
    }

    ui->host_ip_label->setText(hostIp.toString());

    resize(1280, 720);
    plot = ui->plot;
    connect(plot, &QCustomPlot::mouseWheel, this, &MainWindow::mouseWheel);
    setupPlot(plot);
    plot->yAxis->setRange(-VALUE_SIZE, 5*VALUE_SIZE);
    plot->xAxis->setRange(0, frame_size);

    plot->addGraph();
    plot->addGraph();

    plot->graph(0)->setPen(QPen(QColor(82, 247, 79), 2));
    plot->graph(1)->setPen(QPen(QColor(242, 65, 65), 2));
    plot->graph(0)->setName("brightness");
    plot->graph(1)->setName("filtered");

    network_ = std::make_unique<Network>();
    network_->setReceiverParameters(hostIp,
                                   ConfigReader::getInstance().get("network", "serviceProgramPort").toInt());
    network_->setSenderParameters(QHostAddress(ConfigReader::getInstance().get("network", "cameraIp").toString()),
                                   ConfigReader::getInstance().get("network", "controlFromServiceProgramPort").toInt());
    connect(network_.get(), &Network::sendData, this, &MainWindow::receiveData);
    connect(network_.get(), &Network::tcpIsConnected, this, &MainWindow::receiveTCPConnection);
    connect(network_.get(), &Network::tcpDisconnected, this, &MainWindow::tcpDisconnection);
    connect(this, &MainWindow::sendData, network_.get(), &Network::receiveData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receiveData(const QJsonDocument& json)
{
    auto brightness = json["brightness"].toDouble();
    auto filtered = json["filtered"].toDouble();
    auto temperature = json["temperature"].toDouble();
    coreStatement_ = static_cast<CoreStatement>(json["statement"].toInt());
    modeEval(static_cast<EventType>(json["mode"].toInt()));
    if(coreStatement_ == CoreStatement::WORK) {
//        ui->temperature_label->setText(QString::number(temperature));
        plot->graph(0)->addData(sample_, brightness);
        plot->graph(1)->addData(sample_, filtered);
        sample_++;
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

QHostAddress MainWindow::getOwnIp() const
{
    auto interfaces = QNetworkInterface::allInterfaces();
    for(auto&& interface : interfaces) {
        if(interface.flags().testFlag(QNetworkInterface::IsUp)) {
            for(auto&& entry: interface.addressEntries()) {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol && !entry.ip().isLoopback()) {
                    return entry.ip();
                }
            }
        }
    }
    return QHostAddress::Null;
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
    json["statement"] = static_cast<int>(CoreStatement::HALT);
    emit sendData(QJsonDocument(json));
}


void MainWindow::on_startCV_clicked()
{
    QJsonObject json;
    json["statement"] = static_cast<int>(CoreStatement::WORK);
    emit sendData(QJsonDocument(json));
}


void MainWindow::on_slow_cooling_button_clicked()
{
    QJsonObject json;
    json["statement"] = static_cast<int>(CoreStatement::SLOW_COOLING);
    emit sendData(QJsonDocument(json));
}


void MainWindow::on_slow_heating_button_clicked()
{
    QJsonObject json;
    json["statement"] = static_cast<int>(CoreStatement::SLOW_HEATING);
    emit sendData(QJsonDocument(json));
}


void MainWindow::on_fast_cooling_button_clicked()
{
    QJsonObject json;
    json["statement"] = static_cast<int>(CoreStatement::FAST_COOLING);
    emit sendData(QJsonDocument(json));
}


void MainWindow::on_fast_heating_button_clicked()
{
    QJsonObject json;
    json["statement"] = static_cast<int>(CoreStatement::FAST_HEATING);
    emit sendData(QJsonDocument(json));
}


void MainWindow::on_connect_button_clicked()
{
    network_->tcpConnect();
}

void MainWindow::receiveTCPConnection()
{
    ui->camera_connection_status->setStyleSheet(QString("QLabel")+normal_state);
}

void MainWindow::tcpDisconnection()
{
    ui->camera_connection_status->setStyleSheet(QString("QLabel")+crictical_state);
}

