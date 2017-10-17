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
// Hamlib Library
//
/////////////////////////////////////////////////////////////////////////////

#include "base_pch.h"
#include "mqtUtils_pch.h"
#include "RPCCommandConstants.h"
#include "rigcontrolcommonconstants.h"
#include "rigcontrolmainwindow.h"
#include "ui_rigcontrolmainwindow.h"
#include "rigcontrol.h"
#include "setupdialog.h"
#include "rigcontrolrpc.h"
#include <QTimer>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QDebug>




RigControlMainWindow::RigControlMainWindow(QWidget *parent) :
    QMainWindow(parent)
    , msg(0)
    , ui(new Ui::RigControlMainWindow)
    , loggerPbWidth(0)
    , logger_bw_state(hamlibData::NOR)
    , rigErrorFlag(false)
    , mgmModeFlag(false)
    , supRitFlag(false)
    , curVfoFrq(0)
    , curTransVertFrq(0)
    , rRitFreq(0)

{

    ui->setupUi(this);



    connect(&stdinReader, SIGNAL(stdinLine(QString)), this, SLOT(onStdInRead(QString)));
    stdinReader.start();

    // get the antenna name from host process
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    appName = env.value("MQTRPCNAME", "") ;

    createCloseEvent();

    connect(&LogTimer, SIGNAL(timeout()), this, SLOT(LogTimerTimer()));
    LogTimer.start(100);

    msg = new RigControlRpc(this);

    QSettings settings;
    geoStr = "geometry";
    if (appName.length() > 0)
    {
        geoStr = geoStr + appName;
    }

    QByteArray geometry = settings.value(geoStr).toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);



    radio = new RigControl();
    radio->getRigList();

    selectRig = new SetupDialog(radio);
    selectRig->setAppName(appName);

    selectRadio = ui->selectRadioBox;

    pollTimer = new QTimer(this);

    status = new QLabel;
    ui->statusBar->addWidget(status);




    radio->set_serialConnected(false);
    initActionsConnections();



    initSelectRadioBox();




    if (appName.length() > 0)
    {
        logMessage((QString("Read Current Radio for AppName %1 from logger").arg(appName)));

    }
    else
    {
        logMessage((QString("Read Current Antenna for Local selection")));

    }

    selectRig->readCurrentRadio();

    if (selectRig->currentRadio.radioName == "")
    {
        logMessage(QString("No radio selected for this appName, %1").arg(appName));
        QString errmsg = "<font color='Red'>Please select a radio!</font>";
        showStatusMessage(errmsg);
        sendStatusLogger(errmsg);
    }
    else
    {
        selectRadio->setCurrentIndex(selectRadio->findText(selectRig->currentRadio.radioName));
    }





    setPolltime(250);



    readTraceLogFlag();


    upDateRadio();



    trace("*** Rig Started ***");


}




RigControlMainWindow::~RigControlMainWindow()
{
    delete ui;
    delete msg;
}

void RigControlMainWindow::logMessage( QString s )
{
   if (ui->actionTraceLog->isChecked())
        trace( s );
}


void RigControlMainWindow::LogTimerTimer()
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

void RigControlMainWindow::closeEvent(QCloseEvent *event)
{
    MinosRPCObj::clearRPCObjects();
    XMPPClosedown();
    LogTimerTimer( );
    // and tidy up all loose ends

    QSettings settings;
    settings.setValue(geoStr, saveGeometry());
    trace("MinosRigControl Closing");
    QWidget::closeEvent(event);
}


void RigControlMainWindow::onStdInRead(QString cmd)
{
    trace("Command read from stdin: " + cmd);
    if (cmd.indexOf("ShowServers", Qt::CaseInsensitive) >= 0)
        setShowServers(true);
    if (cmd.indexOf("HideServers", Qt::CaseInsensitive) >= 0)
        setShowServers(false);
}

