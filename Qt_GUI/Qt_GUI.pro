#-------------------------------------------------
#
# Project created by QtCreator 2018-05-09T12:04:28
#
#-------------------------------------------------

QT  += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = Qt_GUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mythread.cpp

LIBS += -lwiringPi

HEADERS  += mainwindow.h \
    mythread.h

FORMS    += mainwindow.ui
