/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
// Hamlib Library
//
/////////////////////////////////////////////////////////////////////////////



#ifndef ROTATORMAINWINDOW_H
#define ROTATORMAINWINDOW_H

#include "base_pch.h"
#include "rotatorRpc.h"
#include "rotatorcommon.h"
#include "editpresetsdialog.h"
#include "rotpresetdialog.h"
#include <QMainWindow>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QKeyEvent>
#include <QPushButton>
#include <QShortcut>




#define NUM_PRESETS 10
#define OFF false
#define ON true

const int POLLTIME = 500;

class QLabel;
class QComboBox;
class SetupDialog;
class MinosCompass;
class RotControl;
class EditPresetsDialog;
class RotatorRpc;
class RotatorLog;
class LogDialog;


namespace Ui {
class RotatorMainWindow;
}

class PresetButton : public QObject
{
    Q_OBJECT

public:
    explicit PresetButton(QToolButton *b, QMainWindow *RotatorMainWindow, int no);
    ~PresetButton();

    QMainWindow *rotatorMainWindow;
    QToolButton* presetButton;
    QMenu* presetMenu;
    QShortcut* shortKey;
    QShortcut* shiftShortKey;
    QAction* readAction;
    QAction* writeAction;
    QAction* editAction;
    QAction* clearAction;

    int presetNo;

private slots:
    void presetUpdate();

    void presetShortCutSelected();
    void readActionSelected();
    void editActionSelected();
    void writeActionSelected();
    void clearActionSelected();
signals:
    void clearActionSelected(int);
    void selectRadio(QString);


};

class RotatorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RotatorMainWindow(QWidget *parent = nullptr);
    ~RotatorMainWindow();


    void setPolltime(int interval);
    int getPolltime();

    // setup connections
    void initActionsConnections();



signals:

    void escapePressed();
    void rotate_cwKeyPressed();
    void rotate_ccwKeyPressed();
    void rotateStopKeyPressed();
    void rotateTurnKeyPressed();
    void rotateFunctionKeyPressed(int);

    void sendBearing(QString);
    void sendCompassDial(int);
    void displayOverlap(overlapStat);
    void checkingEndStop();
    void sendBackBearing(QString);
//   void displayOverlapBearing(QString);
    void displayActualBearing(QString);
    void presetRotateTo();

    void presetButtonUpdate(int);
    void presetButReadActSel(int buttonNumber);
    void presetButWriteActSel(int buttonNumber);
    void presetButEditActSel(int buttonNumber);
    void presetButClearActSel(int buttonNumber);

private:
    Ui::RotatorMainWindow *ui;

    StdInReader stdinReader;

    RotatorRpc *msg;

    QTimer LogTimer;
    QTimer RotateTimer;

    QMap<int, PresetButton *> presetButMap;
    QVector<RotPresetData*> rotPresets;

    QComboBox *selectAntenna;
    //QPushButton* presetButtons[NUM_PRESETS];
    QString appName = "";
    RotControl  *rotator;
    QLabel *status;
    QLabel *offSetlbl;
    QLabel *offSetDisplay;
    QLabel *actualRotatorlbl;
    QLabel *actualRotatorDisplay;
    QLabel *rawRotatorlbl;
    QLabel *rawRotatorDisplay;
    //QPalette *redText;
    //QPalette *blackText;
    SetupDialog *setupAntenna;
    EditPresetsDialog *editPresets;
    LogDialog *setupLog;
    RotatorLog *rotlog;
    bool rotLogFlg;
    QTimer *pollTimer;
    int pollTime;
    int rotTimeCount;
    int brakedelay;
    bool rot_left_button_status;
    bool rot_right_button_status;
    bool turn_button_status;
    bool brakeflag;
    bool moving;
    bool movingCW;
    bool movingCCW;
    bool cwCcwCmdflag;     // command sentflag
    bool stopCmdflag;
    bool rotCmdflag;
    bool reqBearCmdflag;

    //endStop endStopType;
    overlapStat overLapStatus;
    bool overLapActiveflag;
    //bool southStopActiveflag;
    bool rotErrorFlag;
    //bool supportCwCcwCmd;
    int rotatorBearing;
    int curBearingWithOffset;
    //int rotatorMinAzimuth;
    //int rotatorMaxAzimuth;
    //int currentMinAzimuth;
    //int currentMaxAzimuth;
    //int rotatorCWEndStop;
    //int rotatorCCWEndStop;
    QString backBearingmsg;

    //QString presetName[NUM_PRESETS];
    //QString presetBearing[NUM_PRESETS];

    QString geoStr;         // geometry registry location

    QString connectStat;
    QString statusMsg;
    QString activeMsg;

    void openRotator();
    void closeRotator();

    void refreshPresetLabels();
    void showStatusMessage(const QString &);
    void sendAntennaListLogger();
    void sendStatusLogger();
    void readPresets();
    void savePresets();

    void initSelectAntennaBox();

    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *);

    void initPresetButtons();
    void saveRotPresetButton(RotPresetData &editData);
    void setRotPresetButData(int buttonNumber, RotPresetData &editData);
    void rotPresetButtonUpdate(int buttonNumber, RotPresetData &editData);



    void hamlibError(int errorCode, QString cmd);



    void sleepFor(qint64 milliseconds);

    void rotatorActive();


public slots:
    void rotateToController();
    void displayBearing(int);
    void updatePresetLabels();
    void clickedPreset(int buttonNumber);
    void logMessage( QString s );


private slots:
    void LogTimerTimer( );

    void onStdInRead(QString);

    void onLoggerSelectAntenna(QString);
    void onLoggerSetRotation(int direction, int angle);
    void checkEndStop();
    void checkMoving(int bearing);

    void rotatingTimer();
    void about();
    void saveTraceLogFlag();
    void overLapDisplayBox(overlapStat status);
    void currentAntennaSettingChanged(QString);
    void updateSelectAntennaBox();
    void setSelectAntennaBoxVisible(bool visible);
    void setAntennaNameLabelVisible(bool visible);

    void upDateAntenna();
    void refreshAntenna();
    void request_bearing();
    void rotateCW(bool);
    void rotateCCW(bool);
    void stopButton();
    void stop_rotation();
    void stopRotation(bool);
    void sendStatusToLogConnected();
    void sendStatusToLogRotCCW();
    void sendStatusToLogRotCW();
    void sendStatusToLogStop();
    void sendStatusToLogTurn();
    void sendStatusToLogDisConnected();
    void sendStatusToLogError();


    void aboutRotatorConfig();

    void onSelectAntennaBox();
    void onLaunchSetup();
private:
    void rotateTo(int bearing);
    int northCalcTarget(int targetBearing);

    void readTraceLogFlag();


    void toggleOverLapDisplay(bool toggle);

//    bool getCwCcwCmdFlag(int rotatorNumber);
    void rot_right_button_off();
    void rot_right_button_on();
    void rot_left_button_off();
    void rot_left_button_on();
    void turn_button_on();
    void turn_button_off();
    void stop_button_on();
    void stop_button_off();
    void dispRawRotBearing(int);
    int calcRotZero360(int targetBearing);
    int calcRotNeg180_180(int targetBearing);
    int calclRot_0_450_Neg180_540(int targetBearing);
    void dumpRotatorToTraceLog();
    void writeWindowTitle(QString appName);

    void sendPresetListLogger();
    void cwCCWControlVisible(bool visible);
};

#endif // ROTATORMAINWINDOW_H
