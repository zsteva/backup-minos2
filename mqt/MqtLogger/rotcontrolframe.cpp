#include "logger_pch.h"
#include "tlogcontainer.h"
#include "rotcontrolframe.h"
#include "ui_rotcontrolframe.h"
#include "SendRPCDM.h"
#include "rotatorCommonConstants.h"


RotControlFrame::RotControlFrame(QWidget *parent):
    QFrame(parent)
    , ui(new Ui::RotControlFrame)
    , rotatorLoaded(false)
{
    ui->setupUi(this);

    // need to fix metrics ***************************************
    //ui->BrgSt->setFixedWidth(metrics.width("(8888)MT"));

    ui->BrgSt->clear();
    ui->DistSt->clear();

    ui->RotateLeft->setShortcut(QKeySequence(ROTATE_CCW_KEY));
    ui->RotateRight->setShortcut(QKeySequence(ROTATE_CW_KEY));
    ui->Rotate->setShortcut(QKeySequence(ROTATE_TURN_KEY));
    ui->StopRotate->setShortcut(QKeySequence(ROTATE_STOP_KEY));

    rot_left_button_off();
    rot_right_button_off();

}



RotControlFrame::~RotControlFrame()
{
    delete ui;

}





int RotControlFrame::getAngle()
{
    QString brgSt = ui->BrgSt->text();

    int brg = 0;

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

void RotControlFrame::on_Rotate_clicked()
{
    int angle = getAngle();

    if (angle == COMPASS_ERROR)
    {
        QString msg = "<font color='Red'>Bearing empty or invalid</font>";
        ui->rotatorState->setText(msg);
        return;
    }


    if (angle > maxAzimuth)
    {
        QString msg = "<font color='Red'>Bearing too large - " + QString::number(angle) + "</font>";
        ui->rotatorState->setText(msg);
    }
    else if (angle < minAzimuth)
    {
        QString msg = "<font color='Red'>Bearing too small - " + QString::number(angle) + "</font>";
        ui->rotatorState->setText(msg);
    }
    else
    {
        emit sendRotator(rpcConstants::eRotateDirect, angle);
        moving = true;
    }

}

void RotControlFrame::on_RotateLeft_clicked(bool clicked)
{
    int angle = 0;

    if (currentBearing <= minAzimuth)
    {
        return;
    }

    if (moving || movingCW || movingCCW)
    {
        on_StopRotate_clicked();
    }


    if (!rot_left_button_status)
    {
        rot_left_button_on();
        emit sendRotator(rpcConstants::eRotateLeft, angle);
        movingCW = true;
    }
    else
    {
        rot_left_button_off();
        //TSendDM::sendRotator(rpcConstants::eRotateStop, getAngle());
    }
}


void RotControlFrame::on_RotateRight_clicked(bool toggle)
{
    int angle = 0;

    if (currentBearing >= maxAzimuth)
    {
        return;
    }

    if (moving || movingCW || movingCCW)
    {
        on_StopRotate_clicked();
    }



    if (!rot_right_button_status)
    {
        rot_right_button_on();
        emit sendRotator(rpcConstants::eRotateRight, angle);
        movingCCW = true;
    }
    else
    {
        rot_right_button_off();
        //TSendDM::sendRotator(rpcConstants::eRotateStop, getAngle());
    }

}



void RotControlFrame::rot_left_button_on()
{
    rot_left_button_status = true;
    ui->RotateLeft->setText("<<--   (CCW) Left");
}

void RotControlFrame::rot_left_button_off()
{
    rot_left_button_status = false;
    ui->RotateLeft->setText("(CCW) Left");
}

void RotControlFrame::rot_right_button_on()
{
    rot_right_button_status = true;
    ui->RotateRight->setText("(CW) Right   -->>");
}

void RotControlFrame::rot_right_button_off()
{
    rot_right_button_status = false;
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
       ui->rotatorState->setText(s);
       if (s == ROT_STATUS_STOP)
       {
           clearRotatorFlags();
       }
       else if (s == ROT_STATUS_ROTATE_CCW)
       {
           moving = false;
           movingCW = false;
           movingCCW = true;
          // clearRotatorFlags();
           ui->RotateLeft->setChecked(true);
       }
       else if (s == ROT_STATUS_ROTATE_CW)
       {
           moving = false;
           movingCW = true;
           movingCCW = false;
           //clearRotatorFlags();
           ui->RotateRight->setChecked(true);
       }
       else if (s == ROT_STATUS_TURN_TO)
       {
           moving = true;
           movingCW = false;
           movingCCW = false;
           //clearRotatorFlags();

       }

}

void RotControlFrame::setRotatorAntennaName(const QString &s)
{
       ui->antennaName->setText(s);
}

void RotControlFrame::setRotatorBearing(const QString &s)
{
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
    bool ok;
    int min_azimuth = 0;
    min_azimuth = s.toInt(&ok, 10);
    if (ok)
    {
        minAzimuth = min_azimuth;
    }
}
