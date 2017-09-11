#-------------------------------------------------
#
# Project created by QtCreator 2017-07-12T16:19:05
#
#-------------------------------------------------
include($$PWD/../mqt.pri)
include($$PWD/../mqtapplibs.pri)

QT       += core gui
QT       += network
QT       += widgets

TARGET = Qs1rSync
TEMPLATE = app

win32:RC_ICONS += ../minos.ico


SOURCES += \
        main.cpp \
        MainWindow.cpp

HEADERS += \
        MainWindow.h \
        qs_defines.h

FORMS += \
        MainWindow.ui

