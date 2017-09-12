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
#include "rotatorCommonConstants.h"


RigMemDialog::RigMemDialog(QString _radioName, QString _radioState, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RigMemDialog)
    ,radioName(_radioName)
    ,radioState(_radioState)


{
    ui->setupUi(this);
    //this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);


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
    connect(this, SIGNAL(returnPressed()), this, SLOT(saveButtonPushed()));
    connect(this, SIGNAL(escapePressed()), this, SLOT(escapePushed()));
    connect(this , SIGNAL(rejected()), this, SLOT(close()));
}

RigMemDialog::~RigMemDialog()
{
    delete ui;
}


void RigMemDialog::keyPressEvent(QKeyEvent *event)
{

    int Key = event->key();

/*
    Qt::KeyboardModifiers mods = event->modifiers();
    bool shift = mods & Qt::ShiftModifier;
    bool ctrl = mods & Qt::ControlModifier;
    bool alt = mods & Qt::AltModifier;
*/


    if (Key == Qt::Key_Escape)
    {
        emit escapePressed();
    }
    else if (Key == Qt::Key_Return)
    {
        emit returnPressed();
    }


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
        memoryList[i].callsign= config.value("Callsign", memDefData::DEFAULT_CALLSIGN).toString();
        memoryList[i].freq = config.value("Frequency", memDefData::DEFAULT_FREQ).toString();
        memoryList[i].mode = config.value("Mode", memDefData::DEFAULT_MODE).toString();
        //memoryList[i].passBand = config.value("PassBand", memDefData::DEFAULT_PBAND).toString();
        memoryList[i].pBandState = config.value("PBandState", memDefData::DEFAULT_PBAND_STATE).toInt();
        memoryList[i].locator = config.value("Locator", memDefData::DEFAULT_LOCATOR).toString();
        memoryList[i].bearing = config.value("Bearing", memDefData::DEFAULT_BEARING).toInt();
        memoryList[i].time = config.value("Time", memDefData::DEFAULT_TIME).toString();
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
    memoryList[memoryLoc].callsign = config.value("Callsign", memDefData::DEFAULT_CALLSIGN).toString();
    memoryList[memoryLoc].freq = config.value("Frequency", memDefData::DEFAULT_FREQ).toString();
    memoryList[memoryLoc].mode = config.value("Mode", memDefData::DEFAULT_MODE).toString();
    //memoryList[memoryLoc].passBand = config.value("PassBand", memDefData::DEFAULT_PBAND).toString();
    memoryList[memoryLoc].pBandState = config.value("PBandState", memDefData::DEFAULT_PBAND_STATE).toInt();
    memoryList[memoryLoc].locator = config.value("Locator", memDefData::DEFAULT_LOCATOR).toString();
    memoryList[memoryLoc].bearing = config.value("Bearing", memDefData::DEFAULT_BEARING).toInt();
    memoryList[memoryLoc].time = config.value("Time", memDefData::DEFAULT_TIME).toString();
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
    //config.setValue("PassBand", memoryList[memoryLoc].passBand);
    config.setValue("PBandState", memoryList[memoryLoc].pBandState);
    config.setValue("Locator", memoryList[memoryLoc].locator);
    config.setValue("Bearing", memoryList[memoryLoc].bearing);
    config.setValue("Time", memoryList[memoryLoc].time);
    config.endGroup();

    return 0;

}


