/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2017
//
//
//
/////////////////////////////////////////////////////////////////////////////




#ifndef ROTCONTROLFRAME_H
#define ROTCONTROLFRAME_H

#include "logger_pch.h"
#include <QShortcut>

namespace Ui {
class RotControlFrame;
}

class RotControlFrame : public QFrame
{
    Q_OBJECT

    LoggerContestLog *ct;
public:
    explicit RotControlFrame(QWidget *parent);
    ~RotControlFrame();

    Ui::RotControlFrame *ui;

    void setContest(BaseContestLog *);

    void setRotatorLoaded();

    void setRotatorState(const QString &s);
    void setRotatorBearing(const QString &s);
    void setRotatorAntennaName(const QString &s);
    void setRotatorMaxAzimuth(const QString &s);
    void setRotatorMinAzimuth(const QString &s);
    int getAngle(QString);

    QString getBrgTxtFrmFrame();
    void setBrgFromRigFrmMemory(QString Brg);
private:

    QShortcut *nudgeRight1;
    QShortcut *nudgeRight2;
    QShortcut *nudgeLeft;
    QPalette *redText;
    QPalette *blackText;

    int maxAzimuth = 0;
    int minAzimuth = 0;
    int currentBearing = 0;
    int rotatorBearing = 0;
    bool moving = false;
    bool movingCW = false;
    bool movingCCW = false;
    bool rot_left_button_status;
    bool rot_right_button_status;
    bool rotConnected = false;
    bool rotError = false;
    void clearRotatorFlags();

    bool rotatorLoaded;
    bool isRotatorLoaded();

    void rot_left_button_on();
    void rot_left_button_off();
    void rot_right_button_on();
    void rot_right_button_off();

    void turnTo(int angle);

    void showRotLeftButOn();
    void showRotLeftButOff();
    void showRotRightButOn();
    void showRotRightButOff();
    void traceMsg(QString msg);
    void showTurnButOn();
    void showTurnButOff();
signals:
    void sendRotator(rpcConstants::RotateDirection direction, int angle );



private slots:

    void on_Rotate_clicked();
    void on_RotateLeft_clicked(bool);
    void on_RotateRight_clicked(bool);
    void on_StopRotate_clicked();
    void getBrgFrmQSOLog(QString);

    void on_nudgeLeft_clicked();
    void on_nudgeRight_clicked();
};

#endif // ROTCONTROLFRAME_H
