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
    explicit RigControlMainWindow(QWidget *parent = 0);
    ~RigControlMainWindow();


private:

    Ui::RigControlMainWindow *ui;
    StdInReader stdinReader;
    RigControlRpc *msg;


    QComboBox *selectRadio;
    SetupDialog *selectRig;
    RigControl  *radio;
    QString appName = "";
    QLabel *status;
    QTimer *pollTimer;
    class QTimer LogTimer;
    int pollTime;
    bool rigErrorFlag;
    bool cmdLockFlag;
    // data from logger
    QString logger_freq;
    QString logger_mode;
    int logger_bw_state;
    rmode_t log_currMode;
    pbwidth_t loggerPbWidth;      // passband from logger


    // data from radio
    freq_t rfrequency;       // read frequency
    QString sfreq;          // read freq converted to string
    rmode_t rmode;          // read radio mode
    pbwidth_t rwidth;        // read radio rx bw
    double curVfoFrq;
    double curTransVertFrq;
    rmode_t curMode;
    bool mgmModeFlag;
    shortfreq_t rRitFreq;
    QString sRitFreq;
    bool supRitFlag;

    QString geoStr;         // geometry registry location

    void initActionsConnections();
    void initSelectRadioBox();
    void openRadio();
    void closeRadio();
    int getFrequency(vfo_t vfo);
    int getMode(vfo_t vfo);
    QString convertFreqString(double);

    void setPolltime(int);
    int getPolltime();
    void showStatusMessage(const QString &);
    void hamlibError(int errorCode, QString cmd);
//    void frequency_updated(double frequency);
//    void mode_updated(QString);
    void setFreq(QString, vfo_t vfo);
    void displayFreqVfo(double);

    void displayModeVfo(QString);

    void displayTransVertVfo(double frequency);




    void readTraceLogFlag();


    void closeEvent(QCloseEvent *event);

    void sendStatusLogger(const QString &message);
    void sendStatusToLogDisConnected();
    void sendStatusToLogConnected();
    void sendStatusToLogError();
    void sendFreqToLog(freq_t freq);
    void sendModeToLog(QString mode);
    void sendRxPbFlagToLog();

    void setCurMode(QString mode);
    void setMode(QString mode, vfo_t vfo);
    void displayPassband(pbwidth_t width);


    void chkRadioMgmModeChanged();
    void dumpRadioToTraceLog();
    void enableRitDisplay(bool state);
    int getRitFreq(vfo_t vfo);
    int setRitFreq(vfo_t vfo, shortfreq_t ritFreq);
    void cmdLockOn();
    void cmdLockOff();
private slots:

    void onStdInRead(QString);
    void saveTraceLogFlag();
    void upDateRadio();
    void getCurMode();
    void getRadioInfo();
    void logMessage(QString s);
    void about();
    void LogTimerTimer();



    void loggerSetFreq(QString freq);
    void loggerSetMode(QString mode);
    void loggerSetPassBand(int);
    void currentRadioSettingChanged(QString radioName);
    void updateSelectRadioBox();
signals:


};

#endif // RIGCONTROLMAINWINDOW_H