void RigControlMainWindow::initActionsConnections()
{
    connect(ui->selectRadioBox, SIGNAL(activated(int)), this, SLOT(upDateRadio()));

    connect(ui->actionSetup_Radios, SIGNAL(triggered()), selectRig, SLOT(show()));
    connect(ui->actionTraceLog, SIGNAL(changed()), this, SLOT(saveTraceLogFlag()));

    connect(pollTimer, SIGNAL(timeout()), this, SLOT(getRadioInfo()));
    //connect(pollTimer, SIGNAL(timeout()), this, SLOT(getCurMode()));
    //connect(radio, SIGNAL(frequency_updated(double)), this, SLOT(updateFreq(const double)));
//    connect(this, SIGNAL(frequency_updated(double)), this, SLOT(displayFreqA(const double)));
    //connect(this, SIGNAL(mode_updated(QString)), this, SLOT(displayModeVfoA(QString)));

    // configure antenna dialog
    connect(selectRig, SIGNAL(currentRadioSettingChanged(QString)), this, SLOT(currentRadioSettingChanged(QString)));
    connect(selectRig, SIGNAL(radioNameChange()), this, SLOT(updateSelectRadioBox()));

    // Message from Logger
    connect(msg, SIGNAL(setFreq(QString)), this, SLOT(loggerSetFreq(QString)));
    connect(msg, SIGNAL(setMode(QString)), this, SLOT(loggerSetMode(QString)));
    connect(msg, SIGNAL(setPassBand(int)), this, SLOT(loggerSetPassBand(int)));

    //connect(this, SIGNAL(frequency_updated(double)), this, SLOT(drawDial(double)));
    //connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    //connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(radio, SIGNAL(debug_protocol(QString)), this, SLOT(logMessage(QString)));

}


void RigControlMainWindow::currentRadioSettingChanged(QString radioName)
{

    switch( QMessageBox::question(
                        this,
                        tr("Minos RigControl"),
                        tr("The settings for the current radio have been changed. \nDo you want to reload the settings for the radio now?"),
                        QMessageBox::Yes |
                        QMessageBox::No |
                        QMessageBox::Cancel,
                         QMessageBox::Cancel ) )
    {
        case QMessageBox::Yes:
            if (selectRadio->currentText() != radioName)
            {
                bool ok;
                selectRadio->setCurrentIndex(selectRig->currentRadio.radioNumber.toInt(&ok, 10));
            }
            upDateRadio();
            break;
        case QMessageBox::No:

            break;
        case QMessageBox::Cancel:

            break;
        default:

            break;
    }
}


void RigControlMainWindow::updateSelectRadioBox()
{
    int curidx = selectRadio->currentIndex();
    selectRadio->clear();
    initSelectRadioBox();
    selectRadio->setCurrentIndex(curidx);
}


void RigControlMainWindow::initSelectRadioBox()
{
    selectRadio->addItem("");
    for (int i= 0; i < NUM_RADIOS; i++)
    {
        selectRadio->addItem(selectRig->availRadios[i].radioName);
    }
}




