/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
//
/////////////////////////////////////////////////////////////////////////////





#include "setupdialog.h"
#include "ui_setupdialog.h"
#include "rigcontrol.h"
#include <QSignalMapper>
#include <QComboBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>

#include <QDebug>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");


SetupDialog::SetupDialog(RigControl *radio, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);

    radio = radio;



    radioName[0] = ui->radioNameEdit1;
    radioName[1] = ui->radioNameEdit2;
    radioName[2] = ui->radioNameEdit3;
    radioName[3] = ui->radioNameEdit4;
    radioName[4] = ui->radioNameEdit5;

    radioModel[0] = ui->radioModelBox1;
    radioModel[1] = ui->radioModelBox2;
    radioModel[2] = ui->radioModelBox3;
    radioModel[3] = ui->radioModelBox4;
    radioModel[4] = ui->radioModelBox5;

    civAddress[0] = ui->CIVlineEdit1;
    civAddress[1] = ui->CIVlineEdit2;
    civAddress[2] = ui->CIVlineEdit3;
    civAddress[3] = ui->CIVlineEdit4;
    civAddress[4] = ui->CIVlineEdit5;

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

    transVertCheck[0] = ui->TransVCheck1;
    transVertCheck[1] = ui->TransVCheck2;
    transVertCheck[2] = ui->TransVCheck3;
    transVertCheck[3] = ui->TransVCheck4;
    transVertCheck[4] = ui->TransVCheck5;

    transVertEdit[0] = ui->TransVertEdit1;
    transVertEdit[1] = ui->TransVertEdit2;
    transVertEdit[2] = ui->TransVertEdit3;
    transVertEdit[3] = ui->TransVertEdit4;
    transVertEdit[4] = ui->TransVertEdit5;

    transNegCheck[0] =ui->negCheckbox1;
    transNegCheck[1] =ui->negCheckbox2;
    transNegCheck[2] =ui->negCheckbox3;
    transNegCheck[3] =ui->negCheckbox4;
    transNegCheck[4] =ui->negCheckbox5;


/******************** Map radioName EditBoxes *****************************************/

    QSignalMapper *radioName_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_RADIOS; i++ )
    {
       radioName_mapper->setMapping(radioName[i], i);
        connect(radioName[i], SIGNAL(editingFinished()), radioName_mapper, SLOT(map()));
    }
    connect(radioName_mapper, SIGNAL(mapped(int)), this, SLOT(radioNameFinished(int)));

/******************** Map radioModel dropboxes *****************************************/

    QSignalMapper *radioModel_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        radioModel_mapper->setMapping(radioModel[i], i);
        connect(radioModel[i], SIGNAL(activated(int)), radioModel_mapper, SLOT(map()));
    }
    connect(radioModel_mapper, SIGNAL(mapped(int)), this, SLOT(radioModelSelected(int)));

/******************** Map CIV Address Editboxes *****************************************/

    QSignalMapper *civAddress_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_RADIOS; i++ )
    {
       civAddress_mapper->setMapping(civAddress[i], i);
        connect(civAddress[i], SIGNAL(editingFinished()), civAddress_mapper, SLOT(map()));
    }
    connect(civAddress_mapper, SIGNAL(mapped(int)), this, SLOT(civAddressFinished(int)));

/******************** Map comport dropboxes *****************************************/

    QSignalMapper *comPorts_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comPorts_mapper->setMapping(comPorts[i], i);
        connect(comPorts[i], SIGNAL(activated(int)), comPorts_mapper, SLOT(map()));
    }

    connect(comPorts_mapper, SIGNAL(mapped(int)), this, SLOT(comportSelected(int)));

/******************** Map comspeed dropboxes *****************************************/

    QSignalMapper *comSpeed_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comSpeed_mapper->setMapping(comSpeed[i], i);
        connect(comSpeed[i], SIGNAL(activated(int)), comSpeed_mapper, SLOT(map()));
    }

    connect(comSpeed_mapper, SIGNAL(mapped(int)), this, SLOT(comSpeedSelected(int)));

