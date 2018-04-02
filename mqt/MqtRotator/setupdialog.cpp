/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
// Hamlib Library
//
/////////////////////////////////////////////////////////////////////////////





#include "setupdialog.h"
#include "ui_setupdialog.h"
#include "rotcontrol.h"

//#include <QSignalMapper>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
//#include <QtSerialPort/QSerialPort>
//#include <QSerialPortInfo>

//#include <QIntValidator>
#include <QMessageBox>
#include <QInputDialog>

#include <QDebug>




SetupDialog::SetupDialog(RotControl *rotator, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    rotator = rotator;

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));
    connect(ui->addAntenna, SIGNAL(clicked()), this, SLOT(addAntenna()));
    connect(ui->removeAntenna, SIGNAL(clicked()), this, SLOT(removeAntenna()));
    connect(ui->editAntName, SIGNAL(clicked()), this, SLOT(editAntennaName()));

    // get the number of available antennas
    QString fileName;
    fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    QSettings settings(fileName, QSettings::IniFormat);
    availAntennas = settings.childGroups();
    numAvailAntennas = availAntennas.count();

    if (numAvailAntennas > 0)
    {
        for (int i = 0; i < numAvailAntennas; i++)
        {
            addTab(i, "");
        }

    }




    //clearAvailRotators(); // clear the AvailRotator table, also init with default serial parameters
    //clearCurrentRotator(); // clear the currently selected Rotator table, also init with default serial parameters
    //clearAntennaValueChanged();
    //clearAntennaNameChanged();



    getAvailAntennas();

    // load current settings to each tab
    for (int i = 0; i < numAvailAntennas; i++)
    {

        loadSettingsToTab(i);

    }


}



SetupDialog::~SetupDialog()
{
    delete ui;
}



void SetupDialog::addTab(int tabNum, QString tabName)
{
    availAntData.append(new srotParams);
    availAntData[tabNum]->antennaName = tabName;
    availAntennas.append(tabName);
    antennaTab.append(new rotSetupForm(rotator, availAntData[tabNum]));
    ui->antennaTab->insertTab(tabNum, antennaTab[tabNum], tabName);
    ui->antennaTab->setTabColor(tabNum, Qt::darkBlue);


}



void SetupDialog::loadSettingsToTab(int tabNum)
{


    ui->antennaTab->setTabText(tabNum, availAntData[tabNum]->antennaName);

    if (availAntData[tabNum]->rotatorModel != "")
    {
        antennaTab[tabNum]->setRotatorModel(availAntData[tabNum]->rotatorModel);
        antennaTab[tabNum]->setPollInterval(availAntData[tabNum]->pollInterval);
        antennaTab[tabNum]->pollIntervalVisible(true);
        antennaTab[tabNum]->setCheckStop(availAntData[tabNum]->southStopFlag);

        // set southstop visible if rotator is 0 - 360

        if (availAntData[tabNum]->endStopType == ROT_0_360)
        {
           antennaTab[tabNum]->setCheckStopVisible(true);
        }
        else if (availAntData[tabNum]->endStopType == ROT_0_450)
        {
           antennaTab[tabNum]->setOverRunFlagVisible(true);
        }
        else
        {
           antennaTab[tabNum]->setCheckStopVisible(false);
           antennaTab[tabNum]->setOverRunFlagVisible(false);
        }

        antennaTab[tabNum]->setCheckOverrun(availAntData[tabNum]->overRunFlag);
        antennaTab[tabNum]->setAntennaOffset(QString::number(availAntData[tabNum]->antennaOffset));
        antennaTab[tabNum]->antennaOffSetVisible(true);
        antennaTab[tabNum]->setComport(availAntData[tabNum]->comport);
        antennaTab[tabNum]->setDataSpeed(QString::number(availAntData[tabNum]->baudrate));
        antennaTab[tabNum]->setDataBits(QString::number(availAntData[tabNum]->databits));
        antennaTab[tabNum]->setStopBits(QString::number(availAntData[tabNum]->stopbits));
        antennaTab[tabNum]->setParityBits(availAntData[tabNum]->parity);
        antennaTab[tabNum]->setHandshake(availAntData[tabNum]->handshake);
        antennaTab[tabNum]->setNetAddress(availAntData[tabNum]->networkAdd);
        antennaTab[tabNum]->setNetPortNum(availAntData[tabNum]->networkPort);

        if (availAntData[tabNum]->portType == RIG_PORT_NETWORK || availAntData[tabNum]->portType == RIG_PORT_UDP_NETWORK)
        {
            antennaTab[tabNum]->serialDataEntryVisible(false);
            antennaTab[tabNum]->networkDataEntryVisible(true);
        }
        else if (availAntData[tabNum]->portType == RIG_PORT_NONE)
        {
            antennaTab[tabNum]->serialDataEntryVisible(false);
            antennaTab[tabNum]->networkDataEntryVisible(false);
        }
        else if (availAntData[tabNum]->portType == RIG_PORT_SERIAL)
        {
            antennaTab[tabNum]->serialDataEntryVisible(true);
            antennaTab[tabNum]->networkDataEntryVisible(false);
        }


    }
    else
    {
        // no rotator model selected, hide elements on tab
        antennaTab[tabNum]->setRotatorModel(availAntData[tabNum]->rotatorModel);
        antennaTab[tabNum]->networkDataEntryVisible(false);
        antennaTab[tabNum]->setOverRunFlagVisible(false);
        antennaTab[tabNum]->setCheckStopVisible(false);
        antennaTab[tabNum]->pollIntervalVisible(false);
        antennaTab[tabNum]->antennaOffSetVisible(false);
        antennaTab[tabNum]->serialDataEntryVisible(false);

    }


}


