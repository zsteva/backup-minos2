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
    setupdialog.cpp \
    rotatormainwindow.cpp \
    minoscompass.cpp \
    main.cpp \
    logdialog.cpp \
    editpresetsdialog.cpp \
    rotcontrol.cpp \
    rotatorlog.cpp \
    rotatorRpc.cpp \
    rotsetupform.cpp

HEADERS  += \
    skyscandialog.h \
    setupdialog.h \
    rotatormainwindow.h \
    minoscompass.h \
    logdialog.h \
    editpresetsdialog.h \
    rotcontrol.h \
    rotatorlog.h \
    rotatorRpc.h \
    rotatorCommonConstants.h \
    rotsetupform.h

FORMS    += \
    skyscandialog.ui \
    setupdialog.ui \
    logdialog.ui \
    editpresetsdialog.ui \
    rotatormainwindow.ui \
    rotsetupform.ui

