#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonDocument>
#include <QTimer>

constexpr int RESERVE_PORTS = 1024;

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = nullptr);
    void setSenderParameters(const QHostAddress&, quint16);
    void setReceiverParameters(const QHostAddress&, quint16);
signals:
    void sendData(const QJsonDocument&);
    void sendCameraIp(const QHostAddress&);
    void tcpIsConnected();
    void tcpIsDisconnected();
private slots:
    void receivePortData();
    void tcpHandler();
    void createTcpConnection();
    void processPendingDatagrams();
    void tcpDisconnectHandler();
public slots:
    void receiveData(const QJsonDocument&);
private:
    void sendPortData(const QByteArray&);
    QTimer disconnectTimer_;
    bool firstConnected_ = true;
    QUdpSocket socket_;
    std::unique_ptr<QUdpSocket> cameraDiscoverSocket_;
    QTcpSocket tcp_socket_;
    QHostAddress senderAddr_ = QHostAddress::LocalHost;
    quint16 senderPort_ = 1024;
    quint16 cameraDiscoverPort_;
    QHostAddress cameraIpAdress_;
    bool cameraIsConnected_ = false;
};



#endif // NETWORK_H