/******************** Map databits dropboxes *****************************************/

    QSignalMapper *comDataBits_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comDataBits_mapper->setMapping(comDataBits[i], i);
        connect(comDataBits[i], SIGNAL(activated(int)), comDataBits_mapper, SLOT(map()));
    }

    connect(comDataBits_mapper, SIGNAL(mapped(int)), this, SLOT(comDataBitsSelected(int)));

/******************** Map stopbits dropboxes *****************************************/

    QSignalMapper *comStopBits_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comStopBits_mapper->setMapping(comStopBits[i], i);
        connect(comStopBits[i], SIGNAL(activated(int)), comStopBits_mapper, SLOT(map()));
    }

    connect(comStopBits_mapper, SIGNAL(mapped(int)), this, SLOT(comStopBitsSelected(int)));

/******************** Map parity dropboxes *****************************************/

    QSignalMapper *comParity_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comParity_mapper->setMapping(comParity[i], i);
        connect(comParity[i], SIGNAL(activated(int)), comParity_mapper, SLOT(map()));
    }

    connect(comParity_mapper, SIGNAL(mapped(int)), this, SLOT(comParitySelected(int)));

/******************** Map handshake dropboxes *****************************************/

    QSignalMapper *comHandShake_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comHandShake_mapper->setMapping(comHandShake[i], i);
        connect(comHandShake[i], SIGNAL(activated(int)), comHandShake_mapper, SLOT(map()));
    }

    connect(comHandShake_mapper, SIGNAL(mapped(int)), this, SLOT(comHandShakeSelected(int)));

/******************** Map transvert checkboxes *****************************************/


    QSignalMapper *transVertCheck_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        transVertCheck_mapper->setMapping(transVertCheck[i], i);
        connect(transVertCheck[i], SIGNAL(stateChanged(int)), transVertCheck_mapper, SLOT(map()));
    }

    connect(transVertCheck_mapper, SIGNAL(mapped(int)), this, SLOT(transVertChecked(int)));



/******************** Map transvert editboxes *****************************************/


    QSignalMapper *transVertEdit_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        transVertEdit_mapper->setMapping(transVertEdit[i], i);
        connect(transVertEdit[i], SIGNAL(editingFinished()), transVertEdit_mapper, SLOT(map()));
    }

    connect(transVertEdit_mapper, SIGNAL(mapped(int)), this, SLOT(transVertEditFinished(int)));



    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));


/******************** Map trans negative offset checkbox *****************************************/


    QSignalMapper *transNegCheck_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        transNegCheck_mapper->setMapping(transNegCheck[i], i);
        connect(transNegCheck[i], SIGNAL(stateChanged(int)), transNegCheck_mapper, SLOT(map()));
    }

    connect(transNegCheck_mapper, SIGNAL(mapped(int)), this, SLOT(transNegChecked(int)));



    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));






    fillRadioModelInfo();  // add radio models to drop down

    fillPortsInfo();     // add comports to drop down
    fillSpeedInfo();
    fillDataBitsInfo();
    fillStopBitsInfo();
    fillParityInfo();
    fillHandShakeInfo();
    clearAvailRadio(); // clear the AvailRadio table, also init with default serial parameters
    clearCurrentRadio(); // clear the currently selected Radio table, also init with default serial parameters

    readSettings();   // get available radio settings from file

    for (int i = 0; i < NUM_RADIOS; i++)
    {
        radioName[i]->setText(availRadios[i].radioName);
        radioModel[i]->setCurrentIndex(radioModel[i]->findText(availRadios[i].radioModel));
        if (availRadios[i].radioMfg_Name == "Icom")
        {
            civAddress[i]->setEnabled(true);
        }
        else
        {
            civAddress[i]->setDisabled(true);
        }
        civAddress[i]->setText(availRadios[i].civAddress);
        comPorts[i]->setCurrentIndex(comPorts[i]->findText(availRadios[i].comport));
        comSpeed[i]->setCurrentIndex(comSpeed[i]->findText(QString::number(availRadios[i].baudrate)));
        comDataBits[i]->setCurrentIndex(comDataBits[i]->findText(QString::number(availRadios[i].databits)));
        comStopBits[i]->setCurrentIndex(comStopBits[i]->findText(QString::number(availRadios[i].stopbits)));
        comParity[i]->setCurrentIndex(availRadios[i].parity);
        comHandShake[i]->setCurrentIndex(availRadios[i].handshake);
        transVertCheck[i]->setChecked(availRadios[i].transVertEnable);
        transVertEdit[i]->setEnabled(transVertCheck[i]->isChecked());
        transNegCheck[i]->setEnabled(transVertCheck[i]->isChecked());
    }

    ui->setupTab->setCurrentIndex(0);       // set first tab
}



