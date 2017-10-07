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
//#include <QLabel>
//#include <QtWidgets>
#include <QLayoutItem>

#include "logger_pch.h"
#include "rigcontrolframe.h"
#include "tlogcontainer.h"
#include "tsinglelogframe.h"
#include "ui_rigcontrolframe.h"
#include "SendRPCDM.h"

#define MODE_ERROR "<font color='Red'>Mode Error</font>"

/*static QStringList memoryShortCut = {QString("Ctrl+1"),QString("Ctrl+2"),
                            QString("Ctrl+3"), QString("Ctrl+4"),
                            QString("Ctrl+5"), QString("Ctrl+6"),
                            QString("Ctrl+7"), QString("Ctrl+8"),
                            QString("Ctrl+9"), QString("Ctrl+0")};


*/
static QKeySequence memoryShortCut[] = {

    QKeySequence(Qt::CTRL + Qt::Key_1),
    QKeySequence(Qt::CTRL + Qt::Key_2),
    QKeySequence(Qt::CTRL + Qt::Key_3),
    QKeySequence(Qt::CTRL + Qt::Key_4),
    QKeySequence(Qt::CTRL + Qt::Key_5),
    QKeySequence(Qt::CTRL + Qt::Key_6),
    QKeySequence(Qt::CTRL + Qt::Key_7),
    QKeySequence(Qt::CTRL + Qt::Key_8),
    QKeySequence(Qt::CTRL + Qt::Key_9),
    QKeySequence(Qt::CTRL + Qt::Key_0)
};



static QKeySequence runButShortCut[] {
    QKeySequence(Qt::CTRL + Qt::Key_BracketLeft),
    QKeySequence(Qt::CTRL + Qt::Key_BracketRight)

};



RigControlFrame::RigControlFrame(QWidget *parent):
    QFrame(parent)
    , ui(new Ui::RigControlFrame)
    , curFreq(memDefData::DEFAULT_FREQ)
    , curMode(memDefData::DEFAULT_MODE)
    , curpbState(memDefData::DEFAULT_PBAND_STATE)
    , radioName("NoRadio")
    , radioState("None")
    , radioLoaded(false)
    , freqEditOn(false)


{

    ui->setupUi(this);
    connect(&MinosLoggerEvents::mle, SIGNAL(FontChanged()), this, SLOT(on_FontChanged()), Qt::QueuedConnection);

    logData.callsign = memDefData::DEFAULT_CALLSIGN;
    logData.freq = memDefData::DEFAULT_FREQ;
    logData.mode = memDefData::DEFAULT_MODE;
    //logData.passBand = memDefData::DEFAULT_PBAND;
    logData.pBandState = memDefData::DEFAULT_PBAND_STATE;
    logData.locator = memDefData::DEFAULT_LOCATOR;
    logData.bearing = memDefData::DEFAULT_BEARING;
    logData.time = memDefData::DEFAULT_TIME;


    initRigFrame(parent);

    initMemoryButtons(parent);
    initRunMemoryButton(parent);
    initPassBandRadioButtons();

    setRxPBFlag("set");

    // init memory button data before radio connection
    setRadioName(radioName);


    on_FontChanged();
    traceMsg("RigControl Frame Started");
}

RigControlFrame::~RigControlFrame()
{
    delete ui;

}


void RigControlFrame::initRigFrame(QWidget */*parent*/)
{

    ui->modelbl->setText(MODE_ERROR);
    ui->normalRb->setChecked(true);
    connect(ui->freqInput, SIGNAL(receivedFocus()), this, SLOT(freqLineEditInFocus()));
    connect(ui->freqInput, SIGNAL(returnPressed()), this, SLOT(changeRadioFreq()));
    connect(this, SIGNAL(escapePressed()), this, SLOT(exitFreqEdit()));
    // when no radio is connected
    connect(this, SIGNAL(noRadioSendFreq(QString)), this, SLOT(noRadioSetFreq(QString)));

    for (int i = 0; i < bandSelData::bandNames.count(); i++)
    {
        ui->bandSelCombo->addItem(bandSelData::bandNames[i]);
    }

    connect(ui->bandSelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(radioBandFreq(int)));

    memDialog = new RigMemDialog(radioName, radioState);
    connect(memDialog, SIGNAL(memorySaved(int)), this, SLOT(memoryUpdate(int)));

    runDialog = new RunButtonDialog(radioName, radioState);
    connect(runDialog, SIGNAL(runButtonSaved(int)), this, SLOT(runButtonUpdate(int)));

    setRxPBFlag("set");

    if (!isRadioLoaded())
    {
        ui->modelbl->setVisible(false);
    }

/*
    freqLabel = new QLabel(parent);
    ui->horizontalLayout_4->addWidget(freqLabel);
    freqLabel->setFrameStyle(QFrame::Panel);
    freqLabel->setText("&Freq");
    freqLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    //Here is how to change position:
    //freqLabel->setGeometry(QRectF(1,1,30,49));
    freqLabel->setBuddy(ui->freqInput);   // CTR-f for Freq Edit
*/
}



