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
#include <QMainWindow>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QKeyEvent>
#include <QPushButton>



#define NUM_PRESETS 10
//#define NUM_ROTATOR_PROTOCOLS 2


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



class RotatorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RotatorMainWindow(QWidget *parent = 0);
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
    void displayOverlap(bool);
    void checkingEndStop();
    void sendBackBearing(QString);
    void displayOverlapBearing(QString);
    void presetRotateTo();

private:
    Ui::RotatorMainWindow *ui;


    RotatorRpc *msg;

    QTimer LogTimer;

    QComboBox *selectAntenna;
    QPushButton* presetButtons[NUM_PRESETS];
//    MinosCompass* compassDial;
    RotControl  *rotator;
    QLabel *status;
    SetupDialog *selectRotator;
    EditPresetsDialog *editPresets;
    LogDialog *setupLog;
    RotatorLog *rotlog;
    bool rotLogFlg;
    QTimer *pollTimer;
    int pollTime;
    int brakedelay;
    bool brakeflag;
    bool moving;
    bool movingCW;
    bool movingCCW;
    bool cwCcwCmdflag;     // command sentflag
    bool stopCmdflag;
    bool rotCmdflag;
    bool reqBearCmdflag;
    bool overLapOnflag;
    bool overLapActiveflag;
    bool southStopActiveflag;
    int currentBearing;
    int currentMinAzimuth;
    int currentMaxAzimuth;
    QString backBearingmsg;
    QString presetName[NUM_PRESETS];
    QString presetBearing[NUM_PRESETS];

    QStringList presetShortCut = {"Ctrl+1","Ctrl+2",
                                "Ctrl+3", "Ctrl+4",
                                "Ctrl+5", "Ctrl+6",
                                "Ctrl+7", "Ctrl+8",
                                "Ctrl+9", "Ctrl+0"};


    void openRotator();
    void closeRotator();

    void refreshPresetLabels();
    void showStatusMessage(const QString &);
    void sendStatusLogger(const QString &);
    void readPresets();


    void initSelectAntennaBox();

    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *);

    void hamlibError(int errorCode);

    void logMessage( QString s );

    void sleepFor(qint64 milliseconds);

    void rotatorActive();


public slots:
    void rotateToController();
    void displayBearing(int);
    void updatePresetLabels();
    void clickedPreset(int buttonNumber);

protected slots:
    void upDateAntenna();
    void request_bearing();
    void rotateCW(bool toogle);
    void rotateCCW(bool toogle);
    void stopButton();
    void stopRotation();
    void sendStatusToLogReady();
    void sendStatusToLogRotCCW();
    void sendStatusToLogRotCW();
    void sendStatusToLogStop();
    void sendStatusToLogTurn();
    void sendStatusToLogDisConnected();
    void sendStatusToLogError();



private slots:
    void LogTimerTimer( );


    void onLoggerSetRotation(int direction, int angle);
    void checkEndStop();
    void checkMoving(int bearing);

private:
    void rotateTo(int bearing);

};

#endif // ROTATORMAINWINDOW_H
