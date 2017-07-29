#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T21:16:20
#
#-------------------------------------------------
include($$PWD/../mqt.pri)

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

HAMLIBDIR = $$absolute_path(../../../hamlib-w32-3.1)

win32: INCLUDEPATH += $$HAMLIBDIR/include

win32-g++: LIBS += -L$$HAMLIBDIR/lib/gcc/ -llibhamlib
msvc: LIBS += -L$$HAMLIBDIR/lib/msvc/ -llibhamlib-2
unix: LIBS += -lhamlib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MqtUtils/release/ -lMqtUtils
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MqtUtils/debug/ -lMqtUtils
else:unix: LIBS += -L$$OUT_PWD/../MqtUtils/ -lMqtUtils

INCLUDEPATH += $$PWD/../MqtUtils
DEPENDPATH += $$PWD/../MqtUtils

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/release/libMqtUtils.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/debug/libMqtUtils.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/release/MqtUtils.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/debug/MqtUtils.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/libMqtUtils.a
