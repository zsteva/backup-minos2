#-------------------------------------------------
#
# Project created by QtCreator 2015-12-31T18:31:13
#
#-------------------------------------------------

QT       -= gui

TARGET = MqtUtils
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

win32-g++:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
else:win32-g++:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

INCLUDEPATH += ../TinyXML

DEFINES += TIXML_USE_STL


SOURCES += \
    fileutils.cpp

HEADERS += \
    fileutils.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
