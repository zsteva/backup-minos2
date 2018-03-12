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



SetupDialog::SetupDialog(RigControl *radio, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog),
    currentRadio(nullptr)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    radio = radio;

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));
    connect(ui->addRadio, SIGNAL(clicked()), this, SLOT(addRadio()));
    connect(ui->removeRadio, SIGNAL(clicked()), this, SLOT(removeRadio()));
    connect(ui->editRadioName, SIGNAL(clicked()), this, SLOT(editRadioName()));


    // get the number of available antennas
    QString fileName;
    if (appName == "")
    {
        fileName = RADIO_PATH_LOCAL + FILENAME_AVAIL_RADIOS;
    }
    else
    {
        fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    }

    QSettings settings(fileName, QSettings::IniFormat);
    availRadios = settings.childGroups();
    numAvailRadios = availRadios.count();

    if (numAvailRadios > 0)
    {
        for (int i = 0; i < numAvailRadios; i++)
        {
            addTab(i, "");
        }

    }



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
    radioTab.append(new RigSetupForm(radio, availRadioData[tabNum]));
    ui->radioTab->insertTab(tabNum, radioTab[tabNum], tabName);
    ui->radioTab->setTabColor(tabNum, Qt::darkBlue);      // radioTab promoted to QLogTabWidget


}


void SetupDialog::loadSettingsToTab(int tabNum)
{

    ui->radioTab->setTabText(tabNum, availRadioData[tabNum]->radioName);

    if (availRadioData[tabNum]->radioModel != "")
    {


        //radioName[i]->setText(availRadioData[i].radioName);
        //radioModel[i]->setCurrentIndex(radioModel[i]->findText(availRadios[i].radioModel));
        if (availRadioData[tabNum]->radioMfg_Name == "Icom")
        {
            radioTab[tabNum]->CIVEditVisible(true);
        }
        else
        {
            radioTab[tabNum]->CIVEditVisible(false);
        }
        radioTab[tabNum]->setCIVAddress(availRadioData[tabNum]->civAddress);
        radioTab[tabNum]->setComport(availRadioData[tabNum]->comport);
        radioTab[tabNum]->setDataSpeed(QString::number(availRadioData[tabNum]->baudrate));
        radioTab[tabNum]->setDataBits(QString::number(availRadioData[tabNum]->databits));
        radioTab[tabNum]->setStopBits(QString::number(availRadioData[tabNum]->stopbits));
        radioTab[tabNum]->setParityBits(availRadioData[tabNum]->parity);
        radioTab[tabNum]->setHandshake(availRadioData[tabNum]->handshake);
        radioTab[tabNum]->setNetAddress(availRadioData[tabNum]->networkAdd);
        radioTab[tabNum]->setNetPortNum(availRadioData[tabNum]->networkPort);
        radioTab[tabNum]->setTransVertSelected(availRadioData[tabNum]->transVertEnable);
        if (availRadioData[tabNum]->transVertEnable)
        {
            radioTab[tabNum]->transVertTabVisible(true);
        }
        //transVertEdit[i]->setText(availRadios[i].transVertOffsetStr); *******************
        //transVertCheck[i]->setChecked(availRadios[i].transVertEnable);
        //transVertEdit[i]->setEnabled(transVertCheck[i]->isChecked());
        //transNegCheck[i]->setEnabled(transVertCheck[i]->isChecked());

        if (rig_port_e(availRadioData[tabNum]->portType) == RIG_PORT_NETWORK || rig_port_e(availRadioData[tabNum]->portType) == RIG_PORT_UDP_NETWORK)
        {
            radioTab[tabNum]->serialDataEntryVisible(false);
            radioTab[tabNum]->networkDataEntryVisible(true);
        }
        else if (rig_port_e(availRadioData[tabNum]->portType) == RIG_PORT_SERIAL)
        {
            radioTab[tabNum]->serialDataEntryVisible(true);
            radioTab[tabNum]->networkDataEntryVisible(false);
        }
        else if (rig_port_e(availRadioData[tabNum]->portType) == RIG_PORT_NONE)
        {
            radioTab[tabNum]->serialDataEntryVisible(false);
            radioTab[tabNum]->networkDataEntryVisible(false);
        }
        radioTab[tabNum]->setMgmMode(availRadioData[tabNum]->mgmMode);

    }

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
    numAvailRadios++;
    loadSettingsToTab(tabNum);
    saveRadio(tabNum);
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
    // remove from availantenna file
    QString fileName;
    if (appName == "")
    {
        fileName = RADIO_PATH_LOCAL + FILENAME_AVAIL_RADIOS;
    }
    else
    {
        fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    }

    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup(currentName);
    config.remove("");   // remove all keys for this group
    config.endGroup();

    numAvailRadios--;

    emit radioTabChanged();



}