void RigControlFrame::setRadioLoaded()
{
    traceMsg("Set Radio Loaded");
    radioLoaded = true;
    ui->modelbl->setVisible(true);
}

bool RigControlFrame::isRadioLoaded()
{
    return radioLoaded;
}

void RigControlFrame::noRadioSetFreq(QString f)
{
    traceMsg("No Radio SetFreq");
    setFreq(f);
}

void RigControlFrame::setFreq(QString f)
{
    traceMsg("Set Freq - ui, curFreq");
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
    traceMsg("Change Radio Freq");
    QString freq = "";
    if (ui->freqInput->isModified())
    {
        freq = ui->freqInput->text();
        if (freq.count() >=4)
        {
            if (isRadioLoaded())
            {
                emit sendFreqControl(freq);
            }
            else
            {
                noRadioSendOutFreq(freq);
            }
        }
    }

    exitFreqEdit();
}



void RigControlFrame::radioBandFreq(int index)
{
    traceMsg("Radio Band Freq");
    if (index > 0 && index < bandSelData::bandFreq.count())
    {
        ui->freqInput->setInputMask(maskData::freqMask[bandSelData::bandMaskIdx[index]]);
        ui->freqInput->setText(bandSelData::freqDialZero[index]);
        if (isRadioLoaded())
        {
            emit sendFreqControl(bandSelData::bandFreq[index]);
        }
        else
        {
            noRadioSendOutFreq(bandSelData::bandFreq[index]);
        }
    }
}


void RigControlFrame::noRadioSendOutFreq(QString f)
{
    traceMsg("No Radio Send Freq to Radio");
    // update rigframe
    emit noRadioSendFreq(f);
    // update logger
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();
    tslf->on_NoRadioSetFreq(f);
}


void RigControlFrame::exitFreqEdit()
{
    traceMsg("Exit Edit Freq");
    freqEditOn = false;
    setFreq(curFreq);
    freqLineEditFrameColour(false);
    ui->freqInput->clearFocus();
}



