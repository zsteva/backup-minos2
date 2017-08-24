#include "rigmemdialog.h"
#include "ui_rigmemdialog.h"
#include "rigmemcommondata.h"


RigMemDialog::RigMemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RigMemDialog)


{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    radioName = "";
    radioState = "";
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));
}

RigMemDialog::~RigMemDialog()
{
    delete ui;
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
    config.setValue("Locator", memoryList[memoryLoc].locator);
    config.setValue("Bearing", memoryList[memoryLoc].bearing);
    config.setValue("Time", memoryList[memoryLoc].time);
    config.endGroup();

    return 0;

}


void RigMemDialog::clearMemory(int memoryLoc)
{
    memoryList[memoryLoc].callsign = "";
    memoryList[memoryLoc].freq = "00.000.000.000";
    memoryList[memoryLoc].locator = "";
    memoryList[memoryLoc].bearing = 0;
    memoryList[memoryLoc].time = "";
}


void RigMemDialog::setDialogTitle(QString number)
{
    this->setWindowTitle("M" + number);

}


void RigMemDialog::setLogData(memoryData::memData* ldata, int buttonNumber, QString f)
{

    memoryNumber = buttonNumber;
    logData.callsign = ldata->callsign;
    logData.freq = ldata->freq;
    logData.locator = ldata->locator;
    logData.bearing = ldata->bearing;
    logData.time = ldata->time;

    readMemory(memoryNumber);

    if (logData.callsign != memoryList[memoryNumber].callsign)
    {
        ui->callSignLineEdit->setText(logData.callsign);
    }
    if (logData.locator != memoryList[memoryNumber].locator)
    {
        ui->locatorLineEdit->setText(logData.locator);
    }

    ui->freqLineEdit->setInputMask(maskData::freqMask[f.count() - 4]);
    ui->freqLineEdit->setText(f);

    if (logData.bearing != memoryList[memoryNumber].bearing)
    {
        ui->bearingLineEdit->setText(QString::number(logData.bearing));
    }

    ui->timeLineEdit->setText(ldata->time);
}


void RigMemDialog::saveButtonPushed()
{
    if (ui->callSignLineEdit->text() != "")
    {
        // update memory data
        memoryList[memoryNumber].callsign = ui->callSignLineEdit->text();
        memoryList[memoryNumber].freq = ui->freqLineEdit->text();
        memoryList[memoryNumber].locator = ui->locatorLineEdit->text();
        memoryList[memoryNumber].bearing = ui->bearingLineEdit->text().toInt();
        memoryList[memoryNumber].time = ui->timeLineEdit->text();
        saveMemory(memoryNumber);
        emit memorySaved(memoryNumber);
    }



}



void RigMemDialog::cancelButtonPushed()
{




}


memoryData::memData RigMemDialog::getMemoryData(int memoryNumber)
{
    memoryData::memData m;
    m.callsign = memoryList[memoryNumber].callsign;
    m.freq = memoryList[memoryNumber].freq;
    m.locator = memoryList[memoryNumber].locator;
    m.bearing = memoryList[memoryNumber].bearing;
    m.time = memoryList[memoryNumber].time;
    return m;
}
