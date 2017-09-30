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




#include "runbuttondialog.h"
#include "ui_runbuttondialog.h"
#include "rigmemcommondata.h"
#include "rigcontrolcommonconstants.h"
#include "rotatorCommonConstants.h"

RunButtonDialog::RunButtonDialog(QString _radioName, QString _radioState, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunButtonDialog),
    radioName(_radioName),
    radioState(_radioState),
    rxPbFlag(false)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);


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
    //connect(ui->callSignLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(callSignToUpper(const QString &)));
    //connect(ui->locatorLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(locatorToUpper(const QString &)));
    connect(this, SIGNAL(returnPressed()), this, SLOT(saveButtonPushed()));
    connect(this, SIGNAL(escapePressed()), this, SLOT(escapePushed()));
    connect(this , SIGNAL(rejected()), this, SLOT(close()));
}




RunButtonDialog::~RunButtonDialog()
{
    delete ui;
}

void RunButtonDialog::keyPressEvent(QKeyEvent *event)
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

void RunButtonDialog::setRxPbFlag(bool flag)
{
    rxPbFlag = flag;
    ui->pbStateCb->setVisible(!flag);
    ui->passbandlbl->setVisible(!flag);
}



void RunButtonDialog::setRadioName(QString name)
{
    radioName = name;
}

void RunButtonDialog::setRadioState(QString state)
{
    radioState = state;
}


int RunButtonDialog::readAllMemories()
{

    if (radioName == "")
    {
        return -1;
    }

    QString fileName = "./Configuration/RadioMemoryData/" + radioName + "RunData.ini";

    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < runButData::NUM_RUNBUTTONS; i++)
    {
        config.beginGroup("RunLoc" + QString::number(i));
        memoryList[i].freq = config.value("Frequency", memDefData::DEFAULT_FREQ).toString();
        memoryList[i].mode = config.value("Mode", memDefData::DEFAULT_MODE).toString();
        memoryList[i].pBandState = config.value("PBandState", memDefData::DEFAULT_PBAND_STATE).toInt();
        config.endGroup();
    }

    return 0;

}


int RunButtonDialog::readMemory(int memoryLoc)
{

    if (radioName == "")
    {
        return -1;
    }

    QString fileName = "./Configuration/RadioMemoryData/" + radioName + "RunData.ini";

    QSettings config(fileName, QSettings::IniFormat);

    config.beginGroup("RunLoc" + QString::number(memoryLoc));
    memoryList[memoryLoc].freq = config.value("Frequency", memDefData::DEFAULT_FREQ).toString();
    memoryList[memoryLoc].mode = config.value("Mode", memDefData::DEFAULT_MODE).toString();
    memoryList[memoryLoc].pBandState = config.value("PBandState", memDefData::DEFAULT_PBAND_STATE).toInt();
    config.endGroup();


    return 0;

}


int RunButtonDialog::saveMemory(int memoryLoc)
{
    if (radioName == "")
    {
        return -1;
    }

    QString fileName = "./Configuration/RadioMemoryData/" + radioName + "RunData.ini";
    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup("RunLoc" + QString::number(memoryLoc));
    config.setValue("Frequency", memoryList[memoryLoc].freq);
    config.setValue("Mode", memoryList[memoryLoc].mode);
    config.setValue("PBandState", memoryList[memoryLoc].pBandState);
    config.endGroup();

    return 0;

}

void RunButtonDialog::setDialogTitle(QString title)
{
    this->setWindowTitle(title);

}


void RunButtonDialog::editMemory(int memoryLoc)
{

    memoryNumber = memoryLoc;
    //ui->callSignLineEdit->setText(memoryList[memoryLoc].callsign);

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
    //ui->locatorLineEdit->setText(memoryList[memoryLoc].locator);
    //ui->bearingLineEdit->setText(QString::number(memoryList[memoryLoc].bearing));
    //ui->timeLineEdit->setText(memoryList[memoryLoc].time);
}



void RunButtonDialog::clearMemory(memoryData::memData* ldata, int memoryLoc)
{


    memoryNumber = memoryLoc;
    //memoryList[memoryLoc].callsign = ldata->callsign;
    memoryList[memoryLoc].freq = ldata->freq;
    memoryList[memoryLoc].mode = ldata->mode;
    //memoryList[memoryLoc].passBand = ldata->passBand;
    memoryList[memoryLoc].pBandState = ldata->pBandState;
    //memoryList[memoryLoc].locator = ldata->locator;
    //memoryList[memoryLoc].bearing = ldata->bearing;
    //memoryList[memoryLoc].bearing = ldata->bearing;
    //memoryList[memoryLoc].time = ldata->time;
    saveMemory(memoryNumber);

    emit runButtonSaved(memoryNumber);




}





void RunButtonDialog::setLogData(memoryData::memData* ldata, int buttonNumber)
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

    //readMemory(memoryNumber);

    if (ldata->freq.remove('.').count() < 4)
    {
        ui->freqLineEdit->setInputMask(maskData::freqMask[7]);
    }
    else
    {
        ui->freqLineEdit->setInputMask(maskData::freqMask[ldata->freq.remove('.').count() - 4]);
    }
    ui->freqLineEdit->setText(ldata->freq);


}





void RunButtonDialog::saveButtonPushed()
{

   // update run data
   memoryList[memoryNumber].freq = ui->freqLineEdit->text();
   memoryList[memoryNumber].mode = ui->modecb->currentText();
   memoryList[memoryNumber].pBandState = ui->pbStateCb->currentIndex();
   saveMemory(memoryNumber);
   emit runButtonSaved(memoryNumber);
   //setMemoryFlag(false);

   closeMemory();

}



void RunButtonDialog::cancelButtonPushed()
{

    closeMemory();

}


void RunButtonDialog::escapePushed()
{


    closeMemory();

}


void RunButtonDialog::closeMemory()
{

    this->close();
}

memoryData::memData RunButtonDialog::getMemoryData(int memoryNumber)
{
    memoryData::memData m;
    //m.callsign = memoryList[memoryNumber].callsign;
    m.freq = memoryList[memoryNumber].freq;
    m.mode = memoryList[memoryNumber].mode;
    m.pBandState = memoryList[memoryNumber].pBandState;
    //m.locator = memoryList[memoryNumber].locator;
    //m.bearing = memoryList[memoryNumber].bearing;
    //m.time = memoryList[memoryNumber].time;
    return m;
}


void RunButtonDialog::setFocusFreq()
{
    ui->freqLineEdit->setFocus();
}
