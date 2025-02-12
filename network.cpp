#include "network.h"
#include <iostream>
#include "configreader.h"


Network::Network(QObject *parent): QObject(parent) {
    connect(&socket_, &QUdpSocket::readyRead, this, &Network::receivePortData);

    connect(&tcp_socket_, &QTcpSocket::readyRead, this, &Network::tcpHandler);
    connect(&tcp_socket_, &QTcpSocket::disconnected, this, &Network::tcpDisconnect);
    connect(&timer_, &QTimer::timeout, this, &Network::tcpDisconnect);
    timer_.setInterval(5000);
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
    auto message = QString(tcp_socket_.readAll());
    if(message == "connected") {
        timer_.start();
        emit tcpIsConnected();
    }
}

void Network::tcpDisconnect()
{
    emit tcpDisconnected();
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

void Network::tcpConnect()
{
    tcp_socket_.connectToHost(ConfigReader::getInstance().get("network", "cameraIp").toString(),
                              ConfigReader::getInstance().get("network", "cameraTcpPort").toInt());
}
