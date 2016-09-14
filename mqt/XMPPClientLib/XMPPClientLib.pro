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

INCLUDEPATH += ../TinyXML
INCLUDEPATH += ../XMPPLib

SOURCES += \
    MinosConnection.cpp \
    MinosRPC.cpp

HEADERS += \
    MinosConnection.h \
    XMPPClient_pch.h \
    MinosRPC.h \
    RPCCommandConstants.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
