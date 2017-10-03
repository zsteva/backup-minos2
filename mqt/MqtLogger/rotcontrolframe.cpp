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





#include "logger_pch.h"
#include "tlogcontainer.h"
#include "rotcontrolframe.h"
#include "qsologframe.h"
#include "ui_rotcontrolframe.h"
#include "SendRPCDM.h"
#include "rotatorCommonConstants.h"


RotControlFrame::RotControlFrame(QWidget *parent):
    QFrame(parent)
    , ui(new Ui::RotControlFrame)
    , rotatorLoaded(false)
{
    ui->setupUi(this);

    ui->BrgSt->clear();

    nudgeRight1 = new QShortcut(QKeySequence("Ctrl++"), parent);   // Ctrl +
    connect(nudgeRight1, SIGNAL(activated()), ui->nudgeRight, SLOT(click()));

    nudgeRight2 = new QShortcut(QKeySequence("Ctrl+="), parent);   // Ctrl +
    connect(nudgeRight2, SIGNAL(activated()), ui->nudgeRight, SLOT(click()));

    nudgeLeft = new QShortcut(QKeySequence::ZoomOut, parent);   // Ctrl -
    connect(nudgeLeft, SIGNAL(activated()), ui->nudgeLeft, SLOT(click()));

    ui->RotateLeft->setShortcut(QKeySequence(ROTATE_CCW_KEY));
    ui->RotateRight->setShortcut(QKeySequence(ROTATE_CW_KEY));
    ui->Rotate->setShortcut(QKeySequence(ROTATE_TURN_KEY));
    ui->StopRotate->setShortcut(QKeySequence(ROTATE_STOP_KEY));


    connect(&MinosLoggerEvents::mle, SIGNAL(BrgStrToRot(QString)), this, SLOT(getBrgFrmQSOLog(QString)));

    redText = new QPalette();
    blackText = new QPalette();
    redText->setColor(QPalette::ButtonText, Qt::red);
    blackText->setColor(QPalette::ButtonText, Qt::black);
    rot_left_button_off();
    rot_right_button_off();
    traceMsg("RotControlFrame Started");

}



RotControlFrame::~RotControlFrame()
{
    delete ui;

}





int RotControlFrame::getAngle(QString brgSt)
{
    int brg = COMPASS_ERROR;

    for (int i = 0;i < brgSt.length(); i++)
    {
        if (brgSt[i].isDigit())
        {
            int j = 0;
            for (j = i; j < brgSt.length(); j++)
            {
                if (!brgSt[j].isDigit())
                {
                    break;
                }
            }
            brgSt = brgSt.mid(i, j - i);
            brg = brgSt.toInt();
            return brg;
        }
    }

    return brg;
}


void RotControlFrame::getBrgFrmQSOLog(QString brg)
{
    traceMsg("Bearing from QSO Log" + brg);
    ui->BrgSt->setText(brg);
}


QString RotControlFrame::getBrgTxtFrmFrame()
{
    return ui->BrgSt->text();
}

void RotControlFrame::setBrgFromRigFrmMemory(QString brg)
{
    traceMsg("Set Bearing from memory " + brg);
    ui->BrgSt->setText(brg);
}

void RotControlFrame::turnTo(int angle)
{
    traceMsg("Turn to - " + QString::number(angle));
    if (angle == COMPASS_ERROR)
    {
        QString msg = "<font color='Red'>Bearing empty or invalid</font>";
        ui->rotatorState->setText(msg);
        return;
    }


    //if (angle > maxAzimuth)
    if (angle > COMPASS_MAX360)
    {
        QString msg = "<font color='Red'>Bearing too large - " + QString::number(angle) + "</font>";
        ui->rotatorState->setText(msg);
    }
    //else if (angle < minAzimuth)
    else if (angle < COMPASS_MIN0)
    {
        QString msg = "<font color='Red'>Bearing too small - " + QString::number(angle) + "</font>";
        ui->rotatorState->setText(msg);
    }
    else
    {
        emit sendRotator(rpcConstants::eRotateDirect, angle);
        showTurnButOn();
        moving = true;
    }
}

