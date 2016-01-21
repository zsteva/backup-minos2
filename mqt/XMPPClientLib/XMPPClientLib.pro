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

QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

DEFINES += TIXML_USE_STL

INCLUDEPATH += C:\Projects\boost\boost_1_59_0

INCLUDEPATH += ../TinyXML
INCLUDEPATH += ../XMPPLib

SOURCES += \
    BandMapRPCObj.cpp \
    ChatRPCObj.cpp \
    ControlRPCObj.cpp \
    KeyerRPCObj.cpp \
    LoggerRPCObj.cpp \
    MinosConnection.cpp

HEADERS += \
    BandMapRPCObj.h \
    ChatRPCObj.h \
    ControlRPCObj.h \
    KeyerRPCObj.h \
    LoggerRPCObj.h \
    MinosConnection.h \
    XMPPClient_pch.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
