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







#include "base_pch.h"
#include "RPCCommandConstants.h"
#include "rotatorCommonConstants.h"
#include "rotatorRpc.h"
#include "rotatorlog.h"
#include "rotatormainwindow.h"
#include "ui_rotatormainwindow.h"
#include "minoscompass.h"
#include "rotcontrol.h"
#include "setupdialog.h"
#include "editpresetsdialog.h"
#include "logdialog.h"
#include <QString>
#include <QLabel>
#include <QMessageBox>
#include <QSignalMapper>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include <QtDebug>


RotatorMainWindow *MinosRotatorForm;


RotatorMainWindow::RotatorMainWindow(QWidget *parent) :
    QMainWindow(parent), msg(0),
    ui(new Ui::RotatorMainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    enableTrace( "./TraceLog", "MqtRotator_" );
    createCloseEvent();
    MinosRotatorForm = this;
    connect(&LogTimer, SIGNAL(timeout()), this, SLOT(LogTimerTimer()));
    LogTimer.start(100);
    msg = new RotatorRpc(this);

    QSettings settings;
    QByteArray geometry = settings.value("geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);


    ui->rot_left_button->setShortcut(QKeySequence(ROTATE_CCW_KEY));
    ui->rot_right_button->setShortcut(QKeySequence(ROTATE_CW_KEY));
    ui->turnButton->setShortcut(QKeySequence(ROTATE_TURN_KEY));
    ui->stopButton->setShortcut(QKeySequence(ROTATE_STOP_KEY));

    // disable some menus for now
    ui->actionHelp->setVisible(false);
    ui->actionSkyScan->setVisible(false);
    ui->actionAlways_On_Top->setVisible(false);

    presetButtons[0] = ui->presetButton1;
    presetButtons[1] = ui->presetButton2;
    presetButtons[2] = ui->presetButton3;
    presetButtons[3] = ui->presetButton4;
    presetButtons[4] = ui->presetButton5;
    presetButtons[5] = ui->presetButton6;
    presetButtons[6] = ui->presetButton7;
    presetButtons[7] = ui->presetButton8;
    presetButtons[8] = ui->presetButton9;
    presetButtons[9] = ui->presetButton10;
    QSignalMapper *preset_mapper = new QSignalMapper(this);

        for (int i = 0; i < NUM_PRESETS; i++ )
    {
        preset_mapper->setMapping(presetButtons[i], i);
        connect(presetButtons[i], SIGNAL(clicked()), preset_mapper, SLOT(map()));

    }
    connect(preset_mapper, SIGNAL(mapped(int)), this, SLOT(clickedPreset(int)));


    for (int i = 0; i < NUM_PRESETS; i++)
    {
        presetButtons[i]->setShortcut(presetShortCut[i]);
    }

    rotator = new RotControl();
    selectRotator = new SetupDialog(rotator);
    editPresets = new EditPresetsDialog;
    setupLog = new LogDialog;
    pollTimer = new QTimer(this);
    status = new QLabel;
    selectAntenna = new QComboBox;
    rotlog = new RotatorLog;

    ui->statusbar->addWidget(status);

    ui->overlaplineEdit->setFixedSize(60,20);

    rotator->set_serialConnected(false);

    refreshPresetLabels();
    initActionsConnections();

    selectAntenna = ui->selectAntennaBox;

    initSelectAntennaBox();

    selectRotator->readCurrentAntenna();

    selectAntenna->setCurrentIndex(selectAntenna->findText(selectRotator->currentAntenna.antennaName));



    brakedelay = 1 * 1000;

    brakeflag = false;
    cwCcwCmdflag = false;
    rotCmdflag = false;
    stopCmdflag = false;
    reqBearCmdflag  = false;
    overLapOnflag = false;
    moving = false;
    movingCW  = false;
    movingCCW = false;

    rotLogFlg = true;
    currentBearing = COMPASS_MIN0;




    openRotator();

    setPolltime(1000);   // to allow variable controller polltime - not implemented!
    rotTimeCount = 0;
    RotateTimer.start(200);  // to set timeout for antenna rotating

    // open rotator to get this info...

    if (rotator->getMaxAzimuth() > COMPASS_MAX360 && selectRotator->currentAntenna.overRunFlag)
    {
        currentMaxAzimuth = rotator->getMaxAzimuth();
        currentMinAzimuth = rotator->getMinAzimuth();
        overLapActiveflag = true;
    }
    else
    {
        currentMaxAzimuth = COMPASS_MAX360;
        currentMinAzimuth = COMPASS_MIN0;
        overLapActiveflag = false;
    }

    // don't display overlap if rotator doesn't support or user turned off overlap
    toggleOverLapDisplay(overLapActiveflag);

    southStopActiveflag = selectRotator->currentAntenna.southStopFlag;

    rotlog->getBearingLogConfig();
    readTraceLogFlag();

    // tell logger that rotator is active, antenna name and maxRotation


    sendStatusToLogReady();
    upDateAntenna();
    trace("MinosRotator Started");
    trace("Rotator Name = " + selectRotator->currentAntenna.antennaName);
    trace("Rotator Model = " + selectRotator->currentAntenna.rotatorModel);
    trace("Rotator Number = " + QString::number(selectRotator->currentAntenna.rotatorModelNumber));
    trace("Rotator Comport = " + selectRotator->currentAntenna.comport);
    trace("Baudrate = " + QString::number(selectRotator->currentAntenna.baudrate));
    trace("Databits = " + QString::number(selectRotator->currentAntenna.databits));
    trace("Stop bits = " + QString::number(selectRotator->currentAntenna.stopbits));
    trace("Handshake = " + QString::number(selectRotator->currentAntenna.handshake));
    trace("Max Azimuth = " + QString::number(currentMaxAzimuth));
    trace("Min Azimuth = " + QString::number(currentMinAzimuth));
    trace("Rotator Offset = " + QString::number(selectRotator->currentAntenna.rotatorOffset));
    trace("South Stop Flag = " + QString::number(selectRotator->currentAntenna.southStopFlag));
    trace("Overrun flag = " + QString::number(selectRotator->currentAntenna.overRunFlag));
    trace("Rotator Max Baudrate = " + QString::number(rotator->getMaxBaudRate()));
    trace("Rotator Min Baud rate = " + QString::number(rotator->getMinBaudRate()));

}

RotatorMainWindow::~RotatorMainWindow()
{
    delete msg;
    delete ui;
}


void RotatorMainWindow::logMessage( QString s )
{
   if (ui->actionTraceLog)
        trace( s );
}


void RotatorMainWindow::closeEvent(QCloseEvent *event)
{
    MinosRPCObj::clearRPCObjects();
    XMPPClosedown();
    LogTimerTimer( );
    // and tidy up all loose ends

    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    logMessage("MinosRotator Closing");
    QWidget::closeEvent(event);
}



void RotatorMainWindow::resizeEvent(QResizeEvent * event)
{

    QSettings settings;
    settings.setValue("geometry", saveGeometry());

    QWidget::resizeEvent(event);
}



void RotatorMainWindow::LogTimerTimer(  )
{
   static bool closed = false;
   if ( !closed )
   {
      if ( checkCloseEvent() )
      {
         closed = true;
         close();
      }
   }
}



void RotatorMainWindow::onLoggerSetRotation(int direction, int angle)
{

    logMessage("Command From Logger direction = " + QString::number(direction) + "angle = " + QString::number(angle));
    int dirCommand = direction;
    if (dirCommand == rpcConstants::eRotateDirect)
    {
        ui->bearingEdit->setText(QString::number(angle));
        rotateTo(angle);
        sendStatusToLogTurn();
    }
    else if (dirCommand == rpcConstants::eRotateLeft)
    {
        rotateCCW(true);


    }
    else if (dirCommand == rpcConstants::eRotateRight)
    {
        rotateCW(true);


    }
    else if (dirCommand == rpcConstants::eRotateStop)
    {
            stopRotation(true);

    }


}





void RotatorMainWindow::openRotator()
{

    int retCode = 0;

    if (selectRotator->currentAntenna.antennaName == "")
    {
        showStatusMessage("Please select an Antenna");
        return;
    }
    if(selectRotator->currentAntenna.comport == "")
    {
        showStatusMessage("Please select a Comport");
        return;
    }
    if (selectRotator->currentAntenna.rotatorModel == "")
    {
        showStatusMessage("Please select a rotator protcol");
        return;
    }



    srotParams p = selectRotator->getCurrentAntenna();

    retCode = rotator->init(selectRotator->currentAntenna);
    if (retCode < 0)
    {
        hamlibError(retCode);
    }
    if (rotator->get_serialConnected())
    {

        pollTimer->start(pollTime);             // start timer to send message to controller
        showStatusMessage(tr("Connected to Antenna: %1 - %2, %3, %4, %5, %6, %7, %8")
                              .arg(p.antennaName).arg(p.rotatorModel).arg(p.comport).arg(p.baudrate).arg(p.databits)
                              .arg(p.stopbits).arg(rotator->getParityCodeNames()[p.parity]).arg(rotator->getHandShakeNames()[p.handshake]));
        sendStatusToLogReady();
    }
    else
    {
//        QMessageBox::critical(this, tr("Error"), serial->errorString());
        pollTimer->stop();
        stopRotation(false);           // clear flags
        showStatusMessage(tr("Rotator Open error"));
    }


}

void RotatorMainWindow::closeRotator()
{
    rotator->closeRotator();
    showStatusMessage(tr("Disconnected"));
    sendStatusToLogDisConnected();
    logMessage("Rotator Closed");
}






void RotatorMainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}


