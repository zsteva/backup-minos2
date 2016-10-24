#include "setupdialog.h"
#include "ui_setupdialog.h"
#include "rotcontrol.h"
#include <QSignalMapper>
#include <QComboBox>
#include <QLineEdit>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>

#include <QDebug>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");


SetupDialog::SetupDialog(RotControl *rotator, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);

    rotator = rotator;

    antennaName[NUM_ANTENNAS] = new QLineEdit;
    rotatorModel[NUM_ANTENNAS] = new  QComboBox;
    comPorts[NUM_ANTENNAS] = new  QComboBox;
    comSpeed[NUM_ANTENNAS] = new QComboBox;
    comDataBits[NUM_ANTENNAS] = new QComboBox;
    comStopBits[NUM_ANTENNAS] = new QComboBox;
    comParity[NUM_ANTENNAS] = new QComboBox;
    comHandShake[NUM_ANTENNAS] = new QComboBox;


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


    QSignalMapper *antennaName_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
       antennaName_mapper->setMapping(antennaName[i], i);
        connect(antennaName[i], SIGNAL(editingFinished()), antennaName_mapper, SLOT(map()));
    }
    connect(antennaName_mapper, SIGNAL(mapped(int)), this, SLOT(antennaNameFinished(int)));


    QSignalMapper *rotatorModel_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        rotatorModel_mapper->setMapping(rotatorModel[i], i);
        connect(rotatorModel[i], SIGNAL(activated(int)), rotatorModel_mapper, SLOT(map()));
    }
    connect(rotatorModel_mapper, SIGNAL(mapped(int)), this, SLOT(rotatorModelSelected(int)));


    QSignalMapper *comPorts_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comPorts_mapper->setMapping(comPorts[i], i);
        connect(comPorts[i], SIGNAL(activated(int)), comPorts_mapper, SLOT(map()));
    }

    connect(comPorts_mapper, SIGNAL(mapped(int)), this, SLOT(comportSelected(int)));


    QSignalMapper *comSpeed_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comSpeed_mapper->setMapping(comSpeed[i], i);
        connect(comSpeed[i], SIGNAL(activated(int)), comSpeed_mapper, SLOT(map()));
    }

    connect(comSpeed_mapper, SIGNAL(mapped(int)), this, SLOT(comSpeedSelected(int)));

    QSignalMapper *comDataBits_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comDataBits_mapper->setMapping(comDataBits[i], i);
        connect(comDataBits[i], SIGNAL(activated(int)), comDataBits_mapper, SLOT(map()));
    }

    connect(comDataBits_mapper, SIGNAL(mapped(int)), this, SLOT(comDataBitsSelected(int)));

    QSignalMapper *comStopBits_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comStopBits_mapper->setMapping(comStopBits[i], i);
        connect(comStopBits[i], SIGNAL(activated(int)), comStopBits_mapper, SLOT(map()));
    }

    connect(comStopBits_mapper, SIGNAL(mapped(int)), this, SLOT(comStopBitsSelected(int)));

    QSignalMapper *comParity_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comParity_mapper->setMapping(comParity[i], i);
        connect(comParity[i], SIGNAL(activated(int)), comParity_mapper, SLOT(map()));
    }

    connect(comParity_mapper, SIGNAL(mapped(int)), this, SLOT(comParitySelected(int)));

    QSignalMapper *comHandShake_mapper = new QSignalMapper(this);
    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comHandShake_mapper->setMapping(comHandShake[i], i);
        connect(comHandShake[i], SIGNAL(activated(int)), comHandShake_mapper, SLOT(map()));
    }

    connect(comHandShake_mapper, SIGNAL(mapped(int)), this, SLOT(comHandShakeSelected(int)));



    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));


