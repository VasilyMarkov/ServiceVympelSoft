#ifndef UDP_H
#define UDP_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonDocument>

constexpr int RESERVE_PORTS = 1024;

class UdpSocket : public QObject
{
    Q_OBJECT
public:
    explicit UdpSocket(QObject *parent = nullptr);
    void setSenderParameters(const QHostAddress&, quint16);
    void setReceiverParameters(const QHostAddress&, quint16);
signals:
    void sendData(const QJsonDocument&);
private slots:
    void receivePortData();
public slots:
    void receiveData(const QJsonDocument&);
private:
    void sendPortData(const QByteArray&);
    QUdpSocket socket_;
    QHostAddress senderAddr_ = QHostAddress::LocalHost;
    quint16 senderPort_ = 1024;
};

#endif // UDP_H