void RotatorMainWindow::sendStatusLogger(const QString &message)
{
    msg->publishState(message);
}



void RotatorMainWindow::initActionsConnections()
{

    connect(ui->selectAntennaBox, SIGNAL(activated(int)), this, SLOT(upDateAntenna()));
    connect(ui->actionTraceLog, SIGNAL(changed()), this, SLOT(saveTraceLogFlag()));
    connect(ui->turnButton, SIGNAL(clicked(bool)), this, SLOT(rotateToController()));
    connect(ui->bearingEdit, SIGNAL(returnPressed()), this, SLOT(rotateToController()));
    connect(this, SIGNAL(presetRotateTo()), this, SLOT(rotateToController()));
    connect(this, SIGNAL(presetRotateTo()), ui->bearingEdit, SLOT(selectAll()));
    connect(this, SIGNAL(presetRotateTo()), ui->bearingEdit, SLOT(setFocus()));
    connect(ui->turnButton, SIGNAL(clicked(bool)), ui->bearingEdit, SLOT(selectAll()));
    connect(ui->turnButton, SIGNAL(clicked(bool)), ui->bearingEdit, SLOT(setFocus()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), ui->bearingEdit, SLOT(selectAll()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), ui->bearingEdit, SLOT(setFocus()));
    connect(ui->bearingEdit, SIGNAL(returnPressed()), ui->bearingEdit, SLOT(setFocus()));
    connect(ui->bearingEdit, SIGNAL(returnPressed()), ui->bearingEdit, SLOT(selectAll()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), this, SLOT(stopButton()));
    connect(ui->rot_right_button, SIGNAL(toggled(bool)), this, SLOT(rotateCW(bool)));
    connect(ui->rot_left_button, SIGNAL(toggled(bool)), this, SLOT(rotateCCW(bool)));
    connect(this, SIGNAL(escapePressed()), this, SLOT(stop_rotation()));

    // display bearing
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(request_bearing()));
    connect(rotator, SIGNAL(bearing_updated(int)), this, SLOT(displayBearing(int)));
    connect(this, SIGNAL(sendCompassDial(int)), ui->compassDial, SLOT(compassDialUpdate(int)));
    connect(this, SIGNAL(sendBearing(QString)), ui->bearingDisplay, SLOT(setText(const QString &)));
    connect(this, SIGNAL(sendBackBearing(QString)), ui->backBearingDisplay, SLOT(setText(const QString &)));
    connect(this, SIGNAL(displayOverlapBearing(QString)), ui->overlapBearingDisplay, SLOT( setText(const QString &)));
    connect(this, SIGNAL(displayOverlap(bool)), this ,SLOT(overLapDisplayBox(bool)));
    // check endstop and turn to rotation stop
    connect(&RotateTimer, SIGNAL(timeout()), this, SLOT(rotatingTimer()));
    connect(this, SIGNAL(checkingEndStop()), this, SLOT(checkEndStop()));
    connect(rotator, SIGNAL(bearing_updated(int)), this, SLOT(checkMoving(int)));
    //connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    //connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    // setup antennas
    connect(ui->actionSetup_Antennas, SIGNAL(triggered()), selectRotator, SLOT(show()));
    connect(ui->actionEdit_Presets, SIGNAL(triggered()), editPresets, SLOT(loadPresetEditFieldsShow()));
    // setup presets
    connect(editPresets, SIGNAL(showEditPresetDialog()), editPresets, SLOT(show()));
    connect(editPresets, SIGNAL(updatePresetButtonLabels()), this, SLOT(updatePresetLabels()));

    // Bearing Log
    connect(ui->actionLog_Heading, SIGNAL(triggered()), setupLog, SLOT(loadLogConfig()));
    connect(setupLog, SIGNAL(showLogDialog()), setupLog, SLOT(show()));
    connect(setupLog, SIGNAL(bearingLogConfigChanged()), rotlog, SLOT(getBearingLogConfig()));
    connect(rotator, SIGNAL(bearing_updated(int)), rotlog, SLOT(saveBearingLog(int)));


    // Message from Logger
    connect(msg, SIGNAL(setRotation(int,int)), this, SLOT(onLoggerSetRotation(int,int)));

    //connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    //connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));




}




