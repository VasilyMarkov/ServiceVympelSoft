QT       += core gui network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    udp.cpp
HEADERS += \
    configreader.h \
    mainwindow.h \
    qcustomplot.h \
    udp.h \
    utility.h

FORMS += \
    mainwindow.ui