//    initRotProtocols();  //build available rotator protocol table

    fillAntennaModelInfo();  // add rotator models to drop down
    fillPortsInfo();     // add comports to drop down
    fillSpeedInfo();
    fillDataBitsInfo();
    fillStopBitsInfo();
    fillParityInfo();
    fillHandShakeInfo();
    clearAvailRotators(); // clear the AvailRotator table, also init with default serial parameters
    clearCurrentRotator(); // clear the currently selected Rotator table, also init with default serial parameters

    readSettings();
    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        antennaName[i]->setText(availAntennas[i].antennaName);
        rotatorModel[i]->setCurrentIndex(availAntennas[i].rotatorModel_idx);
        comPorts[i]->setCurrentIndex(availAntennas[i].comport_idx);
        comSpeed[i]->setCurrentIndex(availAntennas[i].baudrate_idx);
        comDataBits[i]->setCurrentIndex(availAntennas[i].databits_idx);
        comStopBits[i]->setCurrentIndex(availAntennas[i].stopbits_idx);
        comParity[i]->setCurrentIndex(availAntennas[i].parity_idx);
        comHandShake[i]->setCurrentIndex(availAntennas[i].handshake_idx);

    }


}



SetupDialog::~SetupDialog()
{
    delete ui;
}


void SetupDialog::antennaNameFinished(int boxNumber)
{
    qDebug() << "finished name";
    if (antennaName[boxNumber]->text() != availAntennas[boxNumber].antennaName)
    {
        availAntennas[boxNumber].antennaName = antennaName[boxNumber]->text();
        antennaValueChanged[boxNumber] = true;
        antennaChanged = true;

    }
}


void SetupDialog::rotatorModelSelected(int boxNumber)
{
    if (rotatorModel[boxNumber]->currentText() != availAntennas[boxNumber].rotatorModel)
    {
        availAntennas[boxNumber].rotatorModel = rotatorModel[boxNumber]->currentText();
        availAntennas[boxNumber].rotatorModelNumber = rotator->getModelNumber(rotatorModel[boxNumber]->currentIndex());
        antennaValueChanged[boxNumber] = true;
        antennaChanged = true;

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

    availAntennas[boxNumber].parity = comParity[boxNumber]->currentText();
    antennaValueChanged[boxNumber] = true;
    antennaChanged = true;
}


void SetupDialog::comHandShakeSelected(int boxNumber)
{

    availAntennas[boxNumber].handshake = comHandShake[boxNumber]->currentText();
    antennaValueChanged[boxNumber] = true;
    antennaChanged = true;
}






void SetupDialog::fillAntennaModelInfo()
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

    QStringList list;
    list << "1200" << "2400" << "4800" << "9600" << "19200" << "38400";

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comSpeed[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comSpeed[i]->addItems(list);
     }


}

void SetupDialog::fillDataBitsInfo()
{

    QStringList list;
    list << "7" << "8" ;

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comDataBits[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comDataBits[i]->addItems(list);
     }


}

void SetupDialog::fillStopBitsInfo()
{

    QStringList list;
    list << "1" << "2" ;

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comStopBits[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comStopBits[i]->addItems(list);
     }


}



void SetupDialog::fillParityInfo()
{

    QStringList list;
    list << "None" << "Odd" << "Even";

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comParity[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comParity[i]->addItems(list);
     }


}



