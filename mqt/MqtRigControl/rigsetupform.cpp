/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2018
//
//
//
//
/////////////////////////////////////////////////////////////////////////////



#include "rigsetupform.h"
#include "ui_rigsetupform.h"
#include <QDebug>
#include <QLineEdit>
#include <QCheckBox>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QInputDialog>


static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");



RigSetupForm::RigSetupForm(RigControl* _radio, scatParams* _radioData, QWidget *parent):
    QWidget(parent),
    ui(new Ui::rigSetupForm),
    radioValueChanged(false),
    radioNameChanged(false)
{


    ui->setupUi(this);

    radio = _radio;
    radioData = _radioData;
    transVerter = new TransVertSetupForm;


    fillRadioModelInfo();  // add radio models to drop down
    fillPortsInfo();     // add comports to drop down
    fillSpeedInfo();
    fillDataBitsInfo();
    fillStopBitsInfo();
    fillParityInfo();
    fillHandShakeInfo();
    fillMgmModes();

    connect(ui->radioModelBox, SIGNAL(activated(int)), this, SLOT(radioModelSelected()));
    connect(ui->comPortBox, SIGNAL(activated(int)), this, SLOT(comportSelected()));
    connect(ui->comSpeedBox, SIGNAL(activated(int)), this, SLOT(comSpeedSelected()));
    connect(ui->comDataBitsBox, SIGNAL(activated(int)), this, SLOT(comDataBitsSelected()));
    connect(ui->comStopBitsBox, SIGNAL(activated(int)), this, SLOT(comStopBitsSelected()));
    connect(ui->comParityBox, SIGNAL(activated(int)), this, SLOT(comParitySelected()));
    connect(ui->comHandShakeBox, SIGNAL(activated(int)), this, SLOT(comHandShakeSelected()));
    connect(ui->networkAddBox, SIGNAL(editingFinished()), this, SLOT(networkAddressSelected()));
    connect(ui->netPortBox, SIGNAL(editingFinished()), this, SLOT(networkPortSelected()));
    connect(ui->pollInterval, SIGNAL(activated(int)), this, SLOT(pollIntervalSelected()));

    // transvert
    connect(ui->addTransvert, SIGNAL(clicked(bool)), this, SLOT(addTransVerter()));
    connect(ui->removeTransvert, SIGNAL(clicked(bool)), this, SLOT(removeTransVerter()));
    connect(ui->renameTransvert, SIGNAL(clicked(bool)), this, SLOT(renameTransVerter()));
}


RigSetupForm::~RigSetupForm()
{
    delete ui;
}


void RigSetupForm::radioModelSelected()
{
    int rm;
    QString radioModelName;
    QString radioMfgName;
    if (ui->radioModelBox->currentText() != radioData->radioModel)
    {
        radioData->radioModel = ui->radioModelBox->currentText();
        if (radio->getModelInfo(radioData->radioModel, &rm, &radioMfgName, &radioModelName) == -1)
        {
            // error
            radioData->radioModelNumber = 0;
            radioData->radioModelName = "";
            radioData->radioMfg_Name = "";
        }
        else
        {
            radioData->radioModelNumber = rm;
            radioData->radioModelName = radioModelName;
            radioData->radioMfg_Name = radioMfgName;

        }

        if (radioData->radioMfg_Name == "Icom")
        {
            ui->CIVlineEdit->setEnabled(true);
        }
        else
        {
            ui->CIVlineEdit->setDisabled(true);
        }

        rig_port_e portType = RIG_PORT_NONE;

        if (radio->getPortType(radioData->radioModelNumber, &portType) != -1)
        {
            radioData->portType = int(portType);
            if (portType == RIG_PORT_NETWORK || portType == RIG_PORT_UDP_NETWORK)
            {
               serialDataEntryVisible(false);
               networkDataEntryVisible(true);
            }
            else if (portType == RIG_PORT_SERIAL)
            {
                serialDataEntryVisible(true);
                networkDataEntryVisible(false);
            }
            else // RIG_PORT_NONE
            {
                serialDataEntryVisible(false);
                networkDataEntryVisible(false);
            }
        }

        radioValueChanged = true;


    }
}




void RigSetupForm::civSetToolTip()
{

        ui->CIVlineEdit->setToolTip(civToolTip);

}

QString RigSetupForm::getCIVAddress()
{
    return ui->CIVlineEdit->text();
}

void RigSetupForm::setCIVAddress(QString v)
{
    ui->CIVlineEdit->setText(v);
}


void RigSetupForm::enableCIVEdit(bool enable)
{
    ui->CIVlineEdit->setEnabled(enable);
}




/***************** Comports ****************************/



void RigSetupForm::comportSelected()
{

    if (ui->comPortBox->currentText() != radioData->comport)
    {
        radioData->comport = ui->comPortBox->currentText();
        radioValueChanged = true;
    }
}