void RotatorMainWindow::keyPressEvent(QKeyEvent *event)
{

    int Key = event->key();

/*
    Qt::KeyboardModifiers mods = event->modifiers();
    bool shift = mods & Qt::ShiftModifier;
    bool ctrl = mods & Qt::ControlModifier;
    bool alt = mods & Qt::AltModifier;
*/
    if (Key == Qt::Key_Escape)
    {
        emit escapePressed();
    }

}

// receives updates from rotator
// update bearing displays
// backbearing display
// and also signal check of endstops

void RotatorMainWindow::displayBearing(int bearing)
{

    logMessage("Display Bearing from Rotator " + QString::number(bearing));

    if (bearing == rotatorBearing)
    {
        return;
    }

    QString overlapbearingmsg = "";

    rotatorBearing = bearing;

    offsetCurrentBearing += QString::number(selectRotator->currentAntenna.rotatorOffset);


    // send Bearing to displays

    // send to logger
    QString s = QString::number(offsetCurrentBearing);
    msg->publishBearing(s);


    // send Bearing to displays
    if (overLapActiveflag)
    {
        if (_bearing > COMPASS_MAX360)
        {
            _bearing -= COMPASS_MAX360;
            overlapbearingmsg = QString::number(bearing);
            overLapOnflag = true;
        }
        else
        {
            overLapOnflag = false;
            overlapbearingmsg = "   ";
        }

        emit displayOverlap(overLapOnflag);
        emit displayOverlapBearing(overlapbearingmsg);

    }

    QString bearingmsg = QString::number(_bearing, 10);
    if (_bearing < 10)    // prevent display resizing
    {
        bearingmsg = "00" + bearingmsg;
    }
    else if (_bearing < 100)
    {
        bearingmsg = "0" + bearingmsg;
    }

    emit sendBearing(bearingmsg);
    emit sendCompassDial(_bearing);

    // check antenna is not at endstops when manually rotating
    emit checkingEndStop();

    // calc and send backbearing
    int backBearing = bearing;

    backBearing += COMPASS_HALF;
    if (backBearing >= COMPASS_MAX360)
    {
        backBearing -= COMPASS_MAX360;
    }
    // send backBearing to display
    QString backBearingmsg = QString::number(backBearing, 10);
    if (backBearing < 10)
    {
        backBearingmsg = "00" + backBearingmsg;
    }
    else if (backBearing < 100)
    {
        backBearingmsg = "0" + backBearingmsg;
    }
    emit sendBackBearing(backBearingmsg);

}







