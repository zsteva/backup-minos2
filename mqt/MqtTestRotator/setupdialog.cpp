#include "setupdialog.h"
#include "ui_setupdialog.h"

#include <QSignalMapper>
#include <QComboBox>
#include <QLineEdit>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>

#include <QDebug>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");


SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);

    comPorts[NUM_ANTENNAS] = new  QComboBox;
    protocols[NUM_ANTENNAS] = new  QComboBox;
    antennaName[NUM_ANTENNAS] = new QLineEdit;


    comPorts[0] = ui->comPortBox1;
    comPorts[1] = ui->comPortBox2;
    comPorts[2] = ui->comPortBox3;
    comPorts[3] = ui->comPortBox4;
    comPorts[4] = ui->comPortBox5;

    protocols[0] = ui->protcolBox1;
    protocols[1] = ui->protcolBox2;
    protocols[2] = ui->protcolBox3;
    protocols[3] = ui->protcolBox4;
    protocols[4] = ui->protcolBox5;

    antennaName[0] = ui->setupLineEdit1;
    antennaName[1] = ui->setupLineEdit2;
    antennaName[2] = ui->setupLineEdit3;
    antennaName[3] = ui->setupLineEdit4;
    antennaName[4] = ui->setupLineEdit5;

    QSignalMapper *comPorts_mapper = new QSignalMapper(this);


    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        comPorts_mapper->setMapping(comPorts[i], i);
        connect(comPorts[i], SIGNAL(activated(int)), comPorts_mapper, SLOT(map()));
    }

    connect(comPorts_mapper, SIGNAL(mapped(int)), this, SLOT(comportSelected(int)));

    QSignalMapper *protocols_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
        protocols_mapper->setMapping(protocols[i], i);
        connect(protocols[i], SIGNAL(activated(int)), protocols_mapper, SLOT(map()));
    }
    connect(protocols_mapper, SIGNAL(mapped(int)), this, SLOT(protocolSelected(int)));

    QSignalMapper *antennaName_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_ANTENNAS; i++ )
    {
       antennaName_mapper->setMapping(antennaName[i], i);
        connect(antennaName[i], SIGNAL(editingFinished()), antennaName_mapper, SLOT(map()));
    }
    connect(antennaName_mapper, SIGNAL(mapped(int)), this, SLOT(rotatorNameFinished(int)));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));


    initRotProtocols();  //build available rotator protocol table
    fillPortsInfo();     // add comports to drop down
    fillProtocolInfo();  // add rotator protocols to drop down
    clearAvailRotators(); // clear the AvailRotator table, also init with default serial parameters
    clearCurrentRotator(); // clear the currently selected Rotator table, also init with default serial parameters

    readSettings();
    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        comPorts[i]->setCurrentIndex(availAntennas[i].comPort_idx);
        protocols[i]->setCurrentIndex(availAntennas[i].rotator_idx);
        antennaName[i]->setText(availAntennas[i].name);

    }


}



SetupDialog::~SetupDialog()
{
    delete ui;
}

void SetupDialog::comportSelected(int boxNumber)
{

    if (comPorts[boxNumber]->currentText() != availAntennas[boxNumber].comPort)
    {
        availAntennas[boxNumber].comPort = comPorts[boxNumber]->currentText();
        antennaValueChanged[boxNumber] = true;
        antennaChanged = true;

    }
}


void SetupDialog::protocolSelected(int boxNumber)
{

    if (protocols[boxNumber]->currentText() != availAntennas[boxNumber].rotator.protocol)
    {
        availAntennas[boxNumber].rotator.protocol = protocols[boxNumber]->currentText();
        availAntennas[boxNumber].rotator.id = getRotatorId(protocols[boxNumber]->currentText());
        antennaValueChanged[boxNumber] = true;
        antennaChanged = true;

    }
}


void SetupDialog::rotatorNameFinished(int boxNumber)
{
    qDebug() << "finished name";
    if (antennaName[boxNumber]->text() != availAntennas[boxNumber].name)
    {
        availAntennas[boxNumber].name = antennaName[boxNumber]->text();
        antennaValueChanged[boxNumber] = true;
        antennaChanged = true;

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


void SetupDialog::fillProtocolInfo()
{

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        protocols[i]->clear();
    }

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
       protocols[i]->addItem("");

        for (int j =0; j < NUM_ROTATOR_PROTOCOLS; j++)
        {
            protocols[i]->addItem(rotProtocol[j]->protocol);
        }
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
                config.setValue("name", availAntennas[i].name);
                config.setValue("protocol", availAntennas[i].rotator.protocol);
                config.setValue("id", availAntennas[i].rotator.id);
                config.setValue("comport", availAntennas[i].comPort);
                config.setValue("baudrate", availAntennas[i].baudRate);
                config.setValue("stringbaudrate", availAntennas[i].stringBaudRate);
                config.setValue("databits", availAntennas[i].dataBits);
                config.setValue("stringdatabits", availAntennas[i].stringDataBits);
                config.setValue("parity", availAntennas[i].parity);
                config.setValue("stringparity", availAntennas[i].stringParity);
                config.setValue("stopbits", availAntennas[i].stopBits);
                config.setValue("stringstopbits" , availAntennas[i].stringStopBits);
                config.setValue("flowcontrol", availAntennas[i].flowControl);
                config.setValue("stringflowcontrol", availAntennas[i].stringFlowControl);
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
        availAntennas[i].name = config.value("name", "").toString();
        availAntennas[i].rotator.protocol = config.value("protocol", "").toString();
        availAntennas[i].rotator_idx = protocols[i]->findText(availAntennas[i].rotator.protocol);
        availAntennas[i].rotator.id = config.value("id", "").toInt();
        availAntennas[i].comPort = config.value("comport", "").toString();
        availAntennas[i].comPort_idx = comPorts[i]->findText(availAntennas[i].comPort);
        //availAntennas[i].baudRate = config.value("baudrate", 0).toInt();
        //availAntennas[i].dataBits = config.value("databits", 0).toInt();
        //availAntennas[i].parity = config.value("parity", 0).toInt();
        //availAntennas[i].stopBits = config.value("stopbits", 0).toInt();
        //availAntennas[i].flowControl = config.value("flowcontrol", 0).toInt();
        config.endGroup();
    }

}