QString RigSetupForm::getComport()
{
    return ui->comPortBox->currentText();
}

void RigSetupForm::setComport(QString p)
{
    ui->comPortBox->setCurrentIndex(ui->comPortBox->findText(p));
}

/***************************** Data Speed *************************/


void RigSetupForm::comSpeedSelected()
{
    radioData->baudrate = ui->comSpeedBox->currentText().toInt();
    radioValueChanged = true;
}


QString RigSetupForm::getDataSpeed()
{
    ui->comSpeedBox->currentText();
}

void RigSetupForm::setDataSpeed(QString d)
{
    ui->comSpeedBox->setCurrentIndex(ui->comSpeedBox->findText(d));
}

/***************************** Data Bits *************************/

void RigSetupForm::comDataBitsSelected()
{
    radioData->databits = ui->comDataBitsBox->currentText().toInt();
    radioValueChanged = true;

}

QString RigSetupForm::getDataBits()
{
    return ui->comDataBitsBox->currentText();
}


void RigSetupForm::setDataBits(QString d)
{
    ui->comDataBitsBox->setCurrentIndex(ui->comDataBitsBox->findText(d));
}


/***************************** Stop Bits *************************/

void RigSetupForm::comStopBitsSelected()
{
    radioData->stopbits = ui->comStopBitsBox->currentText().toInt();
    radioValueChanged = true;
}

QString RigSetupForm::getStopBits()
{
    return ui->comStopBitsBox->currentText();
}

void RigSetupForm::setStopBits(QString stop)
{
    ui->comStopBitsBox->setCurrentIndex(ui->comStopBitsBox->findText(stop));
}

/***************************** Parity Bits *************************/


void RigSetupForm::comParitySelected()
{

    radioData->parity = radio->getSerialParityCode(ui->comParityBox->currentIndex());
    radioValueChanged = true;
}

QString RigSetupForm::getParityBits()
{
    return ui->comParityBox->currentText();
}

void RigSetupForm::setParityBits(int b)
{
    ui->comParityBox->setCurrentIndex(b);
}

/***************************** Handshake *************************/

void RigSetupForm::comHandShakeSelected()
{
    radioData->handshake = radio->getSerialHandshakeCode(ui->comHandShakeBox->currentIndex());
    radioValueChanged = true;

}


QString RigSetupForm::getHandshake()
{
    return ui->comHandShakeBox->currentText();
}

void RigSetupForm::setHandshake(int h)
{
    ui->comHandShakeBox->setCurrentIndex(h);
}

// returns -1 if no text found
int RigSetupForm::comportAvial(QString comport)
{
    return ui->comPortBox->findText(comport);
}

/***************************** Network Address *************************/

void RigSetupForm::networkAddressSelected()
{

    radioData->networkAdd = ui->networkAddBox->text();
    radioValueChanged = true;
}

QString RigSetupForm::getNetAddress()
{
    return ui->networkAddBox->text();
}

void RigSetupForm::setNetAddress(QString netAdd)
{
    ui->networkAddBox->setText(netAdd);
}
/***************************** Network Port *************************/


void RigSetupForm::networkPortSelected()
{
    radioData->networkPort = ui->netPortBox->text();
    radioValueChanged = true;
}

QString RigSetupForm::getNetPortNum()
{
    return ui->netPortBox->text();
}

void RigSetupForm::setNetPortNum(QString p)
{
    ui->netPortBox->setText(p);
}

/************************* MGM Mode *********************************/



void RigSetupForm::mgmModeSelected()
{
    radioData->mgmMode = ui->mgmBox->currentText();
    radioValueChanged = true;
}

QString RigSetupForm::getMgmMode()
{
    return ui->mgmBox->currentText();
}

void RigSetupForm::setMgmMode(QString p)
{
    ui->mgmBox->setCurrentText(p);
}

/***************************** Poll Interval *************************/

void RigSetupForm::pollIntervalSelected()
{
    if (radioData->pollInterval != ui->pollInterval->currentText())
    {
        radioData->pollInterval = ui->pollInterval->currentText();
        radioValueChanged = true;

    }
}

QString RigSetupForm:: getPollInterval()
{
    return ui->pollInterval->currentText();
}

void RigSetupForm::setPollInterval(QString i)
{
    ui->pollInterval->setCurrentIndex(ui->pollInterval->findText(i));
}

void RigSetupForm::pollIntervalVisible(bool s)
{
    ui->pollInterval->setVisible(s);
    ui->pollIntervalLbl->setVisible(s);
}
/*************************** Serial Data Entry Visible ***************/

