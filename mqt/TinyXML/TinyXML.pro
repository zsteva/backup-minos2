#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T19:07:12
#
#-------------------------------------------------
include($$PWD/../mqt.pri)

QT       += core

TARGET = TinyXML
TEMPLATE = lib
CONFIG += staticlib

DEFINES += TIXML_USE_STL

*g++*:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -DNDEBUG
else:*g++*:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder


SOURCES += tinyxml.cpp \
    tinyxmlerror.cpp \
    tinyxmlparser.cpp \
    TinyUtils.cpp

HEADERS += tinyxml.h \
    TinyUtils.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
