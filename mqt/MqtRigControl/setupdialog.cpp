/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2018
//
//
/////////////////////////////////////////////////////////////////////////////





#include "setupdialog.h"
#include "rigsetupform.h"
#include "transvertsetupform.h"
#include "ui_setupdialog.h"
#include "rigcontrolcommonconstants.h"
#include "rigcontrol.h"
#include "rigutils.h"
#include <QSignalMapper>
#include <QComboBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QCheckBox>
//#include <QtSerialPort/QSerialPort>
//#include <QSerialPortInfo>
#include <QSettings>
#include <QInputDialog>

#include <QDebug>



SetupDialog::SetupDialog(RigControl* _radio, const QVector<BandDetail*> _bands, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)

{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    radio = _radio;
    bands = _bands;

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));
    connect(ui->addRadio, SIGNAL(clicked()), this, SLOT(addRadio()));
    connect(ui->removeRadio, SIGNAL(clicked()), this, SLOT(removeRadio()));
    connect(ui->editRadioName, SIGNAL(clicked()), this, SLOT(editRadioName()));



    // radio settings ini file
    QString fileName;
    fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    QSettings  settings(fileName, QSettings::IniFormat);

    availRadios = settings.childGroups();
    numAvailRadios = availRadios.count();


    if (numAvailRadios > 0)
    {
        for (int i = 0; i < numAvailRadios; i++)
        {
            addTab(i, availRadios[i]);

            // find transverters
            QString fileName;
            fileName = TRANSVERT_PATH_LOGGER + availRadios[i] + FILENAME_TRANSVERT_RADIOS;
            QSettings  configTransvert(fileName, QSettings::IniFormat);

            radioTab[i]->getRadioData()->transVertNames = configTransvert.childGroups();  // get transvert names for this radio
            radioTab[i]->getRadioData()->numTransverters =  radioTab[i]->getRadioData()->transVertNames.count();

            if (radioTab[i]->getRadioData()->numTransverters > 0)
            {
                for (int t = 0; t < availRadioData[i]->numTransverters; t++)
                {
                   radioTab[i]->addTransVertTab(t, radioTab[i]->getRadioData()->transVertNames[t] );
                }
            }


        }


    }

    // get the number of transverters for each radio



    //clearAvailRadio(); // clear the AvailRadio table, also init with default serial parameters
    //clearCurrentRadio(); // clear the currently selected Radio table, also init with default serial parameters
    //clearRadioValueChanged();
    //clearRadioNameChanged();


//    for (int i = 0; i < numAvailRadios; i++)
//    {
//        networkDataEntryVisible(i, false);
//    }

    readSettings();   // get available radio settings from file

    for (int i = 0; i < numAvailRadios; i++)
    {
        loadSettingsToTab(i);

    }

//    ui->setupTab->setCurrentIndex(0);       // set first tab



}

SetupDialog::~SetupDialog()
{
    delete ui;
}



void SetupDialog::addTab(int tabNum, QString tabName)
{
    availRadioData.append(new scatParams);
    availRadioData[tabNum]->radioName = tabName;
    if (!availRadios.contains(tabName))
    {
       availRadios.append(tabName);
    }

    radioTab.append(new RigSetupForm(radio, availRadioData[tabNum], bands));
    ui->radioTab->insertTab(tabNum, radioTab[tabNum], tabName);
    ui->radioTab->setTabColor(tabNum, Qt::darkBlue);      // radioTab promoted to QLogTabWidget


}


