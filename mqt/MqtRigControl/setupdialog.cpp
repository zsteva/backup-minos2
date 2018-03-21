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



    // radio settings ini file
    QString fileName;
    if (appName == "")
    {
        fileName = RADIO_PATH_LOCAL + FILENAME_AVAIL_RADIOS;
    }
    else
    {
        fileName = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    }

    loadBands();            // get band information

    QSettings  settings(fileName, QSettings::IniFormat);

    availRadios = settings.childGroups();
    numAvailRadios = availRadios.count();


    if (numAvailRadios > 0)
    {
        for (int i = 0; i < numAvailRadios; i++)
        {
            addTab(i, "");

            // find transverters
            QString fileName;
            if (appName == "")
            {
                fileName = TRANSVERT_PATH_LOGGER + availRadios[i] + FILENAME_TRANSVERT_RADIOS;
            }
            else
            {
                fileName = TRANSVERT_PATH_LOGGER + availRadios[i] + FILENAME_TRANSVERT_RADIOS;
            }

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
    radioTab.append(new RigSetupForm(radio, availRadioData[tabNum], bands));
    ui->radioTab->insertTab(tabNum, radioTab[tabNum], tabName);
    ui->radioTab->setTabColor(tabNum, Qt::darkBlue);      // radioTab promoted to QLogTabWidget


}


void SetupDialog::
loadSettingsToTab(int tabNum)
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
        for (int t = 0; t < radioTab[tabNum]->getRadioData()->numTransverters; t++)
        {
            radioTab[tabNum]->setTransVertTabText(t, radioTab[tabNum]->getRadioData()->transVertNames[t]);
            //radioTab[tabNum]->transVertTab[t]->setBand(radioTab[tabNum]->getRadioData()->transVertSettings[t]->band);
            //radioTab[tabNum]->transVertTab[t]->setTransVertOffsetFreq(radioTab[tabNum]->getRadioData()->transVertSettings[t]->transVertOffsetStr);
            radioTab[tabNum]->transVertTab[t]->setNegCheckBox(radioTab[tabNum]->getRadioData()->transVertSettings[t]->transVertNegative);
            radioTab[tabNum]->transVertTab[t]->setEnableTransVertSw(radioTab[tabNum]->getRadioData()->transVertSettings[t]->enableTransSwitch);
            radioTab[tabNum]->transVertTab[t]->setTransVerSwNum(radioTab[tabNum]->getRadioData()->transVertSettings[t]->transSwitchNum);
        }











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
        return radioTab[radioNum]->comportAvial(comport);
    }
    else
    {
        return -1;
    }

    return -1;
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
    config.setValue("radioPollInterval", availRadioData[i]->pollInterval);
    config.setValue("transVertEnable", availRadioData[i]->transVertEnable);
    config.setValue("netAddress", availRadioData[i]->networkAdd);
    config.setValue("netPort", availRadioData[i]->networkPort);
    config.setValue("mgmMode", availRadioData[i]->mgmMode);
    config.endGroup();

    if (availRadioData[i]->transVertEnable)
    {
        QString fileName;
        if (appName == "")
        {
            fileName = RADIO_PATH_LOCAL + availRadioData[i]->radioModelName + FILENAME_TRANSVERT_RADIOS;
        }
        else
        {
            fileName = RADIO_PATH_LOGGER + availRadioData[i]->radioModelName + FILENAME_TRANSVERT_RADIOS;
        }

        QSettings  config(fileName, QSettings::IniFormat);

        for (int trv = 0; trv < availRadioData[i]->numTransverters; trv++)
        {
            saveTranVerterSetting(i, trv, config);
        }

    }

}



