#-------------------------------------------------
#
# Project created by QtCreator 2015-11-30T19:20:34
#
#-------------------------------------------------
include($$PWD/../mqt.pri)
include($$PWD/../mqtapplibs.pri)

QT       += core gui
QT       += widgets
QT       += network

TARGET = MqtLogger
TEMPLATE = app

win32:RC_ICONS += ../MinosLogger.ico

android {
    CONFIG_INSTALL_PATH=/assets/Configuration

    config.path = $$CONFIG_INSTALL_PATH
    config.files += android/Configuration/*
    config.depends += FORCE

    INSTALLS += config
}

INCLUDEPATH += $$PWD/../MqtRotator
#DEPENDPATH += $$PWD/../MqtRotator
INCLUDEPATH += $$PWD/../MqtRigControl
#DEPENDPATH += $$PWD/../MqtRigControl
INCLUDEPATH += $$PWD/../MqtBandMap
#DEPENDPATH += $$PWD/../MqtBandMap

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
    TSessionManager.cpp \
    TClockFrame.cpp \
    StartConfig.cpp \
    rigcontrolframe.cpp \
    rotcontrolframe.cpp \
    rigmemdialog.cpp \
    LocCalcFrame.cpp \
    StackedInfoFrame.cpp \
    MatchTreesFrame.cpp \
    runbuttondialog.cpp \
    RigMemoryFrame.cpp

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
    TSessionManager.h \
    TClockFrame.h \
    StartConfig.h \
    rigcontrolframe.h \
    rotcontrolframe.h \
    rigmemcommondata.h \
    rigmemdialog.h \
    LocCalcFrame.h \
    StackedInfoFrame.h \
    MatchTreesFrame.h \
    runbuttondialog.h \
    RigMemoryFrame.h

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
    TSessionManager.ui \
    TClockFrame.ui \
    StartConfig.ui \
    rigcontrolframe.ui \
    rigmemdialog.ui \
    rotcontrolframe.ui \
    LocCalcFrame.ui \
    StackedInfoFrame.ui \
    MatchTreesFrame.ui \
    runbuttondialog.ui \
    RigMemoryFrame.ui


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