void RigControlFrame::initMemoryButtons(QWidget *parent)
{
    /*
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

        shortKey[i] = new QShortcut(memoryShortCut[i], parent);
        memButtons[i]->setToolButtonStyle(Qt::ToolButtonTextOnly);
        memButtons[i]->setPopupMode(QToolButton::InstantPopup);
        memButtons[i]->setText(memoryData::memoryTitle[i] + memoryData::memTitleBlank);
        //memButtons[i]->setShortcut(QKeySequence(memoryShortCut[i]));
        memoryMenu[i] = new QMenu(memButtons[i]);

        readAction[i] = new QAction("&Read", this);
        writeAction[i] = new QAction("&Write",this);
        editAction[i] = new QAction("&Edit", this);
        clearAction[i] = new QAction("&Clear",this);
        memoryMenu[i]->addAction(readAction[i]);
        memoryMenu[i]->addAction(writeAction[i]);
        memoryMenu[i]->addAction(editAction[i]);
        memoryMenu[i]->addAction(clearAction[i]);
        memButtons[i]->setMenu(memoryMenu[i]);
    }


    // map shortcut keys
   QSignalMapper *shortKey_mapper = new QSignalMapper(this);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++ )
    {
        shortKey_mapper->setMapping(shortKey[i], i);
        connect(shortKey[i], SIGNAL(activated()), shortKey_mapper, SLOT(map()));

    }
    connect(shortKey_mapper, SIGNAL(mapped(int)), this, SLOT(memoryShortCutSelected(int)));

//----------------------------------------------------------------------------------------

    // map read Action

    QSignalMapper *readAction_mapper = new QSignalMapper(this);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++ )
    {
        readAction_mapper->setMapping(readAction[i], i);
        connect(readAction[i], SIGNAL(triggered()), readAction_mapper, SLOT(map()));

    }
    connect(readAction_mapper, SIGNAL(mapped(int)), this, SLOT(readActionSelected(int)));

//----------------------------------------------------------------------------------------

    // map write Action

    QSignalMapper *writeAction_mapper = new QSignalMapper(this);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++ )
    {
        writeAction_mapper->setMapping(writeAction[i], i);
        connect(writeAction[i], SIGNAL(triggered()), writeAction_mapper, SLOT(map()));

    }
    connect(writeAction_mapper, SIGNAL(mapped(int)), this, SLOT(writeActionSelected(int)));

//-----------------------------------------------------------------------------------------

    // map edit Action

    QSignalMapper *editAction_mapper = new QSignalMapper(this);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++ )
    {
        editAction_mapper->setMapping(editAction[i], i);
        connect(editAction[i], SIGNAL(triggered()), editAction_mapper, SLOT(map()));

    }
    connect(editAction_mapper, SIGNAL(mapped(int)), this, SLOT(editActionSelected(int)));

//-----------------------------------------------------------------------------------------

    // map Clear Action

    QSignalMapper *clearAction_mapper = new QSignalMapper(this);

    for (int i = 0; i < memoryData::NUM_MEMORIES; i++ )
    {
        clearAction_mapper->setMapping(clearAction[i], i);
        connect(clearAction[i], SIGNAL(triggered()), clearAction_mapper, SLOT(map()));

    }
    connect(clearAction_mapper, SIGNAL(mapped(int)), this, SLOT(clearActionSelected(int)));

//-----------------------------------------------------------------------------------------

    // load button labels

//    loadMemoryButtonLabels();
*/

}

void RigControlFrame::on_FontChanged()
{
    /*
    QFontMetrics fm = ui->memButton1->fontMetrics();
    int w = fm.width("M20: MM/MM0WWW/MM");
    for (int i = 0; i < memoryData::NUM_MEMORIES; i++)
    {
        memButtons[i]->setMinimumWidth(w);
    }
*/
}


void RigControlFrame::initPassBandRadioButtons()
{
    curpbState = hamlibData::NOR;
    pBandButton[0] = ui->narRb;
    pBandButton[1] = ui->normalRb;
    pBandButton[2] = ui->wideRb;

    // map passband radio button

    QSignalMapper *passBand_mapper = new QSignalMapper(this);

    for (int i = 0; i < 3; i++ )
    {
        passBand_mapper->setMapping(pBandButton[i], i);
        connect(pBandButton[i], SIGNAL(clicked()), passBand_mapper, SLOT(map()));

    }
    connect(passBand_mapper, SIGNAL(mapped(int)), this, SLOT(passBandRadioSelected(int)));


}


void RigControlFrame::memoryShortCutSelected(int buttonNumber)
{
    traceMsg(QString("Memory Button Selected = %1").arg(QString::number(buttonNumber +1)));
    memButtons[buttonNumber]->showMenu();
}



void RigControlFrame::readActionSelected(int buttonNumber)
{
    traceMsg(QString("Memory Read Selected = %1").arg(QString::number(buttonNumber +1)));
    memoryData::memData m = memDialog->getMemoryData(buttonNumber);

    if (isRadioLoaded())
    {
        emit sendFreqControl(m.freq);
        sendModeToRadio(m.mode);
        if (!rxPBFlag)
        {
            sendPassBandStateToControl(m.pBandState);
        }
    }
    else
    {
        noRadioSendOutFreq(m.freq);
    }
    // send detail to rotator control frame, locator will give bearing
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();
    tslf->transferDetails(m.callsign, m.locator);
}