void RotatorMainWindow::readPresets()
{
    QSettings config("./Configuration/MinosRotatorConfig.ini", QSettings::IniFormat);
    config.beginGroup("Presets");
    for (int i = 0; i < NUM_PRESETS; i++)
    {
        presetName[i] = config.value("preset" +  QString::number(i+1)).toString();
        presetBearing[i] = config.value("bearing" +  QString::number(i+1)).toString();
    }
    config.endGroup();
}




void RotatorMainWindow::refreshPresetLabels()
{

    readPresets();

    for (int i = 0; i < NUM_PRESETS; i++)
    {
        if (presetName[i] != "" || presetName[i] != presetButtons[i]->text())
        {
            presetButtons[i]->setText(presetName[i]);
            presetButtons[i]->setShortcut(presetShortCut[i]);     // restore the shortcut
        }
    }
}




void RotatorMainWindow::updatePresetLabels()
{
    refreshPresetLabels();
    update();
}




void RotatorMainWindow::clickedPreset(int buttonNumber)
{

    if (presetName[buttonNumber] != "")
    {
        if (presetBearing[buttonNumber] != "")
        {
           ui->bearingEdit->setText(presetBearing[buttonNumber]);
           emit presetRotateTo();
        }
    }
}




void RotatorMainWindow::initSelectAntennaBox()
{
    selectAntenna->addItem("");
    for (int i= 0; i < NUM_ANTENNAS; i++)
    {
        selectAntenna->addItem(selectRotator->availAntennas[i].antennaName);
    }
}




