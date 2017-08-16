#-------------------------------------------------
#
# Project created by QtCreator 2017-07-30T11:35:33
#
#-------------------------------------------------
include($$PWD/../mqt.pri)
include($$PWD/../mqtapplibs.pri)

QT       += core gui network widgets

TARGET = MqtMonitor
TEMPLATE = app

win32:RC_ICONS += ../minos.ico

SOURCES += \
        main.cpp \
        MonitorMain.cpp \
    MonitoringFrame.cpp \
    MonitoredLog.cpp \
    MonitoredContestLog.cpp \
    MonitorParameters.cpp

HEADERS += \
        MonitorMain.h \
    MonitoringFrame.h \
    MonitoredLog.h \
    MonitoredContestLog.h \
    MonitorParameters.h

FORMS += \
        MonitorMain.ui \
    MonitoringFrame.ui
