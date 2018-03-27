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
#include "freqpresetdialog.h"
#include "rigcontrol.h"
#include "setupdialog.h"
#include "rigcontrolrpc.h"
#include "rigutils.h"
#include <QTimer>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QDebug>




RigControlMainWindow::RigControlMainWindow(QWidget *parent) :
    QMainWindow(parent)
    , msg(0)
    , ui(new Ui::RigControlMainWindow)
    , rigErrorFlag(false)
    , mgmModeFlag(false)
    , supRitFlag(false)
    , cmdLockFlag(false)
    , curVfoFrq(0)
    , curTransVertFrq(0)
    , rRitFreq(0)
    , radioIndex(0)

{

    ui->setupUi(this);



    connect(&stdinReader, SIGNAL(stdinLine(QString)), this, SLOT(onStdInRead(QString)));
    stdinReader.start();


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



    if (appName.length() > 0)
    {
        testBoxesVisible(false);
    }
    else
    {
        testBoxesVisible(true);
    }



    QByteArray geometry = settings.value(geoStr).toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    radio = new RigControl();
    radio->getRigList();

    loadBands();
    FreqPresetDialog::readSettings(presetFreq);

    setupRadio = new SetupDialog(radio, bands);
    setupRadio->setAppName(appName);



    if (appName.length() > 0)
    {
        // connected to logger don't show radio selectbox
        setSelectRadioBoxVisible(false);
        setRadioNameLabelVisible(true);
    }
    else
    {
        setSelectRadioBoxVisible(true);
        setRadioNameLabelVisible(false);
    }



    pollTimer = new QTimer(this);

    status = new QLabel;
    ui->statusBar->addWidget(status);
    ui->radioNameDisp->setText("");

    radio->set_serialConnected(false);
    initActionsConnections();

    initSelectRadioBox();

    setTransVertDisplayVisible(false);
    sendTransVertSwitchToLogger(TRANSSW_NUM_DEFAULT);

    enableRitDisplay(false);

    if (appName.length() > 0)
    {
        logMessage((QString("Read Current Radio for AppName %1 from logger").arg(appName)));

    }
    else
    {
        logMessage((QString("Read Current Radio for Local selection")));

        setupRadio->readCurrentRadio();

        if (setupRadio->currentRadioName == "")
        {
            logMessage(QString("No radio selected for this appName, %1").arg(appName));
            QString errmsg = "<font color='Red'>Please select a radio!</font>";
            showStatusMessage(errmsg);
            sendStatusLogger(errmsg);
        }
        else
        {
            ui->selectRadioBox->setCurrentText(setupRadio->currentRadioName);
        }
    }

    setPolltime(250);

    readTraceLogFlag();

    ui->selectRadioBox->clearFocus();

    if (appName.length() == 0)
    {
        upDateRadio();
    }



    trace("*** Rig App Started ***");


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
    bool show = getShowServers();
    if ( !isVisible() && show )
    {
        setVisible(true);
    }
    if ( isVisible() && !show )
    {
        setVisible(false);
    }

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

    LogTimer.stop();
    closeRadio();

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

    connect(ui->actionSetup_Radios, SIGNAL(triggered()), setupRadio, SLOT(exec()));
    connect(ui->actionSetup_Band_Freq, SIGNAL(triggered(bool)), this, SLOT(setupBandFreq()));
    connect(ui->actionTraceLog, SIGNAL(changed()), this, SLOT(saveTraceLogFlag()));

    connect(pollTimer, SIGNAL(timeout()), this, SLOT(getRadioInfo()));


    // configure antenna dialog
    connect(setupRadio, SIGNAL(currentRadioSettingChanged(QString)), this, SLOT(currentRadioSettingChanged(QString)));
    connect(setupRadio, SIGNAL(radioNameChange()), this, SLOT(updateSelectRadioBox()));

    // Message from Logger
    connect(msg, SIGNAL(setFreq(QString)), this, SLOT(loggerSetFreq(QString)));
    connect(msg, SIGNAL(setMode(QString)), this, SLOT(loggerSetMode(QString)));
    connect(msg, SIGNAL(selectLoggerRadio(QString)), this, SLOT(onSelectRadio(QString)));



    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAbout_Radio_Config, SIGNAL(triggered()), this, SLOT(aboutRigConfig()));

    connect(radio, SIGNAL(debug_protocol(QString)), this, SLOT(logMessage(QString)));



    // standalone test
    connect(ui->selFreq, SIGNAL(clicked(bool)), this, SLOT(selFreqClicked()));
    connect(ui->freqInputBox, SIGNAL(editingFinished()), this, SLOT(selFreqClicked()));

}