void RotatorMainWindow::upDateAntenna()
{

    if (moving  || movingCCW || movingCW)
        stopRotation(true);

    int antennaIndex = ui->selectAntennaBox->currentIndex();
    if (antennaIndex > 0)
    {
        antennaIndex -= 1;

        selectRotator->currentAntenna.antennaName = selectRotator->availAntennas[antennaIndex].antennaName;
        selectRotator->currentAntenna.rotatorModel = selectRotator->availAntennas[antennaIndex].rotatorModel;
        selectRotator->currentAntenna.rotatorModelNumber = selectRotator->availAntennas[antennaIndex].rotatorModelNumber;
        selectRotator->currentAntenna.southStopFlag = selectRotator->availAntennas[antennaIndex].southStopFlag;
        selectRotator->currentAntenna.overRunFlag = selectRotator->availAntennas[antennaIndex].overRunFlag;
        selectRotator->currentAntenna.rotatorOffset = selectRotator->availAntennas[antennaIndex].rotatorOffset;
        selectRotator->currentAntenna.comport = selectRotator->availAntennas[antennaIndex].comport;
        selectRotator->currentAntenna.baudrate = selectRotator->availAntennas[antennaIndex].baudrate;
        selectRotator->currentAntenna.databits = selectRotator->availAntennas[antennaIndex].databits;
        selectRotator->currentAntenna.stopbits = selectRotator->availAntennas[antennaIndex].stopbits;
        selectRotator->currentAntenna.parity = selectRotator->availAntennas[antennaIndex].parity;
        selectRotator->currentAntenna.handshake = selectRotator->availAntennas[antennaIndex].handshake;

        selectRotator->saveCurrentAntenna();

       if (rotator->get_serialConnected())
       {
                closeRotator();
       }

       openRotator();

       // update logger
       msg->publishAntennaName(selectRotator->currentAntenna.antennaName);
       msg->publishMaxAzimuth(QString::number(currentMaxAzimuth));
       msg->publishMinAzimuth(QString::number(currentMinAzimuth));

       logMessage("Antenna Updated");
    }


}