void SetupDialog::setTabToCurrentAntenna()
{

    for (int i = 0; i < numAvailAntennas; i++)
    {
        if (currentAntennaName == availAntData[i]->antennaName)
        {
            ui->antennaTab->setTabColor(i, Qt::red);
            ui->antennaTab->setCurrentIndex(i);
        }
        else
        {
            ui->antennaTab->setTabColor(i,Qt::darkBlue);
        }


    }
}


void SetupDialog::saveButtonPushed()
{

    saveSettings();

}



void SetupDialog::cancelButtonPushed()
{

    QString fileName;
    fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < numAvailAntennas; i++)
    {
        if (antennaTab[i]->antennaValueChanged)
        {
            getAvailAntenna(i, config);         // restore settings
            antennaTab[i]->antennaValueChanged = false;
        }
    }


}



void SetupDialog::saveSettings()
{


    bool antennaNameChg = false;
    bool currentAntennaChanged = false;

    QString fileName;
    fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < numAvailAntennas; i++)
    {

        if (antennaTab[i]->antennaValueChanged)
        {
            config.beginGroup(availAntData[i]->antennaName);
            if (currentAntennaName == availAntData[i]->antennaName)
            {
                // settings changed in current antenna
                currentAntennaChanged = true;
            }


            config.setValue("antennaName", availAntData[i]->antennaName);
            if (antennaTab[i]->antennaNameChanged)
            {
                antennaNameChg = true;
                antennaTab[i]->antennaNameChanged = false;
            }
            config.setValue("antennaNumber", i+1);
            config.setValue("rotatorModel", availAntData[i]->rotatorModel);
            config.setValue("rotatorModelName", availAntData[i]->rotatorModelName);
            config.setValue("rotatorModelNumber", availAntData[i]->rotatorModelNumber);
            config.setValue("rotatorManufacturer", availAntData[i]->rotatorManufacturer);
            config.setValue("rotatorMaxAzimuth", double(availAntData[i]->max_azimuth));
            config.setValue("rotatorMinAzimuth", double(availAntData[i]->min_azimuth));
            config.setValue("rotatorCWEndStop", double(availAntData[i]->rotatorCWEndStop));
            config.setValue("rotatorCCWEndStop", double(availAntData[i]->rotatorCCWEndStop));
            config.setValue("rotatorEndStopType", availAntData[i]->endStopType);
            config.setValue("supportCwCcwCmd", availAntData[i]->supportCwCcwCmd);
            config.setValue("rotatorPollInterval", availAntData[i]->pollInterval);
            config.setValue("simulateCwCCw", availAntData[i]->simCwCcwCmd);
            config.setValue("southStop", availAntData[i]->southStopFlag);
            config.setValue("overRun", availAntData[i]->overRunFlag);
            config.setValue("antennaOffset", availAntData[i]->antennaOffset);
            config.setValue("portType", availAntData[i]->portType);
            config.setValue("comport", availAntData[i]->comport);
            config.setValue("baudrate", availAntData[i]->baudrate);
            config.setValue("databits", availAntData[i]->databits);
            config.setValue("parity", availAntData[i]->parity);
            config.setValue("stopbits", availAntData[i]->stopbits);
            config.setValue("handshake", availAntData[i]->handshake);
            config.setValue("netAddress", availAntData[i]->networkAdd);
            config.setValue("netPort", availAntData[i]->networkPort);
            config.endGroup();
            antennaTab[i]->antennaValueChanged = false;

        }

    }




   if (antennaNameChg)
   {
       emit antennaNameChange();
   }

   if (currentAntennaChanged)
   {

      emit currentAntennaSettingChanged(currentAntennaName);

   }


}






