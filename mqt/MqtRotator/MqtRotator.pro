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

*g++*:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wold-style-cast -DNDEBUG
else:*g++*:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wold-style-cast

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
    rotatorRpc.cpp

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
    rotatorCommonConstants.h

FORMS    += \
    skyscandialog.ui \
    setupdialog.ui \
    logdialog.ui \
    editpresetsdialog.ui \
    rotatormainwindow.ui