void RigControlFrame::writeActionSelected(int buttonNumber)
{
    traceMsg(QString("Memory Write Selected %1 = ").arg(QString::number(buttonNumber +1)));

    memDialog->setMemoryFlag(true);


    // get contest information
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();


    ScreenContact sc = tslf->getScreenEntry();
    logData.callsign = sc.cs.fullCall.getValue();
    logData.freq = curFreq;
    logData.locator = sc.loc.loc.getValue();
    logData.mode = curMode;
    logData.pBandState = curpbState;

    QStringList dt = dtg( true ).getIsoDTG().split('T');
    logData.time = dt[1];
    // time now, other formats
    // are available QString qth = sc.extraText;

    //logData.bearing = sc.bearing;

    logData.bearing = tslf->getBearingFrmQSOLog();
    // load log data into memory
    memDialog->setLogData(&logData, buttonNumber);
    memDialog->setDialogTitle(QString("M%1 - Write").arg(QString::number(buttonNumber + 1)));
    memDialog->setFocusCallsign();
    memDialog->exec();
}


void RigControlFrame::editActionSelected(int buttonNumber)
{
    traceMsg(QString("Memory Edit Selected = %1").arg(QString::number(buttonNumber + 1)));
    if (memDialog->getMemoryFlag())
       return;

    memDialog->setMemoryFlag(true);

    memDialog->editMemory(buttonNumber);

    memDialog->setDialogTitle(QString("M%1 - Edit").arg(QString::number(buttonNumber + 1)));
    memDialog->setFocusCallsign();
    memDialog->exec();

}


void RigControlFrame::clearActionSelected(int buttonNumber)
{

    traceMsg(QString("Memory Clear Selected = %1").arg(QString::number(buttonNumber + 1)));
     memDialog->setMemoryFlag(true);

     logData.callsign = memDefData::DEFAULT_CALLSIGN;
     logData.freq = memDefData::DEFAULT_FREQ;
     logData.locator = memDefData::DEFAULT_LOCATOR;
     logData.mode = memDefData::DEFAULT_MODE;
     //logData.passBand = memDefData::DEFAULT_PBAND;
     logData.pBandState = memDefData::DEFAULT_PBAND_STATE;
     QStringList dt = dtg( true ).getIsoDTG().split('T');
     logData.time = dt[1];
     logData.bearing = 0;
     memDialog->clearMemory(&logData, buttonNumber);

     //memDialog->setDialogTitle(QString::number(buttonNumber + 1) + " - Clear");
     //memDialog->setFocusCallsign();
     //memDialog->show();

     if (memButtonMap.contains(buttonNumber))
     {
         RigMemoryButton *rmb = memButtonMap[buttonNumber];

         QGridLayout *gl = qobject_cast<QGridLayout *>(ui->scrollAreaWidgetContents->layout());

         int i = 0;
         QLayoutItem *child1;
         while(( child1 = gl->itemAt(i)) != 0)
         {
             if (child1->widget() == rmb->memButton)
             {
                 QLayoutItem *child2 = gl->takeAt(i);
                 delete child2->widget();
                 delete child2;
                 break;
             }
             i++;
         }
         memButtonMap.remove(buttonNumber);
         delete rmb;
     }
}



void RigControlFrame::passBandRadioSelected(int button)
{
    traceMsg("Memory Clear Selected");
    curpbState = button;
    emit sendPassBandStateToControl(button);
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

    traceMsg("Set mode, ui etc");
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
    traceMsg("Send Mode to Radio");
    emit sendModeToControl(m);
}



void RigControlFrame::setRadioName(QString n)
{
    traceMsg("Set RadioName");
    ui->radioName->setText(n);
    radioName = n;
    memDialog->setRadioName(n);
    runDialog->setRadioName(n);
    loadMemories();

}



void RigControlFrame::loadMemories()
{
    memDialog->readAllMemories();
    loadMemoryButtonLabels();
    runDialog->readAllMemories();
    loadRunButtonLabels();
}


void RigControlFrame::setRadioState(QString s)
{
    traceMsg("Set RadioState");
    if (s !="" && radioState != s)
    {
        ui->rigState->setText(s);
        radioState = s;
        memDialog->setRadioState(s);
    }

}


