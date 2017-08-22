#include "rigmemdialog.h"
#include "ui_rigmemdialog.h"
#include "rigmemcommondata.h"


RigMemDialog::RigMemDialog(QString radioName, QString radioState, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RigMemDialog),
    radioName(radioName),
    radioState(radioState)

{
    ui->setupUi(this);
    readAllMemories();
}

RigMemDialog::~RigMemDialog()
{
    delete ui;
}



int RigMemDialog::readAllMemories()
{

    if (radioName == "")
    {
        return -1;
    }

    QString fileName = "./Configuration/RigMemoryData/" + radioName + "rigMemoryData.ini";

    QSettings config(fileName, QSettings::IniFormat);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++)
    {
        config.beginGroup("MemoryLoc" + QString::number(i+1));
        memoryList[i].callsign= config.value("Callsign", "").toString();
        memoryList[i].freq = config.value("Frequency", "00.000.000.000").toString();
        memoryList[i].locator = config.value("Locator", "").toString();
        memoryList[i].bearing = config.value("Bearing", "").toString();
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

    QString fileName = "./Configuration/RigMemoryData/" + radioName + "rigMemoryData.ini";

    QSettings config(fileName, QSettings::IniFormat);

    config.beginGroup("MemoryLoc" + QString::number(memoryLoc));
    memoryList[memoryLoc].callsign = config.value("Callsign", "").toString();
    memoryList[memoryLoc].freq = config.value("Frequency", "00.000.000.000").toString();
    memoryList[memoryLoc].locator = config.value("Locator", "").toString();
    memoryList[memoryLoc].bearing = config.value("Bearing", "").toString();
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

    QString fileName = "./Configuration/RigMemoryData/" + radioName + "rigMemoryData.ini";
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
    memoryList[memoryLoc].bearing = "";
    memoryList[memoryLoc].time = "";
}
