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
#include <QSignalMapper>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>
//#include <QIntValidator>
#include <QMessageBox>

#include <QDebug>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");


SetupDialog::SetupDialog(RotControl *rotator, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    rotator = rotator;



    antennaName[0] = ui->antennaNameEdit1;
    antennaName[1] = ui->antennaNameEdit2;
    antennaName[2] = ui->antennaNameEdit3;
    antennaName[3] = ui->antennaNameEdit4;
    antennaName[4] = ui->antennaNameEdit5;

    rotatorModel[0] = ui->rotatorModelBox1;
    rotatorModel[1] = ui->rotatorModelBox2;
    rotatorModel[2] = ui->rotatorModelBox3;
    rotatorModel[3] = ui->rotatorModelBox4;
    rotatorModel[4] = ui->rotatorModelBox5;

    southStopFlag[0] = ui->chkSstop1;
    southStopFlag[1] = ui->chkSstop2;
    southStopFlag[2] = ui->chkSstop3;
    southStopFlag[3] = ui->chkSstop4;
    southStopFlag[4] = ui->chkSstop5;

    // set southStop invisible
    ui->chkSstop1->setVisible(false);
    ui->chkSstop2->setVisible(false);
    ui->chkSstop3->setVisible(false);
    ui->chkSstop4->setVisible(false);
    ui->chkSstop5->setVisible(false);
    ui->s_StopLbl1->setVisible(false);
    ui->s_StopLbl2->setVisible(false);
    ui->s_StopLbl3->setVisible(false);
    ui->s_StopLbl4->setVisible(false);
    ui->s_StopLbl5->setVisible(false);


    overRunFlag[0] = ui->chkOverrun1;
    overRunFlag[1] = ui->chkOverrun2;
    overRunFlag[2] = ui->chkOverrun3;
    overRunFlag[3] = ui->chkOverrun4;
    overRunFlag[4] = ui->chkOverrun5;

    antennaOffset[0] = ui->lEditOffset1;
    antennaOffset[1] = ui->lEditOffset2;
    antennaOffset[2] = ui->lEditOffset3;
    antennaOffset[3] = ui->lEditOffset4;
    antennaOffset[4] = ui->lEditOffset5;


    comPorts[0] = ui->comPortBox1;
    comPorts[1] = ui->comPortBox2;
    comPorts[2] = ui->comPortBox3;
    comPorts[3] = ui->comPortBox4;
    comPorts[4] = ui->comPortBox5;

    comSpeed[0] = ui->comSpeedBox1;
    comSpeed[1] = ui->comSpeedBox2;
    comSpeed[2] = ui->comSpeedBox3;
    comSpeed[3] = ui->comSpeedBox4;
    comSpeed[4] = ui->comSpeedBox5;

    comDataBits[0] = ui->comDataBitsBox1;
    comDataBits[1] = ui->comDataBitsBox2;
    comDataBits[2] = ui->comDataBitsBox3;
    comDataBits[3] = ui->comDataBitsBox4;
    comDataBits[4] = ui->comDataBitsBox5;

    comStopBits[0] = ui->comStopBitsBox1;
    comStopBits[1] = ui->comStopBitsBox2;
    comStopBits[2] = ui->comStopBitsBox3;
    comStopBits[3] = ui->comStopBitsBox4;
    comStopBits[4] = ui->comStopBitsBox5;

    comParity[0] = ui->comParityBox1;
    comParity[1] = ui->comParityBox2;
    comParity[2] = ui->comParityBox3;
    comParity[3] = ui->comParityBox4;
    comParity[4] = ui->comParityBox5;

    comHandShake[0] = ui->comHandShakeBox1;
    comHandShake[1] = ui->comHandShakeBox2;
    comHandShake[2] = ui->comHandShakeBox3;
    comHandShake[3] = ui->comHandShakeBox4;
    comHandShake[4] = ui->comHandShakeBox5;

//--------------------------------------------------------------------------------------------------

    QSignalMapper *antennaName_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
       antennaName_mapper->setMapping(antennaName[i], i);
        connect(antennaName[i], SIGNAL(editingFinished()), antennaName_mapper, SLOT(map()));
    }
    connect(antennaName_mapper, SIGNAL(mapped(int)), this, SLOT(antennaNameFinished(int)));

