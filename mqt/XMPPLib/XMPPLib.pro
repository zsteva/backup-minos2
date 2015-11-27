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

QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

INCLUDEPATH += C:\Projects\boost\boost_1_59_0
INCLUDEPATH += ../TinyXML

DEFINES += TIXML_USE_STL

SOURCES += \
    Dispatcher.cpp \
    GJVThreads.cpp \
    LogEvents.cpp \
    MLogFile.cpp \
    MTrace.cpp \
    PortIds.cpp \
    PubSubClient.cpp \
    RPCPubSub.cpp \
    ServerEvent.cpp \
    stringcast.cpp \
    TinyUtils.cpp \
    XMPPEvents.cpp \
    XMPPRPCObj.cpp \
    XMPPRPCParams.cpp \
    XMPPStanzas.cpp

HEADERS += \
    Dispatcher.h \
    GJVThreads.h \
    LogEvents.h \
    MLogFile.h \
    MTrace.h \
    PortIds.h \
    PubSubClient.h \
    RPCPubSub.h \
    ServerEvent.h \
    stringcast.h \
    TinyUtils.h \
    XMPP_pch.h \
    XMPPEvents.h \
    XMPPRPCObj.h \
    XMPPRPCParams.h \
    XMPPStanzas.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