void RigControlMainWindow::upDateRadio()
{
    int retCode = 0;
    int radioIndex = ui->selectRadioBox->currentIndex();
    if (radioIndex > 0)
    {
        radioIndex -= 1;
        if (selectRig->availRadios[radioIndex].radioModelNumber == 0)
        {
            closeRadio();
            QMessageBox::critical(this, tr("Radio Error"), "Please configure a radio name and model");
            return;
        }
        selectRig->currentRadio.radioName = selectRig->availRadios[radioIndex].radioName;
        selectRig->currentRadio.radioNumber = selectRig->availRadios[radioIndex].radioNumber;
        selectRig->currentRadio.radioMfg_Name = selectRig->availRadios[radioIndex].radioMfg_Name;
        selectRig->currentRadio.radioModel = selectRig->availRadios[radioIndex].radioModel;
        selectRig->currentRadio.radioModelNumber = selectRig->availRadios[radioIndex].radioModelNumber;
        selectRig->currentRadio.civAddress = selectRig->availRadios[radioIndex].civAddress;
        selectRig->currentRadio.portType = selectRig->availRadios[radioIndex].portType;
        selectRig->currentRadio.networkAdd = selectRig->availRadios[radioIndex].networkAdd;
        selectRig->currentRadio.networkPort = selectRig->availRadios[radioIndex].networkPort;
        selectRig->currentRadio.comport = selectRig->availRadios[radioIndex].comport;
        selectRig->currentRadio.baudrate = selectRig->availRadios[radioIndex].baudrate;
        selectRig->currentRadio.databits = selectRig->availRadios[radioIndex].databits;
        selectRig->currentRadio.stopbits = selectRig->availRadios[radioIndex].stopbits;
        selectRig->currentRadio.parity = selectRig->availRadios[radioIndex].parity;
        selectRig->currentRadio.handshake = selectRig->availRadios[radioIndex].handshake;
        selectRig->currentRadio.transVertEnable = selectRig->availRadios[radioIndex].transVertEnable;
        if (!selectRig->currentRadio.transVertEnable)
        {
            // only show transvert freq box is enabled
            ui->transVertFreqA->setVisible(selectRig->currentRadio.transVertEnable);
            ui->TVertTitleA->setVisible(selectRig->currentRadio.transVertEnable);
        }
        selectRig->currentRadio.transVertNegative = selectRig->availRadios[radioIndex].transVertNegative;
        selectRig->currentRadio.transVertOffset = selectRig->availRadios[radioIndex].transVertOffset;
        selectRig->currentRadio.transVertOffsetStr = selectRig->availRadios[radioIndex].transVertOffsetStr;
        selectRig->currentRadio.useRxPassBand = selectRig->availRadios[radioIndex].useRxPassBand;
        selectRig->currentRadio.mgmMode = selectRig->availRadios[radioIndex].mgmMode;

        selectRig->saveCurrentRadio();

        if (radio->get_serialConnected())
        {
                closeRadio();
                this->setWindowTitle("Minos Rig Control");
        }

        openRadio();

        if (appName.length() > 0)
        {
            this->setWindowTitle("Minos Rig Control - " + appName + " - Logger");
            msg->publishRadioName(selectRig->currentRadio.radioName);
        }
        else
        {
            this->setWindowTitle("Minos Rig Control - Local");
        }


        // get freq to see if comms are working
        if (radio->get_serialConnected())
        {
            retCode = getFrequency(RIG_VFO_CURR);
            if (retCode != RIG_OK)
            {
                logMessage(QString("Update Radio: Get Freq error %1").arg(QString::number(retCode)));
                hamlibError(retCode, "Update Radio");
            }
        }

        if (radio->get_serialConnected())
        {
            radio->buildPassBandTable();

            // initialise rig state
            logger_mode = "USB";

            retCode = radio->supportRit(selectRig->currentRadio.radioModelNumber, &supRitFlag);
            if (retCode == RIG_OK)
            {
                enableRitDisplay(supRitFlag);
            }

            loggerSetPassBand(hamlibData::NOR );

            sendStatusToLogConnected();


        }
        else
        {
            trace(QString("#### Radio Failed to connect ####"));
            sendStatusToLogDisConnected();
        }



        dumpRadioToTraceLog();
    }
    else
    {   // no radio selected
        trace("No radio selected");
        closeRadio();
        if (appName.length() > 0)
        {
            this->setWindowTitle("Minos Rig Control - - Logger");
            msg->publishRadioName("No radio");

        }
        else
        {
            this->setWindowTitle("Minos Rig Control - - Local");
        }
    }





    if (radio->get_serialConnected())
    {
        pollTimer->start(pollTime);             // start timer to send message to controller
    }


}

