#include "network.h"
#include <iostream>
#include "configreader.h"
#include <QNetworkInterface>
#include <QNetworkAddressEntry>

Network::Network(QObject *parent):
    QObject(parent),
    cameraDiscoverSocket_(std::make_unique<QUdpSocket>()),
    cameraDiscoverPort_(ConfigReader::getInstance().get("network", "cameraDiscoverPort").toInt()),
    ownIpAdress_(ConfigReader::getInstance().get("network", "hostIp").toString())
{
    cameraDiscoverSocket_->bind(cameraDiscoverPort_, QUdpSocket::ShareAddress);

    connect(cameraDiscoverSocket_.get(), &QUdpSocket::readyRead, this, &Network::processPendingDatagrams);
    connect(&socket_, &QUdpSocket::readyRead, this, &Network::receivePortData);
    connect(&tcp_socket_, &QTcpSocket::readyRead, this, &Network::tcpHandler);
    connect(&tcp_socket_, &QTcpSocket::connected, this, &Network::tcpConnectHandler);
    connect(&tcp_socket_, &QTcpSocket::disconnected, this, &Network::tcpDisconnectHandler);
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

void Network::tcpConnectHandler()
{
    tcp_socket_.write(QByteArray(ownIpAdress_.toString().toUtf8()));
    emit tcpIsConnected();
}

void Network::tcpDisconnectHandler()
{
    cameraIsConnected_ = false;
    if (cameraDiscoverSocket_->state() == QAbstractSocket::UnconnectedState) {
        cameraDiscoverSocket_->bind(cameraDiscoverPort_, QUdpSocket::ShareAddress);
    }
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

}

void Network::createTcpConnection()
{
    if(cameraIsConnected_) return;

    tcp_socket_.connectToHost(cameraIpAdress_, ConfigReader::getInstance().get("network", "cameraTcpPort").toInt());
    ownIpAdress_ = getOwnIp(cameraIpAdress_);
    setReceiverParameters(ownIpAdress_,
        ConfigReader::getInstance().get("network", "serviceProgramPort").toInt());
    setSenderParameters(QHostAddress(cameraIpAdress_.toIPv4Address()),
        ConfigReader::getInstance().get("network", "controlFromServiceProgramPort").toInt());
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

QHostAddress Network::getCameraIp() const
{
    return cameraIpAdress_;
}

bool isInSameSubnet(const QHostAddress& ip, const QHostAddress& netmask, const QHostAddress& referenceIp) {
    quint32 ipAddress = ip.toIPv4Address();
    quint32 netmaskAddress = netmask.toIPv4Address();
    quint32 referenceIpAddress = referenceIp.toIPv4Address();

    quint32 network1 = ipAddress & netmaskAddress;
    quint32 network2 = referenceIpAddress & netmaskAddress;

    return network1 == network2;
}

QHostAddress Network::getOwnIp(const QHostAddress& referenceIp) {
    auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto& interface : interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp)) {
            for (const auto& entry : interface.addressEntries()) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol && !entry.ip().isLoopback()) {
                    // Check if the IP is in the same subnet as the reference IP
                    if (isInSameSubnet(entry.ip(), entry.netmask(), referenceIp)) {
                        return entry.ip();
                    }
                }
            }
        }
    }
    return QHostAddress(ConfigReader::getInstance().get("network", "cameraIp").toString());
}