void SetupDialog::saveSettings()
{

    QString fileNameRadio;
    if (appName == "")
    {
       fileNameRadio = RADIO_PATH_LOCAL + FILENAME_AVAIL_RADIOS;
    }
    else
    {
       fileNameRadio = RADIO_PATH_LOGGER + FILENAME_AVAIL_RADIOS;
    }

    QSettings configRadio(fileNameRadio, QSettings::IniFormat);



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
                 emit currentRadioSettingChanged(radioTab[i]->getRadioData()->radioName);
            }

            configRadio.beginGroup(radioTab[i]->getRadioData()->radioName);
            configRadio.setValue("radioName", radioTab[i]->getRadioData()->radioName);
            configRadio.setValue("radioMfgName", radioTab[i]->getRadioData()->radioMfg_Name);
            configRadio.setValue("radioModel", radioTab[i]->getRadioData()->radioModel);
            configRadio.setValue("radioModelName", radioTab[i]->getRadioData()->radioModelName);
            configRadio.setValue("radioModelNumber", radioTab[i]->getRadioData()->radioModelNumber);
            configRadio.setValue("civAddress", radioTab[i]->getRadioData()->civAddress);
            configRadio.setValue("portType", radioTab[i]->getRadioData()->portType);
            configRadio.setValue("comport", radioTab[i]->getRadioData()->comport);
            configRadio.setValue("baudrate", radioTab[i]->getRadioData()->baudrate);
            configRadio.setValue("databits", radioTab[i]->getRadioData()->databits);
            configRadio.setValue("parity", radioTab[i]->getRadioData()->parity);
            configRadio.setValue("stopbits", radioTab[i]->getRadioData()->stopbits);
            configRadio.setValue("handshake", radioTab[i]->getRadioData()->handshake);
            configRadio.setValue("radioPollInterval", radioTab[i]->getRadioData()->pollInterval);
            configRadio.setValue("transVertEnable", radioTab[i]->getRadioData()->transVertEnable);
            configRadio.setValue("netAddress", radioTab[i]->getRadioData()->networkAdd);
            configRadio.setValue("netPort", radioTab[i]->getRadioData()->networkPort);
            configRadio.setValue("mgmMode", radioTab[i]->getRadioData()->mgmMode);
            configRadio.endGroup();
        }


        // now save transvert settings
        if (radioTab[i]->getRadioData()->transVertEnable)
        {
            QString fileNameTransVert;
            if (appName == "")
            {
                fileNameTransVert = TRANSVERT_PATH_LOGGER + radioTab[i]->getRadioData()->radioName + FILENAME_TRANSVERT_RADIOS;
            }
            else
            {
                fileNameTransVert = TRANSVERT_PATH_LOGGER + radioTab[i]->getRadioData()->radioName + FILENAME_TRANSVERT_RADIOS;
            }

            QSettings  configTransVert(fileNameTransVert, QSettings::IniFormat);

            for (int t = 0; t < radioTab[i]->getRadioData()->numTransverters; t++)
            {
                if (radioTab[i]->transVertTab[t]->tansVertValueChanged)
                {
                    if (radioTab[i]->transVertTab[t]->transVertNameChanged)
                    {
                        radioTab[i]->transVertTab[t]->transVertNameChanged = false;
                        emit transVertNameChanged();
                    }

                    saveTranVerterSetting(i, t, configTransVert);


                    radioTab[i]->transVertTab[t]->tansVertValueChanged = false;
                }


            }




        }


            radioTab[i]->radioValueChanged = false;

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
        radioTab[i]->getRadioData()->radioName = config.value("radioName", "").toString();
        radioTab[i]->getRadioData()->radioNumber = config.value("radioNumber", QString::number(i+1)).toString();
        radioTab[i]->getRadioData()->radioMfg_Name = config.value("radioMfgName", "").toString();
        radioTab[i]->getRadioData()->radioModel = config.value("radioModel", "").toString();
        radioTab[i]->getRadioData()->radioModelName = config.value("radioModelName", "").toString();
        radioTab[i]->getRadioData()->radioModelNumber = config.value("radioModelNumber", "").toInt();
        radioTab[i]->getRadioData()->civAddress = config.value("civAddress", "").toString();
        radioTab[i]->getRadioData()->portType = config.value("portType", int(RIG_PORT_SERIAL)).toInt();
        radioTab[i]->getRadioData()->comport = config.value("comport", "").toString();
        radioTab[i]->getRadioData()->baudrate = config.value("baudrate", 9600).toInt();
        radioTab[i]->getRadioData()->databits = config.value("databits", 8).toInt();
        radioTab[i]->getRadioData()->parity = config.value("parity", 0).toInt();
        radioTab[i]->getRadioData()->stopbits = config.value("stopbits", 1).toInt();
        radioTab[i]->getRadioData()->handshake = config.value("handshake", 0).toInt();
        radioTab[i]->getRadioData()->pollInterval = config.value("radioPollInterval", "1").toString();
        radioTab[i]->getRadioData()->transVertEnable = config.value("transVertEnable", false).toBool();
        radioTab[i]->getRadioData()->networkAdd = config.value("netAddress", "").toString();
        radioTab[i]->getRadioData()->networkPort = config.value("netPort", "").toString();
        radioTab[i]->getRadioData()->mgmMode = config.value("mgmMode", hamlibData::USB).toString();
        config.endGroup();

        // now read transverter settings
        QString fileNameTransVert;
        if (appName == "")
        {
            fileNameTransVert = TRANSVERT_PATH_LOGGER + radioTab[i]->getRadioData()->radioName + FILENAME_TRANSVERT_RADIOS;
        }
        else
        {
            fileNameTransVert = TRANSVERT_PATH_LOGGER + radioTab[i]->getRadioData()->radioName + FILENAME_TRANSVERT_RADIOS;
        }

        QSettings  configTransVert(fileNameTransVert, QSettings::IniFormat);

        for (int t = 0; t < radioTab[i]->getRadioData()->numTransverters; t++)
        {
            readTranVerterSetting(i, t, configTransVert);
        }
    }
    chkloadflg = false;
}