void RigSetupForm::serialDataEntryVisible(bool v)
{
    ui->comPortBox->setVisible(v);
    ui->comportLbl->setVisible(v);
    ui->comSpeedBox->setVisible(v);
    ui->speedLbl->setVisible(v);
    ui->comDataBitsBox->setVisible(v);
    ui->dataLbll->setVisible(v);
    ui->comStopBitsBox->setVisible(v);
    ui->stopLbl->setVisible(v);
    ui->comParityBox->setVisible(v);
    ui->parityLbl->setVisible(v);
    ui->comHandShakeBox->setVisible(v);
    ui->handshakeLbl->setVisible(v);
}

/*************************** Network Data Entry Visible ***************/

void RigSetupForm::networkDataEntryVisible(bool v)
{
    ui->networkAddBox->setVisible(v);
    ui->networkAddLbl1->setVisible(v);
    ui->netPortBox->setVisible(v);
    ui->netPortLbl->setVisible(v);
}



void RigSetupForm::fillRadioModelInfo()
{
    ui->radioModelBox->clear();
    radio->getRigList(ui->radioModelBox);

}

void RigSetupForm::fillPollInterValInfo()
{
    QStringList pollTimeStr;
    pollTimeStr << "0.5" << "1" << "2" << "3";

    ui->pollInterval->clear();
    ui->pollInterval->addItems(pollTimeStr);
}


void RigSetupForm::fillPortsInfo()
{

    ui->comPortBox->clear();

    QString description;
    QString manufacturer;
    QString serialNumber;

    ui->comPortBox->addItem("");

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);


        ui->comPortBox->addItem(list.first(), list);

    }


}




void RigSetupForm::fillSpeedInfo()
{
    QStringList baudrateStr = radio->getBaudRateNames();
    ui->comSpeedBox->clear();
    ui->comSpeedBox->addItems(baudrateStr);
}

void RigSetupForm::fillDataBitsInfo()
{

    QStringList databitsStr = radio->getDataBitsNames();
    ui->comDataBitsBox->clear();
    ui->comDataBitsBox->addItems(databitsStr);
}

void RigSetupForm::fillStopBitsInfo()
{
    QStringList stopbitsStr = radio->getStopBitsNames();
    ui->comStopBitsBox->clear();
    ui->comStopBitsBox->addItems(stopbitsStr);
}



void RigSetupForm::fillParityInfo()
{

    QStringList parityStr = radio->getParityCodeNames();
    ui->comParityBox->clear();
    ui->comParityBox->addItems(parityStr);
}



void RigSetupForm::fillHandShakeInfo()
{

    QStringList handshakeStr = radio->getHandShakeNames();
    ui->comHandShakeBox->clear();
    ui->comHandShakeBox->addItems(handshakeStr);
}


void RigSetupForm::fillMgmModes()
{
    QString fileName;

    fileName = RIG_CONFIGURATION_FILEPATH_LOGGER + MINOS_RADIO_CONFIG_FILE;
    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup("MGM_Modes");


    QStringList mgmModes = config.value("MgmModes", "").toStringList();

    config.endGroup();

     ui->mgmBox->clear();
     ui->mgmBox->addItems(mgmModes);
}



/******************* Transverter ***********************************/


void RigSetupForm::addTransVerter()
{
    QString transVerterName = QInputDialog::getText(this, tr("Enter Transverter Name"), tr("Please enter a Transverter Name:"), QLineEdit::Normal);
    transVerterName = transVerterName.trimmed();
    if (transVerterName.isEmpty())
    {
          return;
    }
    if (checkTransVerterNameMatch(transVerterName))
    {
        // error empty name or name already exists
        QMessageBox::information(this, tr("Transverter Name Exists"),
                                 tr("Transverter Name: %1, already exists \nPlease enter another name").arg(transVerterName.trimmed()),
                                  QMessageBox::Ok|QMessageBox::Default,
                                  QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }


    // add the new transverter
    int tabNum = numAvailTransVerters;
    addTransVertTab(tabNum, transVerterName);
    numAvailTransVerters++;
    //loadSettingsToTransVertTab(tabNum);
    //saveTransVerter(tabNum);
    emit transVertTabChanged();

}


void RigSetupForm::addTransVertTab(int tabNum, QString tabName)
{
    transVertData.append(new TransVertParams);
    transVertData[tabNum]->transVertName = tabName;
    transVertTab.append(new TransVertSetupForm());
    ui->transVertTab->insertTab(tabNum, transVertTab[tabNum], tabName);
    ui->transVertTab->setTabColor(tabNum, Qt::darkBlue);      // radioTab promoted to QLogTabWidget


}


bool RigSetupForm::checkTransVerterNameMatch(QString transVertName)
{
    for (int i = 0; i < numAvailTransVerters; i++)
    {
        if (ui->transVertTab->tabText(i) == transVertName)
            return true;
    }

    return false;
}


void RigSetupForm::removeTransVerter()
{




}



void RigSetupForm::renameTransVerter()
{




}