void RotControlFrame::on_Rotate_clicked()
{
    traceMsg("Turn to button Clicked");
    QString brgSt = ui->BrgSt->text();

    int angle = getAngle(brgSt);

    turnTo(angle);
    ui->BrgSt->selectAll();

}

void RotControlFrame::on_nudgeLeft_clicked()
{
    traceMsg("Nudge Left Clicked");
    turnTo(currentBearing - 3);
}

void RotControlFrame::on_nudgeRight_clicked()
{
    traceMsg("Nudge Right Clicked");
    turnTo(currentBearing + 3);
}

void RotControlFrame::on_RotateLeft_clicked(bool /*clicked*/)
{
    traceMsg("RotLeft Button Clicked");

    if (rot_left_button_status)
    {
        traceMsg("RotLeft Button On - Stop and Turn Off");
        on_StopRotate_clicked();
        rot_left_button_off();

    }
    else
    {
        traceMsg("Current Bearing = " + QString::number(currentBearing));
        traceMsg("RotLeft Status = " + QString::number(rot_left_button_status));
        int angle = 0;

        if (currentBearing <= minAzimuth)
        {
            traceMsg("Current Bearing = " + QString::number(currentBearing) + " <= minAzimuth" + QString::number(minAzimuth));
            return;
        }

        if (moving || movingCW || movingCCW)
        {
            traceMsg("RotLeft Stopping");
            on_StopRotate_clicked();
        }

        rot_left_button_on();
        traceMsg("Send RotLeft to Rototor Control");
        emit sendRotator(rpcConstants::eRotateLeft, angle);
        movingCW = true;
        }

}


void RotControlFrame::on_RotateRight_clicked(bool /*toggle*/)
{

    traceMsg("RotRight Button Clicked");

    if (rot_right_button_status)
    {
        traceMsg("RotRight Button On - Stop and Turn Off");
        on_StopRotate_clicked();
        rot_right_button_off();

    }
    else
    {
        traceMsg("Current Bearing = " + QString::number(currentBearing));
        traceMsg("RotRight Status = " + QString::number(rot_right_button_status));
        int angle = 0;

        if (currentBearing >= maxAzimuth)
        {
            traceMsg("Current Bearing = " + QString::number(currentBearing) + " >= maxAzimuth" + QString::number(maxAzimuth));
            return;
        }

        if (moving || movingCW || movingCCW)
        {
            traceMsg("RotRight Stopping");
            on_StopRotate_clicked();
        }

        rot_right_button_on();
        traceMsg("Send RotRight to Rotator Control");
        emit sendRotator(rpcConstants::eRotateRight, angle);
        movingCCW = true;
        }

}



void RotControlFrame::rot_left_button_on()
{
    rot_left_button_status = true;
    showRotLeftButOn();
}

void RotControlFrame::rot_left_button_off()
{
    rot_left_button_status = false;
    showRotLeftButOff();
}

void RotControlFrame::rot_right_button_on()
{
    rot_right_button_status = true;
    showRotRightButOn();
}

void RotControlFrame::rot_right_button_off()
{
    rot_right_button_status = false;
    showRotRightButOff();
}


void RotControlFrame::showTurnButOn()
{
    ui->Rotate->setPalette(*redText);
    ui->Rotate->setText("Turn");
}

void RotControlFrame::showTurnButOff()
{
    ui->Rotate->setPalette(*blackText);
    ui->Rotate->setText("Turn");
}




void RotControlFrame::showRotLeftButOn()
{
    ui->RotateLeft->setPalette(*redText);
    ui->RotateLeft->setText("(CCW) Left");
}

void RotControlFrame::showRotLeftButOff()
{
    ui->RotateLeft->setPalette(*blackText);
    ui->RotateLeft->setText("(CCW) Left");
}

