#-------------------------------------------------
#
# Project created by QtCreator 2016-03-26T21:27:38
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MqtTestRotator
TEMPLATE = app

DEFINES += VERSION=\\\"$$VERSION\\\"


CONFIG += c++14

QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

DEFINES += TIXML_USE_STL

SOURCES += main.cpp\
        rotatormainwindow.cpp \
    rotatorlogic.cpp

HEADERS  += rotatormainwindow.h \
    rotatorlogic.h

FORMS    += rotatormainwindow.ui


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MqtBase/release/ -lMqtBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MqtBase/debug/ -lMqtBase
else:unix: LIBS += -L$$OUT_PWD/../MqtBase/ -lMqtBase

INCLUDEPATH += $$PWD/../MqtBase
DEPENDPATH += $$PWD/../MqtBase

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtBase/release/libMqtBase.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtBase/debug/libMqtBase.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtBase/release/MqtBase.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtBase/debug/MqtBase.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../MqtBase/libMqtBase.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../XMPPLib/release/ -lXMPPLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../XMPPLib/debug/ -lXMPPLib
else:unix: LIBS += -L$$OUT_PWD/../XMPPLib/ -lXMPPLib

INCLUDEPATH += $$PWD/../XMPPLib
DEPENDPATH += $$PWD/../XMPPLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../XMPPLib/release/libXMPPLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../XMPPLib/debug/libXMPPLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../XMPPLib/release/XMPPLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../XMPPLib/debug/XMPPLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../XMPPLib/libXMPPLib.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../TinyXML/release/ -lTinyXML
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../TinyXML/debug/ -lTinyXML
else:unix: LIBS += -L$$OUT_PWD/../TinyXML/ -lTinyXML

INCLUDEPATH += $$PWD/../TinyXML
DEPENDPATH += $$PWD/../TinyXML

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TinyXML/release/libTinyXML.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TinyXML/debug/libTinyXML.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TinyXML/release/TinyXML.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TinyXML/debug/TinyXML.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../TinyXML/libTinyXML.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MqtUtils/release/ -lMqtUtils
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MqtUtils/debug/ -lMqtUtils
else:unix: LIBS += -L$$OUT_PWD/../MqtUtils/ -lMqtUtils

INCLUDEPATH += $$PWD/../MqtUtils
DEPENDPATH += $$PWD/../MqtUtils

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/release/libMqtUtils.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/debug/libMqtUtils.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/release/MqtUtils.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/debug/MqtUtils.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../MqtUtils/libMqtUtils.a
