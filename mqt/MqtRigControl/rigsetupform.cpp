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
#include "BandList.h"
#include "addtransverterdialog.h"
#include "rigutils.h"
#include <QDebug>
#include <QLineEdit>
#include <QCheckBox>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QInputDialog>


//static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");



RigSetupForm::RigSetupForm(RigControl* _radio, scatParams* _radioData, const QVector<BandDetail*> _bands, QWidget *parent):
    QWidget(parent),
    ui(new Ui::rigSetupForm)
{


    ui->setupUi(this);

    radio = _radio;
    radioData = _radioData;

    bands = _bands;


    fillRadioModelInfo();  // add radio models to drop down
    fillPortsInfo(ui->comPortBox);     // add comports to drop down
    fillPortsInfo(ui->locTVComPortSel);
    fillSpeedInfo();
    fillDataBitsInfo();
    fillStopBitsInfo();
    fillParityInfo();
    fillHandShakeInfo();
    fillPollInterValInfo();
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
    connect(ui->enableTransVert, SIGNAL(clicked(bool)), this, SLOT(enableTransVertSelected(bool)));
    connect(ui->mgmBox, SIGNAL(activated(int)), this, SLOT(mgmModeSelected()));
    connect(ui->CIVlineEdit, SIGNAL(editingFinished()), this, SLOT(civAddressFinished()));

    // transvert
    connect(ui->enableTransVertSw, SIGNAL(clicked(bool)), this, SLOT(enableTransVertSwSel(bool)));
    connect(ui->locTvConChk, SIGNAL(clicked(bool)), this, SLOT(localTransVertSwSel(bool)));
    connect(ui->locTVComPortSel, SIGNAL(activated(int)), this, SLOT(locTVComPortSel(int)));


    connect(ui->addTransvert, SIGNAL(clicked(bool)), this, SLOT(addTransVerter()));
    connect(ui->removeTransvert, SIGNAL(clicked(bool)), this, SLOT(removeTransVerter()));
    connect(ui->changeBand, SIGNAL(clicked(bool)), this, SLOT(changeBand()));

}


RigSetupForm::~RigSetupForm()
{
    delete ui;
}





scatParams* RigSetupForm::getRadioData()
{
    return radioData;
}




/************************ Radio Model Dialogue *********************/


void RigSetupForm::radioModelSelected()
{

    setupRadioModel(ui->radioModelBox->currentText());



}


void RigSetupForm::setupRadioModel(QString radioModel)
{

    int rm = 0;
    QString radioModelName;
    QString radioMfgName;
    if (radioModel != radioData->radioModel)
    {
        radioData->radioModel = radioModel;
        ui->radioModelBox->setCurrentText(radioModel);

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
            CIVEditVisible(true);
        }
        else
        {
            CIVEditVisible(false);
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

        // does this radio support antenna sw?
        bool antSwFlg = false;
        int retCode = 0;
        retCode = radio->supportAntSw(radioData->radioModelNumber, &antSwFlg);
        for (int i = 0; i < radioData->numTransverters; i++)
        {
            if (retCode >= 0 && antSwFlg)
            {
               //transVertTab[i]->antSwNumVisible(true);
               radioData->antSwitchAvail = true;
            }
            else
            {
               //transVertTab[i]->antSwNumVisible(false);
               radioData->antSwitchAvail = false;
            }
        }


        setTransVertSelected(radioData->transVertEnable);
        buildSupBandList();
        radioValueChanged = true;
    }


}

QString RigSetupForm::getRadioModel()
{
    return ui->radioModelBox->currentText();
}


void RigSetupForm::setRadioModel(QString m)
{
    ui->radioModelBox->setCurrentIndex(ui->radioModelBox->findText(m));
    if (radioData->radioMfg_Name == "Icom")
    {
        CIVEditVisible(true);
    }
    else
    {
        CIVEditVisible(false);
    }


}

// build the supported band list including transverters
void RigSetupForm::buildSupBandList()
{
    // find the bands the radio supports
    buildSupportedRadioBands(radioData->radioModelNumber);

    // merge radio bands and transverter bands
    radioData->radioTransSupBands.clear();
    if (bands.count() > 0)
    {
        for (int i = 0; i < bands.count(); i++)
        {
            if (findSupRadioBand(bands[i]->name) ||  findSupTransBand(bands[i]->name))
            {
                radioData->radioTransSupBands.append(bands[i]->name);
            }
        }
    }
}