void RotControlFrame::showRotRightButOn()
{
    ui->RotateRight->setPalette(*redText);
    ui->RotateRight->setText("(CW) Right");
}

void RotControlFrame::showRotRightButOff()
{
    ui->RotateRight->setPalette(*blackText);
    ui->RotateRight->setText("(CW) Right");
}

void RotControlFrame::on_StopRotate_clicked()
{
    emit sendRotator(rpcConstants::eRotateStop, 0);
    clearRotatorFlags();

}

void RotControlFrame::clearRotatorFlags()
{
    rot_left_button_off();
    rot_right_button_off();
    moving = false;
    movingCCW = false;
    movingCW = false;
}

void RotControlFrame::setRotatorLoaded()
{
    rotatorLoaded = true;
}
bool RotControlFrame::isRotatorLoaded()
{
    return rotatorLoaded;
}

void RotControlFrame::setRotatorState(const QString &s)
{
       traceMsg("Set Rotator State = " + s);
       ui->rotatorState->setText(s);
       if (s == ROT_STATUS_STOP)
       {
           clearRotatorFlags();
           showRotLeftButOff();
           showRotRightButOff();
           showTurnButOff();
       }
       else if (s == ROT_STATUS_ROTATE_CCW)
       {
           moving = false;
           movingCW = false;
           movingCCW = true;
          // clearRotatorFlags();
           showRotLeftButOn();
       }
       else if (s == ROT_STATUS_ROTATE_CW)
       {
           moving = false;
           movingCW = true;
           movingCCW = false;
           //clearRotatorFlags();
           showRotRightButOn();
       }
       else if (s == ROT_STATUS_TURN_TO)
       {
           moving = true;
           movingCW = false;
           movingCCW = false;
           showTurnButOn();
           //clearRotatorFlags();

       }

}

void RotControlFrame::setRotatorAntennaName(const QString &s)
{
   traceMsg("Set Antenna Name = " + s);
    ui->antennaName->setText(s);
}

void RotControlFrame::setRotatorBearing(const QString &s)
{
    traceMsg("Set Rotator Bearing = " + s);
    bool ok;
    bool overlap = false;
    int iBearing = s.toInt(&ok, 10);
    currentBearing = iBearing;
    if (!ok) return;

    if (iBearing > COMPASS_MAX360)
    {
        iBearing -= COMPASS_MAX360;
        overlap = true;
    }
    QString bearing = bearing.number(iBearing);
    QString brg;
    QChar degsym = QChar(DEGREE_SYMBOL);
    int len = bearing.length();

    if (len < 2)
    {
        brg = QString("%1%2%3")
        .arg("00").arg(bearing).arg(degsym);
    }
    else if (len < 3)
    {
        brg = QString("%1%2%3")
        .arg("0").arg(bearing).arg(degsym);
    }
    else
    {
        brg = QString("%1%2")
        .arg(bearing).arg(degsym);
    }

    brg.append("</font>");

    if (overlap)
    {
        brg.prepend("<font color='Red'>");
        ui->RotBrg->setText(brg);
    }
    else
    {
        brg.prepend("<font color='Black'>");
        ui->RotBrg->setText(brg);
    }

}

void RotControlFrame::setRotatorMaxAzimuth(const QString &s)
{
    traceMsg("Set MaxAzimuth = " + s);
    bool ok;
    int max_azimuth = 0;
    max_azimuth = s.toInt(&ok, 10);
    if (ok)
    {
        maxAzimuth = max_azimuth;
    }
}


void RotControlFrame::setRotatorMinAzimuth(const QString &s)
{
    traceMsg("Set MinAzimuth = " + s);
    bool ok;
    int min_azimuth = 0;
    min_azimuth = s.toInt(&ok, 10);
    if (ok)
    {
        minAzimuth = min_azimuth;
    }
}


void RotControlFrame::traceMsg(QString msg)
{
    trace("Rotcontrol: " + msg);
}
