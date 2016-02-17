#-------------------------------------------------
#
# Project created by QtCreator 2015-11-30T19:20:34
#
#-------------------------------------------------

QT       += core gui
QT       += widgets
QT       += network

TARGET = MqtLogger
TEMPLATE = app

VERSION=0.0.0.1
win32:RC_ICONS += ../minos.ico

DEFINES += VERSION=\\\"$$VERSION\\\"

android {
    CONFIG_INSTALL_PATH=/assets/Configuration

    config.path = $$CONFIG_INSTALL_PATH
    config.files += android/MinosLogger.ini
    config.depends += FORCE

    INSTALLS += config
}

CONFIG += c++14

win32-g++:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
else:win32-g++:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

android:CONFIG(release, debug|release): QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
else:android:CONFIG(debug, debug|release):QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

DEFINES += TIXML_USE_STL

SOURCES += main.cpp \
    tlogcontainer.cpp \
    contestdetails.cpp \
    ContestApp.cpp \
    LoggerContacts.cpp \
    LoggerContest.cpp \
    profiles.cpp \
    tsinglelogframe.cpp \
    tentryoptionsform.cpp \
    MatchThread.cpp \
    AdifImport.cpp \
    enqdlg.cpp \
    MinosTestExport.cpp \
    reg1test.cpp \
    PrintFile.cpp \
    tbundleframe.cpp \
    tsettingseditdlg.cpp \
    tcalendarform.cpp \
    tminoshelpform.cpp \
    taboutbox.cpp \
    tmanagelistsdlg.cpp \
    tminosbandchooser.cpp \
    tclockdlg.cpp \
    tforcelogdlg.cpp \
    tqsoeditdlg.cpp \
    tloccalcform.cpp \
    qsologframe.cpp \
    tstatsdispframe.cpp \
    dxccframe.cpp \
    districtframe.cpp \
    locframe.cpp \
    SendRPCDM.cpp \
    ConfigDM.cpp \
    tconfigframe.cpp

HEADERS  += \
    tlogcontainer.h \
    contestdetails.h \
    logger_pch.h \
    ContestApp.h \
    LoggerContacts.h \
    LoggerContest.h \
    profiles.h \
    tsinglelogframe.h \
    tentryoptionsform.h \
    MatchThread.h \
    AdifImport.h \
    enqdlg.h \
    MinosTestExport.h \
    reg1test.h \
    printfile.h \
    tbundleframe.h \
    tsettingseditdlg.h \
    tcalendarform.h \
    tminoshelpform.h \
    taboutbox.h \
    tmanagelistsdlg.h \
    tminosbandchooser.h \
    tclockdlg.h \
    tforcelogdlg.h \
    tqsoeditdlg.h \
    tloccalcform.h \
    qsologframe.h \
    focuswatcher.h \
    tstatsdispframe.h \
    dxccframe.h \
    districtframe.h \
    locframe.h \
    SendRPCDM.h \
    ConfigDM.h \
    tconfigframe.h

FORMS    += tlogcontainer.ui \
    contestdetails.ui \
    tsinglelogframe.ui \
    tentryoptionsform.ui \
    tbundleframe.ui \
    tsettingseditdlg.ui \
    tcalendarform.ui \
    tminoshelpform.ui \
    taboutbox.ui \
    tmanagelistsdlg.ui \
    tminosbandchooser.ui \
    tclockdlg.ui \
    tforcelogdlg.ui \
    tqsoeditdlg.ui \
    tloccalcform.ui \
    qsologframe.ui \
    tstatsdispframe.ui \
    dxccframe.ui \
    districtframe.ui \
    locframe.ui \
    tconfigframe.ui


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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../XMPPClientLib/release/ -lXMPPClientLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../XMPPClientLib/debug/ -lXMPPClientLib
else:unix: LIBS += -L$$OUT_PWD/../XMPPClientLib/ -lXMPPClientLib

INCLUDEPATH += $$PWD/../XMPPClientLib
DEPENDPATH += $$PWD/../XMPPClientLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../XMPPClientLib/release/libXMPPClientLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../XMPPClientLib/debug/libXMPPClientLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../XMPPClientLib/release/XMPPClientLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../XMPPClientLib/debug/XMPPClientLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../XMPPClientLib/libXMPPClientLib.a

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

DISTFILES += \
    AndroidTemplate/AndroidManifest.xml \
    AndroidTemplate/gradle/wrapper/gradle-wrapper.jar \
    AndroidTemplate/gradlew \
    AndroidTemplate/res/values/libs.xml \
    AndroidTemplate/build.gradle \
    AndroidTemplate/gradle/wrapper/gradle-wrapper.properties \
    AndroidTemplate/gradlew.bat \
    android/MinosLogger.ini

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/AndroidTemplate