void RigMemDialog::editMemory(int memoryLoc)
{
    ui->callSignLineEdit->setText(memoryList[memoryLoc].callsign);
    QString a = memoryList[memoryLoc].freq;
    if (memoryList[memoryLoc].freq.remove('.').count() < 4)
    {
        ui->freqLineEdit->setInputMask(maskData::freqMask[7]);
    }
    else
    {
        ui->freqLineEdit->setInputMask(maskData::freqMask[memoryList[memoryLoc].freq.remove('.').count() - 4]);
    }
    ui->freqLineEdit->setText( memoryList[memoryLoc].freq);
    ui->modecb->setCurrentText( memoryList[memoryLoc].mode);
    ui->pbStateCb->setCurrentText(hamlibData::pBandStateStr[memoryList[memoryLoc].pBandState]);
    ui->locatorLineEdit->setText(memoryList[memoryLoc].locator);
    ui->bearingLineEdit->setText(QString::number(memoryList[memoryLoc].bearing));
    ui->timeLineEdit->setText(memoryList[memoryLoc].time);
}



void RigMemDialog::clearMemory(memoryData::memData* ldata, int memoryLoc)
{


    memoryNumber = memoryLoc;
    memoryList[memoryLoc].callsign = ldata->callsign;
    memoryList[memoryLoc].freq = ldata->freq;
    memoryList[memoryLoc].mode = ldata->mode;
    //memoryList[memoryLoc].passBand = ldata->passBand;
    memoryList[memoryLoc].pBandState = ldata->pBandState;
    memoryList[memoryLoc].locator = ldata->locator;
    memoryList[memoryLoc].bearing = ldata->bearing;
    memoryList[memoryLoc].bearing = ldata->bearing;
    memoryList[memoryLoc].time = ldata->time;
    saveMemory(memoryNumber);

    setMemoryFlag(false);

    emit memorySaved(memoryNumber);


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


    ui->pbStateCb->setCurrentIndex(ldata->pBandState);

    readMemory(memoryNumber);

//    if (logData.callsign != memoryList[memoryNumber].callsign)
//    {
        ui->callSignLineEdit->setText(ldata->callsign);
//    }

//    if (ldata->locator == "" && ldata->locator != memoryList[memoryNumber].locator)
//    {
        ui->locatorLineEdit->setText(ldata->locator);
//    }
    if (ldata->freq.remove('.').count() < 4)
    {
        ui->freqLineEdit->setInputMask(maskData::freqMask[7]);
    }
    else
    {
        ui->freqLineEdit->setInputMask(maskData::freqMask[ldata->freq.remove('.').count() - 4]);
    }
    ui->freqLineEdit->setText(ldata->freq);

    if (ldata->bearing != memoryList[memoryNumber].bearing)
    {
        if (ldata->bearing == COMPASS_ERROR)
        {
            ui->bearingLineEdit->setText("");
        }
        else
        {
            ui->bearingLineEdit->setText(QString::number(ldata->bearing));
        }

    }

    ui->timeLineEdit->setText(ldata->time);
}





void RigMemDialog::saveButtonPushed()
{

   // update memory data
   memoryList[memoryNumber].callsign = ui->callSignLineEdit->text();
   memoryList[memoryNumber].freq = ui->freqLineEdit->text();
   memoryList[memoryNumber].mode = ui->modecb->currentText();
   memoryList[memoryNumber].pBandState = ui->pbStateCb->currentIndex();
   memoryList[memoryNumber].locator = ui->locatorLineEdit->text();
   memoryList[memoryNumber].bearing = ui->bearingLineEdit->text().toInt();
   memoryList[memoryNumber].time = ui->timeLineEdit->text();
   saveMemory(memoryNumber);

   setMemoryFlag(false);

   emit memorySaved(memoryNumber);

}



void RigMemDialog::cancelButtonPushed()
{

    closeMemory();

}


void RigMemDialog::escapePushed()
{


    closeMemory();

}


void RigMemDialog::closeMemory()
{
    setMemoryFlag(false);
    this->close();
}

memoryData::memData RigMemDialog::getMemoryData(int memoryNumber)
{
    memoryData::memData m;
    m.callsign = memoryList[memoryNumber].callsign;
    m.freq = memoryList[memoryNumber].freq;
    m.mode = memoryList[memoryNumber].mode;
    m.pBandState = memoryList[memoryNumber].pBandState;
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
