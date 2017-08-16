#-------------------------------------------------
#
# Project created by QtCreator 2017-07-29T13:06:40
#
#-------------------------------------------------
include($$PWD/../mqt.pri)
include($$PWD/../mqthamlib.pri)
include($$PWD/../mqtapplibs.pri)

QT       += core gui serialport
QT       += widgets
QT       += network


TARGET = MqtRigControl
TEMPLATE = app

win32:RC_ICONS += ../minos.ico


SOURCES += main.cpp\
        rigcontrolmainwindow.cpp \
    rigcontrol.cpp \
    setupdialog.cpp \
    rigcontrolrpc.cpp

HEADERS  += rigcontrolmainwindow.h \
    rigcontrol.h \
    setupdialog.h \
    rigcontrolrpc.h \
    rigcontrolcommonconstants.h

FORMS    += rigcontrolmainwindow.ui \
    setupdialog.ui

win32: INCLUDEPATH += C:/Projects/hamlib-w32-3.1/include
