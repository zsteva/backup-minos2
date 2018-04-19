#include "bmsetupdialog.h"
#include "ui_bmsetupdialog.h"
#include "rigcontrol.h"
#include <QSignalMapper>
#include <QComboBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>

#include <QDebug>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");


BMSetupDialog::BMSetupDialog(RigControl *radio, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BMSetupDialog)
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


    QSignalMapper *radioName_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_RADIOS; i++ )
    {
       radioName_mapper->setMapping(radioName[i], i);
        connect(radioName[i], SIGNAL(editingFinished()), radioName_mapper, SLOT(map()));
    }
    connect(radioName_mapper, SIGNAL(mapped(int)), this, SLOT(radioNameFinished(int)));


    QSignalMapper *radioModel_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        radioModel_mapper->setMapping(radioModel[i], i);
        connect(radioModel[i], SIGNAL(activated(int)), radioModel_mapper, SLOT(map()));
    }
    connect(radioModel_mapper, SIGNAL(mapped(int)), this, SLOT(radioModelSelected(int)));


    QSignalMapper *civAddress_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_RADIOS; i++ )
    {
       civAddress_mapper->setMapping(civAddress[i], i);
        connect(civAddress[i], SIGNAL(editingFinished()), civAddress_mapper, SLOT(map()));
    }
    connect(civAddress_mapper, SIGNAL(mapped(int)), this, SLOT(civAddressFinished(int)));



    QSignalMapper *comPorts_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comPorts_mapper->setMapping(comPorts[i], i);
        connect(comPorts[i], SIGNAL(activated(int)), comPorts_mapper, SLOT(map()));
    }

    connect(comPorts_mapper, SIGNAL(mapped(int)), this, SLOT(comportSelected(int)));


    QSignalMapper *comSpeed_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comSpeed_mapper->setMapping(comSpeed[i], i);
        connect(comSpeed[i], SIGNAL(activated(int)), comSpeed_mapper, SLOT(map()));
    }

    connect(comSpeed_mapper, SIGNAL(mapped(int)), this, SLOT(comSpeedSelected(int)));

    QSignalMapper *comDataBits_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comDataBits_mapper->setMapping(comDataBits[i], i);
        connect(comDataBits[i], SIGNAL(activated(int)), comDataBits_mapper, SLOT(map()));
    }

    connect(comDataBits_mapper, SIGNAL(mapped(int)), this, SLOT(comDataBitsSelected(int)));

    QSignalMapper *comStopBits_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comStopBits_mapper->setMapping(comStopBits[i], i);
        connect(comStopBits[i], SIGNAL(activated(int)), comStopBits_mapper, SLOT(map()));
    }

    connect(comStopBits_mapper, SIGNAL(mapped(int)), this, SLOT(comStopBitsSelected(int)));

    QSignalMapper *comParity_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comParity_mapper->setMapping(comParity[i], i);
        connect(comParity[i], SIGNAL(activated(int)), comParity_mapper, SLOT(map()));
    }

    connect(comParity_mapper, SIGNAL(mapped(int)), this, SLOT(comParitySelected(int)));

    QSignalMapper *comHandShake_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_RADIOS; i++ )
    {
        comHandShake_mapper->setMapping(comHandShake[i], i);
        connect(comHandShake[i], SIGNAL(activated(int)), comHandShake_mapper, SLOT(map()));
    }

    connect(comHandShake_mapper, SIGNAL(mapped(int)), this, SLOT(comHandShakeSelected(int)));



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
        civAddress[i]->setText(availRadios[i].civAddress);
        comPorts[i]->setCurrentIndex(comPorts[i]->findText(availRadios[i].comport));
        comSpeed[i]->setCurrentIndex(comSpeed[i]->findText(QString::number(availRadios[i].baudrate)));
        comDataBits[i]->setCurrentIndex(comDataBits[i]->findText(QString::number(availRadios[i].databits)));
        comStopBits[i]->setCurrentIndex(comStopBits[i]->findText(QString::number(availRadios[i].stopbits)));
        comParity[i]->setCurrentIndex(availRadios[i].parity);
        comHandShake[i]->setCurrentIndex(availRadios[i].handshake);

    }
    enableCIVbox();

}



BMSetupDialog::~BMSetupDialog()
{
    delete ui;
}


