#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonDocument>

constexpr int RESERVE_PORTS = 1024;

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = nullptr);
    void setSenderParameters(const QHostAddress&, quint16);
    void setReceiverParameters(const QHostAddress&, quint16);
    QHostAddress getCameraIp() const;
signals:
    void sendData(const QJsonDocument&);
    void sendCameraIp(const QHostAddress&);
    void tcpIsConnected();
    void tcpIsDisconnected();
    void ready();
    void sendFuncCoeffs(const QVector<double>&);
private slots:
    void receivePortData();
    void createTcpConnection();
    void processPendingDatagrams();
    void tcpConnectHandler();
    void tcpDisconnectHandler();
public slots:
    void receiveData(const QJsonDocument&);
private:
    void sendPortData(const QByteArray&);
    QHostAddress getOwnIp(const QHostAddress&);
    void newTcpConnection();
    void handlingIncomingTcpPackets();
private:
    bool firstConnected_ = true;
    QUdpSocket socket_;
    std::unique_ptr<QUdpSocket> cameraDiscoverSocket_;
    QTcpSocket tcp_socket_;
    QTcpServer tcpServer_;
    QHostAddress senderAddr_ = QHostAddress::LocalHost;
    quint16 senderPort_ = 1024;
    quint16 cameraDiscoverPort_;
    QHostAddress cameraIpAdress_;
    QHostAddress ownIpAdress_;
    bool cameraIsConnected_ = false;
};



#endif // NETWORK_H
