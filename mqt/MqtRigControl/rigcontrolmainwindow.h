/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2007
//
// Hamlib Library
//
/////////////////////////////////////////////////////////////////////////////



#ifndef RIGCONTROLMAINWINDOW_H
#define RIGCONTROLMAINWINDOW_H

#include <QMainWindow>
#include "mqtUtils_pch.h"
#include "rigcontrol.h"

class QLabel;
class QComboBox;
class SetupDialog;
class RigControl;
class RigControlRpc;

#define NUM_VFO 2


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
    QLabel *status;
    QTimer *pollTimer;
    int pollTime;
    QString sfreq;
//    double oldFreq = 0;
    double curFreq = 0;
    vfo_t curVFO;
    freq_t rfrequency;       // read frequency
    rmode_t rmode;          // read radio mode
    pbwidth_t rwidth;        // read radio rx bw
    vfo_t rvfo;              // read vfo
    int strength;           // S-Meter level
    int retcode;            // generic return code from functions

    double curVfoFrq[NUM_VFO];
    rmode_t curMode[NUM_VFO];
    pbwidth_t curRxWidth[NUM_VFO];



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
    void displayFreqVfoA(double);
    void displayFreqVfoB(double);
    void displayModeVfoA(QString);
    void displayModeVfoB(QString);



    void logMessage(QString s);
    void readTraceLogFlag();
    void saveTraceLogFlag();
    void about();
private slots:

    void onStdInRead(QString);

    void upDateRadio();
    void getCurFreq();
    void getCurMode();


//private slots:
//    void updateFreq(double frequency);

signals:


};

#endif // RIGCONTROLMAINWINDOW_H