void SetupDialog::loadSettingsToTab(int tabNum)
{

    ui->radioTab->setTabText(tabNum, radioTab[tabNum]->getRadioData()->radioName);

    if (availRadioData[tabNum]->radioModel != "")
    {


        radioTab[tabNum]->setRadioModel(radioTab[tabNum]->getRadioData()->radioModel);

        radioTab[tabNum]->setCIVAddress(radioTab[tabNum]->getRadioData()->civAddress);
        radioTab[tabNum]->setComport(radioTab[tabNum]->getRadioData()->comport);
        radioTab[tabNum]->setDataSpeed(QString::number(radioTab[tabNum]->getRadioData()->baudrate));
        radioTab[tabNum]->setDataBits(QString::number(radioTab[tabNum]->getRadioData()->databits));
        radioTab[tabNum]->setStopBits(QString::number(radioTab[tabNum]->getRadioData()->stopbits));
        radioTab[tabNum]->setParityBits(radioTab[tabNum]->getRadioData()->parity);
        radioTab[tabNum]->setHandshake(radioTab[tabNum]->getRadioData()->handshake);
        radioTab[tabNum]->setNetAddress(radioTab[tabNum]->getRadioData()->networkAdd);
        radioTab[tabNum]->setNetPortNum(radioTab[tabNum]->getRadioData()->networkPort);
        radioTab[tabNum]->setPollInterval(radioTab[tabNum]->getRadioData()->pollInterval);
        radioTab[tabNum]->setTransVertSelected(radioTab[tabNum]->getRadioData()->transVertEnable);


        if (rig_port_e(radioTab[tabNum]->getRadioData()->portType) == RIG_PORT_NETWORK || rig_port_e(radioTab[tabNum]->getRadioData()->portType) == RIG_PORT_UDP_NETWORK)
        {
            radioTab[tabNum]->serialDataEntryVisible(false);
            radioTab[tabNum]->networkDataEntryVisible(true);
        }
        else if (rig_port_e(radioTab[tabNum]->getRadioData()->portType) == RIG_PORT_SERIAL)
        {
            radioTab[tabNum]->serialDataEntryVisible(true);
            radioTab[tabNum]->networkDataEntryVisible(false);
        }
        else if (rig_port_e(radioTab[tabNum]->getRadioData()->portType) == RIG_PORT_NONE)
        {
            radioTab[tabNum]->serialDataEntryVisible(false);
            radioTab[tabNum]->networkDataEntryVisible(false);
        }
        radioTab[tabNum]->setMgmMode(radioTab[tabNum]->getRadioData()->mgmMode);

        // now load transverter settings
        if(radioTab[tabNum]->getRadioData()->numTransverters >0 )
        {
            for (int t = 0; t < radioTab[tabNum]->getRadioData()->numTransverters; t++)
            {
                radioTab[tabNum]->setTransVertTabText(t, radioTab[tabNum]->getRadioData()->transVertNames[t]);
                radioTab[tabNum]->transVertTab[t]->setRadioFreqBox(convertFreqStrDispSingle(radioTab[tabNum]->getRadioData()->transVertSettings[t]->radioFreqStr));
                radioTab[tabNum]->transVertTab[t]->setTargetFreqBox(convertFreqStrDispSingle(radioTab[tabNum]->getRadioData()->transVertSettings[t]->targetFreqStr));
                radioTab[tabNum]->transVertTab[t]->setOffsetFreqLabel(radioTab[tabNum]->getRadioData()->transVertSettings[t]->transVertOffsetStr);
                radioTab[tabNum]->transVertTab[t]->setNegCheckBox(radioTab[tabNum]->getRadioData()->transVertSettings[t]->transVertNegative);
                radioTab[tabNum]->transVertTab[t]->setEnableTransVertSw(radioTab[tabNum]->getRadioData()->transVertSettings[t]->enableTransSwitch);
                radioTab[tabNum]->transVertTab[t]->setTransVerSwNum(radioTab[tabNum]->getRadioData()->transVertSettings[t]->transSwitchNum);


            }

        }



        radioTab[tabNum]->buildSupBandList();

    }
    else
    {
        // no radio model visible
        radioTab[tabNum]->networkDataEntryVisible(false);
        radioTab[tabNum]->serialDataEntryVisible(false);
        radioTab[tabNum]->pollIntervalVisible(false);
    }

    radioTab[tabNum]->setTransVertSelected(radioTab[tabNum]->getRadioData()->transVertEnable);

}




