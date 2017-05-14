/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////



#include "logdialog.h"
#include "ui_logdialog.h"
#include <QFile>
#include <QDateTime>
#include <QPushButton>
#include <QFileDialog>
#include <QSpinBox>
#include <QSettings>

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
    ui->bearingDifferenceSBox->setRange(0,20);
    readLogConfig();
}

LogDialog::~LogDialog()
{
    delete ui;
}



void LogDialog::on_logDirBrowsePb_clicked()
{
    directory = QFileDialog::getExistingDirectory(0, ("Select Bearing Log Folder"), "./BearingLog");

}






void LogDialog::on_logFilenameEdit_textChanged(const QString &arg1)
{
    filename = arg1;
}



void LogDialog:: saveLogConfig()
{


    QSettings config("./Configuration/MinosRotatorConfig.ini", QSettings::IniFormat);
    config.beginGroup("BearingLog");

    config.setValue("directory", directory);
    config.setValue("filename", filename);
    config.setValue("bearing_difference", bearingDiff);
    config.setValue("loggingEnabled", logEnabled);

    config.endGroup();

}




void LogDialog::readLogConfig()
{
    QSettings config("./Configuration/MinosRotatorConfig.ini", QSettings::IniFormat);
    config.beginGroup("BearingLog");

    directory = config.value("directory", "./Configuration/").toString();
    filename = config.value("filename", "bearingLog").toString();
    bearingDiff = config.value("bearing_difference", 0).toInt();
    logEnabled = config.value("loggingEnabled", false).toBool();

    config.endGroup();
}


void LogDialog::loadLogConfig()
{
    ui->logDirectoryEdit->setText(directory);
    ui->logFilenameEdit->setText(filename);
    ui->bearingDifferenceSBox->setValue(bearingDiff);
    ui->logcheckBox->setChecked(logEnabled);
    emit showLogDialog();
}

