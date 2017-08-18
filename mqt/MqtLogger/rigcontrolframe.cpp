#include "logger_pch.h"

#include "rigcontrolframe.h"
#include "ui_rigcontrolframe.h"
#include "SendRPCDM.h"

#define MODE_ERROR "<font color='Red'>Mode Error</font>"

RigControlFrame::RigControlFrame(QWidget *parent):
    QFrame(parent)
    , ui(new Ui::RigControlFrame)
    , curFreq("")
    , curMode("")
    , radioName("")
    , radioState("")
    , radioLoaded(false)
{

    ui->setupUi(this);
    ui->modelbl->setText(MODE_ERROR);


}

RigControlFrame::~RigControlFrame()
{
    delete ui;

}


void RigControlFrame::setRadioLoaded()
{
    radioLoaded = true;
}

bool RigControlFrame::isRadioLoaded()
{
    return radioLoaded;
}

void RigControlFrame::setFreq(QString f)
{
    QString freq = f;

    freq.remove('.');
    if (freq.count() >= 4)
    {
        ui->freqInput->setInputMask(maskData::freqMask[freq.count() - 4]);
        ui->freqInput->setText(freq);
        curFreq = freq;
    }
    // an error here

}


void RigControlFrame::setMode(QString m)
{


    for (int i = 0; i < hamlibData::supModeList.count(); i++)
    {
            if (m == hamlibData::supModeList[i])
            {
                ui->modelbl->setText(m);
                curMode = m;
                emit sendModeLogFrame(m);
                return;
            }
    }


    // mode not supported by minos
    ui->modelbl->setText(MODE_ERROR);

}



void RigControlFrame::setRadioName(QString n)
{
    ui->radioName->setText(n);
    radioName = n;
}


void RigControlFrame::setRadioState(QString s)
{
    ui->rigState->setText(s);
    radioState = s;
}

