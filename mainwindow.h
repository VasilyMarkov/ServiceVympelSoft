#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QThread>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <deque>
#include <cmath>
#include <iostream>
#include "qcustomplot.h"
#include "network.h"
#include <opencv2/opencv.hpp>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class EventType {
    IDLE,
    CALIBRATION,
    MEASHUREMENT,
    CONDENSATION,
    END
};

enum class Commands {
    no_commands,
    halt,
    work,
    setRateTemprature,
    close,
    reset
};

const QString normal_state{"{border: 1px solid #8f8f8f; \
                            border-radius: 2px; \
                            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #00DD00, stop: 1 #77FF77)}"};

const QString crictical_state{"{border: 1px solid #8f8f8f; \
                     border-radius: 2px; \
                     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F92424, stop: 1 #FA7878)}"};

template <typename T>
void print(const std::vector<T>& vector) {
   for(auto&& el:vector) {
       std::cout << el << ' ';
   }
   std::cout << std::endl;
}


inline double gaussval(const std::vector<double>& coeffs, double x) noexcept {
    double a = coeffs[0];
    double mean = coeffs[1];
    double var = coeffs[2];
    double z = (x-mean)/var;
    return a*std::pow(M_E, -0.5*z*z);
}

inline double polyval(const std::vector<double>& coeffs, double x) noexcept {
    double tmp = 0.0;
    size_t degree = coeffs.size() - 1;
    for( auto&& coeff : coeffs) {
         tmp += coeff*std::pow(x, degree);
         --degree;
    }
    return tmp;
}

inline double gaussPolyVal(const std::vector<double>& coeffs, double x) noexcept {
    double a = coeffs[0];
    double mean = coeffs[1];
    double var = coeffs[2];
    double z = polyval(std::vector<double>(std::next(std::begin(coeffs), 3), std::end(coeffs)),(x-mean));
    return a*std::exp(-(0.5*z*z)/(var*var));
}

template<typename Func>
std::pair<std::vector<double>,std::vector<double>>  applyFunc(
    const std::vector<double>& coeffs,
    int begin,
    int end,
    size_t pointsNum,
    Func&& function)
{
    assert(end > begin);
    std::vector<double> x_data(pointsNum);
    double step = std::fabs(end-begin)/pointsNum;
    x_data[0] = begin;
    std::generate(std::next(std::begin(x_data)), std::end(x_data), [step, begin](){
        static double val = begin;
        return val += step;
    });

    std::vector<double> y_data;
    y_data.reserve(x_data.size());

    for(auto&& x : x_data) {
        y_data.push_back(function(coeffs, x));
    }
    return {x_data, y_data};
}


class VideoReceiver : public QObject {
    Q_OBJECT

public:
    VideoReceiver(QLabel *label) : label(label) {
        udpSocket = new QUdpSocket(this);
        udpSocket->bind(QHostAddress::Any, 12345);
        connect(udpSocket, &QUdpSocket::readyRead, this, &VideoReceiver::readDatagram);

        udpSocket1 = new QUdpSocket(this);
        udpSocket1->bind(QHostAddress::Any, 12346);
        connect(udpSocket1, &QUdpSocket::readyRead, this, &VideoReceiver::readDatagram1);
    }
signals:
    void sendImage(const QPixmap&);
    void sendImage1(const QPixmap&);
private slots:
    void readDatagram() {
        while (udpSocket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(udpSocket->pendingDatagramSize());
            udpSocket->readDatagram(datagram.data(), datagram.size());
            if(datagram.isEmpty()) return;

            std::vector<uchar> buffer(datagram.begin(), datagram.end());
            cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

            if (!frame.empty()) {
                cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

                QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

                if (!qimg.isNull()) {
                    emit sendImage(QPixmap::fromImage(qimg));
                } else {
                    qWarning() << "Failed to create QImage from frame.";
                }
            } else {
                qWarning() << "Received empty or invalid frame.";
            }
        }
    }
    void readDatagram1() {
        while (udpSocket1->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(udpSocket1->pendingDatagramSize());
            udpSocket1->readDatagram(datagram.data(), datagram.size());
            if(datagram.isEmpty()) return;

            std::vector<uchar> buffer(datagram.begin(), datagram.end());
            cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

            if (!frame.empty()) {
                cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

                QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

                if (!qimg.isNull()) {
                    emit sendImage1(QPixmap::fromImage(qimg));
                } else {
                    qWarning() << "Failed to create QImage from frame.";
                }
            } else {
                qWarning() << "Received empty or invalid frame.";
            }
        }
    }

private:
    QUdpSocket *udpSocket;
    QUdpSocket *udpSocket1;
    QLabel *label;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void receiveData(const QJsonDocument& json);
    void mouseWheel();

    void on_stopCV_clicked();
    void on_startCV_clicked();
    void on_setRate_button_clicked();

    void drawFunc(const QVector<double>&);
    void receiveImage(const QPixmap&);
    void receiveImage1(const QPixmap&);

    void on_closeAppButton_clicked();

    void on_reset_button_clicked();

signals:
    void sendData(const QJsonDocument&);
    void sendFuncParameters(const QVector<double>&);

public slots:
    void tcpIsConnected();
    void tcpIsDisconnected();
private:
    void setupPlot(QCustomPlot*);
    void modeEval(EventType);
    void getFuncParameters(const QJsonDocument&);
    size_t findEndPoint(const QVector<double>&);
private:
    Ui::MainWindow *ui;
    QProcess process_;
    QStringList pythonCommandArguments_;
    std::unique_ptr<Network> network_;
    std::unique_ptr<VideoReceiver> videoReceiver_;
    std::vector<double> data_;
    std::vector<double> temp_data_;
    QCustomPlot* plot;
    QCustomPlot* finalPlot;
    QThread videoReceiverThread_;
    QThread networkThread_;
    int sample_ = 0;
    Commands commands_ = Commands::no_commands;
    bool firstCallEvaluateParameters_ = true;
    QTimer test_timer_;
};
#endif // MAINWINDOW_H