// probe radio for supported bands
void RigSetupForm::buildSupportedRadioBands(int radioModelNumber)
{

    radioData->radioSupBands.clear();

    RIG *my_rig = rig_init(radioModelNumber);
    if (my_rig)
    {

        for (int i = 0; i < bands.count(); i++)
        {
            if (radio->chkFreqRange(my_rig, bands[i]->fLow, "USB"))
            {
                radioData->radioSupBands.append(bands[i]->name);
            }
        }
    }

}

// is this band in the supported band list for this model
bool RigSetupForm::findSupRadioBand(const QString band)
{
    if (radioData->radioSupBands.count() > 0)
    {
        for (int i = 0; i < radioData->radioSupBands.count();i++)
        {
            if (band == radioData->radioSupBands[i])
            {
                return true;
            }
        }

        return false;
    }

    return false;
}

// is this band in the transverter list for this radio
bool RigSetupForm::findSupTransBand(const QString band)
{
    if (radioData->numTransverters > 0)
    {
        for (int i = 0; i < radioData->numTransverters;i++)
        {
            if (band == radioData->transVertNames[i])
            {
                return true;
            }
        }

        return false;
    }

    return false;
}


/********************** CIV Entry ***********************/


void RigSetupForm::civAddressFinished()
{

    bool Ok;

    ui->CIVlineEdit->setText(ui->CIVlineEdit->text().trimmed());

    if (!ui->CIVlineEdit->text().isEmpty())
    {
        if (!ui->CIVlineEdit->text().contains('x'))
        {
            QMessageBox::critical(this, "CIV Error", QString(ui->CIVlineEdit->text()) + " Is not a valid CIV value\nPlease enter the CIV as a Hex number in the form of 0xnn");
            ui->CIVlineEdit->setText("");
        }
        else
        {
            int hexValue = ui->CIVlineEdit->text().toInt(&Ok, 16);
            if (Ok &&  (hexValue < 0 || hexValue > 255))
            {
                QMessageBox::critical(this, "CIV Error", QString(ui->CIVlineEdit->text()) + " CIV number out of range 0 - FF");
                ui->CIVlineEdit->setText("");
            }
        }
    }
    else
    {
        ui->CIVlineEdit->setText("");
    }

    if (ui->CIVlineEdit->text() != radioData->civAddress)
    {
        radioData->civAddress = ui->CIVlineEdit->text();
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


void RigSetupForm::CIVEditVisible(bool visible)
{
    ui->CIVLabel->setVisible(visible);
    ui->CIVlineEdit->setVisible(visible);
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
    return ui->comSpeedBox->currentText();
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



/************************** TransVert Enable *************************/


void RigSetupForm::enableTransVertSelected(bool /*flag*/)
{
    bool checked = ui->enableTransVert->isChecked();
    if(radioData->transVertEnable != ui->enableTransVert->isChecked())
    {
        radioData->transVertEnable = checked;
        transVertTabEnable(checked);

        setTransVertSwVisible(checked);
        if (!checked)
        {
            radioData->enableTransSwitch = checked;
            radioData->enableLocTVSwMsg = checked;
            setEnableTransVertSw(checked);
            setEnableLocalTransVertSwVisible(checked);
            setEnableLocalTransVertSw(checked);
            setLocTVSWComportVisible(checked);
        }
        radioValueChanged = true;
    }


}

bool RigSetupForm::getTransVertSelected()
{
    return ui->enableTransVert->isChecked();
}

void RigSetupForm::setTransVertSelected(bool flag)
{
    ui->enableTransVert->setChecked(flag);
    transVertTabEnable(flag);

}


/********************* TransVert Switch Enable  *********************************/

void RigSetupForm::enableTransVertSwSel(bool /*flag*/)
{
    bool checked = ui->enableTransVertSw->isChecked();
    if (radioData->enableTransSwitch != checked)
    {
        radioData->enableTransSwitch = checked;
        for (int i = 0; i < radioData->numTransverters; i++)
        {
            transVertTab[i]->setEnableTransVertSwBoxVisible(checked);
        }
        setEnableLocalTransVertSwVisible(checked);
        if (!checked)
        {
            radioData->enableLocTVSwMsg = checked;
            setEnableLocalTransVertSw(checked);
            setLocTVSWComportVisible(checked);

        }
        //setLocTVSWComportVisible(false);
        radioValueChanged = true;
    }

}


bool RigSetupForm::getEnableTransVertSw()
{
    return ui->enableTransVertSw->isChecked();
}

void RigSetupForm::setEnableTransVertSw(bool b)
{
    ui->enableTransVertSw->setChecked(b);
    for (int i = 0; i < radioData->numTransverters; i++)
    {
        transVertTab[i]->setEnableTransVertSwBoxVisible(b);
    }

}


void RigSetupForm::setTransVertSwVisible(bool b)
{
    ui->enableTransVertSw->setVisible(b);
}


/**************** Local Transvert Switch Control *****************************/


void RigSetupForm::localTransVertSwSel(bool /*flag*/)
{

    bool checked = ui->locTvConChk->isChecked();
    if (radioData->enableLocTVSwMsg != checked)
    {
        radioData->enableLocTVSwMsg = checked;
        setLocTVSWComportVisible(checked);
        radioValueChanged = true;
    }

}

bool RigSetupForm::getEnableLocalTransVertSw()
{
    return ui->locTvConChk->isChecked();
}


void RigSetupForm::setEnableLocalTransVertSw(bool b)
{
    ui->locTvConChk->setChecked(b);

}


void RigSetupForm::setEnableLocalTransVertSwVisible(bool visible)
{
     ui->locTvConChk->setVisible(visible);


}



/**************** Local Transvert Switch Comport **************************/



void RigSetupForm::locTVComPortSel(int /*index*/)
{

    if (ui->locTVComPortSel->currentText() != radioData->locTVSwComport)
    {
        radioData->locTVSwComport = ui->locTVComPortSel->currentText();
        radioValueChanged = true;
    }
}

QString RigSetupForm::getLocTVSwComport()
{
    return ui->locTVComPortSel->currentText();
}

void RigSetupForm::setLocTVSwComport(QString p)
{
    ui->locTVComPortSel->setCurrentIndex(ui->locTVComPortSel->findText(p));
}




void RigSetupForm::setLocTVSWComportVisible(bool visible)
{
     ui->locTVComPortSel->setVisible(visible);
     ui->locComportSwLbl->setVisible(visible);
     //setEnableLocalTransVertSwVisible(visible);

}




/************** RigSetup Enable **************************************/


void RigSetupForm::setEnableRigDataEntry(bool enable)
{
    ui->CIVlineEdit->setEnabled(enable);
    ui->comPortBox->setEnabled(enable);
    ui->comDataBitsBox->setEnabled(enable);
    ui->comSpeedBox->setEnabled(enable);
    ui->comDataBitsBox->setEnabled(enable);
    ui->comHandShakeBox->setEnabled(enable);
    ui->comParityBox->setEnabled(enable);
    ui->netPortBox->setEnabled(enable);
    ui->networkAddBox->setEnabled(enable);
    ui->pollInterval->setEnabled(enable);
    ui->mgmBox->setEnabled(enable);
    ui->RITEnable->setEnabled(enable);
    ui->enableTransVert->setEnabled(enable);

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

/*
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

*/


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


/********************* TabText *****************************************/


void RigSetupForm::setTransVertTabText(int tabNum, QString tabName)
{
    ui->transVertTab->setTabText(tabNum, tabName);
}







void RigSetupForm::addTransVerter()
{

    AddTransVerterDialog addTransDialog(bands, radioData->transVertNames, this);
    if (addTransDialog.exec() == !QDialog::Accepted)
    {
        return;
    }

    QString transVerterName = addTransDialog.getTransVerterName();

    if (transVerterName == "")
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
    int tabNum = radioData->numTransverters;
    radioData->transVertNames.append(transVerterName);
    addTransVertTab(tabNum, transVerterName);
    radioData->numTransverters = tabNum + 1;
    loadTransVertTab(tabNum);

}


void RigSetupForm::addTransVertTab(int tabNum, QString tabName)
{
    radioData->transVertSettings.append(new TransVertParams());
    radioData->transVertSettings[tabNum]->transVertName = tabName;
    radioData->transVertSettings[tabNum]->band = tabName;
    for (int i = 0; i < bands.count(); i++)
    {
         if (bands[i]->name == tabName)
         {
             radioData->transVertSettings[tabNum]->fLow = bands[i]->fLow;
             radioData->transVertSettings[tabNum]->fHigh = bands[i]->fHigh;
             break;
         }
    }
    transVertTab.append(new TransVertSetupForm(radioData->transVertSettings[tabNum]));
    addedTransVertTabs.append(tabName);

    ui->transVertTab->insertTab(tabNum, transVertTab[tabNum], tabName);
    ui->transVertTab->setTabColor(tabNum, Qt::darkBlue);      // radioTab promoted to QLogTabWidget
    ui->transVertTab->setCurrentIndex(tabNum);
    transVertTab[tabNum]->setEnableTransVertSwBoxVisible(false);

    // does this radio support antenna sw?
    bool antSwFlg = false;
    int retCode = 0;
    retCode = radio->supportAntSw(radioData->radioModelNumber, &antSwFlg);

    if (retCode >= 0 && antSwFlg)
    {
       //transVertTab[tabNum]->antSwNumVisible(true);
       radioData->antSwitchAvail = true;
    }
    else
    {
       //transVertTab[tabNum]->antSwNumVisible(false);
       radioData->antSwitchAvail = false;
    }

    buildSupBandList();
    transVertTab[tabNum]->transVertValueChanged = true;

}



void RigSetupForm::loadTransVertTab(int tabNum)
{
    transVertTab[tabNum]->setRadioFreqBox(convertFreqStrDispSingle(radioData->transVertSettings[tabNum]->radioFreqStr));
    transVertTab[tabNum]->setTargetFreqBox(convertFreqStrDispSingle(radioData->transVertSettings[tabNum]->targetFreqStr));
    transVertTab[tabNum]->setOffsetFreqLabel(radioData->transVertSettings[tabNum]->transVertOffsetStr);
    transVertTab[tabNum]->setTransVerSwNum(radioData->transVertSettings[tabNum]->transSwitchNum);

}

bool RigSetupForm::checkTransVerterNameMatch(QString transVertName)
{
    for (int i = 0; i < radioData->numTransverters; i++)
    {
        if (ui->transVertTab->tabText(i) == transVertName)
            return true;
    }

    return false;
}


void RigSetupForm::removeTransVerter()
{

    if (transVertTab.count() < 1)
    {
        return;
    }

    int currentIndex = ui->transVertTab->currentIndex();
    QString currentName = ui->transVertTab->tabText(currentIndex);

    int status = QMessageBox::question( this,
                            tr("Remove Transverter"),
                            tr("Do you really want to remove transverter - %1?")
                            .arg(currentName),
                            QMessageBox::Yes|QMessageBox::Default,
                            QMessageBox::No|QMessageBox::Escape,
                            QMessageBox::NoButton);

    if (status != QMessageBox::Yes)
    {
        return;
    }

    // remove this transverter
    ui->transVertTab->removeTab(currentIndex);
    radioData->transVertNames.removeAt(currentIndex);
    radioData->transVertSettings.removeAt(currentIndex);
    radioData->numTransverters--;
    removedTransVertTabs.append(currentName);


    //QString fileName = TRANSVERT_PATH_LOGGER + radioData->radioName + FILENAME_TRANSVERT_RADIOS;

    //QSettings config(fileName, QSettings::IniFormat);
    //config.beginGroup(currentName);
    //config.remove("");      // remove all keys for this group
    //config.endGroup();

}



void RigSetupForm::changeBand()
{

    if (transVertTab.count() < 1)
    {
        return;
    }

    int tabNum = ui->transVertTab->currentIndex();

    AddTransVerterDialog addTransDialog(bands, radioData->transVertNames, this);
    if (addTransDialog.exec() == !QDialog::Accepted)
    {
        return;
    }

    QString transVertName = addTransDialog.getTransVerterName();

    if (transVertName == "")
    {
        return;
    }

    if (checkTransVerterNameMatch(transVertName))
    {
        // error empty name or name already exists
        QMessageBox::information(this, tr("Transverter Name Exists"),
                                 tr("Transverter Name: %1, already exists \nPlease enter another name").arg(transVertName.trimmed()),
                                  QMessageBox::Ok|QMessageBox::Default,
                                  QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }

    QString oldName = radioData->transVertNames[tabNum];
    ui->transVertTab->setTabText(tabNum, transVertName);
    radioData->transVertNames[tabNum] = transVertName;
    radioData->transVertSettings[tabNum]->band = transVertName;
    radioData->transVertSettings[tabNum]->transVertName = transVertName;


    for (int i = 0; i < bands.count(); i++)
    {
         if (bands[i]->name == transVertName)
         {
             radioData->transVertSettings[tabNum]->fLow = bands[i]->fLow;
             radioData->transVertSettings[tabNum]->fHigh = bands[i]->fHigh;
         }
    }
    renamedTransVertTabs.append(oldName);
    // remove old entry
    //QString fileName = TRANSVERT_PATH_LOGGER + radioData->radioName + FILENAME_TRANSVERT_RADIOS;

    //QSettings config(fileName, QSettings::IniFormat);
    //config.beginGroup(oldName);
    //config.remove("");      // remove all keys for this group
    //config.endGroup();




    transVertTab[tabNum]->transVertValueChanged = true;

}


void RigSetupForm::transVertTabEnable(bool enable)
{

    ui->addTransvert->setDisabled(!enable);
    ui->removeTransvert->setDisabled(!enable);
    ui->changeBand->setDisabled(!enable);
    //ui->transvertFrame->setDisabled(!enable);
    ui->transVertTab->setDisabled(!enable);
}