void SetupDialog::editRadioName()
{
    int tabNum = ui->radioTab->currentIndex();
    QString radioName = ui->radioTab->tabText(tabNum);
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

                // remove from availantenna file
                QString fileName;
                if (appName == "")
                {
                    fileName = RADIO_PATH_LOCAL + FILENAME_AVAIL_RADIOS;
                }
                else
                {
                    fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
                }

                QSettings config(fileName, QSettings::IniFormat);
                config.beginGroup(radioName);
                config.remove("");   // remove all keys for this group
                config.endGroup();

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




int SetupDialog::comportAvial(int radioNum, QString comport)
{
    if (radioTab.count() > 0)
    {
        return radioTab[0]->comportAvial(comport);
    }
    else
    {
        return -1;
    }
}








void SetupDialog::saveButtonPushed()
{

    saveSettings();

}



void SetupDialog::cancelButtonPushed()
{




}



void SetupDialog::saveRadio(int i)
{

    QString fileName;
    if (appName == "")
    {
       fileName = RADIO_PATH_LOCAL + FILENAME_AVAIL_RADIOS;
    }
    else
    {
       fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    }

    QSettings config(fileName, QSettings::IniFormat);



    config.beginGroup(availRadioData[i]->radioName);
    config.setValue("radioName", availRadioData[i]->radioName);
    config.setValue("radioMfgName", availRadioData[i]->radioMfg_Name);
    config.setValue("radioModel", availRadioData[i]->radioModel);
    config.setValue("radioModelName", availRadioData[i]->radioModelName);
    config.setValue("radioModelNumber", availRadioData[i]->radioModelNumber);
    config.setValue("civAddress", availRadioData[i]->civAddress);
    config.setValue("portType", availRadioData[i]->portType);
    config.setValue("comport", availRadioData[i]->comport);
    config.setValue("baudrate", availRadioData[i]->baudrate);
    config.setValue("databits", availRadioData[i]->databits);
    config.setValue("parity", availRadioData[i]->parity);
    config.setValue("stopbits", availRadioData[i]->stopbits);
    config.setValue("handshake", availRadioData[i]->handshake);
    config.setValue("transVertEnable", availRadioData[i]->transVertEnable);
    config.setValue("netAddress", availRadioData[i]->networkAdd);
    config.setValue("netPort", availRadioData[i]->networkPort);
    config.setValue("mgmMode", availRadioData[i]->mgmMode);
    config.endGroup();

}



void SetupDialog::saveSettings()
{

    QString fileName;
    if (appName == "")
    {
       fileName = RADIO_PATH_LOCAL + FILENAME_AVAIL_RADIOS;
    }
    else
    {
       fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    }

    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < numAvailRadios; i++)
    {

        if (radioTab[i]->radioValueChanged)
        {
            if (radioTab[i]->radioNameChanged)
            {
                emit radioNameChange();
                radioTab[i]->radioNameChanged = false;
            }

            if (currentRadioName == availRadioData[i]->radioName)
            {
                // settings changed in current antenna
                 emit currentRadioSettingChanged(availRadioData[i]->radioName);
            }

            config.beginGroup(availRadioData[i]->radioName);
            config.setValue("radioName", availRadioData[i]->radioName);
            config.setValue("radioMfgName", availRadioData[i]->radioMfg_Name);
            config.setValue("radioModel", availRadioData[i]->radioModel);
            config.setValue("radioModelName", availRadioData[i]->radioModelName);
            config.setValue("radioModelNumber", availRadioData[i]->radioModelNumber);
            config.setValue("civAddress", availRadioData[i]->civAddress);
            config.setValue("portType", availRadioData[i]->portType);
            config.setValue("comport", availRadioData[i]->comport);
            config.setValue("baudrate", availRadioData[i]->baudrate);
            config.setValue("databits", availRadioData[i]->databits);
            config.setValue("parity", availRadioData[i]->parity);
            config.setValue("stopbits", availRadioData[i]->stopbits);
            config.setValue("handshake", availRadioData[i]->handshake);
            config.setValue("transVertEnable", availRadioData[i]->transVertEnable);
            config.setValue("netAddress", availRadioData[i]->networkAdd);
            config.setValue("netPort", availRadioData[i]->networkPort);
            config.setValue("mgmMode", availRadioData[i]->mgmMode);
            config.endGroup();
            radioTab[i]->radioValueChanged = false;

        }

    }


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
    if (appName == "")
    {
        fileName = RADIO_PATH_LOCAL + FILENAME_AVAIL_RADIOS;
    }
    else
    {
        fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    }



    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < numAvailRadios; i++)
    {
        config.beginGroup(availRadios[i]);
        availRadioData[i]->radioName = config.value("radioName", "").toString();
        availRadioData[i]->radioNumber = config.value("radioNumber", QString::number(i+1)).toString();
        availRadioData[i]->radioMfg_Name = config.value("radioMfgName", "").toString();
        availRadioData[i]->radioModel = config.value("radioModel", "").toString();
        availRadioData[i]->radioModelName = config.value("radioModelName", "").toString();
        availRadioData[i]->radioModelNumber = config.value("radioModelNumber", "").toInt();
        availRadioData[i]->civAddress = config.value("civAddress", "").toString();
        availRadioData[i]->portType = config.value("portType", int(RIG_PORT_SERIAL)).toInt();
        availRadioData[i]->comport = config.value("comport", "").toString();
        availRadioData[i]->baudrate = config.value("baudrate", 9600).toInt();
        availRadioData[i]->databits = config.value("databits", 8).toInt();
        availRadioData[i]->parity = config.value("parity", 0).toInt();
        availRadioData[i]->stopbits = config.value("stopbits", 1).toInt();
        availRadioData[i]->handshake = config.value("handshake", 0).toInt();
        //availRadioData[i]->transVertOffset = config.value("transVertOffSet", 0).toDouble();
        //availRadioData[i]->transVertOffsetStr = config.value("transVertOffSetStr", "00.000.000.000").toString();
        availRadioData[i]->transVertEnable = config.value("transVertEnable", false).toBool();
        //availRadioData[i]->transVertNegative = config.value("transVertNegative", false).toBool();
        availRadioData[i]->networkAdd = config.value("netAddress", "").toString();
        availRadioData[i]->networkPort = config.value("netPort", "").toString();
        availRadioData[i]->mgmMode = config.value("mgmMode", hamlibData::USB).toString();
        config.endGroup();
    }
    chkloadflg = false;
}





