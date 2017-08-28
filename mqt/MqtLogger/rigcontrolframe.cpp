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
#include "tlogcontainer.h"
#include "tsinglelogframe.h"
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

    logData.callsign = "";
    logData.freq = "00.000.000.000";
    logData.mode = "";
    logData.passBand = "2.200";
    logData.locator = "";
    logData.bearing = 0;
    logData.time = "";


    initRigFrame();
    initMemoryButtons();


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

    memDialog = new RigMemDialog();
    connect(memDialog, SIGNAL(memorySaved(int)), this, SLOT(memoryUpdate(int)));


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
    memButtons[0] = ui->memButton1;
    memButtons[1] = ui->memButton2;
    memButtons[2] = ui->memButton3;
    memButtons[3] = ui->memButton4;
    memButtons[4] = ui->memButton5;
    memButtons[5] = ui->memButton6;
    memButtons[6] = ui->memButton7;
    memButtons[7] = ui->memButton8;
    memButtons[8] = ui->memButton9;
    memButtons[9] = ui->memButton10;



    for (int i = 0; i < memoryData::NUM_MEMORIES; i++)
    {
        memButtons[i]->setToolButtonStyle(Qt::ToolButtonTextOnly);
        memButtons[i]->setText(memoryData::memoryTitle[i] + memoryData::memTitleBlank);
        memButtons[i]->setPopupMode(QToolButton::InstantPopup);

        memoryMenu[i] = new QMenu(memButtons[i]);

        readAction[i] = new QAction("Read", this);
        writeAction[i] = new QAction("Write", this);
        clearAction[i] = new QAction("Clear", this);

        memoryMenu[i]->addAction(readAction[i]);
        memoryMenu[i]->addAction(writeAction[i]);
        memoryMenu[i]->addAction(clearAction[i]);

        memButtons[i]->setMenu(memoryMenu[i]);
    }

    // map read Action

    QSignalMapper *readAction_mapper = new QSignalMapper(this);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++ )
    {
        readAction_mapper->setMapping(readAction[i], i);
        connect(readAction[i], SIGNAL(triggered()), readAction_mapper, SLOT(map()));

    }
    connect(readAction_mapper, SIGNAL(mapped(int)), this, SLOT(readActionSelected(int)));

    // map write Action

    QSignalMapper *writeAction_mapper = new QSignalMapper(this);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++ )
    {
        writeAction_mapper->setMapping(writeAction[i], i);
        connect(writeAction[i], SIGNAL(triggered()), writeAction_mapper, SLOT(map()));

    }
    connect(writeAction_mapper, SIGNAL(mapped(int)), this, SLOT(writeActionSelected(int)));


    // map Clear Action

    QSignalMapper *clearAction_mapper = new QSignalMapper(this);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++ )
    {
        clearAction_mapper->setMapping(clearAction[i], i);
        connect(clearAction[i], SIGNAL(triggered()), clearAction_mapper, SLOT(map()));

    }
    connect(clearAction_mapper, SIGNAL(mapped(int)), this, SLOT(clearActionSelected(int)));

    // load button labels

    loadMemoryButtonLabels();

}



void RigControlFrame::readActionSelected(int buttonNumber)
{
    memoryData::memData m = memDialog->getMemoryData(buttonNumber);

    emit sendFreqControl(m.freq);

}

void RigControlFrame::writeActionSelected(int buttonNumber)
{

    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();
    // get contest information
    //TSingleLogFrame *tslf = dynamic_cast<TSingleLogFrame *>(p);
    ScreenContact sc = tslf->getScreenEntry();
    logData.callsign = sc.cs.fullCall.getValue();
    logData.locator = sc.loc.loc.getValue();
    logData.mode = curMode;
    logData.time = dtg( true ).getIsoDTG();
    // time now, other formats
    // are available QString qth = sc.extraText;

    logData.bearing = sc.bearing;
    // load log data into memory
    memDialog->setLogData(&logData, buttonNumber, curFreq);
    memDialog->setDialogTitle(QString::number(buttonNumber + 1));
    memDialog->show();
}

void RigControlFrame::clearActionSelected(int buttonNumber)
{
     memDialog->clearMemory(buttonNumber);
     memDialog->setDialogTitle(QString::number(buttonNumber + 1));
     memDialog->show();
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
    if (n != "" && radioName != n)
    {
        ui->radioName->setText(n);
        radioName = n;
        memDialog->setRadioName(n);
        memDialog->readAllMemories();
        loadMemoryButtonLabels();
    }










}


void RigControlFrame::setRadioState(QString s)
{
    if (s !="" && radioState != s)
    {
        ui->rigState->setText(s);
        radioState = s;
        memDialog->setRadioState(s);
    }

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

void RigControlFrame::loadMemoryButtonLabels()
{
    for (int i = 0; i < memoryData::NUM_MEMORIES; i++)
    {
        memoryUpdate(i);
    }
}


void RigControlFrame::memoryUpdate(int buttonNumber)
{
    memoryData::memData m = memDialog->getMemoryData(buttonNumber);
    memButtons[buttonNumber]->setText("M" + QString::number(buttonNumber + 1) + ": " + m.callsign + " ");
    QString tTipStr = "Callsign: " + m.callsign + "\n"
            + "Freq: " + m.freq + "\n"
            + "Mode: " + m.mode + "\n"
            + "Passband: " + m.passBand + "\n"
            + "Locator: " + m.locator + "\n"
            + "Bearing: " + "\n"
            + "Time: " + m.time;
    memButtons[buttonNumber]->setToolTip(tTipStr);
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