//--------------------------------------------------------------------------------------------------

    QSignalMapper *rotatorModel_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        rotatorModel_mapper->setMapping(rotatorModel[i], i);
        connect(rotatorModel[i], SIGNAL(activated(int)), rotatorModel_mapper, SLOT(map()));
    }
    connect(rotatorModel_mapper, SIGNAL(mapped(int)), this, SLOT(rotatorModelSelected(int)));

//--------------------------------------------------------------------------------------------------

    QSignalMapper *southStopFlag_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        southStopFlag_mapper->setMapping(southStopFlag[i], i);
        connect(southStopFlag[i], SIGNAL(stateChanged(int)), southStopFlag_mapper, SLOT(map()));
    }
    connect(southStopFlag_mapper, SIGNAL(mapped(int)), this, SLOT(southStopFlagSelected(int)));

//---------------------------------------------------------------------------------------------------

    QSignalMapper *overRunFlag_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        overRunFlag_mapper->setMapping(overRunFlag[i], i);
        connect(overRunFlag[i], SIGNAL(stateChanged(int)), overRunFlag_mapper, SLOT(map()));
    }
    connect(overRunFlag_mapper, SIGNAL(mapped(int)), this, SLOT(overRunFlagSelected(int)));

//--------------------------------------------------------------------------------------------------

    QSignalMapper *antennaOffset_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        antennaOffset_mapper->setMapping(antennaOffset[i], i);
        connect(antennaOffset[i], SIGNAL(editingFinished()), antennaOffset_mapper, SLOT(map()));
    }
    connect(antennaOffset_mapper, SIGNAL(mapped(int)), this, SLOT(antennaOffsetFinished(int)));

//--------------------------------------------------------------------------------------------------

    QSignalMapper *comPorts_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comPorts_mapper->setMapping(comPorts[i], i);
        connect(comPorts[i], SIGNAL(activated(int)), comPorts_mapper, SLOT(map()));
    }

    connect(comPorts_mapper, SIGNAL(mapped(int)), this, SLOT(comportSelected(int)));

//--------------------------------------------------------------------------------------------------

    QSignalMapper *comSpeed_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comSpeed_mapper->setMapping(comSpeed[i], i);
        connect(comSpeed[i], SIGNAL(activated(int)), comSpeed_mapper, SLOT(map()));
    }

    connect(comSpeed_mapper, SIGNAL(mapped(int)), this, SLOT(comSpeedSelected(int)));

//--------------------------------------------------------------------------------------------------

    QSignalMapper *comDataBits_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comDataBits_mapper->setMapping(comDataBits[i], i);
        connect(comDataBits[i], SIGNAL(activated(int)), comDataBits_mapper, SLOT(map()));
    }

    connect(comDataBits_mapper, SIGNAL(mapped(int)), this, SLOT(comDataBitsSelected(int)));

//--------------------------------------------------------------------------------------------------

    QSignalMapper *comStopBits_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comStopBits_mapper->setMapping(comStopBits[i], i);
        connect(comStopBits[i], SIGNAL(activated(int)), comStopBits_mapper, SLOT(map()));
    }

    connect(comStopBits_mapper, SIGNAL(mapped(int)), this, SLOT(comStopBitsSelected(int)));

//--------------------------------------------------------------------------------------------------

    QSignalMapper *comParity_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comParity_mapper->setMapping(comParity[i], i);
        connect(comParity[i], SIGNAL(activated(int)), comParity_mapper, SLOT(map()));
    }

    connect(comParity_mapper, SIGNAL(mapped(int)), this, SLOT(comParitySelected(int)));

//--------------------------------------------------------------------------------------------------

    QSignalMapper *comHandShake_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comHandShake_mapper->setMapping(comHandShake[i], i);
        connect(comHandShake[i], SIGNAL(activated(int)), comHandShake_mapper, SLOT(map()));
    }

    connect(comHandShake_mapper, SIGNAL(mapped(int)), this, SLOT(comHandShakeSelected(int)));

