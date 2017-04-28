#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T21:16:20
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

win32: INCLUDEPATH += C:/Projects/hamlib-w32-3.1/include

win32: LIBS += -LC:/Projects/hamlib-w32-3.1/lib/gcc/ -llibhamlib

TARGET = MqtBandmap
TEMPLATE = app


SOURCES += main.cpp\
        bandmapmainwindow.cpp \
    rigcontrol.cpp \
    setupdialog.cpp \
    bandmap.cpp \
    callsignmarker.cpp \
    freqdial.cpp \
    textmarker.cpp

HEADERS  += bandmapmainwindow.h \
    rigcontrol.h \
    setupdialog.h \
    bandmap.h \
    callsignmarker.h \
    freqdial.h \
    textmarker.h

FORMS    += bandmapmainwindow.ui \
    setupdialog.ui
