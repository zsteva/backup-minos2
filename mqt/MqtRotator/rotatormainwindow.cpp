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
#include "rotatorRpc.h"
#include "rotatorlog.h"
#include "rotatormainwindow.h"
#include "ui_rotatormainwindow.h"
#include "minoscompass.h"
#include "rotcontrol.h"
#include "setupdialog.h"
#include "editpresetsdialog.h"
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

    enableTrace( "./TraceLog", "MinosChat_" );
    createCloseEvent();
    MinosRotatorForm = this;
    connect(&LogTimer, SIGNAL(timeout()), this, SLOT(LogTimerTimer()));
    LogTimer.start(100);
    msg = new RotatorRpc(this);

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
    rotator = new RotControl();
    selectRotator = new SetupDialog(rotator);
    editPresets = new EditPresetsDialog;
    pollTimer = new QTimer(this);
    status = new QLabel;
//    compassDial = new MinosCompass;
    selectAntenna = new QComboBox;
    rotlog = new RotatorLog;

    ui->statusbar->addWidget(status);

//    ui->verticalLayout_3->addWidget(compassDial);

    rotator->set_serialConnected(false);

    refreshPresetLabels();
    initActionsConnections();

    selectAntenna = ui->selectAntennaBox;

    initSelectAntennaBox();

    selectRotator->readCurrentAntenna();

    selectAntenna->setCurrentIndex(selectAntenna->findText(selectRotator->currentAntenna.antennaName));

    brakedelay = 3 * 1000;

    brakeflag = false;
    cwCcwflag = false;
    rotCmdflag = false;
    stopCmdflag = false;
    rotLogFlg = true;

    moving = false;
    bearing = 0;
    min_azimuth = 0;
    max_azimuth = 0;


    setPolltime(1000);



    openRotator();



    // tell logger that rotator is active


    sndStatusLoggger("Rotator Ready");




}

RotatorMainWindow::~RotatorMainWindow()
{
    delete msg;
    delete ui;
}


void RotatorMainWindow::logMessage( QString s )
{
   trace( s );
}


void RotatorMainWindow::closeEvent(QCloseEvent *event)
{
    MinosRPCObj::clearRPCObjects();
    XMPPClosedown();
    LogTimerTimer( );
    // and tidy up all loose ends
    /*
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    */
    QWidget::closeEvent(event);
}



void RotatorMainWindow::resizeEvent(QResizeEvent * event)
{
    /*
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    */
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

    int dirCommand = direction;
    if (dirCommand == rpcConstants::eRotateDirect)
    {
        ui->bearingEdit->setText(QString::number(angle));
        rotator->rotate_to_bearing(angle);

    }
    else if (dirCommand == rpcConstants::eRotateLeft)
    {
            if (!moving)
            {
                rotateCCW(true);
                sndStatusLoggger("Rotating CCW");
            }
    }
    else if (dirCommand == rpcConstants::eRotateRight)
    {
            if (!moving)
            {
                rotateCW(true);
                sndStatusLoggger("Rotating CW");
            }
    }
    else if (dirCommand == rpcConstants::eRotateStop)
    {
            stopRotation();
            sndStatusLoggger("Rotator Stop");

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

    }
    else
    {
//        QMessageBox::critical(this, tr("Error"), serial->errorString());
        pollTimer->stop();
        showStatusMessage(tr("Rotator Open error"));
    }


}

void RotatorMainWindow::closeRotator()
{
    rotator->closeRotator();
    showStatusMessage(tr("Disconnected"));
}






void RotatorMainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}


void RotatorMainWindow::sndStatusLoggger(const QString &message)
{
    msg->publishState(message);
}



void RotatorMainWindow::initActionsConnections()
{
    //connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(openSerialPort()));
    connect(ui->selectAntennaBox, SIGNAL(activated(int)), this, SLOT(upDateAntenna()));
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
    connect(this, SIGNAL(escapePressed()), rotator, SLOT(stop_rotation()));
    connect(this, SIGNAL(sendBearing(QString)), ui->bearingDisplay, SLOT(setText(const QString &)));
    connect(this, SIGNAL(sendBackBearing(QString)), ui->backBearingDisplay, SLOT(setText(const QString &)));
    //connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    //connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionSetup_Antennas, SIGNAL(triggered()), selectRotator, SLOT(show()));
    connect(ui->actionEdit_Presets, SIGNAL(triggered()), editPresets, SLOT(loadPresetEditFieldsShow()));
    connect(editPresets, SIGNAL(showEditPresetDialog()), editPresets, SLOT(show()));
    connect(editPresets, SIGNAL(updatePresetButtonLabels()), this, SLOT(updatePresetLabels()));
//    connect(rotator, SIGNAL(bearing_updated(int)), ui->bearingDisplay, SLOT(setText(const QString &)));
    connect(rotator, SIGNAL(bearing_updated(int)), this, SLOT(displayBearing(int)));
//    connect(rotator, SIGNAL(bearing_updated(int)), this, SLOT(displayBackBearing(const QString)));
    connect(rotator, SIGNAL(bearing_updated(int)), this, SLOT(displayBackBearing(int)));
//    connect(rotator, SIGNAL(bearing_updated(int)), ui->compassDial, SLOT(compassDialUpdate(const QString &)));
    connect(rotator, SIGNAL(bearing_updated(int)), ui->compassDial, SLOT(compassDialUpdate(int)));
//    connect(rotator, SIGNAL(bearing_updated(int)), this, SLOT(logBearing(const QString.s &)));
//    connect(rotator, SIGNAL(bearing_updated(int)), this, SLOT(logBearing(int)));
//    connect(rotator, SIGNAL(bearing_updated(int)), this, SLOT(sendBearingLogger(const QString &)));
//    connect(rotator, SIGNAL(bearing_updated(int)), this, SLOT(sendBearingLogger(int)));
    connect(msg, SIGNAL(setRotation(int,int)), this, SLOT(onLoggerSetRotation(int,int)));
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(request_bearing()));
    //connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    //connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    //connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}




