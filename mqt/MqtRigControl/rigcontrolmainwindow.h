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
#include "BandList.h"

class QLabel;
class QComboBox;
class SetupDialog;
class RigControl;
class RigControlRpc;


void delay(int sec);



namespace Ui {
class RigControlMainWindow;
}

class RigControlMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RigControlMainWindow(QWidget *parent = nullptr);
    ~RigControlMainWindow();

    bool freqPresetChanged = false;


private:

    Ui::RigControlMainWindow *ui;
    StdInReader stdinReader;
    RigControlRpc *msg;

    SetupDialog *setupRadio;
    RigControl  *radio;
    QString appName = "";
    QLabel *status;
    int radioIndex;
    QTimer *pollTimer;
    class QTimer LogTimer;
    int pollTime;
    bool rigErrorFlag;
    bool cmdLockFlag;
    // data from logger
    QString logger_freq;
    QString slogMode;
    QString selRadioMode;   // onSelectRadio mode from logger at startup
    rmode_t logMode;
    QString selTvBand;      // selected band from radio
    QString transVertSwNum;

    const int PASSBAND_NOCHANGE = -1;

    QVector<BandDetail*> bands;
    QStringList presetFreq;

    // data from radio
    freq_t rfrequency;       // read frequency
    QString sfreq;          // read freq converted to string
    rmode_t rmode;          // read radio mode
    pbwidth_t rwidth;        // read radio rx bw
    double curVfoFrq;
    double curTransVertFrq;
    rmode_t curMode;
    QString sCurMode;
    bool mgmModeFlag;
    shortfreq_t rRitFreq;
    QString sRitFreq;
    bool supRitFlag;

    QString geoStr;         // geometry registry location

    void initActionsConnections();
    void initSelectRadioBox();
    void setSelectRadioBoxVisible(bool visible);
    void setRadioNameLabelVisible(bool visible);
    void openRadio();
    void closeRadio();
    int getAndSendFrequency(vfo_t vfo);
    int getAndSendMode(vfo_t vfo);
    //QString convertFreqString(double);

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

    void sendRadioListLogger();
    void sendBandListLogger();
    void sendStatusLogger(const QString &message);
    void sendStatusToLogDisConnected();
    void sendStatusToLogConnected();
    void sendStatusToLogError();
    void sendTransVertOffsetToLogger(int tvNum);
    void sendTransVertSwitchToLogger(const QString &swNum);
    void sendErrorMessageToLogger(QString errMsg);
    void sendFreqToLog(freq_t freq);
    void sendModeToLog(QString mode);
    //void sendRxPbFlagToLog();

    void setMode(QString mode, vfo_t vfo);
    void displayPassband(pbwidth_t width);


    void chkRadioMgmModeChanged();
    void dumpRadioToTraceLog();
    void enableRitDisplay(bool state);
    int getRitFreq(vfo_t vfo);
    int setRitFreq(vfo_t vfo, shortfreq_t ritFreq);
    void cmdLockOn();
    void cmdLockOff();
    int getMinosModeIndex(QString mode);


    void setTransVertDisplayVisible(bool visible);
    void writeWindowTitle(QString appName);
    void sendTransVertStatus(bool status);

    void refreshRadio();

    QString getBand(freq_t freq);

    void testBoxesVisible(bool visible);

    void upDateRadio();
    void loadBands();

private slots:

    void onStdInRead(QString);
    void saveTraceLogFlag();

    void getRadioInfo();
    void logMessage(QString s);
    void about();
    void LogTimerTimer();

    void loggerSetFreq(QString freq);
    void loggerSetMode(QString mode);
    void currentRadioSettingChanged(QString radioName);
    void updateSelectRadioBox();
    void aboutRigConfig();

    void onSelectRadio(QString, QString mode);
    void selFreqClicked();
    void setupBandFreq();
    void selectRadio();
    void onLaunchSetup();

signals:


};

#endif // RIGCONTROLMAINWINDOW_H
