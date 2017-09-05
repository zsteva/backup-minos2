/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2017
//
//
//
/////////////////////////////////////////////////////////////////////////////





#include "rigmemdialog.h"
#include "ui_rigmemdialog.h"
#include "rigmemcommondata.h"
#include "rigcontrolcommonconstants.h"


RigMemDialog::RigMemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RigMemDialog)


{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    radioName = "";
    radioState = "";
    setMemoryFlag(false);

    for (int i = 0; i < hamlibData::supModeList.count(); i++)
    {
        ui->modecb->addItem(hamlibData::supModeList[i]);
    }

    for (int i = 0; i < hamlibData::pBandStateStr.count(); i++)
    {
        ui->pbStateCb->addItem(hamlibData::pBandStateStr[i]);
    }

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));
    connect(ui->callSignLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(callSignToUpper(const QString &)));
    connect(ui->locatorLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(locatorToUpper(const QString &)));

}

RigMemDialog::~RigMemDialog()
{
    delete ui;
}

void RigMemDialog::setMemoryFlag(bool state)
{
    memOnFlag = state;
}

bool RigMemDialog::getMemoryFlag()
{
    return memOnFlag;
}

void RigMemDialog::setRadioName(QString name)
{
    radioName = name;
}

void RigMemDialog::setRadioState(QString state)
{
    radioState = state;
}

int RigMemDialog::readAllMemories()
{

    if (radioName == "")
    {
        return -1;
    }

    QString fileName = "./RadioMemoryData/" + radioName + "MemoryData.ini";

    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++)
    {
        config.beginGroup("MemoryLoc" + QString::number(i));
        memoryList[i].callsign= config.value("Callsign", "").toString();
        memoryList[i].freq = config.value("Frequency", "00.000.000.000").toString();
        memoryList[i].mode = config.value("Mode", "").toString();
        memoryList[i].passBand = config.value("PassBand", "2200").toString();
        memoryList[i].locator = config.value("Locator", "").toString();
        memoryList[i].bearing = config.value("Bearing", 0).toInt();
        memoryList[i].time = config.value("Time", "").toString();
        config.endGroup();
    }

    return 0;

}


int RigMemDialog::readMemory(int memoryLoc)
{

    if (radioName == "")
    {
        return -1;
    }

    QString fileName = "./RadioMemoryData/" + radioName + "MemoryData.ini";

    QSettings config(fileName, QSettings::IniFormat);

    config.beginGroup("MemoryLoc" + QString::number(memoryLoc));
    memoryList[memoryLoc].callsign = config.value("Callsign", "").toString();
    memoryList[memoryLoc].freq = config.value("Frequency", "00.000.000.000").toString();
    memoryList[memoryLoc].mode = config.value("Mode", "").toString();
    memoryList[memoryLoc].passBand = config.value("PassBand", "2200").toString();
    memoryList[memoryLoc].locator = config.value("Locator", "").toString();
    memoryList[memoryLoc].bearing = config.value("Bearing", 0).toInt();
    memoryList[memoryLoc].time = config.value("Time", "").toString();
    config.endGroup();


    return 0;

}


int RigMemDialog::saveMemory(int memoryLoc)
{
    if (radioName == "")
    {
        return -1;
    }

    QString fileName = "./RadioMemoryData/" + radioName + "MemoryData.ini";
    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup("MemoryLoc" + QString::number(memoryLoc));
    config.setValue("Callsign", memoryList[memoryLoc].callsign);
    config.setValue("Frequency", memoryList[memoryLoc].freq);
    config.setValue("Mode", memoryList[memoryLoc].mode);
    config.setValue("PassBand", memoryList[memoryLoc].passBand);
    config.setValue("Locator", memoryList[memoryLoc].locator);
    config.setValue("Bearing", memoryList[memoryLoc].bearing);
    config.setValue("Time", memoryList[memoryLoc].time);
    config.endGroup();

    return 0;

}


