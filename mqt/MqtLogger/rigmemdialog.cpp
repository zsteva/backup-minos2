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

#include "logger_pch.h"

#include "rigmemdialog.h"
#include "ui_rigmemdialog.h"
#include "rigmemcommondata.h"
#include "rigutils.h"
#include "rigcontrolcommonconstants.h"
#include "rotatorCommonConstants.h"


RigMemDialog::RigMemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RigMemDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    for (int i = 0; i < hamlibData::supModeList.count(); i++)
    {
        ui->modecb->addItem(hamlibData::supModeList[i]);
    }

    ui->callSignLineEdit->setValidator(new UpperCaseValidator(true));
    ui->locatorLineEdit->setValidator(new UpperCaseValidator(true));

    ui->callSignLineEdit->setFocus();
    // validate the input
    connect(ui->freqLineEdit, SIGNAL(editingFinished()), this, SLOT(onFreqEditFinish()));
}

RigMemDialog::~RigMemDialog()
{
    delete ui;
}



void RigMemDialog::onFreqEditFinish()
{
    QString f = ui->freqLineEdit->text().trimmed().remove( QRegExp("^[0]*"));
    if (f.contains('.'))
    {
        QStringList fl = f.split('.');
        if (fl[1].count() > 6)
        {
            fl[1].truncate(6);
            f = fl[0] + "." + fl[1];
        }

    }
    if (!validateFreqTxtInput(f))
    {
        // error
        QMessageBox msgBox;
        msgBox.setText(FREQ_EDIT_ERR_MSG);
        msgBox.exec();

    }
}



void RigMemDialog::setLogData(memoryData::memData* ldata, int buttonNumber)
{
    memoryNumber = buttonNumber;
    logData = ldata;
    ui->modecb->setCurrentText(ldata->mode);


    ui->callSignLineEdit->setText(ldata->callsign);
    ui->locatorLineEdit->setText(ldata->locator);

//    if (ldata->freq.remove('.').count() < 4)
//    {
//        ui->freqLineEdit->setInputMask(maskData::freqMask[7]);
//    }
//    else
//    {
//        ui->freqLineEdit->setInputMask(maskData::freqMask[ldata->freq.remove('.').count() - 4]);
//    }
    ui->freqLineEdit->setText(convertFreqStrDispSingle(ldata->freq));

    if (ldata->bearing == COMPASS_ERROR)
    {
        ui->bearingLineEdit->setText("");
    }
    else
    {
        ui->bearingLineEdit->setText(QString::number(ldata->bearing));
    }
    ui->timeLineEdit->setText(ldata->time);
    //setWindowTitle(QString("M%1 - Edit").arg(QString::number(buttonNumber + 1)));
}

void RigMemDialog::on_okButton_clicked()
{
    logData->callsign = ui->callSignLineEdit->text().trimmed();
    if (logData->callsign.isEmpty())
        logData->callsign = "??";

    //QString f = convertSinglePeriodFreqToMultiPeriod(ui->freqLineEdit->text());
    QString f = ui->freqLineEdit->text().remove( QRegExp("^[0]*")); //remove periods and leading zeros
    if (f == "")
    {
        logData->freq = "00000000000";
    }
    else
    {
        QStringList fl = f.split('.');
        if (fl.count() == 0)
        {
            trace(QString("Memory Freq Edit - Missing Period - %1").arg(f));
            return;
        }

        fl[1] = fl[1] + "0000000";
        fl[1].truncate(6);

        logData->freq = fl[0] + fl[1];
    }

    logData->mode = ui->modecb->currentText();
    logData->locator = ui->locatorLineEdit->text();
    logData->bearing = ui->bearingLineEdit->text().toInt();
    logData->time = ui->timeLineEdit->text();
    accept();
}

void RigMemDialog::on_cancelButton_clicked()
{
    reject();
}


