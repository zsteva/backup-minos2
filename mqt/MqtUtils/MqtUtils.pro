#-------------------------------------------------
#
# Project created by QtCreator 2015-12-31T18:31:13
#
#-------------------------------------------------
include($$PWD/../mqt.pri)

QT       += core gui
QT       += widgets
QT       += network
QT       -= gui

TARGET = MqtUtils
TEMPLATE = lib
CONFIG += staticlib

*g++*:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wold-style-cast -DNDEBUG
else:*g++*:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wold-style-cast


SOURCES += \
    fileutils.cpp \
    ConfigFile.cpp \
    AppStartup.cpp \
    StdInReader.cpp

HEADERS += \
    fileutils.h \
    ConfigFile.h \
    AppStartup.h \
    StdInReader.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