void RigControlMainWindow::openRadio()
{

    int retCode = 0;

    if (selectRig->currentRadio.radioName == "")
    {
        logMessage(QString("Open Radio: No radio name!"));
        showStatusMessage("Please select a Radio");
        return;
    }
    if (rig_port_e(selectRig->currentRadio.portType) == RIG_PORT_SERIAL)
    {
        if(selectRig->comportAvial(selectRig->currentRadio.radioNumber.toInt(), selectRig->currentRadio.comport) == -1)
        {
            logMessage(QString("Open Radio: Check comport - defined port %1 not available on computer").arg(selectRig->currentRadio.comport));
            showStatusMessage(QString("Comport %1 no longer configured on computer?").arg(selectRig->currentRadio.comport));
            return;
        }

        if (selectRig->currentRadio.comport == "")
        {
            logMessage(QString("Open Radio: No comport"));
            showStatusMessage("Please select a Comport");
            return;
        }

    }

    if (rig_port_e(selectRig->currentRadio.portType) == RIG_PORT_NETWORK || rig_port_e(selectRig->currentRadio.portType == RIG_PORT_UDP_NETWORK))
    {
        if (selectRig->currentRadio.networkAdd == "" || (selectRig->currentRadio.networkPort == ""))
        {
            logMessage(QString("Open Radio: No network or Port Number"));
            showStatusMessage("Please enter a network Address and Port Number");
            return;
        }

    }
    if (selectRig->currentRadio.radioModel == "")
    {
        logMessage(QString("Open Radio: No radio model"));
        showStatusMessage("Please select a radio model");
        return;
    }

    scatParams p = selectRig->getCurrentRadio();

    retCode = radio->init(selectRig->currentRadio);
    if (retCode < 0)
    {
        logMessage(QString("Error Opening Radio Error Code = $1").arg(QString::number(retCode)));
        hamlibError(retCode, "Open Radio");
    }
    if (radio->get_serialConnected())
    {

        //pollTimer->start(pollTime);             // start timer to send message to controller
        if (rig_port_e(selectRig->currentRadio.portType) == RIG_PORT_SERIAL)
        {
                showStatusMessage(QString("Connected to Radio: %1 - %2, %3, %4, %5, %6, %7, %8")
                                  //.arg(selectRig->currentRadio.radioName, selectRig->currentRadio.radioModel, selectRig->currentRadio.comport, selectRig->currentRadio.baudrate, selectRig->currentRadio.databits, selectRig->currentRadio.stopbits, radio->getParityCodeNames()[selectRig->currentRadio.parity], radio->getHandShakeNames()[selectRig->currentRadio.handshake]));
                                  .arg(p.radioName).arg(p.radioModel).arg(p.comport).arg(p.baudrate).arg(p.databits)
                                  .arg(p.stopbits).arg(radio->getParityCodeNames()[p.parity]).arg(radio->getHandShakeNames()[p.handshake]));


        }
        else if (rig_port_e(selectRig->currentRadio.portType) == RIG_PORT_NETWORK || rig_port_e(selectRig->currentRadio.portType) == RIG_PORT_UDP_NETWORK)
        {
                showStatusMessage(QString("Connected to Radio: %1 - %2, %3").arg(selectRig->currentRadio.radioName, selectRig->currentRadio.radioModel, selectRig->currentRadio.networkAdd + ":" + selectRig->currentRadio.networkPort));
        }
        else if (rig_port_e(selectRig->currentRadio.portType) == RIG_PORT_NONE)
        {
                showStatusMessage(QString("Connected to Radio: %1 - %2").arg(selectRig->currentRadio.radioName, selectRig->currentRadio.radioModel));
        }


    }
    else
    {

        logMessage(QString("Radio Open Error"));
        showStatusMessage(tr("Radio Open error"));
    }


}

void RigControlMainWindow::closeRadio()
{
    radio->closeRig();
    showStatusMessage(tr("Disconnected"));
    sendStatusToLogDisConnected();
    logMessage("Radio Closed");
}





void RigControlMainWindow::setPolltime(int interval)
{
    pollTime = interval;
}

int RigControlMainWindow::getPolltime()
{

    return pollTime;
}



void RigControlMainWindow::getRadioInfo()
{
    logMessage("Request radio info");

    chkRadioMgmModeChanged();

    int retCode;
    if (radio->get_serialConnected())
    {

        retCode = getFrequency(RIG_VFO_CURR);
        if (retCode < 0)
        {
            // error
            logMessage(QString("Get radioInfo: Get Freq error, code = ").arg(QString::number(retCode)));
            hamlibError(retCode, "Request Bearing");
        }

        retCode = getMode(RIG_VFO_CURR);
        if (retCode < 0)
        {
            // error
            logMessage(QString("Get radioInfo: Get Mode error").arg(QString::number(retCode)));
            hamlibError(retCode, "Request Mode");

        }

        if (supRitFlag)
        {
            retCode = getRitFreq(RIG_VFO_CURR);
            if (retCode < 0)
            {
                // error
                logMessage(QString("Get radioInfo: Get RIT error").arg(QString::number(retCode)));
                hamlibError(retCode, "Request RIT");
            }
        }


        sendRxPbFlagToLog();
    }

}


