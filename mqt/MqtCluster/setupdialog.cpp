#include "setupdialog.h"
#include "ui_setupdialog.h"

#include <QSettings>

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);
}

SetupDialog::~SetupDialog()
{
    delete ui;
}



void SetupDialog::savePersonel()
{


    QString fileName;
    fileName = CLUSTER_CONFIG_FILE;


    QSettings config(fileName, QSettings::IniFormat);

    config.beginGroup("Personel");
    config.setValue("Callsign", callsign);
    config.setValue("Name", name);
    config.setValue("Locator", locator);
    config.setValue("Qth", qth);

    config.endGroup();




}


void SetupDialog::readPersonel()
{

    QString fileName;
    fileName = CLUSTER_CONFIG_FILE;


    QSettings config(fileName, QSettings::IniFormat);

    config.beginGroup("Personel");

    callsign = config.value("Callsign", "").toString();
    name = config.value("Name", "").toString();
    locator = config.value("Locator", "").toString();
    qth = config.value("Qth", "").toString();

    config.endGroup();



}
