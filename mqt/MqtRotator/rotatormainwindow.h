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
#define NUM_ROTATOR_PROTOCOLS 2

class QLabel;
class QComboBox;
class SetupDialog;
class MinosCompass;
//class Yaesu;
class RotControl;
class EditPresetsDialog;
class RotatorRpc;
class RotatorLog;

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
    void sendBackBearing(QString);
    void presetRotateTo();

private:
    Ui::RotatorMainWindow *ui;


    RotatorRpc *msg;

    QTimer LogTimer;

    QComboBox *selectAntenna;
    QPushButton* presetButtons[NUM_PRESETS];
    MinosCompass* compassDial;
    RotControl  *rotator;
    QLabel *status;
    SetupDialog *selectRotator;
    EditPresetsDialog *editPresets;
    RotatorLog *rotlog;
    bool rotLogFlg;
    QTimer *timer;
    int pollTime;
    int brakedelay;
    bool brakeflag;
    bool moving;
    bool cwCcwflag;     // command sent
    bool stopCmdflag;
    bool rotCmdflag;
    int bearing;
    int min_azimuth;
    int max_azimuth;
    QString backBearingmsg;
    QString presetName[NUM_PRESETS];
    QString presetBearing[NUM_PRESETS];

    void openRotator();
    void closeRotator();

    void refreshPresetLabels();
    void showStatusMessage(const QString &);
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
    void displayBackBearing(const QString);
    void updatePresetLabels();
    void clickedPreset(int buttonNumber);

protected slots:
    void upDateAntenna();
    void request_bearing();
    void rotateCW(bool toogle);
    void rotateCCW(bool toogle);
    void stopButton();
    void stopRotation();

private slots:
    void LogTimerTimer( );
    void logBearing(const QString bearing);
    void sendBearingLogger(const QString bearing);
    void onLoggerSetRotation(int direction, int angle);
//    void on_pushButton_2_clicked();
//   void on_pushButton_clicked();


};

#endif // ROTATORMAINWINDOW_H