SetupDialog::~SetupDialog()
{
    delete ui;
}


void SetupDialog::radioNameFinished(int boxNumber)
{
    qDebug() << "finished name";
    if (radioName[boxNumber]->text() != availRadios[boxNumber].radioName)
    {
        availRadios[boxNumber].radioName = radioName[boxNumber]->text();
        radioValueChanged[boxNumber] = true;
        radioChanged = true;

    }
}


void SetupDialog::radioModelSelected(int boxNumber)
{
    if (radioModel[boxNumber]->currentText() != availRadios[boxNumber].radioModel)
    {
        availRadios[boxNumber].radioModel = radioModel[boxNumber]->currentText();
        availRadios[boxNumber].radioModelNumber = radio->getModelNumber(radioModel[boxNumber]->currentIndex());
        availRadios[boxNumber].radioModelName = radio->getModel_Name(radioModel[boxNumber]->currentIndex());
        availRadios[boxNumber].radioMfg_Name = radio->getMfg_Name(radioModel[boxNumber]->currentIndex());
        if (availRadios[boxNumber].radioMfg_Name == "Icom")
        {
            civAddress[boxNumber]->setEnabled(true);
        }
        else
        {
            civAddress[boxNumber]->setDisabled(true);
        }
        radioValueChanged[boxNumber] = true;
        radioChanged = true;

    }
}





void SetupDialog::civAddressFinished(int boxNumber)
{
    bool ok;
    int value = civAddress[boxNumber]->text().toInt(&ok, 16);
    if (!ok || value < 0 || value > 255)
    {
         QMessageBox::critical(this, "CIV Error", QString(civAddress[boxNumber]->text()) + " Not a valid CIV value");
         civAddress[boxNumber]->text() = "";
    }
    else if (civAddress[boxNumber]->text() != availRadios[boxNumber].civAddress)
    {
        availRadios[boxNumber].civAddress = civAddress[boxNumber]->text();
        radioValueChanged[boxNumber] = true;
        radioChanged = true;
    }
}



//void SetupDialog::enableCIVbox()
//{
//    for (int  i = 0; i < NUM_RADIOS; i++)
//    {
//        if (availRadios[i].radioMfg_Name == "Icom")
//        {
//            civAddress[i]->setEnabled(true);
//        }
//        else
//        {
//            civAddress[i]->setDisabled(true);
//        }

//    }
//}


void SetupDialog::comportSelected(int boxNumber)
{

    if (comPorts[boxNumber]->currentText() != availRadios[boxNumber].comport)
    {
        availRadios[boxNumber].comport = comPorts[boxNumber]->currentText();
        radioValueChanged[boxNumber] = true;
        radioChanged = true;

    }
}



void SetupDialog::comSpeedSelected(int boxNumber)
{

    availRadios[boxNumber].baudrate = comSpeed[boxNumber]->currentText().toInt();
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}

void SetupDialog::comDataBitsSelected(int boxNumber)
{

    availRadios[boxNumber].databits = comDataBits[boxNumber]->currentText().toInt();
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}

void SetupDialog::comStopBitsSelected(int boxNumber)
{

    availRadios[boxNumber].stopbits = comStopBits[boxNumber]->currentText().toInt();
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}

void SetupDialog::comParitySelected(int boxNumber)
{

    availRadios[boxNumber].parity = radio->getSerialParityCode(comParity[boxNumber]->currentIndex());
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}


