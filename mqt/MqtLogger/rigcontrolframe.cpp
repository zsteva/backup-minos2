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

static QKeySequence memoryShiftShortCut[] = {

    QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_1),
    QKeySequence(Qt::CTRL + Qt::SHIFT +  Qt::Key_2),
    QKeySequence(Qt::CTRL + Qt::SHIFT  + Qt::Key_3),
    QKeySequence(Qt::CTRL + Qt::SHIFT  + Qt::Key_4),
    QKeySequence(Qt::CTRL + Qt::SHIFT  + Qt::Key_5),
    QKeySequence(Qt::CTRL + Qt::SHIFT  + Qt::Key_6),
    QKeySequence(Qt::CTRL + Qt::SHIFT  + Qt::Key_7),
    QKeySequence(Qt::CTRL + Qt::SHIFT  + Qt::Key_8),
    QKeySequence(Qt::CTRL + Qt::SHIFT  + Qt::Key_9),
    QKeySequence(Qt::CTRL + Qt::SHIFT  + Qt::Key_0)

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

    ui->freqInput->installEventFilter(this  );
    initRigFrame(parent);

    initRunMemoryButton();
    initPassBandRadioButtons();

    setRxPBFlag("set");
    mgmLabelVisible(false);

    // init memory button data before radio connection
    setRadioName(radioName);

    freqEditShortKey = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), parent);
    connect(freqEditShortKey, SIGNAL(activated()), this, SLOT(freqEditSelected()));

    traceMsg("RigControl Frame Started");
}

RigControlFrame::~RigControlFrame()
{
    delete ui;

}

void RigControlFrame::setContest(BaseContestLog *c)
{
    ct = dynamic_cast<LoggerContestLog *>( c);

    loadMemories();                 // mem and run should be independant of the radio!
}