void SetupDialog::clearAvailRotators()
{


    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        availAntennas[i].name = "";
        availAntennas[i].rotator.protocol = "";
        availAntennas[i].rotator.id = -1;
        availAntennas[i].comPort = "";
        availAntennas[i].baudRate = QSerialPort::Baud9600;
        availAntennas[i].dataBits = QSerialPort::Data8;
        availAntennas[i].parity = QSerialPort::NoParity;
        availAntennas[i].stopBits = QSerialPort::OneStop;
        availAntennas[i].flowControl = QSerialPort::NoFlowControl;
    }


}


void SetupDialog::clearCurrentRotator()
{
    currentAntenna.name = "";
    currentAntenna.rotator.protocol = "";
    currentAntenna.rotator.id = 0;
    currentAntenna.comPort = "";
    currentAntenna.baudRate = QSerialPort::Baud9600;
    currentAntenna.dataBits = QSerialPort::Data8;
    currentAntenna.parity = QSerialPort::NoParity;
    currentAntenna.stopBits = QSerialPort::OneStop;
    currentAntenna.flowControl = QSerialPort::NoFlowControl;
}


void SetupDialog::initRotProtocols()
{

    yaesu = new RotatorType;
    yaesu->protocol = "Yaesu";
    yaesu->id = 0;
    rotProtocol[0] = yaesu;

    prositel = new RotatorType;
    prositel->protocol = "Prositel";
    prositel->id = 1;
    rotProtocol[1] = prositel;


}

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

QString SetupDialog::getRotatorComPort(QString antennaName)
{

    for (int i = 0; i < NUM_ANTENNAS; i++)
    {
        if (availAntennas[i].name == antennaName)
        {
            return availAntennas[i].comPort;
        }
    }

    return ""; // error, none found

}



void SetupDialog::saveCurrentAntenna()
{


    QSettings config("./Configuration/MinosRotatorAntenna.ini", QSettings::IniFormat);


    config.beginGroup("CurrentAntenna");
    config.setValue("name", currentAntenna.name);
    config.setValue("protocol", currentAntenna.rotator.protocol);
    config.setValue("id", currentAntenna.rotator.id);
    config.setValue("comport", currentAntenna.comPort);
    config.setValue("baudrate", currentAntenna.baudRate);
    config.setValue("stringbaudrate", currentAntenna.stringBaudRate);
    config.setValue("databits", currentAntenna.dataBits);
    config.setValue("stringdatabits", currentAntenna.stringDataBits);
    config.setValue("parity", currentAntenna.parity);
    config.setValue("stringparity", currentAntenna.stringParity);
    config.setValue("stopbits", currentAntenna.stopBits);
    config.setValue("stringstopbits" , currentAntenna.stringStopBits);
    config.setValue("flowcontrol", currentAntenna.flowControl);
    config.setValue("stringflowcontrol", currentAntenna.stringFlowControl);
    config.endGroup();




}

void SetupDialog::readCurrentAntenna()
{

    QSettings config("./Configuration/MinosRotatorAntenna.ini", QSettings::IniFormat);


    {
        config.beginGroup("CurrentAntenna");
        currentAntenna.name = config.value("name", "").toString();
        currentAntenna.rotator.protocol = config.value("protocol", "").toString();
        currentAntenna.rotator.id = config.value("id", "").toInt();
        currentAntenna.comPort = config.value("comport", "").toString();
        //currentAntenna.baudRate = config.value("baudrate", 0).toInt();
        //currentAntenna.dataBits = config.value("databits", 0).toInt();
        //currentAntenna.parity = config.value("parity", 0).toInt();
        //currentAntenna.stopBits = config.value("stopbits", 0).toInt();
        //currentAntenna.flowControl = config.value("flowcontrol", 0).toInt();
        config.endGroup();
    }

}



SetupDialog::Rotators SetupDialog::getCurrentAntenna() const
{
    return currentAntenna;
}