void SetupDialog::comHandShakeSelected(int boxNumber)
{

    availRadios[boxNumber].handshake = radio->getSerialHandshakeCode(comHandShake[boxNumber]->currentIndex());
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}



void SetupDialog::transVertChecked(int boxNumber)
{

    if (transVertCheck[boxNumber]->isChecked())
    {
        availRadios[boxNumber].transVertEnable = true;
        transVertEdit[boxNumber]->setEnabled(true);
    }
    else
    {
        availRadios[boxNumber].transVertEnable = false;
        transVertEdit[boxNumber]->setEnabled(false);
    }
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}



void SetupDialog::transVertEditFinished(int boxNumber)
{
    bool ok;
    QString offsetStr = transVertEdit[boxNumber]->text();
    offsetStr.remove(QChar('.'));

    qDebug() << "offset = " << offsetStr;

    double value = offsetStr.toDouble(&ok);
    if (!ok )
    {
         QMessageBox::critical(this, "TransVert Error", offsetStr + " Not a valid TransVert Offset value");
         transVertEdit[boxNumber]->text() = "<font color='Red'>" + offsetStr + "</font>";

    }
    else if (value != availRadios[boxNumber].transVertOffset)
    {
        qDebug() << "foffset = " << value;
        transVertEdit[boxNumber]->text() = "<font color='Black'>" + offsetStr + "</font>";
        availRadios[boxNumber].transVertOffset = value;
        availRadios[boxNumber].transVertOffsetStr = offsetStr;
        radioValueChanged[boxNumber] = true;
        radioChanged = true;
    }
}



void SetupDialog::transNegChecked(int boxNumber)
{

    availRadios[boxNumber].transVertNegative = transNegCheck[boxNumber]->isChecked();
    radioValueChanged[boxNumber] = true;
    radioChanged = true;

}




void SetupDialog::fillRadioModelInfo()
{

    for (int i = 0; i < NUM_RADIOS; i++)
    {
        radioModel[i]->clear();
    }

     for (int i = 0; i < NUM_RADIOS; i++)
     {
        radio->getRigList(radioModel[i]);
     }


}


void SetupDialog::fillPortsInfo()
{
    for (int i = 0; i < NUM_RADIOS; i++)
    {
        comPorts[i]->clear();
    }

    QString description;
    QString manufacturer;
    QString serialNumber;

    for (int i = 0; i < NUM_RADIOS; i++)
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


        for (int i = 0; i < NUM_RADIOS; i++)
        {

            comPorts[i]->addItem(list.first(), list);
        }
    }


}




void SetupDialog::fillSpeedInfo()
{

    QStringList baudrateStr = radio->getBaudRateNames();

    for (int i = 0; i < NUM_RADIOS; i++)
    {
        comSpeed[i]->clear();
    }

     for (int i = 0; i < NUM_RADIOS; i++)
     {
        comSpeed[i]->addItems(baudrateStr);
     }


}

void SetupDialog::fillDataBitsInfo()
{

    QStringList databitsStr = radio->getDataBitsNames();

    for (int i = 0; i < NUM_RADIOS; i++)
    {
        comDataBits[i]->clear();
    }

     for (int i = 0; i < NUM_RADIOS; i++)
     {
        comDataBits[i]->addItems(databitsStr);
     }


}

void SetupDialog::fillStopBitsInfo()
{
    QStringList stopbitsStr = radio->getStopBitsNames();

    for (int i = 0; i < NUM_RADIOS; i++)
    {
        comStopBits[i]->clear();
    }

     for (int i = 0; i < NUM_RADIOS; i++)
     {
        comStopBits[i]->addItems(stopbitsStr);
     }


}



void SetupDialog::fillParityInfo()
{

    QStringList parityStr = radio->getParityCodeNames();

    for (int i = 0; i < NUM_RADIOS; i++)
    {
        comParity[i]->clear();
    }

     for (int i = 0; i < NUM_RADIOS; i++)
     {
        comParity[i]->addItems(parityStr);
     }


}