void SetupDialog::saveAntenna(int i)
{

    QString fileName;
    fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    QSettings  config(fileName, QSettings::IniFormat);

    config.beginGroup(availAntData[i]->antennaName);
    config.setValue("antennaName", availAntData[i]->antennaName);
    config.setValue("antennaNumber", i+1);
    config.setValue("rotatorModel", availAntData[i]->rotatorModel);
    config.setValue("rotatorModelName", availAntData[i]->rotatorModelName);
    config.setValue("rotatorModelNumber", availAntData[i]->rotatorModelNumber);
    config.setValue("rotatorManufacturer", availAntData[i]->rotatorManufacturer);
    config.setValue("rotatorMaxAzimuth", double(availAntData[i]->max_azimuth));
    config.setValue("rotatorMinAzimuth", double(availAntData[i]->min_azimuth));
    config.setValue("rotatorCWEndStop",double( availAntData[i]->rotatorCWEndStop));
    config.setValue("rotatorCCWEndStop", double(availAntData[i]->rotatorCCWEndStop));
    config.setValue("rotatorEndStopType", availAntData[i]->endStopType);
    config.setValue("supportCwCcwCmd", availAntData[i]->supportCwCcwCmd);
    config.setValue("simulateCwCCw", availAntData[i]->simCwCcwCmd);
    config.setValue("rotatorPollInterval", availAntData[i]->pollInterval);
    config.setValue("southStop", availAntData[i]->southStopFlag);
    config.setValue("overRun", availAntData[i]->overRunFlag);
    config.setValue("antennaOffset", availAntData[i]->antennaOffset);
    config.setValue("portType", availAntData[i]->portType);
    config.setValue("comport", availAntData[i]->comport);
    config.setValue("baudrate", availAntData[i]->baudrate);
    config.setValue("databits", availAntData[i]->databits);
    config.setValue("parity", availAntData[i]->parity);
    config.setValue("stopbits", availAntData[i]->stopbits);
    config.setValue("handshake", availAntData[i]->handshake);
    config.setValue("netAddress", availAntData[i]->networkAdd);
    config.setValue("netPort", availAntData[i]->networkPort);
    config.endGroup();





}




void SetupDialog::getAvailAntennas()
{
    chkloadflg = true;      // stop loading check values tiggering mapper signals

    QString fileName;
    fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < numAvailAntennas; i++)
    {
        getAvailAntenna(i, config);
    }
    chkloadflg = false;

}