//--------------------------------------------------------------------------------------------------

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));




    fillRotatorModelInfo();  // add rotator models to drop down
    fillPortsInfo();     // add comports to drop down
    fillSpeedInfo();
    fillDataBitsInfo();
    fillStopBitsInfo();
    fillParityInfo();
    fillHandShakeInfo();
    clearAvailRotators(); // clear the AvailRotator table, also init with default serial parameters
    clearCurrentRotator(); // clear the currently selected Rotator table, also init with default serial parameters
    clearAntennaValueChanged();


    readSettings();

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        antennaName[i]->setText(availAntennas[i].antennaName);
        rotatorModel[i]->setCurrentIndex(rotatorModel[i]->findText(availAntennas[i].rotatorModel));
        southStopFlag[i]->setChecked(availAntennas[i].southStopFlag);
        overRunFlag[i]->setChecked(availAntennas[i].overRunFlag);
        antennaOffset[i]->setText(QString::number(availAntennas[i].antennaOffset));
        comPorts[i]->setCurrentIndex(comPorts[i]->findText(availAntennas[i].comport));
        comSpeed[i]->setCurrentIndex(comSpeed[i]->findText(QString::number(availAntennas[i].baudrate)));
        comDataBits[i]->setCurrentIndex(comDataBits[i]->findText(QString::number(availAntennas[i].databits)));
        comStopBits[i]->setCurrentIndex(comStopBits[i]->findText(QString::number(availAntennas[i].stopbits)));
        comParity[i]->setCurrentIndex(availAntennas[i].parity);
        comHandShake[i]->setCurrentIndex(availAntennas[i].handshake);

    }


}



SetupDialog::~SetupDialog()
{
    delete ui;
}


void SetupDialog::antennaNameFinished(int boxNumber)
{

    if (antennaName[boxNumber]->text() != availAntennas[boxNumber].antennaName)
    {
        availAntennas[boxNumber].antennaName = antennaName[boxNumber]->text();
        antennaValueChanged[boxNumber] = true;
        antennaNameChanged[boxNumber] = true;
        antennaChanged = true;

    }
}


void SetupDialog::rotatorModelSelected(int boxNumber)
{

    bool ok;

    if (rotatorModel[boxNumber]->currentText() != availAntennas[boxNumber].rotatorModel)
    {
        QString s = rotatorModel[boxNumber]->currentText();
        availAntennas[boxNumber].rotatorModel = s;
        s = s.trimmed();
        QStringList antdetails = s.split(',');
        s = antdetails[0];
        availAntennas[boxNumber].rotatorModelNumber = s.toInt(&ok, 10);
        s = antdetails[1];
        availAntennas[boxNumber].rotatorManufacturer = s.trimmed();
        s = antdetails[2];
        availAntennas[boxNumber].rotatorModelName = s.trimmed();

        antennaValueChanged[boxNumber] = true;
        antennaChanged = true;

    }
}


void SetupDialog::southStopFlagSelected(int boxNumber)
{
    if (southStopFlag[boxNumber]->isChecked() != availAntennas[boxNumber].southStopFlag)
    {
        availAntennas[boxNumber].southStopFlag = southStopFlag[boxNumber]->isChecked();
        antennaValueChanged[boxNumber] = true;
        antennaChanged = true;
    }
}

void SetupDialog::overRunFlagSelected(int boxNumber)
{
    if (overRunFlag[boxNumber]->isChecked() != availAntennas[boxNumber].overRunFlag)
    {
        availAntennas[boxNumber].overRunFlag = overRunFlag[boxNumber]->isChecked();
        antennaValueChanged[boxNumber] = true;
        antennaChanged = true;
    }
}

void SetupDialog::antennaOffsetFinished(int boxNumber)
{

    QMessageBox messageBox;

    if (antennaOffset[boxNumber]->text() != "")
    {
        int offset = antennaOffset[boxNumber]->text().toInt();
        if (offset < minOffset || offset > maxOffset)
        {
            antennaOffset[boxNumber]->setText("0");
            QString msg = "Value must be between " + QString::number(minOffset) + " and " + QString::number(maxOffset);
            messageBox.critical(this, "Antenna Offset Error", msg);

        }
        else
        {

            if (offset  != availAntennas[boxNumber].antennaOffset)
            {
                availAntennas[boxNumber].antennaOffset = offset;
                antennaValueChanged[boxNumber] = true;
                antennaChanged = true;
            }

        }


    }

}