void SetupDialog::fillHandShakeInfo()
{

    QStringList handshakeStr = radio->getHandShakeNames();

    for (int i = 0; i < NUM_RADIOS; i++)
    {
        comHandShake[i]->clear();
    }

     for (int i = 0; i < NUM_RADIOS; i++)
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

    if (radioChanged)
    {

        QSettings config(RIG_CONTROL_CONFIG, QSettings::IniFormat);

        for (int i = 0; i < NUM_RADIOS; i++)
        {

            if (radioValueChanged[i])
            {
                config.beginGroup("Radio" + QString::number(i+1));
                config.setValue("radioName", availRadios[i].radioName);
                config.setValue("radioMfgName", availRadios[i].radioMfg_Name);
                config.setValue("radioModel", availRadios[i].radioModel);
                config.setValue("radioModelName", availRadios[i].radioModelName);
                config.setValue("radioModelNumber", availRadios[i].radioModelNumber);
                config.setValue("civAddress", availRadios[i].civAddress);
                config.setValue("comport", availRadios[i].comport);
                config.setValue("baudrate", availRadios[i].baudrate);
                config.setValue("databits", availRadios[i].databits);
                config.setValue("parity", availRadios[i].parity);
                config.setValue("stopbits", availRadios[i].stopbits);
                config.setValue("handshake", availRadios[i].handshake);
                config.setValue("transVertOffSet", availRadios[i].transVertOffset);
                config.setValue("transVertOffSetStr", availRadios[i].transVertOffsetStr);
                config.setValue("transVertEnable", availRadios[i].transVertEnable);
                config.setValue("transVertNegative", availRadios[i].transVertNegative);
                config.endGroup();
                radioValueChanged[i] = false;

            }

        }

   }
   radioChanged = false;
}


void SetupDialog::readSettings()
{

    QSettings config(RIG_CONTROL_CONFIG, QSettings::IniFormat);

    for (int i = 0; i < NUM_RADIOS; i++)
    {
        config.beginGroup("Radio" + QString::number(i+1));
        availRadios[i].radioName = config.value("radioName", "").toString();
        availRadios[i].radioMfg_Name = config.value("radioMfgName", "").toString();
        availRadios[i].radioModel = config.value("radioModel", "").toString();
        availRadios[i].radioModelName = config.value("radioModelName", "").toString();
        availRadios[i].radioModelNumber = config.value("radioModelNumber", "").toInt();
        availRadios[i].civAddress = config.value("civAddress", "").toString();
        availRadios[i].comport = config.value("comport", "").toString();
        availRadios[i].baudrate = config.value("baudrate", 9600).toInt();
        availRadios[i].databits = config.value("databits", 8).toInt();
        availRadios[i].parity = config.value("parity", 0).toInt();
        availRadios[i].stopbits = config.value("stopbits", 1).toInt();
        availRadios[i].handshake = config.value("handshake", 0).toInt();
        availRadios[i].transVertOffset = config.value("transVertOffSet", 0).toDouble();
        availRadios[i].transVertOffsetStr = config.value("transVertOffSetStr", "00.000.000.000").toString();
        availRadios[i].transVertEnable = config.value("transVertEnable", false).toBool();
        availRadios[i].transVertNegative = config.value("transVertNegative", false).toBool();
        config.endGroup();
    }

}



void SetupDialog::clearAvailRadio()
{


    for (int i = 0; i < NUM_RADIOS; i++)
    {
        availRadios[i].radioName = "";
        availRadios[i].radioMfg_Name = "";
        availRadios[i].radioModelName = "";
        availRadios[i].radioModelNumber = 0;
        availRadios[i].civAddress = "0x00";
        availRadios[i].comport = "";
        availRadios[i].baudrate = 9600;
        availRadios[i].databits = 8;
        availRadios[i].parity = radio->getSerialParityCode(0);
        availRadios[i].handshake = radio->getSerialHandshakeCode(0);
        availRadios[i].transVertOffset = 0;
        availRadios[i].transVertOffsetStr = "00.000.000.000";
        availRadios[i].transVertEnable = false;
        availRadios[i].transVertNegative = false;
    }


}


