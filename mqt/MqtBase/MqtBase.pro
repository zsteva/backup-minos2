#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T23:58:51
#
#-------------------------------------------------

QT       += core gui
QT       += widgets
QT       += network

TARGET = MqtBase
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++14

win32-g++:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
else:win32-g++:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

DEFINES += TIXML_USE_STL

INCLUDEPATH += ../MqtUtils
INCLUDEPATH += ../TinyXML
INCLUDEPATH += ../XMPPLib
INCLUDEPATH += ../XMPPClientLib
INCLUDEPATH += ../qtcsv/src/include

SOURCES += \
    BandList.cpp \
    calcs.cpp \
    contacts.cpp \
    contest.cpp \
    cutils.cpp \
    latlong.cpp \
    list.cpp \
    ListContact.cpp \
    MatchContact.cpp \
    MinosLoggerEvents.cpp \
    MinosParameters.cpp \
    MinosTestImport.cpp \
    mults.cpp \
    MultsImpl.cpp \
    mwin.cpp \
    RunApp.cpp \
    ScreenContact.cpp \
    validators.cpp \
    MMessageDialog.cpp \
    MShowMessageDlg.cpp \
    DisplayContestContact.cpp \
    TreeUtils.cpp \
    minossplitter.cpp \
    htmldelegate.cpp \
    CalendarList.cpp \
    qlogtabwidget.cpp

HEADERS += \
    BandList.h \
    base_pch.h \
    calcs.h \
    contacts.h \
    contest.h \
    latlong.h \
    list.h \
    ListContact.h \
    MatchContact.h \
    MinosLoggerEvents.h \
    MinosParameters.h \
    MinosTestImport.h \
    mults.h \
    MultsImpl.h \
    mwin.h \
    ProfileEnums.h \
    RunApp.h \
    ScreenContact.h \
    sorted_vector.h \
    validators.h \
    MMessageDialog.h \
    MShowMessageDlg.h \
    DisplayContestContact.h \
    TreeUtils.h \
    minossplitter.h \
    htmldelegate.h \
    CalendarList.h \
    cutils.h \
    qlogtabwidget.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    MqtBase.pro.autosave