void RotatorMainWindow::keyPressEvent(QKeyEvent *event)
{

    int Key = event->key();

    Qt::KeyboardModifiers mods = event->modifiers();
    bool shift = mods & Qt::ShiftModifier;
    bool ctrl = mods & Qt::ControlModifier;
    bool alt = mods & Qt::AltModifier;

    if (Key == Qt::Key_Escape)
    {

        emit escapePressed();


    }

    if (Key >= Qt::Key_F1 && Key <= Qt::Key_F10 )
    {
        qDebug() << "rotate function key pressed!";
        qDebug() << Key;
        emit rotateFunctionKeyPressed(Key);
    }
    else if (!(shift || ctrl) && alt)
    {
        switch (Key) {
        case ROTATE_CW_KEY:
            qDebug() << "rotate cw key pressed!";
            emit rotate_cwKeyPressed();
        break;
        case ROTATE_CCW_KEY:
            qDebug() << "rotate ccw key pressed!";
            emit rotate_ccwKeyPressed();
        break;
        case ROTATE_STOP_KEY:
            qDebug() << "rotate stop key pressed!";
            emit rotateStopKeyPressed();
        break;
        case ROTATE_TURN_KEY:
            qDebug() << "rotate turn key pressed!";
            emit rotateTurnKeyPressed();
        break;
        }


     }
}



void RotatorMainWindow::displayBearing(int bearing)
{

    // send Bearing to display
    QString bearingmsg = QString::number(bearing, 10);
    if (bearing < 10)
    {
        bearingmsg = "00" + bearingmsg;
    }
    else if (bearing < 100)
    {
        bearingmsg = "0" + bearingmsg;
    }

    emit sendBearing(bearingmsg);


}




void RotatorMainWindow::displayBackBearing(int bearing)
{

    int backBearing = bearing;

    backBearing += 180;
    if (backBearing >= 360)
    {
        backBearing -= 360;
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

    }


}




void RotatorMainWindow::request_bearing()
{
    int retCode = 0;
    if (brakeflag || cwCcwflag || rotCmdflag) return;
    if (rotator->get_serialConnected())
    {
        retCode = rotator->request_bearing();
        if (retCode < 0)
        {
            hamlibError(retCode);
        }
    }
}


void RotatorMainWindow::rotateToController()
{
    bool ok;
    int intBearing;
    int retCode = 0;
    rotCmdflag = true;
    qDebug() << "Triggered from Turnbutton";
    // get bearing from bearing line edit form
    QString bearing = ui->bearingEdit->text();
    qDebug() << "string from box " << bearing;
    intBearing = bearing.toInt(&ok, 10);
    if (intBearing >= 0 && intBearing <= 359 && ok)
    {
        if (rotator->get_serialConnected())
        {
            //rotate_to_bearing(intbearing);
            retCode = rotator->rotate_to_bearing(intBearing);
            if (retCode < 0)
            {
                hamlibError(retCode);
            }
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Bearing Error"), tr("Invalid Bearing\nPlease enter 0 - 359"));
    }
    rotCmdflag = false;
}


void RotatorMainWindow::stopButton()
{
    if (ui->rot_left_button->isChecked())
    {
        ui->rot_left_button->setChecked(!ui->rot_left_button->isChecked());
        return;
    }
    else if (ui->rot_right_button->isChecked())
    {
        ui->rot_right_button->setChecked(!ui->rot_right_button->isChecked());
        return;
    }
    else
    {
        stopRotation();
    }
}


void RotatorMainWindow::stopRotation()
{


    int retCode = 0;
    brakeflag = true;
    stopCmdflag = true;
    retCode = rotator->stop_rotation();
    if (retCode < 0)
    {
        hamlibError(retCode);
    }
    sleepFor(brakedelay);
    brakeflag = false;
    moving = false;
    stopCmdflag = false;

}


void RotatorMainWindow::rotateCW(bool toggle)
{
    cwCcwflag = true;
    if (toggle)
    {

        int retCode = 0;
        if (rotator->get_serialConnected())
        {
            retCode = rotator->rotateClockwise(rotator->get_rotatorSpeed());
            if (retCode < 0)
            {
                hamlibError(retCode);
                moving = false;
            }
            moving = true;
        }


    }
    else
    {
        stopRotation();
    }
    cwCcwflag = false;
}




void RotatorMainWindow::rotateCCW(bool toggle)
{
    cwCcwflag = true;
    if (toggle)
    {

        int retCode = 0;
        if (rotator->get_serialConnected())
        {
            retCode = rotator->rotateCClockwise(rotator->get_rotatorSpeed());
            if (retCode < 0)
            {
                hamlibError(retCode);
                moving = false;
            }
            moving = true;
        }

     }
    else
    {
        stopRotation();
    }
    cwCcwflag = false;
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


void RotatorMainWindow::logBearing(const QString bearing)
{
    int retCode = 0;
    if (rotLogFlg)
    {
        retCode = rotlog->writeLog(bearing);
    }
}


void RotatorMainWindow::sendBearingLogger(const QString bearing)
{

    msg->publishBearing(bearing);
}


void RotatorMainWindow::sleepFor(qint64 milliseconds)
{
    qint64 timeToExitFunction = QDateTime::currentMSecsSinceEpoch()+milliseconds;
    while(timeToExitFunction>QDateTime::currentMSecsSinceEpoch()){
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}


