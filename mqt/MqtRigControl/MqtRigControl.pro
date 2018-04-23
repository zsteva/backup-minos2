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

win32:RC_ICONS += ../MinosRig.ico


SOURCES += main.cpp\
        rigcontrolmainwindow.cpp \
    rigcontrol.cpp \
    rigcontrolrpc.cpp \
    rigsetupform.cpp \
    transvertsetupform.cpp \
    addtransverterdialog.cpp \
    freqpresetdialog.cpp \
    addradiodialog.cpp \
    rigsetupdialog.cpp \
    serialtvswitch.cpp

HEADERS  += rigcontrolmainwindow.h \
    rigcontrol.h \
    rigcontrolrpc.h \
    rigsetupform.h \
    transvertsetupform.h \
    addtransverterdialog.h \
    freqpresetdialog.h \
    addradiodialog.h \
    rigsetupdialog.h \
    serialtvswitch.h



FORMS    += rigcontrolmainwindow.ui \
    rigsetupdialog.ui \
    rigsetupform.ui \
    transvertsetupform.ui \
    addtransverterdialog.ui \
    freqpresetdialog.ui \
    addradiodialog.ui

win32: INCLUDEPATH += C:/Projects/hamlib-w32-3.1/include