void RigControlMainWindow::setupBandFreq()
{

    FreqPresetDialog  fPresetDialog(presetFreq, bands, freqPresetChanged);

    fPresetDialog.exec();

    if (!freqPresetChanged)
    {
        logMessage(QString("RigControl: Band Freq Change, send new bandlist to logger"));
        sendBandListLogger();
        freqPresetChanged = false;
    }
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
            if (ui->selectRadioBox->currentText() != radioName)
            {
                bool ok;
                ui->selectRadioBox->setCurrentIndex(setupRadio->currentRadio.radioNumber.toInt(&ok, 10));
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
    int curidx = ui->selectRadioBox->currentIndex();
    ui->selectRadioBox->clear();
    initSelectRadioBox();
    ui->selectRadioBox->setCurrentIndex(curidx);
}


void RigControlMainWindow::initSelectRadioBox()
{
    ui->selectRadioBox->addItem("");
    for (int i= 0; i < setupRadio->numAvailRadios; i++)
    {
        ui->selectRadioBox->addItem(setupRadio->availRadioData[i]->radioName);
    }

    if (appName.length() > 0)
    {
        sendRadioListLogger();

    }
}




void RigControlMainWindow::setSelectRadioBoxVisible(bool visible)
{


    ui->SelectRadioTitle->setVisible(visible);
    ui->selectRadioBox->setVisible(visible);


}


void RigControlMainWindow::setRadioNameLabelVisible(bool visible)
{
    ui->radioNameDispLbl->setVisible(visible);
    ui->radioNameDisp->setVisible(visible);

}




void RigControlMainWindow::upDateRadio()
{
    logMessage(QString("UpdateRadio: Index Selected = %1").arg(QString::number(ui->selectRadioBox->currentIndex())));

    pollTimer->stop();      // stop updates

    int ridx = 0;
    if (setupRadio->currentRadioName != "")
    {

        radioIndex = setupRadio->findCurrentRadio(setupRadio->currentRadioName);
        ridx = radioIndex;
        if (ridx > -1 && ridx < setupRadio->numAvailRadios)  // find radio and update current radio pointer
        {
            // found antenna, updatea currentAntenna pointer to select antennadata
            scatParams::copyRig(setupRadio->availRadioData[ridx], setupRadio->currentRadio);

            if (radio->get_serialConnected())
            {
                    closeRadio();
            }


            if (setupRadio->currentRadio.radioModelNumber == 0)
            {
                closeRadio();
                QMessageBox::critical(this, tr("Radio Error"), "Please configure a radio name and model");
                return;
            }




            // only show transvert freq box is enabled
            setTransVertDisplayVisible(setupRadio->currentRadio.transVertEnable);
            sendTransVertStatus(setupRadio->currentRadio.transVertEnable);   // send to logger
            sendTransVertSwitchToLogger(TRANSSW_NUM_DEFAULT);                                 // turn off transVerter Sw

            //setupRadio->currentRadio.transVertNegative = setupRadio->availRadioData[ridx]->transVertNegative;
            //setupRadio->currentRadio.transVertOffset = setupRadio->availRadioData[ridx]->transVertOffset;
            //setupRadio->currentRadio.transVertOffsetStr = setupRadio->availRadioData[ridx]->transVertOffsetStr;
            //setupRadio->currentRadio.mgmMode = setupRadio->availRadioData[ridx]->mgmMode;

            setupRadio->saveCurrentRadio();

            openRadio();

            if (radio->get_serialConnected())
            {

                ui->radioNameDisp->setText(setupRadio->currentRadio.radioName);


                logMessage(QString("Update - Get radio frequency"));
                getAndSendFrequency(RIG_VFO_CURR);   // also sends it

                /*int retCode =*/
                /*
                if (retCode < 0)
                {
                    // error
                    logMessage(QString("Radio Update: Get Freq error, code = %1").arg(QString::number(retCode)));
                    hamlibError(retCode, "Request Frequency");
                }
                else
                {
                    logMessage(QString("Radio Update: Got Frequency = %1").arg(QString::number(rfrequency)));

                    sendFreqToLog(rfrequency, true);// force a freq update
                }
                */



                if (setupRadio->currentRadio.radioModelNumber != 135) // don't send USB if Ft991
                {
                    if (radio->get_serialConnected())
                    {
                        if (appName.count() > 0)
                        {
                            logMessage(QString("Update Radio: Logger Set Mode to %1").arg(slogMode));
                            loggerSetMode(selRadioMode);
                        }
                        else
                        {

                            logMessage(QString("Update Radio: Set Mode USB Standalone"));
                            // initialise rig state

                            slogMode = "USB";
                            // set mode
                            logMode = radio->convertQStrMode("USB");
                            setMode("USB", RIG_VFO_CURR);

                        }


                    }
                }


                if (radio->get_serialConnected())
                {

                    writeWindowTitle(appName);
                    sendStatusToLogConnected();
                    sendRadioNameLogger(setupRadio->currentRadio.radioName);
                    sendBandListLogger();
                    dumpRadioToTraceLog();

                }
                else
                {
                    trace(QString("#### Radio Failed to connect ####"));
                    sendStatusToLogDisConnected();
                }

            }
        }
        else
        {   // no radio selected
            trace("No radio selected");
            ui->radioNameDisp->setText("");
            closeRadio();
            if (appName.length() > 0)
            {
                writeWindowTitle(appName);
                sendRadioNameLogger("");
            }
            else
            {
                writeWindowTitle(appName);
            }

        }

        if (radio->get_serialConnected())
        {
            if (setupRadio->currentRadio.pollInterval == "0.5")
            {
                pollTime = 500;
            }
            else
            {
                pollTime = 1000 * setupRadio->currentRadio.pollInterval.toInt();
            }

            pollTimer->start(pollTime);             // start timer to send poll radio
        }
    }


}

void RigControlMainWindow::refreshRadio()
{
    logMessage(QString("refreshRadio: Index Selected = %1").arg(QString::number(ui->selectRadioBox->currentIndex())));

    int ridx = 0;
    radioIndex = ui->selectRadioBox->currentIndex();
    ridx = radioIndex;
    if (ridx > 0)
    {
        if (radio->get_serialConnected())
        {
            openRadio();    // do everything except init it

            logMessage(QString("Update - Get radio frequency"));
            getAndSendFrequency(RIG_VFO_CURR);   // also sends

            if (setupRadio->currentRadio.radioModelNumber != 135) // don't send USB if Ft991
            {
                logMessage(QString("Update Radio: Logger Set Mode to %1").arg(slogMode));
                loggerSetMode(selRadioMode);
            }
            writeWindowTitle(appName);
            sendStatusToLogConnected();
            sendRadioNameLogger(setupRadio->currentRadio.radioName);
            dumpRadioToTraceLog();
        }
        else
        {
            upDateRadio();
        }
    }
}


void RigControlMainWindow::openRadio()
{

        int retCode = 0;

        if (setupRadio->currentRadio.radioName == "")
        {
            logMessage(QString("Open Radio: No radio name!"));
            showStatusMessage("Please select a Radio");
            return;
        }

        logMessage(QString("Open Radio: Opening Radio %1 PortType %2").arg(setupRadio->currentRadio.radioName).arg(hamlibData::portTypeList[setupRadio->currentRadio.portType]));
        showStatusMessage(QString("Opening Radio: %1").arg(setupRadio->currentRadio.radioName));

        if (rig_port_e(setupRadio->currentRadio.portType) == RIG_PORT_SERIAL)
        {
            if(setupRadio->comportAvial(setupRadio->currentRadio.radioNumber.toInt(), setupRadio->currentRadio.comport) == -1)
            {
                logMessage(QString("Open Radio: Check comport - defined port %1 not available on computer").arg(setupRadio->currentRadio.comport));
                showStatusMessage(QString("Comport %1 no longer configured on computer?").arg(setupRadio->currentRadio.comport));
                return;
            }

            if (setupRadio->currentRadio.comport == "")
            {
                logMessage(QString("Open Radio: No comport"));
                showStatusMessage("Please select a Comport");
                return;
            }

        }

        if (rig_port_e(setupRadio->currentRadio.portType) == RIG_PORT_NETWORK || rig_port_e(setupRadio->currentRadio.portType == RIG_PORT_UDP_NETWORK))
        {
            if (setupRadio->currentRadio.networkAdd == "" || (setupRadio->currentRadio.networkPort == ""))
            {
                logMessage(QString("Open Radio: No network or Port Number"));
                showStatusMessage("Please enter a network Address and Port Number");
                return;
            }

        }
        if (setupRadio->currentRadio.radioModel == "")
        {
            logMessage(QString("Open Radio: No radio model"));
            showStatusMessage("Please select a radio model");
            return;
        }


        if (!radio->get_serialConnected())
        {
            // if radio is already open, don't reinit it
            retCode = radio->init(setupRadio->currentRadio);
            if (retCode < 0)
            {
                logMessage(QString("Error Opening Radio Error Code = %1").arg(QString::number(retCode)));
                hamlibError(retCode, "Open Radio");
            }
        }

        if (radio->get_serialConnected())
        {
            logMessage(QString("Open Radio: Radio Opened %1").arg(setupRadio->currentRadio.radioName));
            showStatusMessage(QString("Radio Opened: %1").arg(setupRadio->currentRadio.radioName));



            if (rig_port_e(setupRadio->currentRadio.portType) == RIG_PORT_SERIAL)
            {
                    showStatusMessage(QString("Connected: %1 - %2, %3, %4, %5, %6, %7, %8")
                                      .arg(setupRadio->currentRadio.radioName).arg(setupRadio->currentRadio.radioModel).trimmed().arg(setupRadio->currentRadio.comport).arg(setupRadio->currentRadio.baudrate).arg(setupRadio->currentRadio.databits)
                                      .arg(setupRadio->currentRadio.stopbits).arg(radio->getParityCodeNames()[setupRadio->currentRadio.parity]).arg(radio->getHandShakeNames()[setupRadio->currentRadio.handshake]));


            }
            else if (rig_port_e(setupRadio->currentRadio.portType) == RIG_PORT_NETWORK || rig_port_e(setupRadio->currentRadio.portType) == RIG_PORT_UDP_NETWORK)
            {
                    showStatusMessage(QString("Connected: %1 - %2, %3:%4").arg(setupRadio->currentRadio.radioName).arg(setupRadio->currentRadio.radioModel.trimmed()).arg(setupRadio->currentRadio.networkAdd).arg(setupRadio->currentRadio.networkPort));
            }
            else if (rig_port_e(setupRadio->currentRadio.portType) == RIG_PORT_NONE)
            {
                    showStatusMessage(QString("Connected: %1 - %2").arg(setupRadio->currentRadio.radioName).arg(setupRadio->currentRadio.radioModel.trimmed()));
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

        if (radio->get_serialConnected())
        {
           radio->closeRig();
        }

        showStatusMessage("Disconnected");
        sendStatusToLogDisConnected();
        logMessage(QString("Radio Closed"));



    }


    void RigControlMainWindow::writeWindowTitle(QString appName)
    {
        if (appName.length() > 0)
        {
            this->setWindowTitle("Minos Rig Control - " + appName + " - Logger");
        }
        else
        {
            this->setWindowTitle("Minos Rig Control - Local");
        }

    }


    void RigControlMainWindow::setPolltime(int interval)
    {
        pollTime = interval;
    }

    int RigControlMainWindow::getPolltime()
    {

        return pollTime;
    }

    void RigControlMainWindow::cmdLockOn()
    {
        cmdLockFlag = true;
        logMessage(QString("Lockon: Command Lock On"));
    }

    void RigControlMainWindow::cmdLockOff()
    {
        cmdLockFlag = false;
        logMessage(QString("Lockoff: Command Lock Off"));
    }

    void RigControlMainWindow::getRadioInfo()
    {
        logMessage(QString("Request radio info"));
        if (cmdLockFlag)
        {
            trace(QString("GetRadioInfo: Command Lock on"));
            return;
        }
        chkRadioMgmModeChanged();

        int retCode;
        if (radio->get_serialConnected())
        {
            logMessage(QString("Get radio frequency"));
            retCode = getAndSendFrequency(RIG_VFO_CURR);

        }

        if (radio->get_serialConnected())
        {

            logMessage("Get radio mode");
            retCode = getAndSendMode(RIG_VFO_CURR);
            if (retCode < 0)
            {
                // error
                logMessage(QString("Get radioInfo: Get Mode error %1").arg(QString::number(retCode)));
                hamlibError(retCode, "Request Mode");

            }
            else
            {
                logMessage(QString("Got Mode = %1").arg(radio->convertModeQstr(rmode)));
            }
        }



    /*
            if (supRitFlag)
            {
                logMessage((QString("Get RIT")));
                retCode = getRitFreq(RIG_VFO_CURR);
                if (retCode < 0)
                {
                    // error
                    logMessage(QString("Get radioInfo: Get RIT error").arg(QString::number(retCode)));
                    hamlibError(retCode, "Request RIT");
                }
            }

    */

    }


    void RigControlMainWindow::onSelectRadio(QString s)
    {

        logMessage(QString("Recieved SelectRadio from Logger = %1").arg(s));


        // the first time rigcontrolframe uses this message the mode is appended to the name
        if (s.contains(':'))
        {
            QStringList sl = s.split(':');
            if (sl.count() != 2)
            {
                logMessage(QString("OnSelectRadio Error Splitting radioName and mode %1").arg(s));
                return;     // error
            }
            s = sl[0];
            if (sl[1] == "")
            {
                selRadioMode = "USB";
            }
            else
            {
                selRadioMode = sl[1];
            }
        }

        QString oldRadio = ui->selectRadioBox->currentText();

        ui->selectRadioBox->setCurrentText(s);
        setupRadio->currentRadioName = s;

        if (!s.isEmpty() && s == oldRadio)
        {
            refreshRadio();
        }
        else
        {
            upDateRadio();
        }
    }

    void RigControlMainWindow::loggerSetFreq(QString freq)
    {
        logMessage(QString("Recieved Freq from Logger = %1").arg(freq));
        if (radio->get_serialConnected() && !rigErrorFlag)
        {
            logger_freq = freq;
            setFreq(freq, RIG_VFO_CURR);
        }

    }


    void RigControlMainWindow::setFreq(QString freq, vfo_t vfo)
    {
        cmdLockOn();    // lock get radio info
        bool ok = false;
        int retCode = 0;
        QString sfreq = freq;
        QString cb;
        int tvNum = 0;


        double f = sfreq.toDouble(&ok);
        logMessage(QString("SetFreq: Change to Freq = %1").arg(QString::number(f)));

        if (ok)
        {
            BandList &blist = BandList::getBandList();
            BandInfo bi;
            bool bandOK = blist.findBand(f, bi);
            if (bandOK)
            {
                cb = bi.adif;
                logMessage(QString("SetFreq: Band found = %1").arg(cb));
            }

            if (cb != selTvBand && setupRadio->currentRadio.transVertEnable && setupRadio->currentRadio.numTransverters != 0)
            {
                // does a transverter support this band?

                bool b = false;
                while (tvNum < setupRadio->currentRadio.numTransverters)
                {
                    if (setupRadio->currentRadio.transVertSettings[tvNum]->band == cb)
                    {
                        b = true;
                        logMessage(QString("SetFreq: Found Transverter %1 for this freq.").arg(setupRadio->currentRadio.transVertSettings[tvNum]->transVertName));
                        break;
                    }
                    tvNum++;
                }

                if (b)  // found a tranverter supporting this band
                {
                    selTvBand = cb;
                    ui->transVertBandDisp->setText(cb);
                    //setTransVertDisplayVisible(true);
                    if (setupRadio->currentRadio.transVertSettings[tvNum]->enableTransSwitch)
                    {
                        if (setupRadio->currentRadio.transVertSettings[tvNum]->transSwitchNum != transVertSwNum)
                        {
                            transVertSwNum = setupRadio->currentRadio.transVertSettings[tvNum]->transSwitchNum;
                            ui->transVertSwNum->setText(transVertSwNum);
                            transVertSwNum = setupRadio->currentRadio.transVertSettings[tvNum]->transSwitchNum;
                            sendTransVertSwitchToLogger(transVertSwNum);
                            logMessage(QString("SetFreq: Send TransVert Switch number - %1").arg(transVertSwNum));

                        }
                    }
                    else
                    {
                        displayTransVertVfo(0.0);
                        transVertSwNum = TRANSSW_NUM_DEFAULT;
                        ui->transVertSwNum->setText(TRANSSW_NUM_DEFAULT);
                        sendTransVertSwitchToLogger(TRANSSW_NUM_DEFAULT);
                        logMessage(QString("SetFreq: Transvert Switch not enabled - %1").arg(TRANSSW_NUM_DEFAULT));
                    }



                    // now calculate the freq
                    if (setupRadio->currentRadio.transVertSettings[tvNum]->transVertNegative)
                    {
                           f = f + setupRadio->currentRadio.transVertSettings[tvNum]->transVertOffset;
                           logMessage(QString("SetFreq: Negative Transvert Enabled Freq = %1").arg(QString::number(f)));
                    }
                    else
                    {
                          f = f - setupRadio->currentRadio.transVertSettings[tvNum]->transVertOffset;
                          logMessage(QString("SetFreq: Transvert Enabled Freq = %1").arg(QString::number(f)));
                    }


                }
                else
                {
                    // no transverter found for this band
                    logMessage(QString("SetFreq: No transverter found for this band"));
                    selTvBand = "";
                    displayTransVertVfo(0.0);
                    ui->transVertBandDisp->setText("");
                    transVertSwNum = TRANSSW_NUM_DEFAULT;
                    ui->transVertSwNum->setText(TRANSSW_NUM_DEFAULT);
                    sendTransVertSwitchToLogger(TRANSSW_NUM_DEFAULT);
                    //setTransVertDisplayVisible(true);
                }

            }

            if (radio->get_serialConnected())
            {

                retCode = radio->setFrequency(f, vfo);
                if (retCode != RIG_OK)
                {
                    if (retCode == -9)
                    {
                        logMessage(QString("SetFreq: Invalid Tx Freq for Radio, Freq = %1").arg(QString::number(f)));
                        cmdLockOff();
                        return;
                    }

                    logMessage(QString("SetFreq: Error Setting Freq Code = %1").arg(retCode));
                    hamlibError(retCode, "SetFreq");
                }
                else
                {
                    logMessage(QString("SetFreq: Rig set to Freq = %1").arg(QString::number(f)));
                }

            }
            else
            {
                logMessage(QString("SetFreq: Radio is not connected"));
            }
        }
        else
        {
            logMessage(QString("SetFreq:: Freq conversion from string %1 failed").arg(sfreq));
        }

        cmdLockOff();
    }

    int RigControlMainWindow::getAndSendFrequency(vfo_t vfo)
    {
        double transVertF = 0;
        int retCode = 0;
        int tvNum = 0;


        retCode = radio->getFrequency(vfo, &rfrequency);
        if (retCode == RIG_OK)
        {

            curVfoFrq = rfrequency;

            if (setupRadio->currentRadio.transVertEnable && setupRadio->currentRadio.numTransverters > 0)
            {
                 if (selTvBand != "")
                 {
                    logMessage(QString("Get Freq: Transvert enabled"));
                    // look for supporting transverter
                    bool b = false;
                    while (tvNum < setupRadio->currentRadio.numTransverters)
                    {
                        if (setupRadio->currentRadio.transVertSettings[tvNum]->band == selTvBand)
                        {
                            b = true;
                            break;
                        }
                        tvNum++;
                    }

                    if (b)
                    {
                        if (setupRadio->currentRadio.transVertSettings[tvNum]->transVertNegative)
                        {
                            logMessage(QString("Get Freq: Negative Transvert"));
                            transVertF = rfrequency - setupRadio->currentRadio.transVertSettings[tvNum]->transVertOffset;
                            logMessage(QString("Get Freq: Transvert F = %1").arg(QString::number(transVertF)));
                        }
                        else
                        {
                            logMessage(QString("Get Freq: Positive Transvert"));
                            transVertF = rfrequency + setupRadio->currentRadio.transVertSettings[tvNum]->transVertOffset;

                        }
                    }

                    logMessage(QString("Get Freq: Transvert Freq. = %1").arg(QString::number(transVertF)));
                    curTransVertFrq = transVertF;
                    displayTransVertVfo(transVertF);

                }
                else
                {
                     //setTransVertDisplayVisible(false);
                     logMessage(QString("GetFreq: No transvert band set for this freq = %1").arg(QString::number(curVfoFrq)));
                }


            }



            displayFreqVfo(rfrequency);

            if (setupRadio->currentRadio.transVertEnable)
            {
                sendFreqToLog(transVertF);
            }
            else
            {
                sendFreqToLog(rfrequency);
            }
        }
        else
        {
            logMessage(QString("Get radioInfo: Get Freq error, code = %1").arg(QString::number(retCode)));
            hamlibError(retCode, "Request Frequency");
        }
        return retCode;
    }


    QString RigControlMainWindow::getBand(freq_t freq)
    {
        for (int i = 0; i < setupRadio->bands.count(); i++)
        {
            if (freq >= setupRadio->bands[i]->fLow && freq <= setupRadio->bands[i]->fHigh)
            {
                return setupRadio->bands[i]->name;
            }
        }
        return "";
    }

    void RigControlMainWindow::setTransVertDisplayVisible(bool visible)
    {
        ui->transVertFreqA->setVisible(visible);
        ui->TVertTitleA->setVisible(visible);
        ui->transVertSwNum->setVisible(visible);
        ui->transVertSwLbl->setVisible(visible);
        ui->transVertBandDisp->setVisible(visible);
        ui->transVertBandDispLbl->setVisible(visible);
    }



    void RigControlMainWindow::chkRadioMgmModeChanged()
    {
        static QString mgmMode;
        if (setupRadio->currentRadio.mgmMode != mgmMode)
        {
            ui->mgmMode->setText(setupRadio->currentRadio.mgmMode);
        }
    }


    int RigControlMainWindow::getAndSendMode(vfo_t vfo)
    {

        int retCode = 0;

        retCode = radio->getMode(vfo, &rmode, &rwidth);
        // get passband state
        //hamlibData::pBandState pBState = modePbState[getMinosModeIndex(slogMode)];
        //QString spBState = QString::number(pBState);

        if (retCode == RIG_OK)
        {
            logMessage(QString("Get Mode: From Rx mode = %1, passband = %2").arg(radio->convertModeQstr(rmode)).arg(QString::number(rwidth)));
            curMode = rmode;
            sCurMode = radio->convertModeQstr(rmode);

            if (mgmModeFlag && sCurMode != setupRadio->currentRadio.mgmMode) // has mode been changed on the radio?
            {
                // yes clear MGM mode
                mgmModeFlag = false;

            }


            if (!mgmModeFlag)
            {
                displayModeVfo(radio->convertModeQstr(rmode));
                displayPassband(rwidth);
                sendModeToLog(QString("%1:%2").arg(radio->convertModeQstr(rmode)).arg(" "));
            }
            else
            {

                displayModeVfo(hamlibData::MGM);
                displayPassband(rwidth);
                sendModeToLog(QString("%1:%2").arg(hamlibData::MGM).arg(setupRadio->currentRadio.mgmMode));
            }

       }

        return retCode;
    }

    void RigControlMainWindow::loggerSetMode(QString mode)
    {
        logMessage(QString("Log SetMode:: Mode Recieved from Logger = %1").arg(mode));


        if (radio->get_serialConnected() && !rigErrorFlag)
        {
            slogMode = mode;
            logMode = radio->convertQStrMode(mode);


            if (slogMode == hamlibData::MGM)
            {
                logMessage(QString("Log SetMode:MGM mode Selected"));
                if (mgmModeFlag)
                {
                    logMessage(QString("Log SetMode: Mgm flag is set"));
                    if (curMode !=  radio->convertQStrMode(setupRadio->currentRadio.mgmMode))
                    {
                        setMode(setupRadio->currentRadio.mgmMode, RIG_VFO_CURR);
                    }
                }
                else
                {
                    mgmModeFlag = true;
                    setMode(setupRadio->currentRadio.mgmMode, RIG_VFO_CURR);
                    logMessage((QString("Log SetMode: Set MgmMode Flag, Selected MGM Mode = %1").arg(setupRadio->currentRadio.mgmMode)));
                }
            }
            else
            {
                mgmModeFlag = false;
                logMessage(QString("Log SetMode: Clear mgmModeFlag, Set mode = %1").arg(mode));
                setMode(mode, RIG_VFO_CURR);
            }
        }

    }




    void RigControlMainWindow::setMode(QString mode, vfo_t vfo)
    {
        int retCode = 0;

        cmdLockOn();      // lock get radio info

        logMessage(QString("Setmode: Requested Mode = %1").arg(mode));
        rmode_t mCode = radio->convertQStrMode(mode);




        if (radio->get_serialConnected())
        {
             retCode = radio->setMode(vfo, mCode, PASSBAND_NOCHANGE);
             if (retCode == RIG_OK)
             {
                 logMessage(QString("SetMode: changed! Mode = %1").arg(radio->convertModeQstr(mCode)));

             }
             else
             {
                 logMessage(QString("SetMode: Change Error Code = %1, Mode = %2").arg(QString::number(retCode)).arg(radio->convertModeQstr(mCode)));
                 hamlibError(retCode, "Set Mode");
             }

        }
        else
        {
            logMessage(QString("Set Mode: radio not connected"));
        }
        cmdLockOff();
    }


    int RigControlMainWindow::getMinosModeIndex(QString mode)
    {
        int index = 0;
        for (int i = 0; i < hamlibData::supModeList.count(); i++)
        {
            if (mode == hamlibData::supModeList[i])
            {
                index = i;
                return index;
            }
        }

        return index;

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
        int retCode = 0;
        cmdLockOn();
        retCode = radio->setRit(vfo, ritFreq);

        cmdLockOff();
        return retCode;

    }


    void RigControlMainWindow::displayPassband(pbwidth_t width)
    {

        ui->passBandlbl->setText(QString::number(width));
    }




    void RigControlMainWindow::displayFreqVfo(double frequency)
    {

        ui->radioFreqA->setText(convertFreqStrDisp(convertFreqToStr(frequency)));
    }


    void RigControlMainWindow::displayTransVertVfo(double frequency)
    {
        ui->transVertFreqA->setText(convertFreqStrDisp(convertFreqToStr(frequency)));

    }




    void RigControlMainWindow::displayModeVfo(QString mode)
    {
        ui->modeA->setText(mode);
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
        logMessage(QString("Hamlib Error - Code = %1 - %2").arg(QString::number(errorCode)).arg(errorMsg));

        QMessageBox::critical(this, "RigControl hamlib Error - " + setupRadio->currentRadio.radioName, QString::number(errorCode) + " - " + errorMsg + "\n" + "Command - " + cmd);

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

    void RigControlMainWindow::sendRadioListLogger()
    {
        QStringList radioList;
        for (int i= 0; i < setupRadio->numAvailRadios; i++)
        {
            if (!setupRadio->availRadioData[i]->radioName.isEmpty())
            {
                radioList.append(setupRadio->availRadioData[i]->radioName);
            }
        }
        logMessage(QString("Sending radiolist to logger"));
        for (int i = 0; i < radioList.count(); i++)
        {
            logMessage(QString("Send radiolist - radio %1, name %2").arg(QString::number(i)).arg(radioList[i]));
        }
        msg->publishRadioNames(radioList);
    }



    void RigControlMainWindow::sendBandListLogger()
    {

        QStringList bandList;
        if (!setupRadio->currentRadio.radioTransSupBands.isEmpty())
        {
            for (int i = 0; i < setupRadio->currentRadio.radioTransSupBands.count(); i++)
            {
                bandList.append(setupRadio->currentRadio.radioTransSupBands[i]);
            }
            logMessage(QString("Send bandlist to logger"));
            for (int i = 0; i < bandList.count(); i++)
            {
                logMessage(QString("Send bandlist - name %1").arg(QString::number(i)).arg(bandList[i]));
            }
            msg->publishBandNames(bandList);
        }

    }



    void RigControlMainWindow::sendRadioNameLogger(QString radioName)
    {
        logMessage(QString("Send radioName %1 to Logger").arg(radioName));
        msg->publishRadioName(radioName);
    }

    void RigControlMainWindow::sendStatusLogger(const QString &message )
    {
        if (appName.length() > 0)
        {
            logMessage(QString("Send status to logger = %1").arg(message));
            msg->publishState(message);
        }
    }


    void RigControlMainWindow::sendStatusToLogConnected()
    {
            logMessage(QString("Send status to logger connected"));
            sendStatusLogger(RIG_STATUS_CONNECTED);

    }

    void RigControlMainWindow::sendStatusToLogDisConnected()
    {
            logMessage(QString("Send status to logger disconnected"));
            sendStatusLogger(RIG_STATUS_DISCONNECTED);

    }


    void RigControlMainWindow::sendStatusToLogError()
    {
        logMessage(QString("Send error status to logger"));
        sendStatusLogger(RIG_STATUS_ERROR);
    }


    void RigControlMainWindow::sendErrorMessageToLogger(QString errMsg)
    {

        logMessage(QString("Send error message to logger: %1").arg(errMsg));
        msg->publishErrorMsg(errMsg);

    }

    void RigControlMainWindow::sendFreqToLog(freq_t freq)
    {

        if (appName.length() > 0)
        {
            logMessage(QString("Send freq to logger = %1").arg(convertFreqToStr(freq)));
            msg->publishFreq(convertFreqToStr(freq));
        }
    }

    void RigControlMainWindow::sendModeToLog(QString mode)
    {
        if (appName.length() > 0)
        {
            logMessage(QString("Send mode to logger = %1").arg(mode));
            msg->publishMode(mode);
        }
    }

    void RigControlMainWindow::sendTransVertStatus(bool status)
    {
        QString flag;
        if (appName.length() > 0)
        {
            if (status)
            {
                flag = TXVERT_ON;
            }
            else
            {
                flag = TXVERT_OFF;
            }
            logMessage(QString("Send Transvert Status to logger = %1").arg(flag));
            msg->publishTransVertStatus(flag);
        }
    }

    void RigControlMainWindow::sendTransVertOffsetToLogger(int tvNum)
    {
        QString f = convertFreqToStr(setupRadio->currentRadio.transVertSettings[tvNum]->transVertOffset);
        logMessage(QString("Send Transvert Offset to logger = %1%2").arg(setupRadio->currentRadio.transVertEnable ? f = "-" : f = "+").arg(f));
        msg->publishTransVertOffSetFreq(setupRadio->currentRadio.transVertSettings[tvNum]->transVertNegative, f);

    }

    void RigControlMainWindow::sendTransVertSwitchToLogger(const QString &swNum)
    {
        logMessage(QString("Send Transvert Switch Number to logger = %1").arg(swNum));
         msg->publishTransVertSwitch(swNum);

    }



    void RigControlMainWindow::aboutRigConfig()
    {
        QString msg = QString("*** Rig Configuration ***\n\n");

        if (setupRadio->currentRadio.radioName != "")
        {

            msg.append(QString("App Instance Name  = %1\n").arg(appName));
            msg.append(QString("Hamlib Version = %1\n").arg(radio->gethamlibVersion()));
            msg.append(QString("Radio Name = %1\n").arg(setupRadio->currentRadio.radioName));
            msg.append(QString("Radio Number = %1\n").arg(setupRadio->currentRadio.radioNumber));
            msg.append(QString("Rig Model = %1\n").arg(setupRadio->currentRadio.radioModel));
            msg.append(QString("Rig Number = %1\n").arg(QString::number(setupRadio->currentRadio.radioModelNumber)));
            msg.append(QString("Rig Manufacturer = %1\n").arg(setupRadio->currentRadio.radioMfg_Name));
            if (setupRadio->currentRadio.radioMfg_Name == "Icom")
            {
                if (setupRadio->currentRadio.civAddress == "")
                {
                    msg.append(QString("Icom CIV address = Using Default CIV Address\n"));
                }
                else
                {
                    msg.append(QString("Icom CIV address = %1\n").arg(setupRadio->currentRadio.civAddress));
                }

            }
            msg.append(QString("Rig PortType = %1\n").arg(hamlibData::portTypeList[setupRadio->currentRadio.portType]));
            msg.append(QString("Network Address = %1\n").arg(setupRadio->currentRadio.networkAdd));
            msg.append(QString("Network Port = %1\n").arg(setupRadio->currentRadio.networkPort));
            msg.append(QString("Comport = %1\n").arg(setupRadio->currentRadio.comport));
            msg.append(QString("Baudrate = %1\n").arg(setupRadio->currentRadio.baudrate));
            msg.append(QString("Stop bits = %1\n").arg(QString::number(setupRadio->currentRadio.stopbits)));
            msg.append(QString("Parity = %1\n").arg(radio->getParityCodeNames()[setupRadio->currentRadio.parity]));
            msg.append(QString("Handshake = %1\n").arg(radio->getHandShakeNames()[setupRadio->currentRadio.handshake]));
            QString f = "";
            setupRadio->currentRadio.transVertEnable ? f = "True" : f = "False";
            msg.append(QString("TransVert Enable = %1\n").arg(f));
            msg.append(QString("Number of TransVerters = $1\n").arg(setupRadio->currentRadio.numTransverters));

            for (int i = 0; i < setupRadio->currentRadio.numTransverters; i++)
            {
                msg.append(QString("Transverter %1\n").arg(i));
                msg.append(QString("Transverter Name = %1\n").arg(setupRadio->currentRadio.transVertSettings[i]->transVertName));
                msg.append(QString("Transverter Band = %1\n").arg(setupRadio->currentRadio.transVertSettings[i]->band));
                msg.append(QString("Transverter Offset = %1\n").arg(setupRadio->currentRadio.transVertSettings[i]->transVertOffsetStr));
                setupRadio->currentRadio.transVertSettings[i]->transVertNegative  ? f = "True" : f = "False";
                msg.append(QString("Transverter Name = %1\n").arg(f));
                msg.append(QString("Transverter Switch num = %1\n").arg(setupRadio->currentRadio.transVertSettings[i]->transSwitchNum));
                setupRadio->currentRadio.transVertSettings[i]->enableTransSwitch  ? f = "True" : f = "False";
                msg.append(QString("Transverter Switch enable = %1\n").arg(f));
            }


            msg.append(QString("Use RX Passband = %1\n").arg(f));
            supRitFlag ? f = "True" : f = "False";
            msg.append(QString("Radio Supports RIT = %1\n").arg(f));
            ui->actionTraceLog->isChecked() ? f = "True" : f = "False";
            msg.append(QString("Tracelog = %1\n").arg(f));
        }
        else
        {
            msg.append(QString("No radio selected"));
        }


        QMessageBox::about(this, "Minos RigControl", msg);


    }




    void RigControlMainWindow::dumpRadioToTraceLog()
    {

        if (setupRadio->currentRadio.radioName != "")
        {
            trace("*** Radio Update ***");
            trace(QString("App Instance Name  = %1").arg(appName));
            trace(QString("Hamlib Version = %1").arg(radio->gethamlibVersion()));
            trace(QString("Radio Name = %1").arg(setupRadio->currentRadio.radioName));
            trace(QString("Radio Number = %1").arg(setupRadio->currentRadio.radioNumber));
            trace(QString("Rig Model = %1").arg(setupRadio->currentRadio.radioModel).trimmed());
            trace(QString("Rig Number = %1").arg(QString::number(setupRadio->currentRadio.radioModelNumber)));
            trace(QString("Rig Manufacturer = %1").arg(setupRadio->currentRadio.radioMfg_Name));
            if (setupRadio->currentRadio.radioMfg_Name == "Icom")
            {
                if (setupRadio->currentRadio.civAddress == "")
                {
                    trace(QString("Icom CIV address = Using Default CIV Address"));
                }
                else
                {
                    trace(QString("Icom CIV address = %1").arg(setupRadio->currentRadio.civAddress));
                }

            }
            trace(QString("Rig PortType = %1").arg(hamlibData::portTypeList[setupRadio->currentRadio.portType]));
            trace(QString("Network Address = %1").arg(setupRadio->currentRadio.networkAdd));
            trace(QString("Network Port = %1").arg(setupRadio->currentRadio.networkPort));
            trace(QString("Comport = %1").arg(setupRadio->currentRadio.comport));
            trace(QString("Baudrate = %1").arg(setupRadio->currentRadio.baudrate));
            trace(QString("Stop bits = %1").arg(QString::number(setupRadio->currentRadio.stopbits)));
            trace(QString("Parity = %1").arg(radio->getParityCodeNames()[setupRadio->currentRadio.parity]));
            trace(QString("Handshake = %1").arg(radio->getHandShakeNames()[setupRadio->currentRadio.handshake]));
            QString f = "";
            setupRadio->currentRadio.transVertEnable ? f = "True" : f = "False";
            trace(QString("TransVert Enable = %1").arg(f));
            trace(QString("Number of TransVerters = $1").arg(setupRadio->currentRadio.numTransverters));
            for (int i = 0; i < setupRadio->currentRadio.numTransverters; i++)
            {
                trace(QString("Transverter %1").arg(i));
                trace(QString("Transverter Name = %1").arg(setupRadio->currentRadio.transVertSettings[i]->transVertName));
                trace(QString("Transverter Band = %1").arg(setupRadio->currentRadio.transVertSettings[i]->band));
                trace(QString("Transverter Offset = %1").arg(setupRadio->currentRadio.transVertSettings[i]->transVertOffsetStr));
                setupRadio->currentRadio.transVertSettings[i]->transVertNegative  ? f = "True" : f = "False";
                trace(QString("Transverter Name = %1").arg(f));
                trace(QString("Transverter Switch num = %1").arg(setupRadio->currentRadio.transVertSettings[i]->transSwitchNum));
                setupRadio->currentRadio.transVertSettings[i]->enableTransSwitch  ? f = "True" : f = "False";
                trace(QString("Transverter Switch enable = %1").arg(f));
            }

            trace(QString("Use RX Passband = %1").arg(f));
            supRitFlag ? f = "True" : f = "False";
            trace(QString("Radio Supports RIT = %1").arg(f));

            ui->actionTraceLog->isChecked() ? f = "True" : f = "False";
            trace(QString("Tracelog = %1").arg(f));

        }
        else
        {
            trace(QString("No radio selected"));
        }


    }


    /************************** Band Information ***********************************/

    void RigControlMainWindow::loadBands()
    {
        BandList &blist = BandList::getBandList();

        for (int i = 5; i < 15; i++)   // just load VHF/UHF bands
        {
            bands.append(new BandDetail(blist.bandList[i].adif, blist.bandList[i].flow, blist.bandList[i].fhigh));
        }

    }




    void delay(int sec)
    {
        QTime dieTime= QTime::currentTime().addSecs(sec);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }


    /*********************************** test *********************************************/


    void RigControlMainWindow::selFreqClicked()
    {


        // check freq valid format
        QString f = ui->freqInputBox->text().trimmed().remove( QRegExp("^[0]*"));


        if (valInputFreq(f, "Invalid freq!"))
        {
            // convert radio freq
            f = convertSinglePeriodFreqToFullDigit(f).remove('.');
            setFreq(f, RIG_VFO_CURR);
        }


    }


    void RigControlMainWindow::testBoxesVisible(bool visible)
    {
        ui->selFreq->setVisible(visible);
        ui->freqInputBox->setVisible(visible);

    }