void SetupDialog::addRadio()
{
    QString radioName = QInputDialog::getText(this, tr("Enter Radio Name"), tr("Please enter a Radio Name:"), QLineEdit::Normal);
    radioName = radioName.trimmed();
    if (radioName.isEmpty())
    {
          return;
    }
    if (checkRadioNameMatch(radioName))
    {
        // error empty name or name already exists
        QMessageBox::information(this, tr("Radio Name Exists"),
                                 tr("Radio Name: %1, already exists \nPlease enter another name").arg(radioName.trimmed()),
                                  QMessageBox::Ok|QMessageBox::Default,
                                  QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }


    // add the new radio
    int tabNum = numAvailRadios;
    addTab(tabNum, radioName);
    addedRadioTabs.append(radioName);   // track added radios until save
    numAvailRadios++;
    loadSettingsToTab(tabNum);
    radioTab[tabNum]->radioValueChanged = true;
    //saveRadio(tabNum);
    ui->radioTab->setCurrentIndex(tabNum);
    emit radioTabChanged();

}


bool SetupDialog::checkRadioNameMatch(QString radioName)
{
    for (int i = 0; i < numAvailRadios; i++)
    {
        if (ui->radioTab->tabText(i) == radioName)
            return true;
    }

    return false;
}


void SetupDialog::removeRadio()
{

    int currentIndex = ui->radioTab->currentIndex();
    QString currentName = ui->radioTab->tabText(currentIndex);

    if (currentRadioName == ui->radioTab->tabText(currentIndex))
    {
        // can't remove current RadioName
        QMessageBox msgBox;
        msgBox.setText("You can not remove the current radio!");
        msgBox.exec();
        return;

    }

    int status = QMessageBox::question( this,
                            tr("Remove Radio"),
                            tr("Do you really want to remove radio - %1?")
                            .arg(currentName),
                            QMessageBox::Yes|QMessageBox::Default,
                            QMessageBox::No|QMessageBox::Escape,
                            QMessageBox::NoButton);

    if (status != QMessageBox::Yes)
    {
        return;
    }

    // remove this radio
    ui->radioTab->removeTab(currentIndex);
    availRadioData.remove(currentIndex);
    availRadios.removeAt(currentIndex);
    renameRadioTabs.append(currentName);            // save old name for deletion
    // remove from availantenna file
    //QString fileName;
    //fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    //QSettings config(fileName, QSettings::IniFormat);
    //config.beginGroup(currentName);
    //config.remove("");   // remove all keys for this group
    //config.endGroup();

    numAvailRadios--;

    emit radioTabChanged();



}




void SetupDialog::editRadioName()
{
    int tabNum = ui->radioTab->currentIndex();
    QString radioName = ui->radioTab->tabText(tabNum);
    QString oldName = radioName;
    if (currentRadioName == radioName)
    {
        // can't change current antennaName
        QMessageBox msgBox;
        msgBox.setText(QString("You can not change the name of the current radio - %1!").arg(radioName));
        msgBox.exec();
        return;
    }

    bool ok;
    QString text = QInputDialog::getText(this, QString("Edit Radio Name - %1").arg(radioName),
                                         tr("New Radio Name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
    {
        ui->radioTab->setTabText(tabNum, text);
        for (int i = 0; i < numAvailRadios; i++)
        {
            if (radioName == availRadioData[i]->radioName)
            {
                availRadioData[i]->radioName = text;  // update with new name
                availRadios[i] = text;
                renameRadioTabs.append(oldName);
                // remove from availantenna file
                //QString fileName;
                //fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
                //QSettings config(fileName, QSettings::IniFormat);
                //config.beginGroup(radioName);
                //config.remove("");   // remove all keys for this group
                //config.endGroup();

                saveRadio(tabNum);

            }
        }
    }
    else
    {
        return;
    }


    emit radioTabChanged();


}


void SetupDialog::setTabToCurrentRadio()
{

    for (int i = 0; i < numAvailRadios; i++)
    {
        if (currentRadioName == availRadioData[i]->radioName)
        {
            ui->radioTab->setTabColor(i, Qt::red);
            ui->radioTab->setCurrentIndex(i);
        }
        else
        {
            ui->radioTab->setTabColor(i,Qt::darkBlue);
        }


    }
}

int SetupDialog::comportAvial(int radioNum, QString comport)
{
    if (radioTab.count() > 0)
    {
        return radioTab[radioNum]->comportAvial(comport);
    }

    return -1;
}








void SetupDialog::saveButtonPushed()
{

    saveSettings();

}



void SetupDialog::cancelButtonPushed()
{



    QString fileName;
    fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < numAvailRadios; i++)
    {
        if (radioTab[i]->radioValueChanged)
        {
            getRadioSetting(i, config);
            radioTab[i]->radioValueChanged = false;
        }
    }



}



void SetupDialog::saveRadio(int i)
{

    QString fileName;
    fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    QSettings config(fileName, QSettings::IniFormat);


    saveRadioData(i, config);

    if (availRadioData[i]->transVertEnable)
    {
        fileName = RADIO_PATH_LOGGER + availRadioData[i]->radioModelName + FILENAME_TRANSVERT_RADIOS;
        QSettings  config(fileName, QSettings::IniFormat);

        for (int trv = 0; trv < availRadioData[i]->numTransverters; trv++)
        {
            saveTranVerterSetting(i, trv, config);
        }

    }

}



void SetupDialog::saveSettings()
{

    QString fileNameTransVert;


    addedRadioTabs.clear();

    QString fileNameRadio;
    fileNameRadio = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    QSettings configRadio(fileNameRadio, QSettings::IniFormat);

    for (int i = 0; i < removeRadioTabs.count(); i++)
    {
        configRadio.beginGroup(removeRadioTabs[i]);
        configRadio.remove("");   // remove all keys for this group
        configRadio.endGroup();
        fileNameTransVert = TRANSVERT_PATH_LOGGER + removeRadioTabs[i] + FILENAME_TRANSVERT_RADIOS;
        if (QFile::exists(fileNameTransVert))
        {
            QFile::remove(fileNameTransVert);
        }

    }
    removeRadioTabs.clear();

    for (int i = 0; i < renameRadioTabs.count(); i++)
    {
        configRadio.beginGroup(renameRadioTabs[i]);
        configRadio.remove("");   // remove all keys for this group
        configRadio.endGroup();
        fileNameTransVert = TRANSVERT_PATH_LOGGER + renameRadioTabs[i] + FILENAME_TRANSVERT_RADIOS;
        if (QFile::exists(fileNameTransVert))
        {
            QFile::remove(fileNameTransVert);
        }

    }
    renameRadioTabs.clear();

    bool currRadioChanged = false;


    for (int i = 0; i < numAvailRadios; i++)
    {

        if (radioTab[i]->radioValueChanged)
        {
            if (radioTab[i]->radioNameChanged)
            {
                emit radioNameChange();
                radioTab[i]->radioNameChanged = false;
            }

            if (currentRadioName == radioTab[i]->getRadioData()->radioName)
            {
                // settings changed in current antenna
                currRadioChanged = true;
            }

            saveRadioData(i, configRadio);

        }


        // now save transvert settings
        if (radioTab[i]->getRadioData()->transVertEnable)
        {

            fileNameTransVert = TRANSVERT_PATH_LOGGER + radioTab[i]->getRadioData()->radioName + FILENAME_TRANSVERT_RADIOS;
            QSettings  configTransVert(fileNameTransVert, QSettings::IniFormat);

            if (radioTab[i]->getRadioData()->numTransverters > 0)
            {

                radioTab[i]->addedTransVertTabs.clear();
                for (int t = 0; t < radioTab[i]->removedTransVertTabs.count(); t++)
                {
                    QSettings config(fileNameTransVert, QSettings::IniFormat);
                    config.beginGroup(radioTab[i]->removedTransVertTabs[t]);
                    config.remove("");      // remove all keys for this group
                    config.endGroup();
                }
                radioTab[i]->removedTransVertTabs.clear();

                for (int t = 0; t < radioTab[i]->renamedTransVertTabs.count(); t++)
                {
                    QSettings config(fileNameTransVert, QSettings::IniFormat);
                    config.beginGroup(radioTab[i]->renamedTransVertTabs[t]);
                    config.remove("");      // remove all keys for this group
                    config.endGroup();
                }
                radioTab[i]->renamedTransVertTabs.clear();


                for (int t = 0; t < radioTab[i]->getRadioData()->numTransverters; t++)
                {
                    if (radioTab[i]->transVertTab[t]->transVertValueChanged)
                    {
                        if (radioTab[i]->transVertTab[t]->transVertNameChanged)
                        {
                            radioTab[i]->transVertTab[t]->transVertNameChanged = false;
                            emit transVertNameChanged();
                        }

                        if (currentRadioName == radioTab[i]->getRadioData()->radioName)
                        {
                            // settings changed in current antenna
                            currRadioChanged = true;
                        }

                        saveTranVerterSetting(i, t, configTransVert);


                        radioTab[i]->transVertTab[t]->transVertValueChanged = false;
                    }


                }


            }





        }

        if (currRadioChanged)
        {
            emit currentRadioSettingChanged(radioTab[i]->getRadioData()->radioName);
        }
        radioTab[i]->radioValueChanged = false;


    }
}


void SetupDialog::saveRadioData(int radNum, QSettings& config)
{

    config.beginGroup(radioTab[radNum]->getRadioData()->radioName);
    config.setValue("radioName", radioTab[radNum]->getRadioData()->radioName);
    config.setValue("radioMfgName", radioTab[radNum]->getRadioData()->radioMfg_Name);
    config.setValue("radioModel", radioTab[radNum]->getRadioData()->radioModel);
    config.setValue("radioModelName", radioTab[radNum]->getRadioData()->radioModelName);
    config.setValue("radioModelNumber", radioTab[radNum]->getRadioData()->radioModelNumber);
    config.setValue("civAddress", radioTab[radNum]->getRadioData()->civAddress);
    config.setValue("portType", radioTab[radNum]->getRadioData()->portType);
    config.setValue("comport", radioTab[radNum]->getRadioData()->comport);
    config.setValue("baudrate", radioTab[radNum]->getRadioData()->baudrate);
    config.setValue("databits", radioTab[radNum]->getRadioData()->databits);
    config.setValue("parity", radioTab[radNum]->getRadioData()->parity);
    config.setValue("stopbits", radioTab[radNum]->getRadioData()->stopbits);
    config.setValue("handshake", radioTab[radNum]->getRadioData()->handshake);
    config.setValue("radioPollInterval", radioTab[radNum]->getRadioData()->pollInterval);
    config.setValue("antSwitchAvail", radioTab[radNum]->getRadioData()->antSwitchAvail);
    config.setValue("transVertEnable", radioTab[radNum]->getRadioData()->transVertEnable);
    config.setValue("netAddress", radioTab[radNum]->getRadioData()->networkAdd);
    config.setValue("netPort", radioTab[radNum]->getRadioData()->networkPort);
    config.setValue("mgmMode", radioTab[radNum]->getRadioData()->mgmMode);
    config.endGroup();



}







/*
void SetupDialog::clearRadioValueChanged()
{
    for (int i = 0; i < numAvailRadios; i++)
    {
        radioValueChanged[i] = false;
    }
}

void SetupDialog::clearRadioNameChanged()
{
    for (int i = 0; i < numAvailRadios; i++)
    {
        radioNameChanged[i] = false;
    }
}

*/
void SetupDialog::readSettings()
{

    chkloadflg = true;      // stop loading check values tiggering mapper signals

    QString fileName;
    fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < numAvailRadios; i++)
    {
        getRadioSetting(i, config);
    }
    chkloadflg = false;
}

void SetupDialog::getRadioSetting(int radNum, QSettings& config)
{
    config.beginGroup(availRadios[radNum]);
    radioTab[radNum]->getRadioData()->radioName = config.value("radioName", "").toString();
    radioTab[radNum]->getRadioData()->radioNumber = config.value("radioNumber", QString::number(radNum)).toString();
    radioTab[radNum]->getRadioData()->radioMfg_Name = config.value("radioMfgName", "").toString();
    radioTab[radNum]->getRadioData()->radioModel = config.value("radioModel", "").toString();
    radioTab[radNum]->getRadioData()->radioModelName = config.value("radioModelName", "").toString();
    radioTab[radNum]->getRadioData()->radioModelNumber = config.value("radioModelNumber", "").toInt();
    radioTab[radNum]->getRadioData()->civAddress = config.value("civAddress", "").toString();
    radioTab[radNum]->getRadioData()->portType = config.value("portType", int(RIG_PORT_SERIAL)).toInt();
    radioTab[radNum]->getRadioData()->comport = config.value("comport", "").toString();
    radioTab[radNum]->getRadioData()->baudrate = config.value("baudrate", 9600).toInt();
    radioTab[radNum]->getRadioData()->databits = config.value("databits", 8).toInt();
    radioTab[radNum]->getRadioData()->parity = config.value("parity", 0).toInt();
    radioTab[radNum]->getRadioData()->stopbits = config.value("stopbits", 1).toInt();
    radioTab[radNum]->getRadioData()->handshake = config.value("handshake", 0).toInt();
    radioTab[radNum]->getRadioData()->pollInterval = config.value("radioPollInterval", "1").toString();
    radioTab[radNum]->getRadioData()->transVertEnable = config.value("transVertEnable", false).toBool();
    radioTab[radNum]->getRadioData()->antSwitchAvail = config.value("antSwitchAvail", false).toBool();
    radioTab[radNum]->getRadioData()->networkAdd = config.value("netAddress", "").toString();
    radioTab[radNum]->getRadioData()->networkPort = config.value("netPort", "").toString();
    radioTab[radNum]->getRadioData()->mgmMode = config.value("mgmMode", hamlibData::USB).toString();
    config.endGroup();

    // now read transverter settings
    QString fileNameTransVert;
    fileNameTransVert = TRANSVERT_PATH_LOGGER + radioTab[radNum]->getRadioData()->radioName + FILENAME_TRANSVERT_RADIOS;
    QSettings  configTransVert(fileNameTransVert, QSettings::IniFormat);

    for (int t = 0; t < radioTab[radNum]->getRadioData()->numTransverters; t++)
    {
        readTranVerterSetting(radNum, t, configTransVert);
    }
}



void SetupDialog::saveTranVerterSetting(int radioNum, int transVertNum, QSettings  &config)
{
    config.beginGroup(radioTab[radioNum]->getRadioData()->transVertNames[transVertNum]);
    config.setValue("name", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertName);
    config.setValue("band", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->band);
    config.setValue("radioFreqStr", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->radioFreqStr);
    config.setValue("radioFreq", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->radioFreq);
    config.setValue("targetFreqStr", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->targetFreqStr);
    config.setValue("targetFreq", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->targetFreq);
    config.setValue("offsetString", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertOffsetStr);
    config.setValue("offsetDouble", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertOffset);
    config.setValue("negOffset", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertNegative);
    config.setValue("antSwNumber", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->antSwitchNum);
    config.setValue("enableTransVertSw", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->enableTransSwitch);
    config.setValue("transVertSw", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transSwitchNum);
    config.setValue("locTransSwEnable", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->enableLocTVSwMsg);
    config.setValue("locTransVertSwComport", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->locTVSwComport);
    config.endGroup();
}



void SetupDialog::readTranVerterSetting(int radioNum, int transVertNum, QSettings  &config)
{
    config.beginGroup(radioTab[radioNum]->getRadioData()->transVertNames[transVertNum]);
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertName = config.value("name", "").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->band = config.value("band", "").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->radioFreqStr = config.value("radioFreqStr", "00.000.000.000").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->radioFreq = config.value("radioFreq", 0.0).toDouble();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->targetFreqStr = config.value("targetFreqStr", "00.000.000.000").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->targetFreq = config.value("targetFreq", 0.0).toDouble();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertOffsetStr = config.value("offsetString", "00.000.000.000").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertOffset = config.value("offsetDouble", 0.0).toDouble();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertNegative = config.value("negOffset", false).toBool();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->antSwitchNum = config.value("antSwNumber", "0").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->enableTransSwitch = config.value("enableTransVertSw", false).toBool();
    radioTab[radioNum]->transVertTab[transVertNum]->setEnableTransVertSwBoxVisible(radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->enableTransSwitch);
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transSwitchNum = config.value("transVertSw", "0").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->enableLocTVSwMsg = config.value("locTransSwEnable", false).toBool();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->locTVSwComport = config.value("locTransVertSwComport", "").toString();
    config.endGroup();
}



/*
void SetupDialog::clearAvailRadio()
{
    scatParams nulParam;

    for (int i = 0; i < numAvailRadios; i++)
    {
        availRadios[i] = nulParam;
    }


}


void SetupDialog::clearCurrentRadio()
{
    scatParams nulParam;
    currentRadio = nulParam;
}


void SetupDialog::copyRadioToCurrent(int radioNumber)
{
    currentRadio = availRadios[radioNumber];
}

*/


QString SetupDialog::getRadioComPort(QString radioName)
{

    for (int i = 0; i < numAvailRadios; i++)
    {
        if (availRadioData[i]->radioName == radioName)
        {
            return availRadioData[i]->comport;
        }
    }

    return ""; // error, none found

}



void SetupDialog::saveCurrentRadio()
{

    QString fileName;
    fileName = RADIO_PATH_LOGGER + appName + FILENAME_CURRENT_RADIO;
    QSettings config(fileName, QSettings::IniFormat);


    config.beginGroup("CurrentRadio");
    config.setValue("radioName", currentRadioName);
    config.endGroup();




}

void SetupDialog::readCurrentRadio()
{

    QString fileName;
    fileName = RADIO_PATH_LOGGER + appName + FILENAME_CURRENT_RADIO;
    QSettings config(fileName, QSettings::IniFormat);


    {
        config.beginGroup("CurrentRadio");
        currentRadioName = config.value("radioName", "").toString();
        config.endGroup();
    }

}




int SetupDialog::findCurrentRadio(QString currentRadioName)
{
    int err = -1;
    for (int i = 0; i < numAvailRadios; i++)
    {
        if (currentRadioName == radioTab[i]->getRadioData()->radioName)
        {
            // current antenna points to selected available antenna
            //currentAntenna = availAntData[i];
            return i;
        }
    }

    return err;
}




void SetupDialog::setAppName(QString name)
{
    appName = name;
}






void SetupDialog::saveMgmList()
{
    const QStringList  mList = {"USB", "LSB", "RTTY", "PKTLSB", "PKTUSB", "PKTFM" };

    QString fileName;
    fileName = RIG_CONFIGURATION_FILEPATH_LOGGER + MINOS_RADIO_CONFIG_FILE;
    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup("MGM_Modes");

    config.setValue("MgmModes", mList);

    config.endGroup();


}



