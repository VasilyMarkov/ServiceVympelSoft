#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
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
    void tcpConnect();
signals:
    void sendData(const QJsonDocument&);
    void tcpIsConnected();
    void tcpDisconnected();
private slots:
    void receivePortData();
    void tcpHandler();
    void tcpDisconnect();
public slots:
    void receiveData(const QJsonDocument&);
private:
    void sendPortData(const QByteArray&);
    QTimer timer_;
    bool firstConnected_ = true;
    QUdpSocket socket_;
    QTcpSocket tcp_socket_;
    QHostAddress senderAddr_ = QHostAddress::LocalHost;
    quint16 senderPort_ = 1024;
};

#endif // NETWORK_H
