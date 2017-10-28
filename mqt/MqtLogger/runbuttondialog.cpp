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

#include "runbuttondialog.h"
#include "ui_runbuttondialog.h"
#include "rigmemcommondata.h"
#include "rigcontrolcommonconstants.h"
#include "rotatorCommonConstants.h"

RunButtonDialog::RunButtonDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunButtonDialog)
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
    ui->freqLineEdit->setFocus();
}




RunButtonDialog::~RunButtonDialog()
{
    delete ui;
}




void RunButtonDialog::setLogData(memoryData::memData* ldata, int buttonNumber)
{

    memoryNumber = buttonNumber;
    logdata = ldata;

    //setWindowTitle(QString("Run%1 - Edit").arg(QString::number(buttonNumber + 1)));

    ui->modecb->setCurrentText(ldata->mode);
    ui->pbStateCb->setCurrentIndex(ldata->pBandState);

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

void RunButtonDialog::on_okButton_clicked()
{
    // update run data
    logdata->freq = ui->freqLineEdit->text();
    logdata->mode = ui->modecb->currentText();
    logdata->pBandState = ui->pbStateCb->currentIndex();
    accept();
}

void RunButtonDialog::on_cancelbutton_clicked()
{
    reject();
}



void RunButtonDialog::setFreqCurPos(int pos)
{
    ui->freqLineEdit->setCursorPosition(pos);
}