void RigControlMainWindow::loggerSetFreq(QString freq)
{
    if (radio->get_serialConnected() && !rigErrorFlag)
    {
        logger_freq = freq;
        setFreq(freq, RIG_VFO_CURR);
    }

}


void RigControlMainWindow::setFreq(QString freq, vfo_t vfo)
{
    bool ok = false;
    int retCode = 0;
    QString sfreq = freq.remove('.');

    double f = sfreq.toDouble(&ok);

    if (ok)
    {
        if (selectRig->currentRadio.transVertEnable)
        {
            if (selectRig->currentRadio.transVertNegative)
            {
                f = f + selectRig->currentRadio.transVertOffset;
            }
            else
            {
                f = f - selectRig->currentRadio.transVertOffset;
            }
        }
        if (radio->get_serialConnected())
        {
            retCode = radio->setFrequency(f, vfo);
            if (retCode == RIG_OK)
            {
                qDebug() << "frequency changed!";
            }
            else
            {
                hamlibError(retCode, "SetFreq");
            }
        }
        else
        {
            qDebug() << "radio not conntected";
        }
    }
    else
    {
        qDebug() << "freq conversion from string failed";
    }
}

int RigControlMainWindow::getFrequency(vfo_t vfo)
{
    double transVertF = 0;
    int retCode = 0;


    retCode = radio->getFrequency(vfo, &rfrequency);
    if (retCode == RIG_OK)
    {
        if (rfrequency != curVfoFrq)
        {
            curVfoFrq = rfrequency;
            logMessage(QString("Trans Enable = %1").arg(QString::number(selectRig->currentRadio.transVertEnable)));
            if (selectRig->currentRadio.transVertEnable)
            {
                logMessage("Transvert enabled");
                if (selectRig->currentRadio.transVertNegative)
                {
                    logMessage("Negative Transvert");
                    transVertF = rfrequency - selectRig->currentRadio.transVertOffset;
                    logMessage(QString("Transvert F = %1").arg(QString::number(transVertF)));
                }
                else
                {
                    logMessage(("Positive Transvert"));
                    transVertF = rfrequency + selectRig->currentRadio.transVertOffset;

                }
                logMessage(QString("Transvert Freq. = %1").arg(QString::number(transVertF)));
                curTransVertFrq = transVertF;
                displayTransVertVfo(transVertF);

            }
            displayFreqVfo(rfrequency);

            if (selectRig->currentRadio.transVertEnable)
            {
                sendFreqToLog(transVertF);
            }
            else
            {
                sendFreqToLog(rfrequency);
            }
        }
        //else
        //{
        //    return retCode;
       // }
    }
    //else
    //{
    return retCode;



}

void RigControlMainWindow::chkRadioMgmModeChanged()
{
    static QString mgmMode;
    if (selectRig->currentRadio.mgmMode != mgmMode)
    {
        ui->mgmMode->setText(selectRig->currentRadio.mgmMode);
    }
}


int RigControlMainWindow::getMode(vfo_t vfo)
{

    int retCode = 0;

    retCode = radio->getMode(vfo, &rmode, &rwidth);
    if (retCode == RIG_OK)
    {
        curMode = rmode;
        if (!mgmModeFlag)
        {
            displayModeVfo(radio->convertModeQstr(rmode));
            displayPassband(rwidth);
            sendModeToLog(radio->convertModeQstr(rmode));
        }
        else
        {
            displayModeVfo(hamlibData::MGM);
            sendModeToLog(hamlibData::MGM);
        }




 /*
        if (!mgmModeFlag && rmode != curMode)
        {
            curMode = rmode;
            displayModeVfo(radio->convertModeQstr(rmode));
            displayPassband(rwidth);
            sendModeToLog(radio->convertModeQstr(rmode));
        }
        else if (mgmModeFlag && rmode != curMode)
        {
            // mode changed on radio
            mgmModeFlag = false;
            curMode = rmode;
            displayModeVfo(radio->convertModeQstr(rmode));
            displayPassband(rwidth);
            sendModeToLog(radio->convertModeQstr(rmode));
        }
        else if (mgmModeFlag)
        {
            displayModeVfo(hamlibData::MGM);
            sendModeToLog(hamlibData::MGM);
        }
 */
   }

    return retCode;
}

