#include "network.h"
#include <iostream>
#include "configreader.h"


Network::Network(QObject *parent):
    QObject(parent),
    cameraDiscoverSocket_(std::make_unique<QUdpSocket>()),
    cameraDiscoverPort_(ConfigReader::getInstance().get("network", "cameraDiscoverPort").toInt())
{
    cameraDiscoverSocket_->bind(cameraDiscoverPort_, QUdpSocket::ShareAddress);

    connect(cameraDiscoverSocket_.get(), &QUdpSocket::readyRead, this, &Network::processPendingDatagrams);
    connect(&socket_, &QUdpSocket::readyRead, this, &Network::receivePortData);
    connect(&tcp_socket_, &QTcpSocket::readyRead, this, &Network::tcpHandler);
    connect(&tcp_socket_, &QTcpSocket::disconnected, this, &Network::tcpIsDisconnected);
    connect(&tcp_socket_, &QTcpSocket::connected, this, &Network::tcpIsConnected);

//    connect(&disconnectTimer_, &QTimer::timeout, this, &Network::tcpDisconnect);
//    disconnectTimer_.setInterval(5000);
}

void Network::processPendingDatagrams()
{
    while (cameraDiscoverSocket_ && cameraDiscoverSocket_->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(cameraDiscoverSocket_->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        cameraDiscoverSocket_->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        cameraIpAdress_ = sender;
        createTcpConnection();
    }
}

void Network::tcpDisconnectHandler()
{
    qDebug() << "Disconnected";
    cameraIsConnected_ = false;
    cameraDiscoverSocket_->bind(cameraDiscoverPort_, QUdpSocket::ShareAddress);
    emit tcpIsDisconnected();
}

void Network::sendPortData(const QByteArray& data)
{
    if(data.isEmpty()) return;
    socket_.writeDatagram(data, senderAddr_, senderPort_);
}

void Network::receivePortData()
{
    QByteArray datagram;
    datagram.resize(socket_.pendingDatagramSize());
    socket_.readDatagram(datagram.data(), datagram.size(), nullptr, nullptr);
    emit sendData(QJsonDocument::fromJson(datagram, nullptr));
}

void Network::tcpHandler()
{
//    auto message = QString(tcp_socket_.readAll());
//    if(message == "connected") {
//        disconnectTimer_.start();
//        emit tcpIsConnected();
//    }
}

void Network::createTcpConnection()
{
    qDebug() << cameraIsConnected_;
    if(cameraIsConnected_) return;

//    cameraDiscoverSocket_->close();
    tcp_socket_.connectToHost(cameraIpAdress_, ConfigReader::getInstance().get("network", "cameraTcpPort").toInt());
    cameraIsConnected_ = true;
}

void Network::receiveData(const QJsonDocument& json) {
    sendPortData(json.toJson());
}

void Network::setSenderParameters(const QHostAddress& senderIp = QHostAddress::Any, quint16 senderPort = 1024)
{
    if (senderPort <= RESERVE_PORTS) throw std::runtime_error("Invalid sender port");

    senderAddr_ = senderIp;
    senderPort_ = senderPort;
}

void Network::setReceiverParameters(const QHostAddress& receiverIp = QHostAddress::Any, quint16 receiverPort = 1024)
{
    if (receiverPort <= RESERVE_PORTS) throw std::runtime_error("Invalid receiver port");

    socket_.bind(receiverIp, receiverPort);
}