void RotatorMainWindow::request_bearing()
{
    logMessage("Request Bearing");
    reqBearCmdflag = true;
    int retCode = 0;
    if (brakeflag || cwCcwCmdflag || rotCmdflag) return;
    if (rotator->get_serialConnected())
    {
        retCode = rotator->request_bearing();
        if (retCode < 0)
        {
            hamlibError(retCode);
        }
    }
    reqBearCmdflag = false;
}


void RotatorMainWindow::checkEndStop()
{
    logMessage("Check EndStop");
    if (movingCW)
    {
        if (currentBearing >= currentMaxAzimuth)
        {
            stopButton();
        }
    }
    else if (movingCCW)
    {
        if (currentBearing <= currentMinAzimuth)
        {
            stopButton();
        }
    }
}


void RotatorMainWindow::rotatingTimer()
{

     rotTimeCount++;

}


void RotatorMainWindow::checkMoving(int bearing)
{
    static int oldBearing;

    if (!moving)
    {
        return;
    }

    if (oldBearing != bearing)
    {
            oldBearing = bearing;
            rotTimeCount = 0;
            return;
    }
    else
    {
        if (rotTimeCount > ROTATE_MOVE_TIMEOUT)
        {
            stopButton();
            sendStatusToLogStop();
        }

    }

}

void RotatorMainWindow::rotateToController()
{

    if (reqBearCmdflag) return;
    bool ok;
    int intBearing;

    rotCmdflag = true;
    QString bearing = ui->bearingEdit->text();

    intBearing = bearing.toInt(&ok, 10);

    if (intBearing >= currentMinAzimuth && intBearing <= currentMaxAzimuth && ok)
    {
        rotateTo(intBearing);
        logMessage("Rotate to bearing " + bearing);
    }
    else
    {
        QString s = "Invalid Bearing\nPlease enter " + QString::number(currentMinAzimuth) + " - " + QString::number(currentMaxAzimuth);
        QMessageBox::critical(this, tr("Bearing Error"), s);
    }
    rotCmdflag = false;
}


void RotatorMainWindow::rotateTo(int bearing)
{
    int retCode = 0;
    int rotateTo = bearing;


    if (bearing > currentMaxAzimuth || bearing < currentMinAzimuth)
    {
        return; //error
    }



    if (movingCW || movingCCW)
    {
        stopRotation(true);
    }

    // calculate target bearing based on current position
    rotateTo  = northCalcTarget(rotateTo);

    // check if we are already at bearing
    if (rotateTo == currentBearing)
    {
        return;
    }

/*
    if (overLapActiveflag && !ui->overLapDisable->isChecked())
    {
        if (currentBearing < COMPASS_MAX360)
        {
            if ((COMPASS_MAX360 + bearing) < currentMaxAzimuth)
            {
                if ((COMPASS_MAX360 - currentBearing + bearing) < (currentBearing - bearing))
                {
                    rotateTo = COMPASS_MAX360 + bearing;
                }
            }
        }
        else // current bearing > COMPASS_MAX360
        {
           if (COMPASS_MAX360 + bearing < currentMaxAzimuth)
           {
              rotateTo = COMPASS_MAX360 + bearing;
           }
        }
    }
*/
    if (rotator->get_serialConnected())
    {

        retCode = rotator->rotate_to_bearing(rotateTo);
        if (retCode < 0)
        {
            hamlibError(retCode);
        }
        else
        {
            moving = true;
            sendStatusToLogTurn();
            rotTimeCount = 0;           // clear timer count
        }

    }

}



