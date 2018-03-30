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
#include "tsinglelogframe.h"
#include "rotcontrolframe.h"
#include "qsologframe.h"
#include "ui_rotcontrolframe.h"
#include "SendRPCDM.h"
#include "rotpresetdialog.h"
#include "rotatorcommon.h"


RotControlFrame::RotControlFrame(QWidget *parent):
    QFrame(parent)
    , ct(nullptr)
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

    connect(this, SIGNAL(bearingEditReturn()), this, SLOT(on_Rotate_clicked()));

    connect(&MinosLoggerEvents::mle, SIGNAL(BrgStrToRot(QString)), this, SLOT(getBrgFrmQSOLog(QString)));

    initPresetButtons();


    rot_left_button_off();
    rot_right_button_off();
    showTurnButOff();
    traceMsg("RotControlFrame Started");

}



RotControlFrame::~RotControlFrame()
{
    delete ui;

}

void RotControlFrame::setContest(BaseContestLog *c)
{
    ct = dynamic_cast<LoggerContestLog *>( c);
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
    QString brg = ui->BrgSt->text();
    traceMsg(QString("Bearing from Bearing Edit Box = %1").arg(brg));
    return brg;
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
        traceMsg(QString("TurnTo: Bearing empty or invalid"));
        QString msg = "<font color='Red'>Bearing empty or invalid</font>";
        ui->rotatorStatMsg->setText(msg);
        return;
    }


    //if (angle > maxAzimuth)
    if (angle > COMPASS_MAX360)
    {
        traceMsg(QString("TurnTo: Bearing too large"));
        QString msg = "<font color='Red'>Bearing too large - " + QString::number(angle) + "</font>";
        ui->rotatorStatMsg->setText(msg);
        return;
    }
    //else if (angle < minAzimuth)
    else if (angle < COMPASS_MIN0)
    {
        traceMsg(QString("TurnTo: Bearing too small"));
        QString msg = "<font color='Red'>Bearing too small - " + QString::number(angle) + "</font>";
        ui->rotatorStatMsg->setText(msg);
        return;
    }
    else if (angle == currentBearing)
    {
        traceMsg(QString("TurnTo: Bearing = CurrentBearing"));
        return;
    }
    else
    {
        traceMsg(QString("Send Bearing %1 to Rotator Control").arg(QString::number(angle)));
        emit sendRotator(rpcConstants::eRotateDirect, angle);
        showTurnButOn();
        moving = true;
    }
}

void RotControlFrame::on_Rotate_clicked()
{
    if (rotConnected && !rotError)
    {
        traceMsg("Turn to button Clicked");
        QString brgSt = ui->BrgSt->text();

        int angle = getAngle(brgSt);

        turnTo(angle);
        ui->BrgSt->selectAll();
    }
    else
    {
        traceMsg(QString("On Rotate:Rotconnected = %1, RotError = %2").arg(rotConnected).arg(rotError));
    }


}

void RotControlFrame::on_nudgeLeft_clicked()
{
    if (rotConnected && !rotError)
    {
        traceMsg("Nudge Left Clicked");

        int newBearing = currentBearing - 3;
        if (newBearing < 0)
            newBearing += 360;
        turnTo(newBearing);
    }
    else
    {
        traceMsg(QString("NudgeLeft:Rotconnected = %1, RotError = %2").arg(rotConnected).arg(rotError));
    }
}

void RotControlFrame::on_nudgeRight_clicked()
{

    if (rotConnected && !rotError)
    {
        traceMsg("Nudge Right Clicked");
        int newBearing = currentBearing + 3;
        if (newBearing >= 360)
            newBearing -= 360;
        turnTo(newBearing);
    }
    else
    {
        traceMsg(QString("NudgeRight:Rotconnected = %1, RotError = %2").arg(rotConnected).arg(rotError));
    }

}

