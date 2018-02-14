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
    connect(ui->addAntenna, SIGNAL(clicked(bool)), this, SLOT(addAntenna(bool)));
    connect(ui->removeAntenna, SIGNAL(clicked(bool)), this, SLOT(removeAntenna(bool)));


    // get the number of available antennas
    QString fileName;
    if (appName == "")
    {
        fileName = ANTENNA_PATH_LOCAL + FILENAME_AVAIL_ANTENNAS;
    }
    else
    {
        fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    }

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
//        antennaTab[i]->setAntennaName(availAntData[i]->antennaName); ***************************************************************
        // name the tab
        ui->antennaTab->setTabText(i, availAntData[i]->antennaName);
        antennaTab[i]->setRotatorModel(availAntData[i]->rotatorModel);
        antennaTab[i]->setPollInterval(availAntData[i]->pollInterval);
        antennaTab[i]->setCheckStop(availAntData[i]->southStopFlag);

        // set southstop visible if rotator is 0 - 360
        int minRot = 0;
        int maxRot = 0;
        if (antennaTab[i]->getMaxMinRotationData(availAntData[i]->rotatorModelNumber, &maxRot, &minRot) >= 0)
        {
            if (minRot == 0 && maxRot == 360)
            {
                antennaTab[i]->setCheckStopVisible(true);

            }
            else if (minRot == 0 && maxRot == 450)
            {
                antennaTab[i]->setOverRunFlagVisible(true);

            }
            else
            {
                antennaTab[i]->setCheckStopVisible(false);
                antennaTab[i]->setOverRunFlagVisible(false);
            }
        }

        antennaTab[i]->setCheckOverrun(availAntData[i]->overRunFlag);
        antennaTab[i]->setAntennaOffset(QString::number(availAntData[i]->antennaOffset));
        antennaTab[i]->setComport(availAntData[i]->comport);
        antennaTab[i]->setDataSpeed(QString::number(availAntData[i]->baudrate));
        antennaTab[i]->setDataBits(QString::number(availAntData[i]->databits));
        antennaTab[i]->setStopBits(QString::number(availAntData[i]->stopbits));
        antennaTab[i]->setParityBits(availAntData[i]->parity);
        antennaTab[i]->setHandshake(availAntData[i]->handshake);
        antennaTab[i]->setNetAddress(availAntData[i]->networkAdd);
        antennaTab[i]->setNetPortNum(availAntData[i]->networkPort);
        if (rig_port_e(availAntData[i]->portType) == RIG_PORT_NETWORK || rig_port_e(availAntData[i]->portType) == RIG_PORT_UDP_NETWORK)
        {
            antennaTab[i]->serialDataEntryVisible(false);
            antennaTab[i]->networkDataEntryVisible(true);
        }
        else if (rig_port_e(availAntData[i]->portType) == RIG_PORT_NONE)
        {
            antennaTab[i]->serialDataEntryVisible(false);
            antennaTab[i]->networkDataEntryVisible(false);
        }
        else if (rig_port_e(availAntData[i]->portType) == RIG_PORT_SERIAL)
        {
            antennaTab[i]->serialDataEntryVisible(true);
            antennaTab[i]->networkDataEntryVisible(false);
        }

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
    antennaTab.append(new rotSetupForm(rotator, availAntData[tabNum]));
//    antennaTab[tabNum]->setAntennaName(tabName); *****************************************
    ui->antennaTab->insertTab(tabNum, antennaTab[tabNum], tabName);
}



void SetupDialog::saveButtonPushed()
{

    saveSettings();

}



void SetupDialog::cancelButtonPushed()
{




}



void SetupDialog::saveSettings()
{

/***************************************************************

    // have the current antenna settings been changed?
    bool antennaNameChg = false;

    bool currentAntennaChanged = false;
    int ca = -1;
    bool ok;
    ca = currentAntenna->antennaNumber.toInt(&ok, 10);
    if (ok  && ca >= 0 && ca < numAvailAntennas)
    {
        if (currentAntenna->antennaNumber != "")
        {
            if (antennaValueChanged[ca-1])
            {
               currentAntennaChanged = true;
            }
        }
    }
    else
    {
        ca = -1;
    }


    if (antennaChanged)
    {
        QString fileName;
        if (appName == "")
        {
            fileName = ANTENNA_PATH_LOCAL + FILENAME_AVAIL_ANTENNAS;
        }
        else
        {
            fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
        }

        QSettings config(fileName, QSettings::IniFormat);

        for (int i = 0; i < numAvailAntennas; i++)
        {

            if (antennaValueChanged[i])
            {
                config.beginGroup("Antenna" + QString::number(i+1));
                config.setValue("antennaName", availAntData[i]->antennaName);
                if (antennaNameChanged[i])
                {
                    antennaNameChg = true;
                    antennaNameChanged[i] = false;
                }
                config.setValue("antennaNumber", i+1);
                config.setValue("rotatorModel", availAntData[i]->rotatorModel);
                config.setValue("rotatorModelName", availAntData[i]->rotatorModelName);
                config.setValue("rotatorModelNumber", availAntData[i]->rotatorModelNumber);
                config.setValue("rotatorManufacturer", availAntData[i]->rotatorManufacturer);
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
                antennaValueChanged[i] = false;

            }

        }

   }
   antennaChanged = false;

   if (antennaNameChg)
   {
       emit antennaNameChange();
   }

   if (currentAntennaChanged && ca != -1)
   {

      emit currentAntennaSettingChanged(availAntData[ca]->antennaName);

   }

*/
}






