QT       += core gui network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    network.cpp \
    qcustomplot.cpp
HEADERS += \
    configreader.h \
    mainwindow.h \
    network.h \
    qcustomplot.h \
    utility.h

FORMS += \
    mainwindow.ui
