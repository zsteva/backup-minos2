#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T19:07:12
#
#-------------------------------------------------

QT       -= core gui

TARGET = TinyXML
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++14

DEFINES += TIXML_USE_STL

win32-g++:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
else:win32-g++:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

android:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
else:android:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

SOURCES += tinyxml.cpp \
    tinyxmlerror.cpp \
    tinyxmlparser.cpp

HEADERS += tinyxml.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
