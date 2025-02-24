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

INCLUDEPATH += /usr/include/opencv4

LIBS += -L/usr/lib/x86_64-linux-gnu \
        -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lopencv_imgcodecs
