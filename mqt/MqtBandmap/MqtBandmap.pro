#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T21:16:20
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION=0.6.0.0
win32:RC_ICONS += ../minos.ico

DEFINES += VERSION=\\\"$$VERSION\\\"

CONFIG += c++11

win32: INCLUDEPATH += C:/Projects/hamlib-w32-3.1/include

win32-g++: LIBS += -LC:/Projects/hamlib-w32-3.1/lib/gcc/ -llibhamlib
msvc: LIBS += -LC:/Projects/hamlib-w32-3.1/lib/msvc/ -llibhamlib-2

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