void RigControlMainWindow::getCurMode()
{
    getMode(RIG_VFO_CURR);
}


void RigControlMainWindow::loggerSetMode(QString mode)
{
    if (radio->get_serialConnected() && !rigErrorFlag)
    {
        logger_mode = mode;
        if (logger_mode == hamlibData::MGM)
        {
            if (mgmModeFlag)
            {
                if (curMode !=  radio->convertQStrMode(selectRig->currentRadio.mgmMode))
                {
                    setCurMode(selectRig->currentRadio.mgmMode);
                }
            }
            else
            {
                mgmModeFlag = true;
                setCurMode(selectRig->currentRadio.mgmMode);
            }
        }
        else
        {
            mgmModeFlag = false;
            setCurMode(logger_mode);
        }

    }

}




void RigControlMainWindow::setCurMode(QString mode)
{
    setMode(mode, RIG_VFO_CURR);
}


void RigControlMainWindow::setMode(QString mode, vfo_t vfo)
{
    rmode_t mCode = radio->convertQStrMode(mode);
    int retCode = 0;
    pbwidth_t passBand;
    if (selectRig->currentRadio.useRxPassBand)
    {
        passBand = rwidth;
    }
    else
    {
        passBand = loggerPbWidth;
    }

    if (radio->get_serialConnected())
    {
         retCode = radio->setMode(vfo, mCode, passBand);
         if (retCode == RIG_OK)
         {
             logMessage(QString("Mode changed! Mode = %1, PassBand = %2").arg(radio->convertModeQstr(rmode), QString::number(passBand)));
         }
         else
         {
             logMessage(QString("Mode Change Error Code = %1, Mode = %1, PassBand = %2").arg(QString::number(retCode), radio->convertModeQstr(rmode), QString::number(passBand)));
             hamlibError(retCode, "Set Mode");
         }
    }
    else
    {
        qDebug() << "radio not connected";
    }
}

void RigControlMainWindow::enableRitDisplay(bool state)
{
    ui->ritLbl->setVisible(state);
    ui->ritFreq->setVisible(state);
}


int RigControlMainWindow::getRitFreq(vfo_t vfo)
{
    int retCode = 0;


    retCode = radio->getRit(vfo, &rRitFreq);
    if (retCode == RIG_OK)
    {
        sRitFreq = QString::number(rRitFreq);
        ui->ritFreq->setText(sRitFreq);
    }
    return retCode;
}


int RigControlMainWindow::setRitFreq(vfo_t vfo, shortfreq_t ritFreq)
{

    return radio->setRit(vfo, ritFreq);

}

void RigControlMainWindow::loggerSetPassBand(int state)
{
    if (radio->get_serialConnected() && !rigErrorFlag)
    {
        logger_bw_state = state;
        loggerPbWidth = radio->lookUpPassBand(logger_mode, state);
        ui->passBandState->setText(hamlibData::pBandStateStr[state]);
        ui->logpbwidthlbl->setText(QString::number(loggerPbWidth));
        setMode(logger_mode, RIG_VFO_CURR);
    }

}







/*
void RigControlMainWindow::updateFreq(double frequency)
{
    if (curFreq == frequency)
    {
        return;
    }
    else
    {
        curFreq = frequency;
    }
    emit frequency_updated(frequency);
}

*/

void RigControlMainWindow::displayFreqVfo(double frequency)
{

    ui->radioFreqA->setText(convertFreqString(frequency));
}




void RigControlMainWindow::displayTransVertVfo(double frequency)
{
    ui->transVertFreqA->setText(convertFreqString(frequency));
}




void RigControlMainWindow::displayModeVfo(QString mode)
{
    ui->modeA->setText(mode);
}


void RigControlMainWindow::displayPassband(pbwidth_t width)
{

    ui->passBandlbl->setText(QString::number(width));
}


