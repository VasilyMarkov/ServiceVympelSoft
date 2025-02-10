/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QCustomPlot *plot;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;
    QLabel *status;
    QLabel *end;
    QLabel *begin;
    QGridLayout *gridLayout_2;
    QLabel *mean_label;
    QLabel *label;
    QLabel *label_3;
    QLabel *std_label;
    QVBoxLayout *verticalLayout_2;
    QPushButton *startCV;
    QPushButton *stopCV;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_2;
    QPushButton *slow_cooling_button;
    QPushButton *slow_heating_button;
    QPushButton *fast_cooling_button;
    QPushButton *fast_heating_button;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_6;
    QLabel *temperature_label;
    QSpacerItem *verticalSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(970, 784);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        plot = new QCustomPlot(centralwidget);
        plot->setObjectName(QString::fromUtf8("plot"));

        verticalLayout->addWidget(plot);

        verticalLayout->setStretch(0, 1);

        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        status = new QLabel(centralwidget);
        status->setObjectName(QString::fromUtf8("status"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(status->sizePolicy().hasHeightForWidth());
        status->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        font.setItalic(false);
        font.setWeight(75);
        status->setFont(font);
        status->setWordWrap(false);

        gridLayout->addWidget(status, 1, 0, 1, 1);

        end = new QLabel(centralwidget);
        end->setObjectName(QString::fromUtf8("end"));
        QFont font1;
        font1.setPointSize(14);
        end->setFont(font1);

        gridLayout->addWidget(end, 3, 0, 1, 1);

        begin = new QLabel(centralwidget);
        begin->setObjectName(QString::fromUtf8("begin"));
        begin->setFont(font1);

        gridLayout->addWidget(begin, 2, 0, 1, 1);


        verticalLayout_3->addLayout(gridLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        mean_label = new QLabel(centralwidget);
        mean_label->setObjectName(QString::fromUtf8("mean_label"));

        gridLayout_2->addWidget(mean_label, 0, 1, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        std_label = new QLabel(centralwidget);
        std_label->setObjectName(QString::fromUtf8("std_label"));

        gridLayout_2->addWidget(std_label, 1, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout_2);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        startCV = new QPushButton(centralwidget);
        startCV->setObjectName(QString::fromUtf8("startCV"));
        sizePolicy.setHeightForWidth(startCV->sizePolicy().hasHeightForWidth());
        startCV->setSizePolicy(sizePolicy);

        verticalLayout_2->addWidget(startCV);

        stopCV = new QPushButton(centralwidget);
        stopCV->setObjectName(QString::fromUtf8("stopCV"));
        sizePolicy.setHeightForWidth(stopCV->sizePolicy().hasHeightForWidth());
        stopCV->setSizePolicy(sizePolicy);

        verticalLayout_2->addWidget(stopCV);

        verticalLayout_2->setStretch(0, 1);
        verticalLayout_2->setStretch(1, 1);

        verticalLayout_3->addLayout(verticalLayout_2);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_4->addWidget(label_2);

        slow_cooling_button = new QPushButton(centralwidget);
        slow_cooling_button->setObjectName(QString::fromUtf8("slow_cooling_button"));

        verticalLayout_4->addWidget(slow_cooling_button);

        slow_heating_button = new QPushButton(centralwidget);
        slow_heating_button->setObjectName(QString::fromUtf8("slow_heating_button"));

        verticalLayout_4->addWidget(slow_heating_button);

        fast_cooling_button = new QPushButton(centralwidget);
        fast_cooling_button->setObjectName(QString::fromUtf8("fast_cooling_button"));

        verticalLayout_4->addWidget(fast_cooling_button);

        fast_heating_button = new QPushButton(centralwidget);
        fast_heating_button->setObjectName(QString::fromUtf8("fast_heating_button"));

        verticalLayout_4->addWidget(fast_heating_button);


        verticalLayout_3->addLayout(verticalLayout_4);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_6 = new QVBoxLayout(groupBox);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        temperature_label = new QLabel(groupBox);
        temperature_label->setObjectName(QString::fromUtf8("temperature_label"));

        verticalLayout_6->addWidget(temperature_label);


        verticalLayout_3->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        verticalLayout_3->setStretch(0, 1);
        verticalLayout_3->setStretch(1, 1);
        verticalLayout_3->setStretch(2, 2);
        verticalLayout_3->setStretch(3, 3);
        verticalLayout_3->setStretch(4, 1);
        verticalLayout_3->setStretch(5, 6);

        horizontalLayout->addLayout(verticalLayout_3);

        horizontalLayout->setStretch(0, 5);
        horizontalLayout->setStretch(1, 1);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 970, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        status->setText(QApplication::translate("MainWindow", "Ready", nullptr));
        end->setText(QString());
        begin->setText(QString());
        mean_label->setText(QString());
        label->setText(QApplication::translate("MainWindow", "Mean:", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Std:", nullptr));
        std_label->setText(QString());
        startCV->setText(QApplication::translate("MainWindow", "START", nullptr));
        stopCV->setText(QApplication::translate("MainWindow", "STOP", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Hygrovision", nullptr));
        slow_cooling_button->setText(QApplication::translate("MainWindow", "Slow cooling", nullptr));
        slow_heating_button->setText(QApplication::translate("MainWindow", "Slow heating", nullptr));
        fast_cooling_button->setText(QApplication::translate("MainWindow", "Fast cooling", nullptr));
        fast_heating_button->setText(QApplication::translate("MainWindow", "Fast heating", nullptr));
        groupBox->setTitle(QApplication::translate("MainWindow", "Temperature, C:", nullptr));
        temperature_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