void SetupDialog::comportSelected(int boxNumber)
{

    if (comPorts[boxNumber]->currentText() != availAntennas[boxNumber].comport)
    {
        availAntennas[boxNumber].comport = comPorts[boxNumber]->currentText();
        antennaValueChanged[boxNumber] = true;
        antennaChanged = true;

    }
}



void SetupDialog::comSpeedSelected(int boxNumber)
{

    availAntennas[boxNumber].baudrate = comSpeed[boxNumber]->currentText().toInt();
    antennaValueChanged[boxNumber] = true;
    antennaChanged = true;
}

void SetupDialog::comDataBitsSelected(int boxNumber)
{

    availAntennas[boxNumber].databits = comDataBits[boxNumber]->currentText().toInt();
    antennaValueChanged[boxNumber] = true;
    antennaChanged = true;
}

void SetupDialog::comStopBitsSelected(int boxNumber)
{

    availAntennas[boxNumber].stopbits = comStopBits[boxNumber]->currentText().toInt();
    antennaValueChanged[boxNumber] = true;
    antennaChanged = true;
}

void SetupDialog::comParitySelected(int boxNumber)
{

    availAntennas[boxNumber].parity = rotator->getSerialParityCode(comParity[boxNumber]->currentIndex());
    antennaValueChanged[boxNumber] = true;
    antennaChanged = true;
}


void SetupDialog::comHandShakeSelected(int boxNumber)
{

    availAntennas[boxNumber].handshake = rotator->getSerialHandshakeCode(comHandShake[boxNumber]->currentIndex());
    antennaValueChanged[boxNumber] = true;
    antennaChanged = true;
}






void SetupDialog::fillRotatorModelInfo()
{

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        rotatorModel[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        rotator->getRotatorList(rotatorModel[i]);
     }


}


void SetupDialog::fillPortsInfo()
{
    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comPorts[i]->clear();
    }

    QString description;
    QString manufacturer;
    QString serialNumber;

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comPorts[i]->addItem("");
    }
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


        for (int i = 0; i < NUM_ANTENNAS; i++)
        {

            comPorts[i]->addItem(list.first(), list);
        }
    }


}




void SetupDialog::fillSpeedInfo()
{

    QStringList baudrateStr = rotator->getBaudRateNames();

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comSpeed[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comSpeed[i]->addItems(baudrateStr);
     }


}

void SetupDialog::fillDataBitsInfo()
{

    QStringList databitsStr = rotator->getDataBitsNames();

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comDataBits[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comDataBits[i]->addItems(databitsStr);
     }


}

void SetupDialog::fillStopBitsInfo()
{
    QStringList stopbitsStr = rotator->getStopBitsNames();

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comStopBits[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comStopBits[i]->addItems(stopbitsStr);
     }


}



void SetupDialog::fillParityInfo()
{

    QStringList parityStr = rotator->getParityCodeNames();

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comParity[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comParity[i]->addItems(parityStr);
     }


}