QString RigControlMainWindow::convertFreqString(double frequency)
{
    double freq = frequency;
    sfreq = QString::number(freq,'f', 0);
    int len = sfreq.length();


    switch(len)
    {
        case 11:
            sfreq = sfreq.insert(8, '.');
            sfreq = sfreq.insert(5, '.');
            sfreq = sfreq.insert(2, '.');
            break;
        case 10:
            sfreq = sfreq.insert(7, '.');
            sfreq = sfreq.insert(4, '.');
            sfreq = sfreq.insert(1, '.');
            break;
        case 9:
            sfreq = sfreq.insert(3, '.');
            sfreq = sfreq.insert(7, '.');
            break;
        case 8:
            sfreq = sfreq.insert(2, '.');
            sfreq = sfreq.insert(6, '.');
            break;
        case 7:
            sfreq = sfreq.insert(4, '.');
            sfreq = sfreq.insert(1, '.');
            break;
        case 6:
            sfreq = sfreq.insert(3,'.');
            break;
        case 5:
            sfreq = sfreq.insert(2,'.');
            break;
        case 4:
            sfreq = sfreq.insert(1,'.');
            break;
        default:
            sfreq = "??.???.???.???";    // error

    }


    return sfreq;
}




void RigControlMainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}






void RigControlMainWindow::hamlibError(int errorCode, QString cmd)
{

    pollTimer->stop();


    if ( errorCode >= 0)
    {
        return;
    }
    if(appName > 0)
    {
        sendStatusToLogError();
    }

    rigErrorFlag = true;

    errorCode *= -1;
    QString errorMsg = radio->gethamlibErrorMsg(errorCode);
    logMessage("Hamlib Error - Code = " + QString::number(errorCode) + " " + errorMsg);

    QMessageBox::critical(this, "RigControl hamlib Error - " + selectRig->currentRadio.radioName, QString::number(errorCode) + " - " + errorMsg + "\n" + "Command - " + cmd);

    closeRadio();
    rigErrorFlag = false;
    if (appName.length() >0)
    {
        sendStatusToLogDisConnected();
    }

}



void RigControlMainWindow::readTraceLogFlag()
{
    QString fileName;
    if (appName == "")
    {
        fileName = RIG_CONFIGURATION_FILEPATH_LOCAL + MINOS_RADIO_CONFIG_FILE;
    }
    else
    {
        fileName = RIG_CONFIGURATION_FILEPATH_LOGGER + MINOS_RADIO_CONFIG_FILE;
    }


    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup("TraceLog");


    ui->actionTraceLog->setChecked(config.value("TraceLog", false).toBool());

    config.endGroup();
}

void RigControlMainWindow::saveTraceLogFlag()
{
    QString fileName;
    if (appName == "")
    {
        fileName = RIG_CONFIGURATION_FILEPATH_LOCAL + MINOS_RADIO_CONFIG_FILE;
    }
    else
    {
        fileName = RIG_CONFIGURATION_FILEPATH_LOGGER + MINOS_RADIO_CONFIG_FILE;
    }


    qDebug() << "traceLog selected";
    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup("TraceLog");

    config.setValue("TraceLog", ui->actionTraceLog->isChecked());

    config.endGroup();
    trace("Tracelog Changed = " + QString::number(ui->actionTraceLog->isChecked()));
}





void RigControlMainWindow::about()
{
    QMessageBox::about(this, "Minos RigControl", "Minos QT RigControl\nCopyright D Balharrie G8FKH/M0DGB 2017");
}



void RigControlMainWindow::sendStatusLogger(const QString &message)
{
    if (appName.length() > 0)
    {
        msg->publishState(message);
    }
}


void RigControlMainWindow::sendStatusToLogConnected()
{

        sendStatusLogger(RIG_STATUS_CONNECTED);

}

void RigControlMainWindow::sendStatusToLogDisConnected()
{

        sendStatusLogger(RIG_STATUS_DISCONNECTED);

}


void RigControlMainWindow::sendStatusToLogError()
{
    sendStatusLogger(RIG_STATUS_ERROR);
}

void RigControlMainWindow::sendFreqToLog(freq_t freq)
{
    if (appName.length() > 0)
    {
        msg->publishFreq(convertFreqString(freq));
    }
}

void RigControlMainWindow::sendModeToLog(QString mode)
{
    if (appName.length() > 0)
    {
        msg->publishMode(mode);
    }
}

