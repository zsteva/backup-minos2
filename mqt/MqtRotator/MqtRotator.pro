#-------------------------------------------------
#
# Project created by QtCreator 2016-03-26T21:27:38
#
#-------------------------------------------------
include($$PWD/../mqt.pri)
include($$PWD/../mqthamlib.pri)
include($$PWD/../mqtapplibs.pri)

QT       += core gui network widgets serialport

TARGET = MqtRotator
TEMPLATE = app

win32:RC_ICONS += ../MinosRotator.ico

SOURCES += \
    skyscandialog.cpp \
    rotatormainwindow.cpp \
    minoscompass.cpp \
    main.cpp \
    logdialog.cpp \
    rotcontrol.cpp \
    rotatorlog.cpp \
    rotatorRpc.cpp \
    rotsetupform.cpp \
    addantennadialog.cpp \
    rotsetupdialog.cpp

HEADERS  += \
    skyscandialog.h \
    rotatormainwindow.h \
    minoscompass.h \
    logdialog.h \
    rotcontrol.h \
    rotatorlog.h \
    rotatorRpc.h \
    rotsetupform.h \
    addantennadialog.h \
    rotsetupdialog.h

FORMS    += \
    skyscandialog.ui \
    logdialog.ui \
    rotatormainwindow.ui \
    rotsetupform.ui \
    addantennadialog.ui \
    rotsetupdialog.ui