int RotatorMainWindow::northCalcTarget(int targetBearing)
{

    int target = 0;


    if (currentBearing >= COMPASS_MAX360)
    {
        if (targetBearing >= COMPASS_MIN0 && targetBearing <= currentMaxAzimuth - COMPASS_MAX360)
        {
            target = COMPASS_MAX360 + targetBearing;
            return target;
        }
    }
    else if (currentBearing < COMPASS_MIN0)
    {
        if (targetBearing < COMPASS_MAX360 and targetBearing > COMPASS_MAX360 - (currentMinAzimuth * -1))
        {
            target = (COMPASS_MAX360 - targetBearing) * -1;
            return target;
        }
        else if (targetBearing > COMPASS_MIN0)
        {
            target = targetBearing;
                return target;
         }
     }


     if (currentBearing >= COMPASS_MIN0 && currentBearing <= COMPASS_HALF)
     {
        if (targetBearing > COMPASS_MIN0 && targetBearing <= COMPASS_HALF)
        {
            target = targetBearing;
        }
        else if (targetBearing - currentBearing < COMPASS_MAX360 - targetBearing + currentBearing)
        {
            target = targetBearing;
            return target;
        }


        else
        {
            if (currentMinAzimuth < COMPASS_MIN0)
            {
                target = (COMPASS_MAX360 - targetBearing) * -1;
                return target;
            }
            else
            {
                target = targetBearing;
                return target;
            }
        }
     }

    else if (currentBearing >= COMPASS_HALF && currentBearing <= COMPASS_MAX360)
    {
        if (targetBearing > COMPASS_HALF && targetBearing <= COMPASS_MAX360)
        {
            target = targetBearing;
            return target;
        }
        else if (COMPASS_MAX360 - currentBearing + targetBearing > currentBearing - targetBearing)
        {
            target = targetBearing;
            return target;
        }
        else
        {
            if (COMPASS_MAX360 + targetBearing > currentMaxAzimuth)
            {
                target = targetBearing;
            }
            else
            {
                target = COMPASS_MAX360 + targetBearing;
            }
            return target;
        }
     }

     return target;

}




void RotatorMainWindow::stopButton()
{
    logMessage("StopButton");
    if (ui->rot_left_button->isChecked())
    {
        ui->rot_left_button->setChecked(false);
        return;
    }
    else if (ui->rot_right_button->isChecked())
    {
        ui->rot_right_button->setChecked(false);
        return;
    }

    stopRotation(true);


}

void RotatorMainWindow::stop_rotation()
{
    stopRotation(true);
}

void RotatorMainWindow::stopRotation(bool sendStop)
{

    logMessage("Stop Rotation");
    int retCode = 0;
    brakeflag = true;
    stopCmdflag = true;
    if (sendStop)
    {
        retCode = rotator->stop_rotation();
        if (retCode < 0)
        {
            hamlibError(retCode);
            sendStatusToLogError();
        }
    }
    ui->rot_left_button->setChecked(false);
    ui->rot_right_button->setChecked(false);
    sendStatusToLogStop();
    sleepFor(brakedelay);
    brakeflag = false;
    moving = false;
    movingCW = false;
    movingCCW = false;
    stopCmdflag = false;

}


void RotatorMainWindow::rotateCW(bool toggle)
{


    logMessage("RotateCW");
    if (moving || movingCW || movingCCW)
    {
        stopButton();
    }

    if (currentBearing >= currentMaxAzimuth)
    {
        ui->rot_right_button->setChecked(false);
        return;
    }

    cwCcwCmdflag = true;
    if (toggle)
    {

        int retCode = 0;
        if (rotator->get_serialConnected())
        {
            retCode = rotator->rotateClockwise(rotator->get_rotatorSpeed());
            if (retCode < 0)
            {
                hamlibError(retCode);
                movingCW = false;
                sendStatusToLogError();
            }
            else
            {
                ui->rot_right_button->setChecked(true);
                movingCW = true;
                sendStatusToLogRotCW();
            }
            cwCcwCmdflag = false;
        }
    }


    cwCcwCmdflag = false;
}