void RigControlMainWindow::sendRxPbFlagToLog()
{
    if (appName.length() > 0)
    {
        QString s;
        if (selectRig->currentRadio.useRxPassBand)
        {
            s = "set";
        }
        else
        {
            s = "clear";
        }

        msg->publishRxPbFlag(s);
    }
}

void RigControlMainWindow::dumpRadioToTraceLog()
{
    trace("*** Radio Update ***");
    trace(QString("App Instance Name  = %1").arg(appName));
    trace(QString("Radio Name = %1").arg(selectRig->currentRadio.radioName));
    trace(QString("Radio Number = %1").arg(selectRig->currentRadio.radioNumber));
    trace(QString("Rig Model = %1").arg(selectRig->currentRadio.radioModel).trimmed());
    trace(QString("Rig Number = %1").arg(QString::number(selectRig->currentRadio.radioModelNumber)));
    trace(QString("Rig Manufacturer = %1").arg(selectRig->currentRadio.radioMfg_Name));
    if (selectRig->currentRadio.radioMfg_Name == "Icom")
    {
        if (selectRig->currentRadio.civAddress == "")
        {
            trace(QString("Icom CIV address = Using Default Rig Address"));
        }
        else
        {
            trace(QString("Icom CIV address = %1").arg(selectRig->currentRadio.civAddress));
        }

    }
    trace(QString("Rig PortType = %1").arg(hamlibData::portTypeList[selectRig->currentRadio.portType]));
    trace(QString("Network Address = %1").arg(selectRig->currentRadio.networkAdd));
    trace(QString("Network Port = %1").arg(selectRig->currentRadio.networkPort));
    trace(QString("Comport = %1").arg(selectRig->currentRadio.comport));
    trace(QString("Baudrate = %1").arg(selectRig->currentRadio.baudrate));
    trace(QString("Stop bits = %1").arg(QString::number(selectRig->currentRadio.stopbits)));
    trace(QString("Parity = %1").arg(radio->getParityCodeNames()[selectRig->currentRadio.parity]));
    trace(QString("Handshake = %1").arg(radio->getHandShakeNames()[selectRig->currentRadio.handshake]));
    QString f = "";
    if (selectRig->currentRadio.transVertEnable)
    {
        f = "True";
    }
    else
    {
        f = "False";
    }
    trace(QString("TransVert Enable = %1").arg(f));
    if (selectRig->currentRadio.transVertNegative)
    {
        f = "True";
    }
    else
    {
        f = "False";
    }
    trace(QString("TransVert Negative = %1").arg(f));
    trace(QString("TransVert Offset = %1").arg(convertFreqString(selectRig->currentRadio.transVertOffset)));
    if (selectRig->currentRadio.useRxPassBand)
    {
        f = "True";
    }
    else
    {
        f = "False";
    }
    trace(QString("Use RX Passband = %1").arg(f));
    if (supRitFlag)
    {
        f = "True";
    }
    else
    {
        f = "False";
    }
    trace(QString("Radio Supports RIT = ").arg(f));
    trace(QString("Radio Passband CW NAR = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::CW, hamlibData::NAR))));
    trace(QString("Radio Passband CW NOR = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::CW, hamlibData::NOR))));
    trace(QString("Radio Passband CW WID = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::CW, hamlibData::WIDE))));
    trace(QString("Radio Passband USB NAR = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::USB, hamlibData::NAR))));
    trace(QString("Radio Passband USB NOR = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::USB, hamlibData::NOR))));
    trace(QString("Radio Passband USB WID = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::USB, hamlibData::WIDE))));
    trace(QString("Radio Passband FM NAR = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::FM, hamlibData::NAR))));
    trace(QString("Radio Passband FM NOR = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::FM, hamlibData::NOR))));
    trace(QString("Radio Passband FM WID = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::FM, hamlibData::WIDE))));
    trace(QString("Radio Passband MGM NAR = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::MGM, hamlibData::NAR))));
    trace(QString("Radio Passband MGM NOR = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::MGM, hamlibData::NOR))));
    trace(QString("Radio Passband MGM WID = %1").arg(QString::number(radio->lookUpPassBand(hamlibData::MGM, hamlibData::WIDE))));

}
