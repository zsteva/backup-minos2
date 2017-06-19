#-------------------------------------------------
#
# Project created by QtCreator 2015-12-31T18:31:13
#
#-------------------------------------------------

QT       -= gui

TARGET = MqtUtils
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

*g++*:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wold-style-cast -DNDEBUG
else:*g++*:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wold-style-cast


SOURCES += \
    fileutils.cpp \
    ConfigFile.cpp

HEADERS += \
    fileutils.h \
    ConfigFile.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
