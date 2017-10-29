#-------------------------------------------------
#
# Project created by QtCreator 2017-02-13T17:38:22
#
#-------------------------------------------------

include($$PWD/../mqt.pri)
include($$PWD/../mqtapplibs.pri)

QT       += core gui
QT       += widgets
QT       += network

TARGET = MqtAppStarter
TEMPLATE = app

win32:RC_ICONS += ../MinosAppStarter.ico


SOURCES += main.cpp\
        MainWindow.cpp

HEADERS  += MainWindow.h

FORMS    += MainWindow.ui
