#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T19:18:09
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = XMPPClientLib
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

win32-g++:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
else:win32-g++:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

android:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
else:android:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

DEFINES += TIXML_USE_STL

INCLUDEPATH += ../TinyXML
INCLUDEPATH += ../XMPPLib

SOURCES += \
    BandMapRPCObj.cpp \
    ChatRPCObj.cpp \
    ControlRPCObj.cpp \
    KeyerRPCObj.cpp \
    LoggerRPCObj.cpp \
    MinosConnection.cpp \
    RotatorRPCObj.cpp

HEADERS += \
    BandMapRPCObj.h \
    ChatRPCObj.h \
    ControlRPCObj.h \
    KeyerRPCObj.h \
    LoggerRPCObj.h \
    MinosConnection.h \
    XMPPClient_pch.h \
    RotatorRPCObj.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