void RigControlFrame::freqLineEditInFocus()
{
    traceMsg("Freq LineEdit in Focus");
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


void RigControlFrame::setRxPBFlag(QString flag)
{
    bool fl = (flag == "set") ? true: false;

    rxPBFlag = fl;
    ui->narRb->setVisible(!fl);
    ui->normalRb->setVisible(!fl);
    ui->wideRb->setVisible(!fl);
    ui->groupBox->setVisible(!fl);
    memDialog->setRxPbFlag(fl);         // tell dialogs to hide Passband
    runDialog->setRxPbFlag(fl);
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

    if (!m.callsign.isEmpty())
    {
        int row = buttonNumber%2;
        int col = buttonNumber/2;
        memButtonMap[buttonNumber] = new RigMemoryButton(ui->scrollArea, this, buttonNumber);
        connect( memButtonMap[buttonNumber], SIGNAL( clearActionSelected(int)) , this, SLOT(clearActionSelected(int)), Qt::QueuedConnection );

        QGridLayout *gl = qobject_cast<QGridLayout *>(ui->scrollAreaWidgetContents->layout());
        gl->addWidget(memButtonMap[buttonNumber]->memButton, row, col);
    }

    if (memButtonMap.contains(buttonNumber))
    {

        QToolButton *mb = memButtonMap[buttonNumber]->memButton;

        mb->setText("M" + QString::number(buttonNumber + 1) + ": " + m.callsign + " ");
        QString tTipStr = "Callsign: " + m.callsign + "\n"
                + "Freq: " + m.freq + "\n"
                + "Mode: " + m.mode + "\n"
                + "Passband: " + hamlibData::pBandStateStr[m.pBandState] + "\n"
                + "Locator: " + m.locator + "\n"
                + "Bearing: " + QString::number(m.bearing) + "\n"
                + "Time: " + m.time;
        mb->setToolTip(tTipStr);
    }
}



void RigControlFrame::traceMsg(QString msg)
{
    trace("Rigcontrol: " + msg);
}


//********************** Run Buttons *******************************


void RigControlFrame::initRunMemoryButton(QWidget *parent)
{

        runButton[0] = ui->RunButton1;
        runButton[1] = ui->RunButton2;

        for (int i = 0; i < runButData::NUM_RUNBUTTONS; i++)
        {
            runButShortKey[i] = new QShortcut(runButShortCut[i], parent);
            runButton[i]->setToolButtonStyle(Qt::ToolButtonTextOnly);
            runButton[i]->setPopupMode(QToolButton::InstantPopup);
            runButton[i]->setText(runButData::runButTitle[i]);
            runButMenu[i] = new QMenu(runButton[i]);

            runButReadAct[i] = new QAction("&Read", this);
            runButWriteAct[i] = new QAction("&Write",this);
            runButEditAct[i] = new QAction("&Edit", this);
            runButClearAct[i] = new QAction("&Clear",this);
            runButMenu[i]->addAction(runButReadAct[i]);
            runButMenu[i]->addAction(runButWriteAct[i]);
            runButMenu[i]->addAction(runButEditAct[i]);
            runButMenu[i]->addAction(runButClearAct[i]);
            runButton[i]->setMenu(runButMenu[i]);
        }


        // map shortcut keys
       QSignalMapper *runButShortKey_mapper = new QSignalMapper(this);

        for (int i = 0; i < runButData::NUM_RUNBUTTONS; i++ )
        {
            runButShortKey_mapper->setMapping(runButShortKey[i], i);
            connect(runButShortKey[i], SIGNAL(activated()), runButShortKey_mapper, SLOT(map()));

        }
        connect(runButShortKey_mapper, SIGNAL(mapped(int)), this, SLOT(runButShortCutSel(int)));

    //----------------------------------------------------------------------------------------

        // map read Action

        QSignalMapper *runButReadAct_mapper = new QSignalMapper(this);

        for (int i = 0; i < runButData::NUM_RUNBUTTONS; i++ )
        {
            runButReadAct_mapper->setMapping(runButReadAct[i], i);
            connect(runButReadAct[i], SIGNAL(triggered()), runButReadAct_mapper, SLOT(map()));

        }
        connect(runButReadAct_mapper, SIGNAL(mapped(int)), this, SLOT(runButReadActSel(int)));

    //----------------------------------------------------------------------------------------

        // map write Action

        QSignalMapper *runButWriteAct_mapper = new QSignalMapper(this);

        for (int i = 0; i < runButData::NUM_RUNBUTTONS; i++ )
        {
            runButWriteAct_mapper->setMapping(runButWriteAct[i], i);
            connect(runButWriteAct[i], SIGNAL(triggered()), runButWriteAct_mapper, SLOT(map()));

        }
        connect(runButWriteAct_mapper, SIGNAL(mapped(int)), this, SLOT(runButWriteActSel(int)));

    //-----------------------------------------------------------------------------------------

        // map edit Action

        QSignalMapper *runButEditAct_mapper = new QSignalMapper(this);

        for (int i = 0; i < runButData::NUM_RUNBUTTONS; i++ )
        {
            runButEditAct_mapper->setMapping(runButEditAct[i], i);
            connect(runButEditAct[i], SIGNAL(triggered()), runButEditAct_mapper, SLOT(map()));

        }
        connect(runButEditAct_mapper, SIGNAL(mapped(int)), this, SLOT(runButEditActSel(int)));

    //-----------------------------------------------------------------------------------------

        // map Clear Action

        QSignalMapper *runButClearAct_mapper = new QSignalMapper(this);

        for (int i = 0; i < runButData::NUM_RUNBUTTONS; i++ )
        {
            runButClearAct_mapper->setMapping(runButClearAct[i], i);
            connect(runButClearAct[i], SIGNAL(triggered()), runButClearAct_mapper, SLOT(map()));

        }
        connect(runButClearAct_mapper, SIGNAL(mapped(int)), this, SLOT(runButClearActSel(int)));



}



void RigControlFrame::runButShortCutSel(int buttonNumber)
{
    traceMsg(QString("Run Button Selected = %1").arg(QString::number(buttonNumber + 1)));
    runButton[buttonNumber]->showMenu();
}

void RigControlFrame::runButReadActSel(int buttonNumber)
{
    traceMsg(QString("Run Button Read Selected = %1").arg(QString::number(buttonNumber + 1)));
    memoryData::memData m = runDialog->getMemoryData(buttonNumber);
    if (isRadioLoaded())
    {
        emit sendFreqControl(m.freq);
        sendModeToRadio(m.mode);
        if (!rxPBFlag)
        {
            sendPassBandStateToControl(m.pBandState);
        }
    }
    else
    {
        noRadioSendOutFreq(m.freq);
    }

}

void RigControlFrame::runButWriteActSel(int buttonNumber)
{
    traceMsg(QString("Memory Write Selected %1 = ").arg(QString::number(buttonNumber + 1)));
    runData.callsign = "Run" + QString::number(buttonNumber + 1);
    runData.freq = curFreq;
    runData.locator = "";
    runData.mode = curMode;
    runData.pBandState = curpbState;
    runData.bearing = COMPASS_ERROR;
    runData.time = "00:00";
    // load run data into run memory
    runDialog->setLogData(&runData, buttonNumber);
    runDialog->setDialogTitle(QString("Run%1 - Write").arg(QString::number(buttonNumber + 1)));
    runDialog->setFocus();
    runDialog->exec();

}

void RigControlFrame::runButEditActSel(int buttonNumber)
{
    traceMsg(QString("Run Button Edit Selected = %1").arg(QString::number(buttonNumber + 1)));
    runDialog->editMemory(buttonNumber);
    runDialog->setDialogTitle(QString("Run%1 - Edit").arg(QString::number(buttonNumber + 1)));
    runDialog->setFocusFreq();
    runDialog->exec();
}

void RigControlFrame::runButClearActSel(int buttonNumber)
{
    traceMsg(QString("Run Button Clear Selected = %1").arg(QString::number(buttonNumber + 1)));

    runData.callsign = memDefData::DEFAULT_CALLSIGN;
    runData.freq = memDefData::DEFAULT_FREQ;
    runData.locator = memDefData::DEFAULT_LOCATOR;
    runData.mode = memDefData::DEFAULT_MODE;
    //logData.passBand = memDefData::DEFAULT_PBAND;
    runData.pBandState = memDefData::DEFAULT_PBAND_STATE;
    runData.time = "00:00";
    runData.bearing = 0;
    runDialog->clearMemory(&runData, buttonNumber);
}


void RigControlFrame::loadRunButtonLabels()
{
    for (int i = 0; i < runButData::NUM_RUNBUTTONS; i++)
    {
        runButtonUpdate(i);
    }
}


void RigControlFrame::runButtonUpdate(int buttonNumber)
{
    memoryData::memData m = runDialog->getMemoryData(buttonNumber);
    runButton[buttonNumber]->setText("R" + QString::number(buttonNumber + 1) + ": " + extractKhz(m.freq) + " ");
    QString tTipStr = "Freq: " + m.freq + "\n"
            + "Mode: " + m.mode + "\n"
            + "Passband: " + hamlibData::pBandStateStr[m.pBandState];
    runButton[buttonNumber]->setToolTip(tTipStr);
}

QString RigControlFrame::extractKhz(QString f)
{
    QString khz = "000";

    if (f.contains('.'))
    {
        QStringList k = f.split('.');
        int i = k.length();
        if (i >=2)
        {
            return k[i-2];
        }
    }
    else
    {
        int i = f.length();
        if (i >= 6)
        {
            khz = f.mid(i - 6, 3);
            return khz;
        }
    }

    return khz;
}




//********************************************//

FreqLineEdit::FreqLineEdit(QWidget *parent):
    QLineEdit(parent)
{

}


FreqLineEdit::~FreqLineEdit()
{


}

void FreqLineEdit::focusInEvent( QFocusEvent * /*ev*/ )
{
    emit receivedFocus() ;

}

void RigControlFrame::on_newMemoryButton_clicked()
{
    int n = -1;

    for (int i = 0; i < memButtonMap.size() + 1; i++)
    {
        if  (!memButtonMap.contains(i))
        {
            n = i;
            break;
        }
    }
    if (n == -1)
    {
        mShowMessage("Panic", this);
        return;
    }
    if (n >= memoryData::NUM_MEMORIES)
    {
        mShowMessage("Too many memories defined", this);
        return;
    }

    writeActionSelected(n); // which creates the button as well

}
RigMemoryButton::RigMemoryButton(QWidget *parent, RigControlFrame *rcf, int no)
{
    memNo = no;
    rigControlFrame = rcf;

    memButton = new QToolButton(parent);

    memoryMenu = new QMenu(memButton);

    memButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    memButton->setPopupMode(QToolButton::MenuButtonPopup);
    memButton->setFocusPolicy(Qt::NoFocus);
    memButton->setText(memoryData::memoryTitle[memNo] + memoryData::memTitleBlank);

    shortKey = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_1 + memNo), memButton);
    readAction = new QAction("&Read", memButton);
    writeAction = new QAction("&Write",memButton);
    editAction = new QAction("&Edit", memButton);
    clearAction = new QAction("&Clear",memButton);
    memoryMenu->addAction(readAction);
    memoryMenu->addAction(writeAction);
    memoryMenu->addAction(editAction);
    memoryMenu->addAction(clearAction);
    memButton->setMenu(memoryMenu);

    connect(memButton, SIGNAL(clicked(bool)), this, SLOT(readActionSelected()));
    connect( readAction, SIGNAL( triggered() ), this, SLOT(readActionSelected()) );
    connect( writeAction, SIGNAL( triggered() ), this, SLOT(writeActionSelected()) );
    connect( editAction, SIGNAL( triggered() ), this, SLOT(editActionSelected()) );
    connect( clearAction, SIGNAL( triggered() ), this, SLOT(clearActionSelected()) );
}
RigMemoryButton::~RigMemoryButton()
{
//    delete memButton;
}
void RigMemoryButton::memoryUpdate()
{
    rigControlFrame->memoryUpdate(memNo);
}

void RigMemoryButton::memoryShortCutSelected()
{
//    rigControlFrame->memoryShortCutSelected(memNo);
    memButton->showMenu();
}
void RigMemoryButton::readActionSelected()
{
    rigControlFrame->readActionSelected(memNo);
}
void RigMemoryButton::editActionSelected()
{
    rigControlFrame->editActionSelected(memNo);
}
void RigMemoryButton::writeActionSelected()
{
    rigControlFrame->writeActionSelected(memNo);
}
void RigMemoryButton::clearActionSelected()
{
    emit clearActionSelected(memNo);
}