void SetupDialog::saveTranVerterSetting(int radioNum, int transVertNum, QString radioName)
{

    QString fileName;
    if (appName == "")
    {
        fileName = RADIO_PATH_LOCAL + radioName + FILENAME_TRANSVERT_RADIOS;
    }
    else
    {
        fileName = RADIO_PATH_LOGGER + radioName + FILENAME_TRANSVERT_RADIOS;
    }

    QSettings  config(fileName, QSettings::IniFormat);

    config.beginGroup(availRadioData[radioNum]->transVertSettings[transVertNum]->transVertName);
    config.setValue("name", availRadioData[radioNum]->transVertSettings[transVertNum]->transVertName);
    config.setValue("band", availRadioData[radioNum]->transVertSettings[transVertNum]->transVertName);
    config.setValue("offset", availRadioData[radioNum]->transVertSettings[transVertNum]->transVertOffsetStr);
    config.setValue("negOffset", availRadioData[radioNum]->transVertSettings[transVertNum]->transVertNegative);
    config.setValue("enableTransVertSw", availRadioData[radioNum]->transVertSettings[transVertNum]->enableTransSwitch);
    config.setValue("transVertSw", availRadioData[radioNum]->transVertSettings[transVertNum]->transSwitchNum);
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
    if (appName == "")
    {
        fileName = RADIO_PATH_LOCAL + LOCAL_RADIO + FILENAME_CURRENT_RADIO;
    }
    else
    {
        fileName = RADIO_PATH_LOGGER + appName + FILENAME_CURRENT_RADIO;
    }


    QSettings config(fileName, QSettings::IniFormat);


    config.beginGroup("CurrentRadio");
    config.setValue("radioName", currentRadioName);
    config.endGroup();




}

void SetupDialog::readCurrentRadio()
{

    QString fileName;
    if (appName == "")
    {
        fileName = RADIO_PATH_LOCAL + LOCAL_RADIO + FILENAME_CURRENT_RADIO;
    }
    else
    {
        fileName = RADIO_PATH_LOGGER + appName + FILENAME_CURRENT_RADIO;
    }


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
        if (currentRadioName == availRadioData[i]->radioName)
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


scatParams* SetupDialog::getCurrentRadio() const
{
    return currentRadio;
}






void SetupDialog::saveMgmList()
{
    const QStringList  mList = {"USB", "LSB", "RTTY", "PKTLSB", "PKTUSB", "PKTFM" };

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
    config.beginGroup("MGM_Modes");

    config.setValue("MgmModes", mList);

    config.endGroup();


}





