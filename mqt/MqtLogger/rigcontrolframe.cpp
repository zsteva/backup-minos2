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





#include <QSignalMapper>

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
    , freqEditOn(false)
    , memReadFlag(true)
{

    ui->setupUi(this);

    initRigFrame();

}

RigControlFrame::~RigControlFrame()
{
    delete ui;

}


void RigControlFrame::initRigFrame()
{

    ui->modelbl->setText(MODE_ERROR);
    connect(ui->freqInput, SIGNAL(receivedFocus()), this, SLOT(freqLineEditInFocus()));
    connect(ui->freqInput, SIGNAL(returnPressed()), this, SLOT(changeRadioFreq()));
    connect(this, SIGNAL(escapePressed()), this, SLOT(exitFreqEdit()));

    for (int i = 0; i < bandSelData::bandNames.count(); i++)
    {
        ui->bandSelCombo->addItem(bandSelData::bandNames[i]);
    }

    connect(ui->bandSelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(radioBandFreq(int)));
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
        if (!freqEditOn)
        {
            ui->freqInput->setInputMask(maskData::freqMask[freq.count() - 4]);
            ui->freqInput->setText(freq);
        }
        curFreq = freq;
    }
    // an error here

}


void RigControlFrame::changeRadioFreq()
{
    QString freq = "";
    if (ui->freqInput->isModified())
    {
        freq = ui->freqInput->text();
        if (freq.count() >=4)
        {
            emit sendFreqControl(freq);
        }
    }

    exitFreqEdit();
}



void RigControlFrame::radioBandFreq(int index)
{
    if (index > 0 && index < bandSelData::bandFreq.count())
    {
        ui->freqInput->setInputMask(maskData::freqMask[bandSelData::bandMaskIdx[index]]);
        ui->freqInput->setText(bandSelData::freqDialZero[index]);
        emit sendFreqControl(bandSelData::bandFreq[index]);
    }
}


void RigControlFrame::exitFreqEdit()
{

    setFreq(curFreq);
    freqLineEditFrameColour(false);
    ui->freqInput->clearFocus();
    freqEditOn = false;

}


void RigControlFrame::initMemoryButtons()
{
    memButtons[0] = ui->mem1pb;
    memButtons[1] = ui->mem2pb;
    memButtons[2] = ui->mem3pb;
    memButtons[3] = ui->mem4pb;
    memButtons[4] = ui->mem5pb;
    memButtons[5] = ui->mem6pb;

    QSignalMapper *memButton_mapper = new QSignalMapper(this);

    for (int i = 0; i < NUM_MEMORIES; i++ )
    {
        memButton_mapper->setMapping(memButtons[i], i);
        connect(memButtons[i], SIGNAL(clicked()), memButton_mapper, SLOT(map()));

    }
    connect(memButton_mapper, SIGNAL(mapped(int)), this, SLOT(clickedMemory(int)));

    connect(ui->memReadSel, SIGNAL(clicked(bool)), this, SLOT(memRadioButtonClicked(bool)));

    connect(ui->memWriteSel, SIGNAL(clicked(bool)), this, SLOT(memRadioButtonClicked(bool)));



}


void RigControlFrame::memRadioButtonClicked(bool)
{

    if (ui->memReadSel->isChecked())
    {
        memReadFlag = true;
        ui->memReadSel->setStyleSheet("border: 1px solid black");
    }
    else if (ui->memWriteSel->isChecked())
    {
       memReadFlag = false;
       ui->memWriteSel->setStyleSheet("border: 1px solid red");
    }


}






void RigControlFrame::clickedMemory(int buttonNumber)
{


    qDebug() << "rightclicked button " << buttonNumber;
/*

    if (presetName[buttonNumber] != "")
    {
        if (presetBearing[buttonNumber] != "")
        {
           ui->bearingEdit->setText(presetBearing[buttonNumber]);
           emit presetRotateTo();
        }
    }
*/
}



void RigControlFrame::keyPressEvent(QKeyEvent *event)
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


void RigControlFrame::sendModeToRadio(QString m)
{
    sendModeToControl(m);
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


void RigControlFrame::freqLineEditInFocus()
{

    freqEditOn = true;
    freqLineEditFrameColour(true);
}




void RigControlFrame::freqLineEditBkgnd(bool status)
{

    QPalette palette;

    if (status)
    {
        palette.setColor(QPalette::Base,Qt::yellow);
    }
    else
    {
        palette.setColor(QPalette::Base,Qt::white);
    }

    ui->freqInput->setPalette(palette);

}


void RigControlFrame::freqLineEditFrameColour(bool status)
{
    int curPos = ui->freqInput->cursorPosition();
    if (status)
    {
        ui->freqInput->setStyleSheet("border: 1px solid red");
        // restore cursor selection
        ui->freqInput->setSelection(curPos, 1);
    }
    else
    {
        ui->freqInput->setStyleSheet("border: 1px solid black");

    }

}



//********************************************//

FreqLineEdit::FreqLineEdit(QWidget *parent):
    QLineEdit(parent)
{

}


FreqLineEdit::~FreqLineEdit()
{


}

void FreqLineEdit::focusInEvent( QFocusEvent * ev )
{
    emit receivedFocus() ;

}
