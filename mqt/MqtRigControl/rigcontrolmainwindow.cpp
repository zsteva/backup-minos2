/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2007
//
// Hamlib Library
//
/////////////////////////////////////////////////////////////////////////////

#include "base_pch.h"
#include "mqtUtils_pch.h"
#include "RPCCommandConstants.h"
#include "rigcontrolmainwindow.h"
#include "ui_rigcontrolmainwindow.h"
#include "rigcontrol.h"
#include "setupdialog.h"
#include "rigcontrolrpc.h"
#include <QTimer>
#include <QMessageBox>
#include <QDebug>


RigControlMainWindow::RigControlMainWindow(QString _loggerRadio, QWidget *parent) :
    QMainWindow(parent), msg(0),
    ui(new Ui::RigControlMainWindow)
{
    ui->setupUi(this);

    loggerRadio  = _loggerRadio.trimmed();

    connect(&stdinReader, SIGNAL(stdinLine(QString)), this, SLOT(onStdInRead(QString)));
    stdinReader.start();

    createCloseEvent();

    msg = new RigControlRpc(this);

    QSettings settings;
    geoStr = "geometry";
    if (loggerRadio.length() > 0)
    {
        geoStr = geoStr + loggerRadio;
    }

    QByteArray geometry = settings.value("geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);



    radio = new RigControl();
    selectRig = new SetupDialog(radio);
    selectRadio = new QComboBox;
    pollTimer = new QTimer(this);

    status = new QLabel;
    ui->statusBar->addWidget(status);




    radio->set_serialConnected(false);
    initActionsConnections();

    selectRadio = ui->selectRadioBox;

    if (loggerRadio.length() > 0)
    {
        ui->selectRadioBox->hide();
        ui->SelectRadioTitle->hide();
    }

    initSelectRadioBox();

    if (loggerRadio.length() > 0)
    {
        int a = selectRadio->findText(loggerRadio);
        if (a == -1)
        {
            showStatusMessage("<font color='Red'>Invalid radio name from logger!</font>");
            logRadError = true;
        }
        else
        {
            selectRadio->setCurrentIndex(a);
            selectRig->readSettings();      // get antenna settings
            selectRig->copyRadioToCurrent(a);
        }

    }
    else
    {

        selectRig->readCurrentRadio();
        selectRadio->setCurrentIndex(selectRadio->findText(selectRig->currentRadio.radioName));
    }



    setPolltime(250);

 //   openRadio();

    readTraceLogFlag();

    trace("*** Rig Started ***");

    if (!logRadError)
    {
        upDateRadio();

    }




}



RigControlMainWindow::~RigControlMainWindow()
{
    delete ui;
}

void RigControlMainWindow::logMessage( QString s )
{
   if (ui->actionTraceLog->isChecked())
        trace( s );
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
    connect(this, SIGNAL(mode_updated(QString)), this, SLOT(displayModeVfoA(QString)));


    //connect(this, SIGNAL(frequency_updated(double)), this, SLOT(drawDial(double)));
    //connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    //connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    //connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
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
    int radioIndex = ui->selectRadioBox->currentIndex();
    if (radioIndex > 0)
    {
        radioIndex -= 1;
        if (selectRig->availRadios[radioIndex].radioModelNumber == 0)
        {
            closeRadio();
            QMessageBox::critical(this, tr("Radio Error"), "Please select a radio model");
            return;
        }
        selectRig->currentRadio.radioName = selectRig->availRadios[radioIndex].radioName;
        selectRig->currentRadio.radioNumber = selectRig->availRadios[radioIndex].radioNumber;
        selectRig->currentRadio.radioMfg_Name = selectRig->availRadios[radioIndex].radioMfg_Name;
        selectRig->currentRadio.radioModel = selectRig->availRadios[radioIndex].radioModel;
        selectRig->currentRadio.radioModelNumber = selectRig->availRadios[radioIndex].radioModelNumber;
        selectRig->currentRadio.civAddress = selectRig->availRadios[radioIndex].civAddress;
        selectRig->currentRadio.comport = selectRig->availRadios[radioIndex].comport;
        selectRig->currentRadio.baudrate = selectRig->availRadios[radioIndex].baudrate;
        selectRig->currentRadio.databits = selectRig->availRadios[radioIndex].databits;
        selectRig->currentRadio.stopbits = selectRig->availRadios[radioIndex].stopbits;
        selectRig->currentRadio.parity = selectRig->availRadios[radioIndex].parity;
        selectRig->currentRadio.handshake = selectRig->availRadios[radioIndex].handshake;
        selectRig->currentRadio.transVertEnable = selectRig->availRadios[radioIndex].transVertEnable;
        selectRig->currentRadio.transVertNegative = selectRig->availRadios[radioIndex].transVertNegative;
        selectRig->currentRadio.transVertOffset = selectRig->availRadios[radioIndex].transVertOffset;
        selectRig->currentRadio.transVertOffsetStr = selectRig->availRadios[radioIndex].transVertOffsetStr;
        selectRig->saveCurrentRadio();
        if (radio->get_serialConnected())
       {
                closeRadio();
       }
       openRadio();
       trace("*** Radio Update ***");
       trace("Radio Name = " + selectRig->currentRadio.radioName);
       trace("Radio Number = " + selectRig->currentRadio.radioNumber);
       trace("Radio Model = " + selectRig->currentRadio.radioModel);
       trace("Radio Number = " + QString::number(selectRig->currentRadio.radioModelNumber));
       trace("Radio Manufacturer = " + selectRig->currentRadio.radioMfg_Name);
       trace("Radio Comport = " + selectRig->currentRadio.comport);
       trace("Baudrate = " + QString::number(selectRig->currentRadio.databits));
       trace("Stop bits = " + QString::number(selectRig->currentRadio.stopbits));
       trace("Handshake = " + QString::number(selectRig->currentRadio.handshake));
       trace("TransVert Enable = " + QString::number(selectRig->currentRadio.transVertEnable));
       trace("TransVert Negative = " + QString::number(selectRig->currentRadio.transVertNegative));
       trace("TransVert Offset = " + convertStringFreq(selectRig->currentRadio.transVertOffset));


    }


}

void RigControlMainWindow::openRadio()
{

    int retCode = 0;

    if (selectRig->currentRadio.radioName == "")
    {
        showStatusMessage("Please select a Radio");
        return;
    }
    if(selectRig->currentRadio.comport == "")
    {
        showStatusMessage("Please select a Comport");
        return;
    }
    if (selectRig->currentRadio.radioModel == "")
    {
        showStatusMessage("Please select a radio model");
        return;
    }



    scatParams p = selectRig->getCurrentRadio();

    retCode = radio->init(selectRig->currentRadio);
    if (retCode < 0)
    {
        hamlibError(retCode);
    }
    if (radio->get_serialConnected())
    {

        pollTimer->start(pollTime);             // start timer to send message to controller
        showStatusMessage(tr("Connected to Radio: %1 - %2, %3, %4, %5, %6, %7, %8")
                              .arg(p.radioName).arg(p.radioModel).arg(p.comport).arg(p.baudrate).arg(p.databits)
                              .arg(p.stopbits).arg(radio->getParityCodeNames()[p.parity]).arg(radio->getHandShakeNames()[p.handshake]));

    }
    else
    {
//        QMessageBox::critical(this, tr("Error"), serial->errorString());
        showStatusMessage(tr("Radio Open error"));
    }


}

void RigControlMainWindow::closeRadio()
{
    radio->closeRig();
    showStatusMessage(tr("Disconnected"));
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
    getFrequency(RIG_VFO_CURR);

    getMode(RIG_VFO_CURR);

}


void RigControlMainWindow::getFrequency(vfo_t vfo)
{
    double transVertF = 0;
    int retCode = 0;
    if (radio->get_serialConnected())
    {
        retCode = radio->getFrequency(vfo, &rfrequency);
        if (retCode == RIG_OK)
        {
            if (rfrequency != curVfoFrq[0])
            {
                curVfoFrq[0] = rfrequency;
                qDebug() << "Trans Enable = " << selectRig->currentRadio.transVertEnable;
                if (selectRig->currentRadio.transVertEnable)
                {
                    qDebug() << "Transvert enabled";
                    if (selectRig->currentRadio.transVertNegative)
                    {
                        transVertF = rfrequency - selectRig->currentRadio.transVertOffset;

                    }
                    else
                    {
                        transVertF = rfrequency + selectRig->currentRadio.transVertOffset;
                    }
                    qDebug() << "Transvert f " << transVertF;
                    curTransVertFrq[0] = transVertF;
                    displayTransVertVfoA(transVertF);
                }
                displayFreqVfoA(rfrequency);


            }
            else
            {
                return;
            }
        }
        else
        {
            hamlibError(retCode);
        }
    }


}




void RigControlMainWindow::getMode(vfo_t vfo)
{

    int retCode = 0;

    if (radio->get_serialConnected())
    {
        retCode = radio->getMode(vfo, &rmode, &rwidth);
        if (retCode == RIG_OK)
        {
            if (rmode != curMode[0])
            {
                curMode[0] = rmode;
                displayModeVfoA(radio->convertModeQstr(rmode));
            }
            else
            {
                return;
            }

        }
        else
        {
            hamlibError(retCode);
        }
    }

}

void RigControlMainWindow::getCurMode()
{
    getMode(RIG_VFO_CURR);
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

void RigControlMainWindow::displayFreqVfoA(double frequency)
{

    qDebug() << "Got freq " << frequency;
    ui->radioFreqA->setText(convertStringFreq(frequency));
}

void RigControlMainWindow::displayFreqVfoB(double frequency)
{

    ui->radioFreqB->setText(convertStringFreq(frequency));
}


void RigControlMainWindow::displayTransVertVfoA(double frequency)
{
    ui->transVertFreqA->setText(convertStringFreq(frequency));
}


void RigControlMainWindow::displayTransVertVfoB(double frequency)
{
    ui->transVertFreqB->setText(convertStringFreq(frequency));
}

void RigControlMainWindow::displayModeVfoA(QString mode)
{
    ui->modeA->setText(mode);
}

void RigControlMainWindow::displayModeVfoB(QString mode)
{
    ui->modeB->setText(mode);
}

QString RigControlMainWindow::convertStringFreq(double frequency)
{
    double freq = frequency;
    sfreq = sfreq = QString::number(freq,'f', 0);
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






void RigControlMainWindow::hamlibError(int errorCode)
{

    pollTimer->stop();

    int errCode = errorCode;
    if ( errCode >= 0)
    {
        return;
    }

    errCode *= -1;
    QString errorMsg = radio->gethamlibErrorMsg(errCode);

    QMessageBox::critical(this, "hamlib Error", QString::number(errCode) + " - " + errorMsg);

    closeRadio();

}



void RigControlMainWindow::readTraceLogFlag()
{
    QSettings config(RIG_CONTROL_CONFIG, QSettings::IniFormat);
    config.beginGroup("TraceLog");


    ui->actionTraceLog->setChecked(config.value("TraceLog", false).toBool());

    config.endGroup();
}

void RigControlMainWindow::saveTraceLogFlag()
{
    qDebug() << "traceLog selected";
    QSettings config(RIG_CONTROL_CONFIG, QSettings::IniFormat);
    config.beginGroup("TraceLog");

    config.setValue("TraceLog", ui->actionTraceLog->isChecked());

    config.endGroup();
}





void RigControlMainWindow::about()
{
    QMessageBox::about(this, "Minos RigControl", "Minos QT RigControl\nCopyright D Balharrie G8FKH/M0DGB 2017\nVersion 0.1");
}
