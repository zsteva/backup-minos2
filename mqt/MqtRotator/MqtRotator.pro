#-------------------------------------------------
#
# Project created by QtCreator 2016-03-26T21:27:38
#
#-------------------------------------------------

QT       += core gui network widgets serialport


TARGET = MqtRotator
TEMPLATE = app

VERSION=0.6.0.0
DEFINES += VERSION=\\\"$$VERSION\\\"

win32:RC_ICONS += antenna_icon.ico

#RC_FILE = MqtRotator.rc

CONFIG += c++11

*g++*:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wold-style-cast -DNDEBUG
else:*g++*:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wold-style-cast

win32: INCLUDEPATH += C:/Projects/hamlib-w32-3.1/include

win32-g++: LIBS += -LC:/Projects/hamlib-w32-3.1/lib/gcc/ -llibhamlib
msvc: LIBS += -LC:/Projects/hamlib-w32-3.1/lib/msvc/ -llibhamlib-2

win32-g++:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
else:win32-g++:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder


DEFINES += TIXML_USE_STL

SOURCES += \
    skyscandialog.cpp \
    setupdialog.cpp \
    rotatormainwindow.cpp \
    minoscompass.cpp \
    main.cpp \
    logdialog.cpp \
    editpresetsdialog.cpp \
    rotcontrol.cpp \
    rotatorlog.cpp \
    rotatorRpc.cpp

HEADERS  += \
    skyscandialog.h \
    setupdialog.h \
    rotatormainwindow.h \
    minoscompass.h \
    logdialog.h \
    editpresetsdialog.h \
    rotcontrol.h \
    rotatorlog.h \
    rotatorRpc.h \
    rotatorCommonConstants.h

FORMS    += \
    skyscandialog.ui \
    setupdialog.ui \
    logdialog.ui \
    editpresetsdialog.ui \
    rotatormainwindow.ui

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