void SetupDialog::fillHandShakeInfo()
{

    QStringList list;
    list << "None" << "XON/XOFF" << "CTS/RTS";

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comHandShake[i]->clear();
    }

     for (int i = 0; i < NUM_ANTENNAS; i++)
     {
        comHandShake[i]->addItems(list);
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

    if (antennaChanged)
    {

        QSettings config("./Configuration/MinosRotatorAntenna.ini", QSettings::IniFormat);

        for (int i = 0; i < NUM_ANTENNAS; i++)
        {

            if (antennaValueChanged[i])
            {
                config.beginGroup("Antenna" + QString::number(i+1));
                config.setValue("antennaName", availAntennas[i].antennaName);
                config.setValue("rotatorModel", availAntennas[i].rotatorModel);
                config.setValue("rotatorModelNumber", availAntennas[i].rotatorModelNumber);
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
}


void SetupDialog::readSettings()
{

    QSettings config("./Configuration/MinosRotatorAntenna.ini", QSettings::IniFormat);

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        config.beginGroup("Antenna" + QString::number(i+1));
        availAntennas[i].antennaName = config.value("antennaName", "").toString();
        availAntennas[i].rotatorModel = config.value("rotatorModel", "").toString();
        availAntennas[i].rotatorModelNumber = config.value("rotatorModelNumber", "").toInt();
        availAntennas[i].comport = config.value("comport", "").toString();
        availAntennas[i].baudrate = config.value("baudrate", 0).toInt();
        availAntennas[i].databits = config.value("databits", 0).toInt();
        availAntennas[i].parity = config.value("parity", 0).toString();
        availAntennas[i].stopbits = config.value("stopbits", 0).toInt();
        availAntennas[i].handshake = config.value("handshake", 0).toInt();
        config.endGroup();
    }

}



void SetupDialog::clearAvailRotators()
{


    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
//        availAntennas[i].name = "";
//        availAntennas[i].rotator.protocol = "";
//        availAntennas[i].rotator.id = -1;
//        availAntennas[i].comPort = "";
//        availAntennas[i].baudRate = QSerialPort::Baud9600;
//        availAntennas[i].dataBits = QSerialPort::Data8;
//        availAntennas[i].parity = QSerialPort::NoParity;
//        availAntennas[i].stopBits = QSerialPort::OneStop;
//        availAntennas[i].flowControl = QSerialPort::NoFlowControl;
    }


}


void SetupDialog::clearCurrentRotator()
{
//    currentAntenna.name = "";
//    currentAntenna.rotator.protocol = "";
//    currentAntenna.rotator.id = 0;
//    currentAntenna.comPort = "";
//    currentAntenna.baudRate = QSerialPort::Baud9600;
//    currentAntenna.dataBits = QSerialPort::Data8;
//    currentAntenna.parity = QSerialPort::NoParity;
//    currentAntenna.stopBits = QSerialPort::OneStop;
//    currentAntenna.flowControl = QSerialPort::NoFlowControl;
}


//void SetupDialog::initRotProtocols()
//{

//    return

    //yaesu = new RotatorType;
    //yaesu->protocol = "Yaesu";
    //yaesu->id = 0;
    //rotProtocol[0] = yaesu;

    //prositel = new RotatorType;
    //prositel->protocol = "Prositel";
    //prositel->id = 1;
    //rotProtocol[1] = prositel;


//}

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
    config.setValue("rotatorModel", currentAntenna.rotatorModel);
    config.setValue("rotatorModelNumber", currentAntenna.rotatorModelNumber);
    config.setValue("comport", currentAntenna.comport);
    config.setValue("baudrate", currentAntenna.baudrate);
//    config.setValue("stringbaudrate", currentAntenna.BaudRate);
    config.setValue("databits", currentAntenna.databits);
//    config.setValue("stringdatabits", currentAntenna.stringDataBits);
    config.setValue("parity", currentAntenna.parity);
//    config.setValue("stringparity", currentAntenna.stringParity);
    config.setValue("stopbits", currentAntenna.stopbits);
//    config.setValue("stringstopbits" , currentAntenna.stringStopBits);
    config.setValue("handshake", currentAntenna.handshake);
//    config.setValue("stringflowcontrol", currentAntenna.stringFlowControl);
    config.endGroup();




}

void SetupDialog::readCurrentAntenna()
{

    QSettings config("./Configuration/MinosRotatorAntenna.ini", QSettings::IniFormat);


    {
        config.beginGroup("CurrentAntenna");
        currentAntenna.antennaName = config.value("antennaName", "").toString();
        currentAntenna.rotatorModel = config.value("rotatorModel", "").toString();
        currentAntenna.rotatorModelNumber = config.value("rotatorModelNumber", "").toInt();
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