void RigControlFrame::initRigFrame(QWidget */*parent*/)
{

    ui->modelbl->setText(MODE_ERROR);
    ui->normalRb->setChecked(true);
    //connect(ui->freqInput, SIGNAL(lostFocus()), this, SLOT(exitFreqEdit()));
    connect(ui->freqInput, SIGNAL(returnPressed()), this, SLOT(returnChangeRadioFreq()));
    connect(ui->freqInput, SIGNAL(newFreq()), this, SLOT(changeRadioFreq()));

    // when no radio is connected
    connect(this, SIGNAL(noRadioSendFreq(QString)), this, SLOT(noRadioSetFreq(QString)));

    for (int i = 0; i < bandSelData::bandNames.count(); i++)
    {
        ui->bandSelCombo->addItem(bandSelData::bandNames[i]);
    }

    connect(ui->bandSelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(radioBandFreq(int)));

    setRxPBFlag("set");

    if (!isRadioLoaded())
    {
        ui->modelbl->setVisible(false);
    }
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
    static QString freq = "";

    QString newfreq = ui->freqInput->text().trimmed();
    if (newfreq != freq)
    {
        freq = newfreq;
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
}

void RigControlFrame::returnChangeRadioFreq()
{
    changeRadioFreq();
    //exitFreqEdit();
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

bool RigControlFrame::eventFilter(QObject *obj, QEvent *event)
{
   Q_UNUSED(obj)


   if (event->type() == QEvent::FocusIn)
      freqLineEditInFocus();
   else if (event->type() == QEvent::FocusOut)
      exitFreqEdit();

   return false;
}

void RigControlFrame::exitFreqEdit()
{
    traceMsg("Exit Edit Freq");
    freqEditOn = false;
    setFreq(curFreq);
    freqLineEditFrameColour(false);
    ui->freqInput->clearFocus();
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



void RigControlFrame::freqEditSelected()
{
    qDebug() << "Freq Edit Selected";
}


void RigControlFrame::readActionSelected(int buttonNumber)
{
    traceMsg(QString("Memory Read Selected = %1").arg(QString::number(buttonNumber +1)));
    memoryData::memData m = getRigMemoryData(buttonNumber);

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

    //memDialog->setMemoryFlag(true);


    // get contest information
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();


    ScreenContact sc = tslf->getScreenEntry();

    memoryData::memData logData;
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

    RigMemDialog memDialog(this);
    memDialog.setLogData(&logData, buttonNumber);
    memDialog.setWindowTitle(QString("M%1 - Write").arg(QString::number(buttonNumber + 1)));
   if ( memDialog.exec() == QDialog::Accepted)
   {
       setRigMemoryData(buttonNumber, logData);
       memoryUpdate(buttonNumber);
   }
}


void RigControlFrame::editActionSelected(int buttonNumber)
{
    traceMsg(QString("Memory Edit Selected = %1").arg(QString::number(buttonNumber + 1)));

    memoryData::memData logData = getRigMemoryData(buttonNumber);

    RigMemDialog memDialog(this);

    memDialog.setLogData(&logData, buttonNumber);
    memDialog.setWindowTitle(QString("M%1 - Edit").arg(QString::number(buttonNumber + 1)));

    if ( memDialog.exec() == QDialog::Accepted)
    {
        setRigMemoryData(buttonNumber, logData);
        memoryUpdate(buttonNumber);
    }

}


void RigControlFrame::clearActionSelected(int buttonNumber)
{

    traceMsg(QString("Memory Clear Selected = %1").arg(QString::number(buttonNumber + 1)));

    memoryData::memData m;
    setRigMemoryData(buttonNumber, m);

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

void RigControlFrame::setMode(QString m)
{

    traceMsg("Set mode, ui etc");
    QStringList mode = m.split(':');
    if (mode.length() == 2 )
    {
        for (int i = 0; i < hamlibData::supModeList.count(); i++)
        {
                if (mode[0] == hamlibData::supModeList[i])
                {
                    ui->modelbl->setText(mode[0]);
                    curMode = mode[0];
                    if (mode[0] == hamlibData::MGM)
                    {
                        mgmLabelVisible(true);
                        ui->mgmLbl->setText(mode[1]);
                    }
                    else
                    {
                       mgmLabelVisible(false);
                    }
                   return;
                }
        }


        // mode not supported by minos
        ui->modelbl->setText(MODE_ERROR);
    }


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
}



void RigControlFrame::loadMemories()
{
    loadMemoryButtonLabels();
    loadRunButtonLabels();
}


void RigControlFrame::setRadioState(QString s)
{
    traceMsg("Set RadioState");
    if (s !="" && radioState != s)
    {
        ui->rigState->setText(s);
        radioState = s;
    }

}


void RigControlFrame::freqLineEditInFocus()
{
    traceMsg("Freq LineEdit in Focus");
    freqEditOn = true;
    ui->freqInput->setReadOnly(false);
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
}

void RigControlFrame::loadMemoryButtonLabels()
{
    int mcount = ct->rigMemories.size();
    for (int i = 0; i < mcount; i ++)
    {
        memoryUpdate(i);
    }
}


void RigControlFrame::mgmLabelVisible(bool state)
{
    ui->mgmbreak->setVisible(state);
    ui->mgmLbl->setVisible(state);
}

void RigControlFrame::memoryUpdate(int buttonNumber)
{
    memoryData::memData m = getRigMemoryData(buttonNumber);

    if (!memButtonMap.contains(buttonNumber) && m.callsign != memDefData::DEFAULT_CALLSIGN)
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


void RigControlFrame::initRunMemoryButton()
{

    runButtonMap[0] = new RunMemoryButton(ui->RunButton1, this, 0);
    connect( runButtonMap[0], SIGNAL( clearActionSelected(int)) , this, SLOT(runButClearActSel(int)), Qt::QueuedConnection );

    runButtonMap[1] = new RunMemoryButton(ui->RunButton2, this, 1);
    connect( runButtonMap[1], SIGNAL( clearActionSelected(int)) , this, SLOT(runButClearActSel(int)), Qt::QueuedConnection );
}


void RigControlFrame::runButReadActSel(int buttonNumber)
{
    traceMsg(QString("Run Button Read Selected = %1").arg(QString::number(buttonNumber + 1)));
    memoryData::memData m = getRunMemoryData(buttonNumber);
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
    memoryData::memData runData;
    runData.callsign = "Run" + QString::number(buttonNumber + 1);
    runData.freq = curFreq;
    runData.locator = "";
    runData.mode = curMode;
    runData.pBandState = curpbState;
    runData.bearing = COMPASS_ERROR;
    runData.time = "00:00";
    // load run data into run memory

    RunButtonDialog runDialog(this);
    runDialog.setLogData(&runData, buttonNumber);
    if (runDialog.exec() == QDialog::Accepted)
    {
        setRunMemoryData(buttonNumber, runData);
        runButtonUpdate(buttonNumber);
    }

}

void RigControlFrame::runButEditActSel(int buttonNumber)
{
    memoryData::memData runData = getRunMemoryData(buttonNumber);

    traceMsg(QString("Run Button Edit Selected = %1").arg(QString::number(buttonNumber + 1)));
    RunButtonDialog runDialog(this);
    runDialog.setLogData(&runData, buttonNumber);
    if (runDialog.exec() == QDialog::Accepted)
    {
        setRunMemoryData(buttonNumber, runData);
        runButtonUpdate(buttonNumber);
    }
}

void RigControlFrame::runButClearActSel(int buttonNumber)
{
    traceMsg(QString("Run Button Clear Selected = %1").arg(QString::number(buttonNumber + 1)));

    memoryData::memData m;
    setRunMemoryData(buttonNumber, m);
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
    memoryData::memData m = getRunMemoryData(buttonNumber);
    QString sc = ((buttonNumber == 0)?QString(" [ "):QString( " ] "));

    runButtonMap[buttonNumber]->memButton->setText("R" + QString::number(buttonNumber + 1) + "(" + sc + ") " + extractKhz(m.freq) + " ");
    QString tTipStr = "Freq: " + m.freq + "\n"
            + "Mode: " + m.mode + "\n"
            + "Passband: " + hamlibData::pBandStateStr[m.pBandState];
    runButtonMap[buttonNumber]->memButton->setToolTip(tTipStr);
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

memoryData::memData RigControlFrame::getRigMemoryData(int memoryNumber)
{
    memoryData::memData m;
    if (ct->rigMemories.size() > memoryNumber)
        m = ct->rigMemories[memoryNumber].getValue();
    return m;
}
memoryData::memData RigControlFrame::getRunMemoryData(int memoryNumber)
{
    memoryData::memData m;

    if (ct->runMemories.size() > memoryNumber)
        m = ct->runMemories[memoryNumber].getValue();
    return m;
}
void RigControlFrame::setRigMemoryData(int memoryNumber, memoryData::memData m)
{
    ct->saveRigMemory(memoryNumber, m);
}
void RigControlFrame::setRunMemoryData(int memoryNumber, memoryData::memData m)
{
    ct->saveRunMemory(memoryNumber, m);
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
    memButton->setText("M" + QString::number(memNo + 1) );

    readAction = new QAction("&Read", memButton);
    writeAction = new QAction("&Write",memButton);
    editAction = new QAction("&Edit", memButton);
    clearAction = new QAction("&Clear",memButton);
    memoryMenu->addAction(readAction);
    memoryMenu->addAction(writeAction);
    memoryMenu->addAction(editAction);
    memoryMenu->addAction(clearAction);
    memButton->setMenu(memoryMenu);

    if (memNo  < 10)
    {
        shortKey = new QShortcut(QKeySequence(memoryShortCut[memNo]), memButton);
        connect(shortKey, SIGNAL(activated()), this, SLOT(readActionSelected()));
        // shift shortcut
        shiftShortKey = new QShortcut(QKeySequence(memoryShiftShortCut[memNo]), memButton);
        connect(shiftShortKey, SIGNAL(activated()), this, SLOT(memoryShortCutSelected()));
    }
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
RunMemoryButton::RunMemoryButton(QToolButton *b, RigControlFrame *rcf, int no)
{
    memNo = no;
    rigControlFrame = rcf;

    memButton = b;

    memoryMenu = new QMenu(memButton);

    memButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    memButton->setPopupMode(QToolButton::MenuButtonPopup);
    memButton->setFocusPolicy(Qt::NoFocus);
    memButton->setText(runButData::runButTitle[memNo]);

    shortKey = new QShortcut(QKeySequence(runButShortCut[memNo]), memButton);
    readAction = new QAction("&Read", memButton);
    writeAction = new QAction("&Write",memButton);
    editAction = new QAction("&Edit", memButton);
    clearAction = new QAction("&Clear",memButton);
    memoryMenu->addAction(readAction);
    memoryMenu->addAction(writeAction);
    memoryMenu->addAction(editAction);
    memoryMenu->addAction(clearAction);
    memButton->setMenu(memoryMenu);

    connect(shortKey, SIGNAL(activated()), this, SLOT(memoryShortCutSelected()));
    connect(memButton, SIGNAL(clicked(bool)), this, SLOT(readActionSelected()));
    connect( readAction, SIGNAL( triggered() ), this, SLOT(readActionSelected()) );
    connect( writeAction, SIGNAL( triggered() ), this, SLOT(writeActionSelected()) );
    connect( editAction, SIGNAL( triggered() ), this, SLOT(editActionSelected()) );
    connect( clearAction, SIGNAL( triggered() ), this, SLOT(clearActionSelected()) );
}
RunMemoryButton::~RunMemoryButton()
{
//    delete memButton;
}
void RunMemoryButton::memoryUpdate()
{
    rigControlFrame->runButtonUpdate(memNo);
}

void RunMemoryButton::memoryShortCutSelected()
{
//    rigControlFrame->memoryShortCutSelected(memNo);
    memButton->showMenu();
    //emit lostFocus();
}
void RunMemoryButton::readActionSelected()
{
    rigControlFrame->runButReadActSel(memNo);
}
void RunMemoryButton::editActionSelected()
{
    rigControlFrame->runButEditActSel(memNo);
}
void RunMemoryButton::writeActionSelected()
{
    rigControlFrame->runButWriteActSel(memNo);
}
void RunMemoryButton::clearActionSelected()
{
    emit clearActionSelected(memNo);
}
//********************************************//

FreqLineEdit::FreqLineEdit(QWidget *parent):
    QLineEdit(parent)
{

}


FreqLineEdit::~FreqLineEdit()
{


}

void FreqLineEdit::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numTicks = numDegrees / 15;

    if (numTicks == 1)
    {
       changeFreq(true);
    }
    else
    {
        changeFreq(false);
    }

    event->accept();
}


void FreqLineEdit::keyPressEvent(QKeyEvent *event)
{

    if(event->key() == Qt::Key_Up)
    {
        changeFreq(true);
    }
    else if(event->key() == Qt::Key_Down)
    {
        changeFreq(false);

    }
    else
    {
        // default handler for event
        QLineEdit::keyPressEvent(event);
    }
}



void FreqLineEdit::changeFreq(bool direction)
{
    static const double tuningData[][14] =
                                        {
                                        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {10, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {100, 10, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {1000, 0, 100, 10, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {10000, 1000, 0, 100, 10, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {100000, 10000, 1000, 0, 100, 10, 1, 0, 0, 0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {1000000, 0, 100000, 10000, 1000, 0, 100, 10, 1, 0, 0, 0, 0, 0},
                                        {10000000, 1000000, 0, 100000, 10000, 1000, 0, 100, 10, 1, 0, 0, 0, 0},
                                        {100000000, 10000000, 1000000, 0, 100000, 10000, 1000, 0, 100, 10, 1, 0, 0, 0},
                                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {1000000000, 0, 100000000, 10000000, 1000000, 0, 100000, 10000, 1000, 0, 100, 10, 1, 0},
                                        {10000000000, 1000000000, 0, 100000000, 10000000, 1000000, 0, 100000, 10000, 1000, 0, 100, 10, 1}
                                        };



    bool ok = false;
    QString sfreq = text();
    int sfreqLen = sfreq.length();
    int pos = cursorPosition();
    if (sfreqLen < 0 || sfreqLen > 13 || pos < 0 || pos > sfreqLen)
    {
        return;
    }
    const double tuneStep = tuningData[sfreqLen - 1][pos];


    sfreq = sfreq.trimmed().remove('.');
    double freq = sfreq.toDouble(&ok);

    if (!ok)
    {
        return;

    }

    if (direction)
    {
        freq = freq + tuneStep;
    }
    else
    {
        freq = freq - tuneStep;
    }


    sfreq = convertFreqString(freq);
    setText(sfreq);
    setCursorPosition(pos);

    emit newFreq();

}

QString FreqLineEdit::convertFreqString(double frequency)
{
    QString sfreq;
    double freq = frequency;
    sfreq = QString::number(freq,'f', 0);
    int len = sfreq.length();


    switch(len)
    {
        case 11:
            sfreq = sfreq.insert(8, '.');
            sfreq = sfreq.insert(5, '.');
            sfreq = sfreq.insert(2, '.');
            break;
        case 10:
            sfreq = sfreq.insert(7, '.');
            sfreq = sfreq.insert(4, '.');
            sfreq = sfreq.insert(1, '.');
            break;
        case 9:
            sfreq = sfreq.insert(3, '.');
            sfreq = sfreq.insert(7, '.');
            break;
        case 8:
            sfreq = sfreq.insert(2, '.');
            sfreq = sfreq.insert(6, '.');
            break;
        case 7:
            sfreq = sfreq.insert(4, '.');
            sfreq = sfreq.insert(1, '.');
            break;
        case 6:
            sfreq = sfreq.insert(3,'.');
            break;
        case 5:
            sfreq = sfreq.insert(2,'.');
            break;
        case 4:
            sfreq = sfreq.insert(1,'.');
            break;
        default:
            sfreq = "??.???.???.???";    // error

    }


    return sfreq;
}
