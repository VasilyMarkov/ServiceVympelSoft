#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include "qcustomplot.h"
#include <QThread>
#include <unordered_map>
#include <deque>
#include "network.h"
#include <cmath>
#include <iostream>


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
    return a*std::exp(-(0.5*z)/(var*var));
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
    void on_connect_button_clicked();

    void on_setRate_button_clicked();

Q_SIGNALS:
    void sendData(const QJsonDocument&);
public slots:
    void receiveTCPConnection();
    void tcpDisconnection();
private:
    void setupPlot(QCustomPlot*);
    void modeEval(EventType);
    QHostAddress getOwnIp() const;


private:
    Ui::MainWindow *ui;
    QProcess process_;
    QStringList pythonCommandArguments_;
    std::unique_ptr<Network> network_;
    QCustomPlot* plot;
    std::unordered_map<EventType, QString> modes_;
    int sample_ = 0;
    Commands commands_;
};
#endif // MAINWINDOW_H