void SetupDialog::getAvailAntenna(int antNum, QSettings& config)
{

    config.beginGroup(availAntennas[antNum]);
    availAntData[antNum]->antennaName = config.value("antennaName", "").toString();
    availAntData[antNum]->antennaNumber = config.value("antennaNumber", QString::number(antNum+1)).toString();
    availAntData[antNum]->rotatorModel = config.value("rotatorModel", "").toString();
    availAntData[antNum]->rotatorModelName = config.value("rotatorModelName", "").toString();
    availAntData[antNum]->rotatorModelNumber = config.value("rotatorModelNumber", "").toInt();
    availAntData[antNum]->rotatorManufacturer = config.value("rotatorManufacturer", "").toString();
    availAntData[antNum]->pollInterval = config.value("rotatorPollInterval", "1").toString();
    availAntData[antNum]->max_azimuth = azimuth_t(config.value("rotatorMaxAzimuth", 360).toDouble());
    availAntData[antNum]->min_azimuth = azimuth_t(config.value("rotatorMinAzimuth", 0).toDouble());
    availAntData[antNum]->rotatorCWEndStop = azimuth_t(config.value("rotatorCWEndStop", 360).toDouble());
    availAntData[antNum]->rotatorCCWEndStop = azimuth_t(config.value("rotatorCWEndStop", 0).toDouble());
    availAntData[antNum]->endStopType = endStop(config.value("rotatorEndStopType", int(ROT_0_360)).toInt());
    availAntData[antNum]->supportCwCcwCmd = config.value("supportCwCcwCmd", false).toBool();
    availAntData[antNum]->simCwCcwCmd = config.value("simulateCwCCw", true).toBool();
    availAntData[antNum]->southStopFlag = config.value("southStop", false).toBool();
    availAntData[antNum]->overRunFlag = config.value("overRun", false).toBool();
    availAntData[antNum]->antennaOffset = config.value("antennaOffset", "").toInt();
    availAntData[antNum]->portType = rig_port_e(config.value("portType", int(RIG_PORT_NONE)).toInt());
    availAntData[antNum]->comport = config.value("comport", "").toString();
    availAntData[antNum]->baudrate = config.value("baudrate", 9600).toInt();
    availAntData[antNum]->databits = config.value("databits", 8).toInt();
    availAntData[antNum]->parity = config.value("parity", 0).toInt();
    availAntData[antNum]->stopbits = config.value("stopbits", 1).toInt();
    availAntData[antNum]->handshake = config.value("handshake", 0).toInt();
    availAntData[antNum]->networkAdd = config.value("netAddress", "").toString();
    availAntData[antNum]->networkPort = config.value("netPort", "").toString();
    config.endGroup();

}


int SetupDialog::comportAvial(QString comport)
{
    if (antennaTab.count() > 0)
    {
        return antennaTab[0]->comportAvial(comport);
    }
    else
    {
        return -1;
    }
}

//void SetupDialog::clearAvailRotators()
//{
//    srotParams nulParam;

//    for (int i = 0; i < numAvailAntennas; i++)
//    {
//        availAntData[i] = nulParam;
//    }




//void SetupDialog::clearCurrentRotator()
//{
//    srotParams nulParam;
//    currentAntenna = nulParam;
//}


//void SetupDialog::copyAntennaToCurrent(int antennaNumber)
//{
//    currentAntenna = availAntennas[antennaNumber];
//}

//void SetupDialog::clearAntennaValueChanged()
//{
//    for (int i = 0; i < NUM_ANTENNAS; i++)
//    {
//        antennaValueChanged[i] = false;
//    }
//}

//void SetupDialog::clearAntennaNameChanged()
//{
//    for (int i = 0; i < NUM_ANTENNAS; i++)
//    {
//        antennaNameChanged[i] = false;
//    }
//}


QString SetupDialog::getRotatorComPort(QString antennaName)
{

    for (int i = 0; i < numAvailAntennas ; i++)
    {
        if (availAntData[i]->antennaName == antennaName)
        {
            return availAntData[i]->comport;
        }
    }

    return ""; // error, none found

}



void SetupDialog::saveCurrentAntenna()
{

    QString fileName;
    fileName = ANTENNA_PATH_LOGGER + appName + FILENAME_CURRENT_ANTENNA;
    QSettings config(fileName, QSettings::IniFormat);


    config.beginGroup("CurrentAntenna");
    config.setValue("antennaName", currentAntennaName);
    config.endGroup();




}