void RotatorMainWindow::rotateCCW(bool toggle)
{
    logMessage("RotateCCW");
    if (moving  || movingCW || movingCCW)
    {
        stopButton();
    }

    if (currentBearing <= COMPASS_MIN0)
    {
        ui->rot_left_button->setChecked(false);
        return;
    }

    cwCcwCmdflag = true;
    if (toggle)
    {

        int retCode = 0;
        if (rotator->get_serialConnected())
        {
            retCode = rotator->rotateCClockwise(rotator->get_rotatorSpeed());
            if (retCode < 0)
            {
                hamlibError(retCode);
                movingCCW = false;
                sendStatusToLogError();
            }
            else
            {
                ui->rot_left_button->setChecked(true);
                movingCCW = true;
                sendStatusToLogRotCCW();
            }

        }

    }
    cwCcwCmdflag = false;
}


void RotatorMainWindow::setPolltime(int interval)
{
    pollTime = interval;
}

int RotatorMainWindow::getPolltime()
{

    return pollTime;
}


void RotatorMainWindow::hamlibError(int errorCode)
{

    pollTimer->stop();

    int errCode = errorCode;
    if ( errCode >= 0)
    {
        return;
    }

    errCode *= -1;
    QString errorMsg = rotator->gethamlibErrorMsg(errCode);

    QMessageBox::critical(this, "hamlib Error", QString::number(errCode) + " - " + errorMsg);

    closeRotator();

}


void RotatorMainWindow::sendStatusToLogReady()
{
    sendStatusLogger(STATUS_READY);
}

void RotatorMainWindow::sendStatusToLogRotCCW()
{
    sendStatusLogger(STATUS_ROTATE_CCW);
}

void RotatorMainWindow::sendStatusToLogRotCW()
{
    sendStatusLogger(STATUS_ROTATE_CW);
}

void RotatorMainWindow::sendStatusToLogStop()
{
    sendStatusLogger(STATUS_STOP);
}

void RotatorMainWindow::sendStatusToLogTurn()
{
    sendStatusLogger(STATUS_TURN_TO);
}

void RotatorMainWindow::sendStatusToLogDisConnected()
{
    sendStatusLogger(STATUS_DISCONNECTED);
}

void RotatorMainWindow::sendStatusToLogError()
{
    sendStatusLogger(STATUS_ERROR);
}

void RotatorMainWindow::sleepFor(qint64 milliseconds)
{
    qint64 timeToExitFunction = QDateTime::currentMSecsSinceEpoch()+milliseconds;
    while(timeToExitFunction>QDateTime::currentMSecsSinceEpoch()){
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}


void RotatorMainWindow::readTraceLogFlag()
{
    QSettings config("./Configuration/MinosRotatorConfig.ini", QSettings::IniFormat);
    config.beginGroup("TraceLog");

    ui->actionTraceLog->setChecked(config.value("TraceLog", false).toBool());

    config.endGroup();
}

void RotatorMainWindow::saveTraceLogFlag()
{
    QSettings config("./Configuration/MinosRotatorConfig.ini", QSettings::IniFormat);
    config.beginGroup("TraceLog");

    config.setValue("TraceLog", ui->actionTraceLog->isChecked());

    config.endGroup();
}


void RotatorMainWindow::about()
{
    QMessageBox::about(this, "Minos Rotator", "Minos QT Rotator\nCopyright D Balharrie G8FKH/M0DGB 2017\nVersion 0.1");
}


void RotatorMainWindow::overLapDisplayBox(bool status)
{
    QPalette palette;

    if (status)
    {
        palette.setColor(QPalette::Base,Qt::red);
    }
    else
    {
        palette.setColor(QPalette::Base,Qt::lightGray);
    }

    ui->overlaplineEdit->setPalette(palette);
}


void RotatorMainWindow::toggleOverLapDisplay(bool toggle)
{
    // don't display overlap if rotator doesn't support or user turned off overlap
    ui->overlap->setVisible(toggle);
    ui->overlapBearingDisplay->setVisible(toggle);
    ui->overlaplineEdit->setVisible(toggle);

}