void SetupDialog::fillHandShakeInfo()
{

    QStringList handshakeStr = rotator->getHandShakeNames();

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comHandShake[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comHandShake[i]->addItems(handshakeStr);
     }


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
    bool antennaNameChg = false;
    // have the current antenna settings been changed?
    bool currentAntennaChanged = false;
    int ca = -1;
    bool ok;
    ca = currentAntenna.antennaNumber.toInt(&ok, 10);
    if (ok  && ca >= 0 && ca < NUM_ANTENNAS)
    {
        if (currentAntenna.antennaNumber != "")
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

        QSettings config("./Configuration/MinosRotatorAntenna.ini", QSettings::IniFormat);

        for (int i = 0; i < NUM_ANTENNAS; i++)
        {

            if (antennaValueChanged[i])
            {
                config.beginGroup("Antenna" + QString::number(i+1));
                config.setValue("antennaName", availAntennas[i].antennaName);
                if (antennaNameChanged[i])
                {
                    antennaNameChg = true;
                    antennaNameChanged[i] = false;
                }
                config.setValue("antennaNumber", i+1);
                config.setValue("rotatorModel", availAntennas[i].rotatorModel);
                config.setValue("rotatorModelName", availAntennas[i].rotatorModelName);
                config.setValue("rotatorModelNumber", availAntennas[i].rotatorModelNumber);
                config.setValue("rotatorManufacturer", availAntennas[i].rotatorManufacturer);
                config.setValue("southStop", availAntennas[i].southStopFlag);
                config.setValue("overRun", availAntennas[i].overRunFlag);
                config.setValue("antennaOffset", availAntennas[i].antennaOffset);
                config.setValue("comport", availAntennas[i].comport);
                config.setValue("baudrate", availAntennas[i].baudrate);
                config.setValue("databits", availAntennas[i].databits);
                config.setValue("parity", availAntennas[i].parity);
                config.setValue("stopbits", availAntennas[i].stopbits);
                config.setValue("handshake", availAntennas[i].handshake);
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

      emit currentAntennaSettingChanged(availAntennas[ca].antennaName);

   }
}



void SetupDialog::readSettings()
{

    QSettings config("./Configuration/MinosRotatorAntenna.ini", QSettings::IniFormat);

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        config.beginGroup("Antenna" + QString::number(i+1));
        availAntennas[i].antennaName = config.value("antennaName", "").toString();
        availAntennas[i].antennaNumber = config.value("antennaNumber", QString::number(i+1)).toString();
        availAntennas[i].rotatorModel = config.value("rotatorModel", "").toString();
        availAntennas[i].rotatorModelName = config.value("rotatorModelName", "").toString();
        availAntennas[i].rotatorModelNumber = config.value("rotatorModelNumber", "").toInt();
        availAntennas[i].rotatorManufacturer = config.value("rotatorManufacturer", "").toString();
        availAntennas[i].southStopFlag = config.value("southStop", true).toBool();
        availAntennas[i].overRunFlag = config.value("overRun", false).toBool();
        availAntennas[i].antennaOffset = config.value("antennaOffset", "").toInt();
        availAntennas[i].comport = config.value("comport", "").toString();
        availAntennas[i].baudrate = config.value("baudrate", 9600).toInt();
        availAntennas[i].databits = config.value("databits", 8).toInt();
        availAntennas[i].parity = config.value("parity", 0).toInt();
        availAntennas[i].stopbits = config.value("stopbits", 1).toInt();
        availAntennas[i].handshake = config.value("handshake", 0).toInt();
        config.endGroup();
    }

}



void SetupDialog::clearAvailRotators()
{


    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        availAntennas[i].antennaName = "";
        availAntennas[i].antennaNumber = "";
        availAntennas[i].rotatorModelName = "";
        availAntennas[i].rotatorModelNumber = 0;
        availAntennas[i].rotatorManufacturer = "";
        availAntennas[i].southStopFlag = false;
        availAntennas[i].overRunFlag = false;
        availAntennas[i].antennaOffset = 0;
        availAntennas[i].comport = "";
        availAntennas[i].baudrate = 9600;
        availAntennas[i].databits = 8;
        availAntennas[i].parity = rotator->getSerialParityCode(0);
        availAntennas[i].handshake = rotator->getSerialHandshakeCode(0);
    }


}


void SetupDialog::clearCurrentRotator()
{

    currentAntenna.antennaName = "";
    currentAntenna.antennaNumber = "";
    currentAntenna.rotatorModelName = "";
    currentAntenna.rotatorModelNumber = 0;
    currentAntenna.rotatorManufacturer = "";
    currentAntenna.southStopFlag = false;
    currentAntenna.overRunFlag = false;
    currentAntenna.antennaOffset = 0;
    currentAntenna.comport = "";
    currentAntenna.baudrate = 9600;
    currentAntenna.databits = 8;
    currentAntenna.parity = rotator->getSerialParityCode(0);
    currentAntenna.handshake = rotator->getSerialHandshakeCode(0);

}


