#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T19:15:23
#
#-------------------------------------------------
include($$PWD/../mqt.pri)

QT       += core gui
QT       += widgets
QT       += network

TARGET = XMPPLib
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../TinyXML
INCLUDEPATH += ../MqtUtils

SOURCES += \
    Dispatcher.cpp \
    PortIds.cpp \
    PubSubClient.cpp \
    RPCPubSub.cpp \
    ServerEvent.cpp \
    XMPPEvents.cpp \
    XMPPRPCObj.cpp \
    XMPPRPCParams.cpp \
    XMPPStanzas.cpp \
    MinosConnection.cpp \
    MinosRPC.cpp

HEADERS += \
    Dispatcher.h \
    PortIds.h \
    PubSubClient.h \
    RPCPubSub.h \
    ServerEvent.h \
    XMPP_pch.h \
    XMPPEvents.h \
    XMPPRPCObj.h \
    XMPPRPCParams.h \
    XMPPStanzas.h \
    MinosConnection.h \
    MinosRPC.h \
    RPCCommandConstants.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
