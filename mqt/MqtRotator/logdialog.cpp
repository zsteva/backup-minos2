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

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
    ui->bearingDifferenceSBox->setRange(0,20);
}

LogDialog::~LogDialog()
{
    delete ui;
}



void LogDialog::on_logDirBrowsePb_clicked()
{
    QString logDir = QFileDialog::getExistingDirectory(0, ("Select Bearing Log Folder"), "./AzimuthLog");
    int a = 0;
}


void LogDialog::on_logFileBrowsePb_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Bearing Log File"), "./AzimuthLog", tr("Bearing Log Files (*.txt)"));
    int a = 0;
}
