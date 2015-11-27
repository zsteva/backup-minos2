#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T19:07:12
#
#-------------------------------------------------

QT       -= core gui

TARGET = TinyXML
TEMPLATE = lib
CONFIG += staticlib

DEFINES += TIXML_USE_STL

SOURCES += tinyxml.cpp \
    tinyxmlerror.cpp \
    tinyxmlparser.cpp

HEADERS += tinyxml.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