void SetupDialog::copyAntennaToCurrent(int antennaNumber)
{
    currentAntenna.antennaName = availAntennas[antennaNumber].antennaName;
    currentAntenna.antennaNumber = availAntennas[antennaNumber].antennaNumber;
    currentAntenna.rotatorModel = availAntennas[antennaNumber].rotatorModel;
    currentAntenna.rotatorModelNumber = availAntennas[antennaNumber].rotatorModelNumber;
    currentAntenna.rotatorModelName = availAntennas[antennaNumber].rotatorModelName;
    currentAntenna.rotatorManufacturer = availAntennas[antennaNumber].rotatorManufacturer;
    currentAntenna.southStopFlag = availAntennas[antennaNumber].southStopFlag;
    currentAntenna.overRunFlag = availAntennas[antennaNumber].overRunFlag;
    currentAntenna.antennaOffset = availAntennas[antennaNumber].antennaOffset;
    currentAntenna.comport = availAntennas[antennaNumber].comport;
    currentAntenna.baudrate = availAntennas[antennaNumber].baudrate;
    currentAntenna.databits = availAntennas[antennaNumber].databits;
    currentAntenna.parity = availAntennas[antennaNumber].parity;
    currentAntenna.stopbits = availAntennas[antennaNumber].stopbits;
    currentAntenna.handshake = availAntennas[antennaNumber].handshake;
}

void SetupDialog::clearAntennaValueChanged()
{
    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        antennaValueChanged[i] = false;
    }
}


QString SetupDialog::getRotatorComPort(QString antennaName)
{

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        if (availAntennas[i].antennaName == antennaName)
        {
            return availAntennas[i].comport;
        }
    }

    return ""; // error, none found

}



void SetupDialog::saveCurrentAntenna()
{


    QSettings config("./Configuration/MinosRotatorAntenna.ini", QSettings::IniFormat);


    config.beginGroup("CurrentAntenna");
    config.setValue("antennaName", currentAntenna.antennaName);
    config.setValue("antennaNumber", currentAntenna.antennaNumber);
    config.setValue("rotatorModel", currentAntenna.rotatorModel);
    config.setValue("rotatorModelNumber", currentAntenna.rotatorModelNumber);
    config.setValue("rotatorModelName", currentAntenna.rotatorModelName);
    config.setValue("rotatorManufacturer", currentAntenna.rotatorManufacturer);
    config.setValue("southStop", currentAntenna.southStopFlag);
    config.setValue("overRun", currentAntenna.overRunFlag);
    config.setValue("antennaOffset", currentAntenna.antennaOffset);
    config.setValue("comport", currentAntenna.comport);
    config.setValue("baudrate", currentAntenna.baudrate);
    config.setValue("databits", currentAntenna.databits);
    config.setValue("parity", currentAntenna.parity);
    config.setValue("stopbits", currentAntenna.stopbits);
    config.setValue("handshake", currentAntenna.handshake);
    config.endGroup();




}

void SetupDialog::readCurrentAntenna()
{

    QSettings config("./Configuration/MinosRotatorAntenna.ini", QSettings::IniFormat);


    {
        config.beginGroup("CurrentAntenna");
        currentAntenna.antennaName = config.value("antennaName", "").toString();
        currentAntenna.antennaNumber = config.value("antennaNumber", "").toString();
        currentAntenna.rotatorModel = config.value("rotatorModel", "").toString();
        currentAntenna.rotatorModelNumber = config.value("rotatorModelNumber", "").toInt();
        currentAntenna.rotatorManufacturer = config.value("rotatorManufacturer", "").toString();
        currentAntenna.southStopFlag = config.value("southStop", false).toBool();
        currentAntenna.overRunFlag = config.value("overRun", false).toBool();
        currentAntenna.antennaOffset = config.value("antennaOffset", "").toInt();
        currentAntenna.comport = config.value("comport", "").toString();
        currentAntenna.baudrate = config.value("baudrate", 0).toInt();
        currentAntenna.databits = config.value("databits", 0).toInt();
        currentAntenna.parity = config.value("parity", 0).toInt();
        currentAntenna.stopbits = config.value("stopbits", 0).toInt();
        currentAntenna.handshake = config.value("handshake", 0).toInt();
        config.endGroup();
    }

}



srotParams SetupDialog::getCurrentAntenna() const
{
    return currentAntenna;
}