void SetupDialog::saveAntenna(int i)
{

    QString fileName;
    if (appName == "")
    {
        fileName = ANTENNA_PATH_LOCAL + FILENAME_AVAIL_ANTENNAS;
    }
    else
    {
        fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    }

    QSettings  config(fileName, QSettings::IniFormat);

    config.beginGroup(availAntData[i]->antennaName);
    config.setValue("antennaName", availAntData[i]->antennaName);
    config.setValue("antennaNumber", i+1);
    config.setValue("rotatorModel", availAntData[i]->rotatorModel);
    config.setValue("rotatorModelName", availAntData[i]->rotatorModelName);
    config.setValue("rotatorModelNumber", availAntData[i]->rotatorModelNumber);
    config.setValue("rotatorManufacturer", availAntData[i]->rotatorManufacturer);
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
    if (appName == "")
    {
        fileName = ANTENNA_PATH_LOCAL + FILENAME_AVAIL_ANTENNAS;
    }
    else
    {
        fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    }

    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < numAvailAntennas; i++)
    {
        config.beginGroup(availAntennas[i]);
        availAntData[i]->antennaName = config.value("antennaName", "").toString();
        availAntData[i]->antennaNumber = config.value("antennaNumber", QString::number(i+1)).toString();
        availAntData[i]->rotatorModel = config.value("rotatorModel", "").toString();
        availAntData[i]->rotatorModelName = config.value("rotatorModelName", "").toString();
        availAntData[i]->rotatorModelNumber = config.value("rotatorModelNumber", "").toInt();
        availAntData[i]->rotatorManufacturer = config.value("rotatorManufacturer", "").toString();
        availAntData[i]->pollInterval = config.value("rotatorPollInterval", "1").toString();
        availAntData[i]->southStopFlag = config.value("southStop", false).toBool();
        availAntData[i]->overRunFlag = config.value("overRun", false).toBool();
        availAntData[i]->antennaOffset = config.value("antennaOffset", "").toInt();
        availAntData[i]->portType = config.value("portType", int(RIG_PORT_NONE)).toInt();
        availAntData[i]->comport = config.value("comport", "").toString();
        availAntData[i]->baudrate = config.value("baudrate", 9600).toInt();
        availAntData[i]->databits = config.value("databits", 8).toInt();
        availAntData[i]->parity = config.value("parity", 0).toInt();
        availAntData[i]->stopbits = config.value("stopbits", 1).toInt();
        availAntData[i]->handshake = config.value("handshake", 0).toInt();
        availAntData[i]->networkAdd = config.value("netAddress", "").toString();
        availAntData[i]->networkPort = config.value("netPort", "").toString();
        config.endGroup();
    }
    chkloadflg = false;

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
    if (appName == "")
    {
        fileName = ANTENNA_PATH_LOCAL + LOCAL_ANTENNA + FILENAME_CURRENT_ANTENNA;
    }
    else
    {
        fileName = ANTENNA_PATH_LOGGER + appName + FILENAME_CURRENT_ANTENNA;
    }

    QSettings config(fileName, QSettings::IniFormat);


    config.beginGroup("CurrentAntenna");
    config.setValue("antennaName", currentAntenna->antennaName);
    config.endGroup();




}

void SetupDialog::readCurrentAntenna()
{

    QString fileName;
    if (appName == "")
    {
        fileName = ANTENNA_PATH_LOCAL + LOCAL_ANTENNA + FILENAME_CURRENT_ANTENNA;
    }
    else
    {
        fileName = ANTENNA_PATH_LOGGER + appName + FILENAME_CURRENT_ANTENNA;
    }

    QSettings config(fileName, QSettings::IniFormat);

    {
        config.beginGroup("CurrentAntenna");
        currentAntennaName = config.value("antennaName", "").toString();

        config.endGroup();
    }

    // find index to antenna data
    for (int i = 0; i < numAvailAntennas; i++)
    {
        if (currentAntennaName == antennaTab[i]->antennaData->antennaName)
        {
            currentAntIndex = i;
            // load current antenna data from available antennas
            currentAntenna = antennaTab[i]->antennaData;
            break;
        }
    }

}


void SetupDialog::setAppName(QString name)
{
    appName = name;
}


void SetupDialog::addAntenna(bool /*st*/)
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
  numAvailAntennas++;
  addTab(numAvailAntennas - 1, antName);
  saveAntenna(numAvailAntennas - 1);
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


void SetupDialog::removeAntenna(bool /*st*/)
{

    int currentIndex = ui->antennaTab->currentIndex();
    QString currentName = ui->antennaTab->tabText(currentIndex);
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
    // remove from availantenna file
    QString fileName;
    if (appName == "")
    {
        fileName = ANTENNA_PATH_LOCAL + FILENAME_AVAIL_ANTENNAS;
    }
    else
    {
        fileName = ANTENNA_PATH_LOGGER + FILENAME_AVAIL_ANTENNAS;
    }

    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup(currentName);
    config.remove("");   // remove all keys for this group
    config.endGroup();

    numAvailAntennas--;

    emit antennaTabChanged();



}

srotParams SetupDialog::getCurrentAntenna() const
{
/*

    return currentAntenna;
*/
}