void BMSetupDialog::radioNameFinished(int boxNumber)
{
    qDebug() << "finished name";
    if (radioName[boxNumber]->text() != availRadios[boxNumber].radioName)
    {
        availRadios[boxNumber].radioName = radioName[boxNumber]->text();
        radioValueChanged[boxNumber] = true;
        radioChanged = true;

    }
}


void BMSetupDialog::radioModelSelected(int boxNumber)
{
    if (radioModel[boxNumber]->currentText() != availRadios[boxNumber].radioModel)
    {
        availRadios[boxNumber].radioModel = radioModel[boxNumber]->currentText();
        availRadios[boxNumber].radioModelNumber = radio->getModelNumber(radioModel[boxNumber]->currentIndex());
        availRadios[boxNumber].radioModelName = radio->getModel_Name(radioModel[boxNumber]->currentIndex());
        availRadios[boxNumber].radioMfg_Name = radio->getMfg_Name(radioModel[boxNumber]->currentIndex());
        radioValueChanged[boxNumber] = true;
        radioChanged = true;

    }
}





void BMSetupDialog::civAddressFinished(int boxNumber)
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



void BMSetupDialog::enableCIVbox()
{
    for (int  i = 0; i < NUM_RADIOS; i++)
    {
        if (availRadios[i].radioMfg_Name == "Icom")
        {
            civAddress[i]->setEnabled(true);
        }
        else
        {
            civAddress[i]->setDisabled(true);
        }

    }
}


void BMSetupDialog::comportSelected(int boxNumber)
{

    if (comPorts[boxNumber]->currentText() != availRadios[boxNumber].comport)
    {
        availRadios[boxNumber].comport = comPorts[boxNumber]->currentText();
        radioValueChanged[boxNumber] = true;
        radioChanged = true;

    }
}



void BMSetupDialog::comSpeedSelected(int boxNumber)
{

    availRadios[boxNumber].baudrate = comSpeed[boxNumber]->currentText().toInt();
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}

void BMSetupDialog::comDataBitsSelected(int boxNumber)
{

    availRadios[boxNumber].databits = comDataBits[boxNumber]->currentText().toInt();
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}

void BMSetupDialog::comStopBitsSelected(int boxNumber)
{

    availRadios[boxNumber].stopbits = comStopBits[boxNumber]->currentText().toInt();
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}

void BMSetupDialog::comParitySelected(int boxNumber)
{

    availRadios[boxNumber].parity = radio->getSerialParityCode(comParity[boxNumber]->currentIndex());
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}


void BMSetupDialog::comHandShakeSelected(int boxNumber)
{

    availRadios[boxNumber].handshake = radio->getSerialHandshakeCode(comHandShake[boxNumber]->currentIndex());
    radioValueChanged[boxNumber] = true;
    radioChanged = true;
}






void BMSetupDialog::fillRadioModelInfo()
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


void BMSetupDialog::fillPortsInfo()
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




void BMSetupDialog::fillSpeedInfo()
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

void BMSetupDialog::fillDataBitsInfo()
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

void BMSetupDialog::fillStopBitsInfo()
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



void BMSetupDialog::fillParityInfo()
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



void BMSetupDialog::fillHandShakeInfo()
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





void BMSetupDialog::saveButtonPushed()
{

    saveSettings();

}



void BMSetupDialog::cancelButtonPushed()
{




}



void BMSetupDialog::saveSettings()
{

    if (radioChanged)
    {

        QSettings config("./Configuration/MinosBandmap.ini", QSettings::IniFormat);

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
                config.endGroup();
                radioValueChanged[i] = false;

            }

        }

   }
   radioChanged = false;
}


void BMSetupDialog::readSettings()
{

    QSettings config("./Configuration/MinosBandmap.ini", QSettings::IniFormat);

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
        config.endGroup();
    }

}



void BMSetupDialog::clearAvailRadio()
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
    }


}


void BMSetupDialog::clearCurrentRadio()
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

}




/*
int BMSetupDialog::getRotatorId(QString rotator)
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


QString BMSetupDialog::getRotatorProtocol(QString antennaName)
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

QString BMSetupDialog::getRadioComPort(QString radioName)
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



void BMSetupDialog::saveCurrentRadio()
{


    QSettings config("./Configuration/MinosBandmap.ini", QSettings::IniFormat);


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
    config.endGroup();




}

void BMSetupDialog::readCurrentRadio()
{

    QSettings config("./Configuration/MinosBandmap.ini", QSettings::IniFormat);


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
        config.endGroup();
    }

}



scatParams BMSetupDialog::getCurrentRadio() const
{
    return currentRadio;
}