void RotControlFrame::on_RotateLeft_clicked(bool /*clicked*/)
{

    if (!rotConnected || rotError)
    {
        traceMsg(QString("On Rotate Left:Rotconnected = %1, RotError = %2").arg(rotConnected).arg(rotError));
        return;
    }

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
        traceMsg("Rotator Bearing = " + QString::number(rotatorBearing));
        traceMsg("RotLeft Status = " + QString::number(rot_left_button_status));
        int angle = 0;

        if (rotatorBearing <= minAzimuth)
        {
            traceMsg(QString("Current Bearing = %1 <= minAzimuth %2").arg(QString::number(rotatorBearing)).arg( QString::number(minAzimuth)));
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

    if (!rotConnected || rotError)
    {
        traceMsg(QString("On Rotate Right:Rotconnected = %1, RotError = %2").arg(rotConnected).arg(rotError));
        return;
    }


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
        traceMsg("Rotator Bearing = " + QString::number(rotatorBearing));
        traceMsg("RotRight Status = " + QString::number(rot_right_button_status));
        int angle = 0;

        if (rotatorBearing >= maxAzimuth)
        {
            traceMsg(QString("Current Bearing = %1 >= maxAzimuth %2").arg(QString::number(currentBearing)).arg(QString::number(maxAzimuth)));
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


void RotControlFrame::keyPressEvent(QKeyEvent *event)
{

    int Key = event->key();

/*
    Qt::KeyboardModifiers mods = event->modifiers();
    bool shift = mods & Qt::ShiftModifier;
    bool ctrl = mods & Qt::ControlModifier;
    bool alt = mods & Qt::AltModifier;
*/

    if (Key == Qt::Key_Return && ui->BrgSt->hasFocus())
    {
        emit bearingEditReturn();
    }
    else
    {
        QFrame::keyPressEvent(event);
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
    //ui->Rotate->setPalette(*redText);
    ui->Rotate->setStyleSheet(BUTTON_ON_STYLE);
    ui->Rotate->setText("Turn");
}

void RotControlFrame::showTurnButOff()
{
    //ui->Rotate->setPalette(*blackText);
    ui->Rotate->setStyleSheet(BUTTON_OFF_STYLE);
    ui->Rotate->setText("Turn");
}




void RotControlFrame::showRotLeftButOn()
{
    //ui->RotateLeft->setPalette(*redText);
    ui->RotateLeft->setStyleSheet(BUTTON_ON_STYLE);
    ui->RotateLeft->setText("(CCW) Left");
}

void RotControlFrame::showRotLeftButOff()
{
    //ui->RotateLeft->setPalette(*blackText);
    ui->RotateLeft->setStyleSheet(BUTTON_OFF_STYLE);
    ui->RotateLeft->setText("(CCW) Left");
}

void RotControlFrame::showRotRightButOn()
{
    //ui->RotateRight->setPalette(*redText);
    ui->RotateRight->setStyleSheet(BUTTON_ON_STYLE);
    ui->RotateRight->setText("(CW) Right");
}

void RotControlFrame::showRotRightButOff()
{
    //ui->RotateRight->setPalette(*blackText);
    ui->RotateRight->setStyleSheet(BUTTON_OFF_STYLE);
    ui->RotateRight->setText("(CW) Right");
}

void RotControlFrame::on_StopRotate_clicked()
{
    emit sendRotator(rpcConstants::eRotateStop, 0);
    clearRotatorFlags();
    showTurnButOff();
    showRotLeftButOff();
    showRotRightButOff();
}

void RotControlFrame::clearRotatorFlags()
{
    rot_left_button_off();
    rot_right_button_off();
    moving = false;
    movingCCW = false;
    movingCW = false;
    showTurnButOff();
    showRotLeftButOff();
    showRotRightButOff();
}

void RotControlFrame::setRotatorLoaded()
{
    rotatorLoaded = true;
}
bool RotControlFrame::isRotatorLoaded()
{
    return rotatorLoaded;
}
void RotControlFrame::setRotatorList(QString s)
{
    QStringList rots = s.split(":");
    ui->antennaName->clear();
    ui->antennaName->addItem("");
    ui->antennaName->addItems(rots);

    if (ct && ct == TContestApp::getContestApp() ->getCurrentContest())
        setRotatorAntennaName(ct->rotatorName.getValue().toString());
        //ui->antennaName->setCurrentText(ct->rotatorName.getValue());

}
void RotControlFrame::setRotatorPresetList(QString s)
{
    if (!s.isEmpty() && s.contains(':'))
    {

        rotPresets.clear();

        QStringList presets = s.split(':');
        for (int i = 0; i < presets.count(); i++)
        {
            QStringList p = presets[i].split(',');
            rotPresets.append(new RotPresetData(p[0].toInt(), p[1], p[2]));
        }

    }

}

void RotControlFrame::setRotatorState(const QString &s)
{
       traceMsg("Set Rotator State = " + s);
       // split the message

       QStringList sl = s.split(':');

       if (sl.count() < 3)
       {
           if (sl.count() > 0 && sl[0] != lastConnectStat)
           {
               lastConnectStat = sl[0];
               if (lastConnectStat == ROT_STATUS_CONNECTED)
               {
                   ui->rotConnectState->setText(lastConnectStat);
                   rotError = false;
                   rotConnected = true;
               }
               else if (lastConnectStat == ROT_STATUS_DISCONNECTED)
               {
                   ui->rotConnectState->setText(lastConnectStat);
                   rotError = false;
                   rotConnected = false;
               }
           }
           if (sl.count() > 1 && sl[1] != lastStatus)
           {
               lastStatus = sl[1];

               if (lastStatus == ROT_STATUS_STOP)
               {
                   ui->rotatorStatMsg->setText(lastStatus);
                   rotError = false;
                   clearRotatorFlags();
                   showRotLeftButOff();
                   showRotRightButOff();
                   showTurnButOff();
               }
               else if (lastStatus == ROT_STATUS_ROTATE_CCW)
               {
                   ui->rotatorStatMsg->setText(lastStatus);
                   rotError = false;
                   moving = false;
                   movingCW = false;
                   movingCCW = true;
                  // clearRotatorFlags();
                   showRotLeftButOn();
               }
               else if (lastStatus == ROT_STATUS_ROTATE_CW)
               {
                   ui->rotatorStatMsg->setText(lastStatus);
                   rotError = false;
                   moving = false;
                   movingCW = true;
                   movingCCW = false;
                   //clearRotatorFlags();
                   showRotRightButOn();
               }
               else if (lastStatus == ROT_STATUS_TURN_TO)
               {
                   ui->rotatorStatMsg->setText(lastStatus);
                   rotError = false;
                   moving = true;
                   movingCW = false;
                   movingCCW = false;
                   showTurnButOn();
                   //clearRotatorFlags();

               }
               else if (lastStatus == ROT_STATUS_CONNECTED)
               {
                   ui->rotConnectState->setText(lastStatus);
                   rotError = false;
                   rotConnected = true;
               }
               else if (lastStatus == ROT_STATUS_DISCONNECTED)
               {
                   ui->rotConnectState->setText(lastStatus);
                   rotError = false;
                   rotConnected = false;
               }
               else if (lastStatus == ROT_STATUS_ERROR)
               {
                   ui->rotatorStatMsg->setText(lastStatus);
                   rotError = true;
               }
               else
               {
                   ui->rotatorStatMsg->setText(lastStatus);
               }

           }
       }
       if (sl.count() <= 1)     // will be a revoked state
       {
           ui->rotConnectState->setText("Disconnected");
           rotError = false;
           rotConnected = false;
       }
}

void RotControlFrame::setRotatorAntennaName(const QString &s)
{
   traceMsg("Set Antenna Name = " + s);
   int index = ui->antennaName->findText(s, Qt::MatchFixedString);
   if (index >= 0)
       ui->antennaName->setCurrentIndex(index);
   else
       ui->antennaName->setCurrentText(s);

   antennaName = ui->antennaName->currentText();
   if (ct && !ct->isProtected() && ct == TContestApp::getContestApp() ->getCurrentContest())
   {
        emit selectRotator(s);
   }
}
void RotControlFrame::on_ContestPageChanged()
{
    // send rotator select to rotator app

    emit selectRotator(ct->rotatorName.getValue().toString());
}

void RotControlFrame::setRotatorBearing(const QString &s)
{
    traceMsg("Bearings from rotator control");
    // extract displayBearing:rotatorBearing:overlapstatus
    QStringList sl = s.split(':');
    if (sl.size() < 3)
        return;

    traceMsg("Display Bearing = " + sl[0]);
    traceMsg("Rotator Bearing = " + sl[1]);
    traceMsg("OverlapStatus = " + sl[2]);

    // save rotatorBearing
    bool ok;
    rotatorBearing = sl[1].toInt(&ok, 10);

    if (!ok)
    {
        trace("Error converting rotatorBearing to int");
        return;
    }

    int iBearing = sl[0].toInt(&ok, 10);
    currentBearing = iBearing;

    if (!ok)
    {
        trace("Error converting displayBearing to int");
        return;
    }


    //QString bearing = bearing.number(iBearing);
    QString brg;
    QChar degsym = QChar(DEGREE_SYMBOL);
    //int len = bearing.length();
    int len = sl[0].length();
    if (len < 2)
    {
        brg = QString("%1%2%3")
        .arg("00").arg(sl[0]).arg(degsym);
    }
    else if (len < 3)
    {
        brg = QString("%1%2%3")
        .arg("0").arg(sl[0]).arg(degsym);
    }
    else
    {
        brg = QString("%1%2")
        .arg(sl[0]).arg(degsym);
    }

    brg.append("</font>");

    if (rotatorBearing > COMPASS_MAX360 && sl[2] == "1")
    {
        brg.prepend("<font color='Red'>");
        ui->RotBrg->setText(brg);
    }
    else if (rotatorBearing < COMPASS_MIN0 && sl[2] == "1")
    {
        brg.prepend("<font color='Blue'>");
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
    trace(QString("RotcontrolFrame: %1 - %2").arg(antennaName).arg( msg));
}

void RotControlFrame::on_antennaName_activated(const QString &arg1)
{

    antennaName = arg1;

    emit selectRotator(antennaName);

}
void RotControlFrame::getRotDetails(memoryData::memData &m)
{
    m.bearing = currentBearing;
}



/**************************** Quick Preset Buttons **************************/


void RotControlFrame::initPresetButtons()
{
    presetButMap[0] = new PresetButton(ui->presetButton0, this, 0);
    connect( presetButMap[0], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );
    presetButMap[1] = new PresetButton(ui->presetButton1, this, 1);
    connect( presetButMap[1], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );
    presetButMap[2] = new PresetButton(ui->presetButton2, this, 2);
    connect( presetButMap[2], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );
    presetButMap[3] = new PresetButton(ui->presetButton3, this, 3);
    connect( presetButMap[3], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );
    presetButMap[4] = new PresetButton(ui->presetButton4, this, 4);
    connect( presetButMap[4], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );
    presetButMap[5] = new PresetButton(ui->presetButton5, this, 5);
    connect( presetButMap[5], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );
    presetButMap[6] = new PresetButton(ui->presetButton6, this, 6);
    connect( presetButMap[6], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );
    presetButMap[7] = new PresetButton(ui->presetButton7, this, 7);
    connect( presetButMap[7], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );
    presetButMap[8] = new PresetButton(ui->presetButton8, this, 8);
    connect( presetButMap[8], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );
    presetButMap[9] = new PresetButton(ui->presetButton9, this, 9);
    connect( presetButMap[9], SIGNAL( clearActionSelected(int)) , this, SLOT(presetButClearActSel(int)), Qt::QueuedConnection );

}

void RotControlFrame::presetButReadActSel(int buttonNumber)
{



}

void RotControlFrame::presetButEditActSel(int buttonNumber)
{
    if (!rotPresets.isEmpty())
    {
        RotPresetData editData(0, "", "0");
        RotPresetData curData(0, "", "0");

        curData.name = rotPresets[buttonNumber]->name;
        curData.bearing = rotPresets[buttonNumber]->bearing;

        traceMsg(QString("RotFrame: Preset Edit Selected = %1").arg(QString::number(buttonNumber + 1)));
        RotPresetDialog presetDialog(this);
//        presetDialog.setPresetDialogData(editData, curData); ******************************************
        presetDialog.setWindowTitle(QString("Rotator Preset %1 - Edit").arg(QString::number(buttonNumber + 1)));

        if (presetDialog.exec() == QDialog::Accepted)
        {
            setRotPresetButData(buttonNumber, editData);
            rotPresetButtonUpdate(buttonNumber, editData);
            saveRotPresetButton(editData);
        }
    }


}

void RotControlFrame::presetButClearActSel(int buttonNumber)
{

}

void RotControlFrame::presetButtonUpdate(int buttonNumber)
{

}

void RotControlFrame::presetButWriteActSel(int buttonNumber)
{

}


void RotControlFrame::setRotPresetButData(int buttonNumber, RotPresetData& editData)
{
    rotPresets[buttonNumber]->name = editData.name;
    rotPresets[buttonNumber]->bearing = editData.bearing;
    saveRotPresetButton(editData);
}


void RotControlFrame::rotPresetButtonUpdate(int buttonNumber, RotPresetData& editData)
{
    presetButMap[buttonNumber]->presetButton->setText(QString("%1: %2").arg(QString::number(buttonNumber)).arg(editData.name) );
    QString tTipStr = "Bearing = " + editData.bearing;
    presetButMap[buttonNumber]->presetButton->setToolTip(tTipStr);
}

void RotControlFrame::saveRotPresetButton(RotPresetData& editData)
{
    QString msg;
    msg = QString("%1:%2:%3").arg(QString::number(editData.number)).arg(editData.name).arg(editData.bearing);
    emit sendPresetButton(msg);
}

PresetButton::PresetButton(QToolButton *b, RotControlFrame *rcf, int num)
{
    presetNo = num;
    rotControlFrame = rcf;

    presetButton = b;

    presetMenu = new QMenu(presetButton);

    presetButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    presetButton->setPopupMode(QToolButton::MenuButtonPopup);
    presetButton->setFocusPolicy(Qt::NoFocus);
    presetButton->setText(QString::number(num+1));

    //shortKey = new QShortcut(QKeySequence(runButShortCut[memNo]), memButton);
    //shiftShortKey = new QShortcut(QKeySequence(runButShiftShortCut[memNo]), memButton);
    readAction = new QAction("&Read", presetButton);
    writeAction = new QAction("&Write",presetButton);
    editAction = new QAction("&Edit", presetButton);
    clearAction = new QAction("&Clear",presetButton);
    presetMenu->addAction(readAction);
    presetMenu->addAction(writeAction);
    presetMenu->addAction(editAction);
    presetMenu->addAction(clearAction);
    presetButton->setMenu(presetMenu);

    //connect(shortKey, SIGNAL(activated()), this, SLOT(readActionSelected()));
    //connect(shiftShortKey, SIGNAL(activated()), this, SLOT(memoryShortCutSelected()));
    connect(presetButton, SIGNAL(clicked(bool)), this, SLOT(readActionSelected()));
    connect( readAction, SIGNAL( triggered() ), this, SLOT(readActionSelected()));
    connect( writeAction, SIGNAL( triggered() ), this, SLOT(writeActionSelected()));
    connect( editAction, SIGNAL( triggered() ), this, SLOT(editActionSelected()));
    connect( clearAction, SIGNAL( triggered() ), this, SLOT(clearActionSelected()));
}
PresetButton::~PresetButton()
{
//    delete memButton;
}
void PresetButton::presetUpdate()
{
    rotControlFrame->presetButtonUpdate(presetNo);
}

void PresetButton::presetShortCutSelected()
{
//    rigControlFrame->memoryShortCutSelected(memNo);
    presetButton->showMenu();
    //emit lostFocus();
}
void PresetButton::readActionSelected()
{
    rotControlFrame->presetButReadActSel(presetNo);
}
void PresetButton::editActionSelected()
{
    rotControlFrame->presetButEditActSel(presetNo);
}
void PresetButton::writeActionSelected()
{
    rotControlFrame->presetButWriteActSel(presetNo);
}
void PresetButton::clearActionSelected()
{
    emit clearActionSelected(presetNo);
}


