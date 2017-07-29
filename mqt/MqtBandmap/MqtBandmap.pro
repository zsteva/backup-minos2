#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T21:16:20
#
#-------------------------------------------------
include($$PWD/../mqt.pri)
include($$PWD/../mqthamlib.pri)
include($$PWD/../mqtapplibs.pri)

QT       += core gui serialport
QT       += widgets
QT       += network

VERSION=0.6.0.0

win32:RC_ICONS += ../minos.ico


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

