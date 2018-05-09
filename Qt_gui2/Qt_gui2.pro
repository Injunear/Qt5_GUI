#-------------------------------------------------
#
# Project created by QtCreator 2015-01-11T09:53:16
#
#-------------------------------------------------

QT  += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = Qt_gui2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mythread.cpp

LIBS += -lwiringPi

HEADERS  += mainwindow.h \
    mythread.h

FORMS    += mainwindow.ui