void SetupDialog::saveTranVerterSetting(int radioNum, int transVertNum, QSettings  &config)
{
    config.beginGroup(radioTab[radioNum]->getRadioData()->transVertNames[transVertNum]);
    config.setValue("name", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertName);
    config.setValue("band", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->band);
    config.setValue("radioFreqStr", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->radioFreqStr);
    config.setValue("radioFreq", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->radioFreq);
    config.setValue("offsetString", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertOffsetStr);
    config.setValue("offsetDouble", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertOffset);
    config.setValue("negOffset", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertNegative);
    config.setValue("enableTransVertSw", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->enableTransSwitch);
    config.setValue("transVertSw", radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transSwitchNum);
    config.endGroup();
}



void SetupDialog::readTranVerterSetting(int radioNum, int transVertNum, QSettings  &config)
{
    config.beginGroup(radioTab[radioNum]->getRadioData()->transVertNames[transVertNum]);
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertName = config.value("name", "").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->band = config.value("band", "").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->radioFreqStr = config.value("radioFreqStr", "00.000.000.000").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->radioFreq = config.value("radioFreq", 0.0).toDouble();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertOffsetStr = config.value("offsetString", "00.000.000.000").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertOffsetStr = config.value("offsetString", "00.000.000.000").toString();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertOffset = config.value("offsetDouble", 0.0).toDouble();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transVertNegative = config.value("negOffset", false).toBool();
    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->enableTransSwitch = config.value("enableTransVertSw", false).toBool();

    radioTab[radioNum]->transVertTab[transVertNum]->setEnableTransVertSwBoxVisible(radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->enableTransSwitch);

    radioTab[radioNum]->getRadioData()->transVertSettings[transVertNum]->transSwitchNum = config.value("transVertSw", "0").toString();
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


/************************** Band Information ***********************************/

void SetupDialog::loadBands()
{
    BandList &blist = BandList::getBandList();

    for (int i = 6; i < 15; i++)   // just load VHF/UHF bands
    {
        bands.append(new BandDetail(blist.bandList[i].adif, blist.bandList[i].flow, blist.bandList[i].fhigh));
    }

}