void RigMemDialog::clearMemory(memoryData::memData* ldata, int memoryLoc)
{

    memoryNumber = memoryLoc;
    ui->callSignLineEdit->setText(memoryList[memoryLoc].callsign = ldata->callsign);
    ui->freqLineEdit->setInputMask(maskData::freqMask[ldata->freq.count() - 4]);
    ui->freqLineEdit->setText( memoryList[memoryLoc].freq = ldata->freq);
    ui->modecb->setCurrentText( memoryList[memoryLoc].mode = ldata->mode);
    ui->pbStateCb->setCurrentText(memoryList[memoryLoc].passBand = ldata->passBand);
    ui->locatorLineEdit->setText(memoryList[memoryLoc].locator = ldata->locator);
    ui->bearingLineEdit->setText(QString::number(memoryList[memoryLoc].bearing = ldata->bearing));
    ui->timeLineEdit->setText(memoryList[memoryLoc].time = ldata->time);
}


void RigMemDialog::setDialogTitle(QString number)
{
    this->setWindowTitle("M" + number);

}


void RigMemDialog::setLogData(memoryData::memData* ldata, int buttonNumber)
{

    memoryNumber = buttonNumber;
//    logData.callsign = ldata->callsign;
//    logData.freq = ldata->freq;
//    logData.mode = ldata->mode;
//    logData.passBand = ldata->passBand;
//    logData.locator = ldata->locator;
//    logData.bearing = ldata->bearing;
//    logData.time = ldata->time;


    ui->modecb->setCurrentText(ldata->mode);


    ui->pbStateCb->setCurrentText(ldata->passBand);

    readMemory(memoryNumber);

//    if (logData.callsign != memoryList[memoryNumber].callsign)
//    {
        ui->callSignLineEdit->setText(ldata->callsign);
//    }
    if (ldata->locator != memoryList[memoryNumber].locator)
    {
        ui->locatorLineEdit->setText(ldata->locator);
    }
    int a = ldata->freq.count();
    ui->freqLineEdit->setInputMask(maskData::freqMask[ldata->freq.count() - 4]);
    ui->freqLineEdit->setText(ldata->freq);

    if (ldata->bearing != memoryList[memoryNumber].bearing)
    {
        ui->bearingLineEdit->setText(QString::number(ldata->bearing));
    }

    ui->timeLineEdit->setText(ldata->time);
}


void RigMemDialog::saveButtonPushed()
{

   // update memory data
   memoryList[memoryNumber].callsign = ui->callSignLineEdit->text();
   memoryList[memoryNumber].freq = ui->freqLineEdit->text();
   memoryList[memoryNumber].mode = ui->modecb->currentText();
   memoryList[memoryNumber].passBand = ui->pbStateCb->currentText();
   memoryList[memoryNumber].locator = ui->locatorLineEdit->text();
   memoryList[memoryNumber].bearing = ui->bearingLineEdit->text().toInt();
   memoryList[memoryNumber].time = ui->timeLineEdit->text();
   saveMemory(memoryNumber);

   setMemoryFlag(false);

   emit memorySaved(memoryNumber);

}



void RigMemDialog::cancelButtonPushed()
{

    setMemoryFlag(false);

}


memoryData::memData RigMemDialog::getMemoryData(int memoryNumber)
{
    memoryData::memData m;
    m.callsign = memoryList[memoryNumber].callsign;
    m.freq = memoryList[memoryNumber].freq;
    m.mode = memoryList[memoryNumber].mode;
    m.passBand = memoryList[memoryNumber].passBand;
    m.locator = memoryList[memoryNumber].locator;
    m.bearing = memoryList[memoryNumber].bearing;
    m.time = memoryList[memoryNumber].time;
    return m;
}


void RigMemDialog::callSignToUpper(QString callSign)
{

    if (callSign != "")
    {
        ui->callSignLineEdit->setText(callSign.toUpper());
    }
}


void RigMemDialog::locatorToUpper(QString locator)
{

    if (locator != "")
    {
        ui->locatorLineEdit->setText(locator.toUpper());
    }
}


void RigMemDialog::setFocusCallsign()
{
    ui->callSignLineEdit->setFocus();
}
