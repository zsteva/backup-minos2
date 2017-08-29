/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2017
//
// Hamlib Library
//
/////////////////////////////////////////////////////////////////////////////



#ifndef RIGCONTROLMAINWINDOW_H
#define RIGCONTROLMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "mqtUtils_pch.h"
#include "rigcontrol.h"

class QLabel;
class QComboBox;
class SetupDialog;
class RigControl;
class RigControlRpc;






namespace Ui {
class RigControlMainWindow;
}

class RigControlMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RigControlMainWindow(QString, QWidget *parent = 0);
    ~RigControlMainWindow();


private:

    Ui::RigControlMainWindow *ui;
    StdInReader stdinReader;
    RigControlRpc *msg;


    QComboBox *selectRadio;
    SetupDialog *selectRig;
    RigControl  *radio;
    QString loggerRadio = "";
    QLabel *status;
    QTimer *pollTimer;
    class QTimer LogTimer;
    int pollTime;
    QString sfreq;
//    double oldFreq = 0;
    double curFreq = 0;
    vfo_t curVFO;
    freq_t rfrequency;       // read frequency
    rmode_t rmode;          // read radio mode

    int bandWidthState;
    pbwidth_t rwidth;        // read radio rx bw
    pbwidth_t loggerPbWidth;      // passband from logger
    bool useLogWidth;
    vfo_t rvfo;              // read vfo
    int strength;           // S-Meter level
    int retcode;            // generic return code from functions
    bool logRadError;
    double curVfoFrq[NUM_VFO];
    double curTransVertFrq[NUM_VFO];
    rmode_t curMode[NUM_VFO];
    pbwidth_t curRxWidth[NUM_VFO];

    QString geoStr;         // geometry registry location

    void initActionsConnections();
    void initSelectRadioBox();
    void openRadio();
    void closeRadio();
    void getFrequency(vfo_t vfo);
    void getMode(vfo_t vfo);
    QString convertStringFreq(double);

    void setPolltime(int);
    int getPolltime();
    void showStatusMessage(const QString &);
    void hamlibError(int errorCode);
//    void frequency_updated(double frequency);
//    void mode_updated(QString);
    void setFreq(QString, vfo_t vfo);
    void displayFreqVfo(double);

    void displayModeVfo(QString);

    void displayTransVertVfo(double frequency);




    void readTraceLogFlag();

    void about();
    void closeEvent(QCloseEvent *event);
    void LogTimerTimer(  );
    void sendStatusLogger(const QString &message);
    void sendStatusToLogDisConnected();
    void sendStatusToLogReady();
    void sendStatusToLogError();
    void sendFreqToLog(freq_t freq);
    void sendModeToLog(QString mode);

    void setCurMode(QString mode);
    void setMode(QString mode, vfo_t vfo);
    void displayPassband(pbwidth_t width);
private slots:

    void onStdInRead(QString);
    void saveTraceLogFlag();
    void upDateRadio();
    void getCurMode();
    void getRadioInfo();
    void logMessage(QString s);

//    void updateFreq(double frequency);


    void loggerSetFreq(QString freq);
    void loggerSetMode(QString mode);
    void loggerSetPassBand(int);
signals:


};

#endif // RIGCONTROLMAINWINDOW_H