void SetupDialog::readCurrentAntenna()
{

    QString fileName;
    fileName = ANTENNA_PATH_LOGGER + appName + FILENAME_CURRENT_ANTENNA;
    QSettings config(fileName, QSettings::IniFormat);

    {
        config.beginGroup("CurrentAntenna");
        currentAntennaName = config.value("antennaName", "").toString();

        config.endGroup();
    }

}



int SetupDialog::findCurrentAntenna(QString currentAntName)
{
    int err = -1;
    for (int i = 0; i < numAvailAntennas; i++)
    {
        if (currentAntName == availAntData[i]->antennaName)
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


void SetupDialog::addAntenna()
{

  QString antName = QInputDialog::getText(this, tr("Enter Antenna Name"), tr("Please enter an Antenna Name:"), QLineEdit::Normal);
  antName = antName.trimmed();
  if (antName.isEmpty())
  {
        return;
  }
  if (checkAntNameMatch(antName))
  {
      // error empty name or name already exists
      QMessageBox::information(this, tr("Antenna Name Exists"),
                               tr("Antenna Name: %1, already exists \nPlease enter another name").arg(antName.trimmed()),
                                QMessageBox::Ok|QMessageBox::Default,
                                QMessageBox::NoButton, QMessageBox::NoButton);
      return;
  }

  // add the new antenna
  int tabNum = numAvailAntennas;
  addTab(tabNum, antName);
  numAvailAntennas++;
  loadSettingsToTab(tabNum);
  saveAntenna(tabNum);
  emit antennaTabChanged();


}

bool SetupDialog::checkAntNameMatch(QString antName)
{
    for (int i = 0; i < numAvailAntennas; i++)
    {
        if (ui->antennaTab->tabText(i) == antName)
            return true;
    }

    return false;
}


void SetupDialog::removeAntenna()
{

    int currentIndex = ui->antennaTab->currentIndex();
    QString currentName = ui->antennaTab->tabText(currentIndex);

    if (currentAntennaName == ui->antennaTab->tabText(currentIndex))
    {
        // can't remove current antennaName
        QMessageBox msgBox;
        msgBox.setText("You can not remove the current antenna!");
        msgBox.exec();
        return;

    }

    int status = QMessageBox::question( this,
                            tr("Remove Antenna"),
                            tr("Do you really want to remove antenna - %1?")
                            .arg(currentName),
                            QMessageBox::Yes|QMessageBox::Default,
                            QMessageBox::No|QMessageBox::Escape,
                            QMessageBox::NoButton);

    if (status != QMessageBox::Yes)
    {
        return;
    }

    // remove this antenna
    ui->antennaTab->removeTab(currentIndex);
    availAntData.remove(currentIndex);
    availAntennas.removeAt(currentIndex);
    // remove from availantenna file
    QString fileName;
    fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup(currentName);
    config.remove("");   // remove all keys for this group
    config.endGroup();

    numAvailAntennas--;

    emit antennaTabChanged();



}


void SetupDialog::editAntennaName()
{
    int tabNum = ui->antennaTab->currentIndex();
    QString antName = ui->antennaTab->tabText(tabNum);
    if (currentAntennaName == antName)
    {
        // can't change current antennaName
        QMessageBox msgBox;
        msgBox.setText(QString("You can not change the name of the current antenna - %1!").arg(antName));
        msgBox.exec();
        return;
    }

    bool ok;
    QString text = QInputDialog::getText(this, QString("Edit Antenna Name - %1").arg(antName),
                                         tr("New Antenna Name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
    {
        ui->antennaTab->setTabText(tabNum, text);
        for (int i = 0; i < numAvailAntennas; i++)
        {
            if (antName == availAntData[i]->antennaName)
            {
                availAntData[i]->antennaName = text;  // update with new name
                availAntennas[i] = text;
                // remove from availantenna file
                QString fileName;
                fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
                QSettings config(fileName, QSettings::IniFormat);
                config.beginGroup(antName);
                config.remove("");   // remove all keys for this group
                config.endGroup();

                saveAntenna(tabNum);

            }
        }
    }
    else
    {
        return;
    }


    emit antennaTabChanged();


}




QString SetupDialog::getCurrentAntenna() const
{


    return currentAntennaName;

}





