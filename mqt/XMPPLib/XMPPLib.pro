#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T19:15:23
#
#-------------------------------------------------

QT       += network
QT       -= gui

TARGET = XMPPLib
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wold-style-cast

INCLUDEPATH += ../TinyXML
INCLUDEPATH += ../MqtUtils

DEFINES += TIXML_USE_STL

SOURCES += \
    Dispatcher.cpp \
    LogEvents.cpp \
    MLogFile.cpp \
    MTrace.cpp \
    PortIds.cpp \
    PubSubClient.cpp \
    RPCPubSub.cpp \
    ServerEvent.cpp \
    TinyUtils.cpp \
    XMPPEvents.cpp \
    XMPPRPCObj.cpp \
    XMPPRPCParams.cpp \
    XMPPStanzas.cpp \
    MinosConnection.cpp \
    MinosRPC.cpp

HEADERS += \
    Dispatcher.h \
    LogEvents.h \
    MLogFile.h \
    MTrace.h \
    PortIds.h \
    PubSubClient.h \
    RPCPubSub.h \
    ServerEvent.h \
    TinyUtils.h \
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