void SetupDialog::clearCurrentRadio()
{

    currentRadio.radioName = "";
    currentRadio.radioMfg_Name = "";
    currentRadio.radioModelName = "";
    currentRadio.radioModelNumber = 0;
    currentRadio.civAddress = "0x00";
    currentRadio.comport = "";
    currentRadio.baudrate = 9600;
    currentRadio.databits = 8;
    currentRadio.parity = radio->getSerialParityCode(0);
    currentRadio.handshake = radio->getSerialHandshakeCode(0);
    currentRadio.transVertOffset = 0;
    currentRadio.transVertOffsetStr = "00.000.000.000";
    currentRadio.transVertEnable = false;
    currentRadio.transVertNegative = false;

}




/*
int SetupDialog::getRotatorId(QString rotator)
{

    for (int i = 0; i < NUM_ROTATOR_PROTOCOLS; i++)
    {
        if (rotProtocol[i]->protocol == rotator)
        {
            return rotProtocol[i]->id;
        }
    }

    return -1; // error, none found

}


QString SetupDialog::getRotatorProtocol(QString antennaName)
{

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        if (availAntennas[i].name == antennaName)
        {
            return availAntennas[i].rotator.protocol;
        }
    }

    return ""; // error, none found

}

*/

QString SetupDialog::getRadioComPort(QString radioName)
{

    for (int i = 0; i < NUM_RADIOS; i++)
    {
        if (availRadios[i].radioName == radioName)
        {
            return availRadios[i].comport;
        }
    }

    return ""; // error, none found

}



void SetupDialog::saveCurrentRadio()
{


    QSettings config(RIG_CONTROL_CONFIG, QSettings::IniFormat);


    config.beginGroup("CurrentRadio");
    config.setValue("radioName", currentRadio.radioName);
    config.setValue("radioMfgName", currentRadio.radioMfg_Name);
    config.setValue("radioModel", currentRadio.radioModel);
    config.setValue("radioModelNumber", currentRadio.radioModelNumber);
    config.setValue("civAddress", currentRadio.civAddress);
    config.setValue("comport", currentRadio.comport);
    config.setValue("baudrate", currentRadio.baudrate);
    config.setValue("databits", currentRadio.databits);
    config.setValue("parity", currentRadio.parity);
    config.setValue("stopbits", currentRadio.stopbits);
    config.setValue("handshake", currentRadio.handshake);
    config.setValue("transVertOffSet", currentRadio.transVertOffset);
    config.setValue("transVertOffSetStr", currentRadio.transVertOffsetStr);
    config.setValue("transVertEnable", currentRadio.transVertEnable);
    config.setValue("transVertNegative", currentRadio.transVertNegative);
    config.endGroup();




}

void SetupDialog::readCurrentRadio()
{

    QSettings config(RIG_CONTROL_CONFIG, QSettings::IniFormat);


    {
        config.beginGroup("CurrentRadio");
        currentRadio.radioName = config.value("radioName", "").toString();
        currentRadio.radioMfg_Name = config.value("radioMfgName", "").toString();
        currentRadio.radioModel = config.value("radioModel", "").toString();
        currentRadio.radioModelNumber = config.value("radioModelNumber", "").toInt();
        currentRadio.civAddress = config.value("civAddress", "").toString();
        currentRadio.comport = config.value("comport", "").toString();
        currentRadio.baudrate = config.value("baudrate", 0).toInt();
        currentRadio.databits = config.value("databits", 0).toInt();
        currentRadio.parity = config.value("parity", 0).toInt();
        currentRadio.stopbits = config.value("stopbits", 0).toInt();
        currentRadio.handshake = config.value("handshake", 0).toInt();
        currentRadio.transVertOffset = config.value("transVertOffSet", 0).toDouble();
        currentRadio.transVertOffsetStr = config.value("transVertOffSetStr", "00.000.000.000").toString();
        currentRadio.transVertEnable = config.value("transVertEnable", false).toBool();
        currentRadio.transVertNegative = config.value("transVertNegative", false).toBool();
        config.endGroup();
    }

}



scatParams SetupDialog::getCurrentRadio() const
{
    return currentRadio;
}
