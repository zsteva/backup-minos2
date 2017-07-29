#-------------------------------------------------
#
# Project created by QtCreator 2015-11-30T15:50:02
#
#-------------------------------------------------
include($$PWD/../mqt.pri)
include($$PWD/../mqtapplibs.pri)

QT       += testlib
QT       += core gui
QT       += widgets
QT       += network

TARGET = tst_rpctesttest
CONFIG   += console

TEMPLATE = app

SOURCES += tst_rpctesttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

