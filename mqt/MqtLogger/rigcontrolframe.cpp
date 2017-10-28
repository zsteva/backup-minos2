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
#include "tlogcontainer.h"
#include "tsinglelogframe.h"
#include "SendRPCDM.h"
#include "runbuttondialog.h"
#include "BandList.h"

#include "rigcontrolframe.h"
#include "ui_rigcontrolframe.h"

#define MODE_ERROR "<font color='Red'>Mode Error</font>"


static QKeySequence runButShortCut[] {
    QKeySequence(Qt::CTRL + Qt::Key_BracketLeft),
    QKeySequence(Qt::CTRL + Qt::Key_BracketRight)

};

static QKeySequence runButShiftShortCut[] {
    QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_BracketLeft),
    QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_BracketRight)

};



RigControlFrame::RigControlFrame(QWidget *parent):
    QFrame(parent)
    , ui(new Ui::RigControlFrame)
    , curFreq(memDefData::DEFAULT_FREQ)
    , curMode(memDefData::DEFAULT_MODE)
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

    //setRxPBFlag("set");
    mgmLabelVisible(false);

    // init memory button data before radio connection
    setRadioName(radioName);

    freqEditShortKey = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), parent);
    connect(freqEditShortKey, SIGNAL(activated()), this, SLOT(freqEditSelected()));

    freqPlusShortCut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_U), parent);
    connect(freqPlusShortCut, SIGNAL(activated()), this, SLOT(freqPlus_ShortCut()));
    connect(ui->freqUp, SIGNAL(clicked(bool)), this, SLOT(freqPlusShortCut_clicked(bool)));

    freqNegShortCut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_D), parent);
    connect(freqNegShortCut, SIGNAL(activated()), this, SLOT(freqNeg_ShortCut()));
    connect(ui->freqDown, SIGNAL(clicked(bool)), this, SLOT(freqNegShortCut_clicked(bool)));


    connect(&MinosLoggerEvents::mle, SIGNAL(FontChanged()), this, SLOT(on_FontChanged()), Qt::QueuedConnection);

    on_FontChanged();
    traceMsg("RigControl Frame Started");
}

RigControlFrame::~RigControlFrame()
{
    delete ui;

}
void RigControlFrame::on_FontChanged()
{
    QFont cf = QApplication::font();
    qreal fs = cf.pointSizeF();
    int fsi = fs * 1.25;
    cf.setPointSize(fsi);
    cf.setBold(true);
    ui->freqInput->setFont(cf);
}

void RigControlFrame::setContest(BaseContestLog *c)
{
    ct = dynamic_cast<LoggerContestLog *>( c);

    loadMemories();                 // mem and run should be independant of the radio!
}

void RigControlFrame::initRigFrame(QWidget * /*parent*/)
{

    ui->modelbl->setText(MODE_ERROR);
    //ui->normalRb->setChecked(true);
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

    //setRxPBFlag("set");

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
        if (checkValidFreq(freq))
        {
            if (freq.count() >=4)
            {
                if (isRadioLoaded())
                {
                    sendFreq(freq);

                }
                else
                {
                    noRadioSendOutFreq(freq);
                }

            }

        }
        else
        {
            ui->freqInput->setText(QString("%1 %2 %3").arg("<font color='Red'>", freq, "</font>"));
        }

    }
}


bool RigControlFrame::checkValidFreq(QString freq)
{
    bool ok = false;
    BandList &blist = BandList::getBandList();
    BandInfo bi;
    bool bandOK = false;
    QString sfreq = freq.trimmed().remove('.');

    double dfreq = sfreq.toDouble(&ok);

    if (ok)
    {
        bandOK = blist.findBand(dfreq, bi);
    }
    return bandOK;
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
            sendFreq(bandSelData::bandFreq[index]);
        }
        else
        {
            noRadioSendOutFreq(bandSelData::bandFreq[index]);
        }
    }
}

void RigControlFrame::sendFreq(QString f)
{

    bool ok = false;
    QString sf = f.remove('.');
    if (curFreq.remove('.') != sf)
    {
        double df = sf.toDouble(&ok);
        if (ok && df > 0.0)
        {
            emit sendFreqControl(f);
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
    //curpbState = hamlibData::NOR;
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
    ui->freqInput->setFocus();
    int len = ui->freqInput->text().length();
    if (len > 5)
    {
       ui->freqInput->setCursorPosition(len - 5);
    }


}


// this is the routine called from read memory

void RigControlFrame::transferDetails(memoryData::memData &m)
{
    if (isRadioLoaded())
    {
        if (m.freq.remove('.') != curFreq.remove('.'))
        {
            sendFreq(m.freq);
        }

        if (m.mode != curMode)
        {
            sendModeToRadio(m.mode);
        }


        if (m.pBandState != curpbState[calcMinosMode(m.mode)])
        {
            sendPassBandStateToControl(m.pBandState);
        }


    }
    else
    {
        noRadioSendOutFreq(m.freq);
    }
}


void RigControlFrame::getDetails(memoryData::memData &logData)
{
    // get contest information
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();
    ScreenContact sc = tslf->getScreenEntry();

    logData.callsign = sc.cs.fullCall.getValue();
    logData.freq = curFreq;
    logData.locator = sc.loc.loc.getValue();
    logData.mode = curMode;
    logData.pBandState = curpbState[calcMinosMode(curMode)];

    QStringList dt = dtg( true ).getIsoDTG().split('T');
    logData.time = dt[1];
    // time now, other formats
    // are available QString qth = sc.extraText;

    //logData.bearing = sc.bearing;

    logData.bearing = tslf->getBearingFrmQSOLog();
    // load log data into memory
}

void RigControlFrame::passBandRadioSelected(int button)
{
    traceMsg("Memory Clear Selected");
    //curpbState = button;
    emit sendPassBandStateToControl(button);
}

void RigControlFrame::setMode(QString m)
{

    traceMsg(QString("Set mode, ui etc = %1").arg(m));
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
    traceMsg(QString("Send Mode to Radio - Mode = %1").arg(m));
    emit sendModeToControl(QString("%1").arg(m));

}



void RigControlFrame::setRadioName(QString n)
{
    traceMsg("Set RadioName");
    ui->radioName->setText(n);
    radioName = n;
}



void RigControlFrame::loadMemories()
{
    loadRunButtonLabels();
}


void RigControlFrame::setRadioState(QString s)
{
    traceMsg(QString("Set RadioState = %1").arg(s));

    if (s != "")
    {
        ui->rigState->setText(s);
        radioState = s;
    }
}


void RigControlFrame::setRadioPassbandState(QString s)
{
    traceMsg(QString("Set RadioPassbandState = %1").arg(s));
    if (s != "")
    {
        for (int i = 0; i < hamlibData::pBandStateStr.count(); i++)
        {
            if (s == hamlibData::pBandStateStr[i])
            {
                pBandButton[i]->setChecked(true);
                storePassBandState(s);
                return;
            }
        }
    }
}


void RigControlFrame::storePassBandState(QString state)
{
    int iMode = calcMinosMode(curMode);
    int iState = calcMinosPBState(state);
    if (iMode < 0 || iState < 0)
    {
        return;
    }
    curpbState[iMode] = iState;
    scurpbState[iMode] = state;
}

int RigControlFrame::getIntPassBandState(QString mode)
{
    int iMode = calcMinosMode(mode);
    if (iMode > 0)
    {
        return curpbState[iMode];
    }

    return iMode;

}

QString RigControlFrame::getStrPassBandState(QString mode)
{
    int iMode = calcMinosMode(mode);
    if (iMode > 0)
    {
        return scurpbState[iMode];
    }

    return "";
}

int RigControlFrame::calcMinosMode(QString mode)
{
    int iMode = -1;
    for (int i = 0; i < hamlibData::supModeList.count(); i++ )
    {
        if (mode == hamlibData::supModeList[i])
        {
            iMode = i;
            return iMode;
        }
    }
    return iMode;
}


int RigControlFrame::calcMinosPBState(QString state)
{
    int iState = -1;
    for (int i = 0; i < hamlibData::pBandStateStr.count(); i++)
    {
        if (state == hamlibData::pBandStateStr[i])
        {
            iState = i;
            return iState;
        }
    }

    return iState;
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

void RigControlFrame::freqPlus_ShortCut()
{
    freqPlusShortCut_clicked(true);
}

void RigControlFrame::freqNeg_ShortCut()
{
    freqNegShortCut_clicked(true);
}


void RigControlFrame::freqPlusShortCut_clicked(bool /*click*/)
{
    QString freq = calcNewFreq(5000.0);
    if (freq != "")
    {
        emit sendFreqControl(freq);
    }

}



void RigControlFrame::freqNegShortCut_clicked(bool /*click*/)
{

   QString freq = calcNewFreq(-5000.0);
   if (freq != "")
   {
       ui->freqInput->setText(freq);
       emit sendFreqControl(freq);
   }

}


QString RigControlFrame::calcNewFreq(double incFreq)
{

    BandList &blist = BandList::getBandList();
    BandInfo bi;
    bool ok = false;
    bool bandOk = false;
    QString sfreq = "";
    sfreq = curFreq.trimmed().remove('.');

    double freq = sfreq.toDouble(&ok);
    if (ok)
    {
        freq += incFreq;
        bandOk = blist.findBand(freq, bi);
        if (!bandOk)
        {
            freq -= incFreq;
        }
        else
        {
            sfreq = ui->freqInput->convertFreqString(freq);
            trace(QString("CalcNewFreq: Freq  = %1").arg(sfreq));

        }
    }

    return sfreq;

}




void RigControlFrame::mgmLabelVisible(bool state)
{
    ui->mgmbreak->setVisible(state);
    ui->mgmLbl->setVisible(state);
}

void RigControlFrame::traceMsg(QString msg)
{
    trace("Rigcontrol: " + msg);
}


//********************** Run Buttons *******************************


void RigControlFrame::initRunMemoryButton()
{
    memoryData::memData m;
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
        if (m.freq.remove('.') != curFreq.remove('.'))
        {
            sendFreq(m.freq);
        }


        if (m.mode != curMode)
        {
            sendModeToRadio(m.mode);
        }



        if (m.pBandState != curpbState[calcMinosMode(m.mode)])
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
    runData.pBandState = curpbState[calcMinosMode(curMode)];
    runData.bearing = COMPASS_ERROR;
    runData.time = "00:00";
    // load run data into run memory

    RunButtonDialog runDialog(this);
    runDialog.setWindowTitle(QString("Run %1 - Write").arg(QString::number(buttonNumber + 1)));
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
    runDialog.setWindowTitle(QString("Run %1 - Edit").arg(QString::number(buttonNumber + 1)));
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
    runButtonUpdate(buttonNumber);
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
    QString khz = "***";
    QString sf = f.remove('.');
    bool ok = false;
    double df = sf.toDouble(&ok);
    if (ok && df != 0.0)
    {
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
    }


    return khz;


}




memoryData::memData RigControlFrame::getRunMemoryData(int memoryNumber)
{
    memoryData::memData m;

    if (ct->runMemories.size() > memoryNumber)
        m = ct->runMemories[memoryNumber].getValue();
    return m;
}
void RigControlFrame::setRunMemoryData(int memoryNumber, memoryData::memData m)
{
    ct->saveRunMemory(memoryNumber, m);
}


//*******************Run Memory Button *************************//


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
    shiftShortKey = new QShortcut(QKeySequence(runButShiftShortCut[memNo]), memButton);
    readAction = new QAction("&Read", memButton);
    writeAction = new QAction("&Write",memButton);
    editAction = new QAction("&Edit", memButton);
    clearAction = new QAction("&Clear",memButton);
    memoryMenu->addAction(readAction);
    memoryMenu->addAction(writeAction);
    memoryMenu->addAction(editAction);
    memoryMenu->addAction(clearAction);
    memButton->setMenu(memoryMenu);

    connect(shortKey, SIGNAL(activated()), this, SLOT(readActionSelected()));
    connect(shiftShortKey, SIGNAL(activated()), this, SLOT(memoryShortCutSelected()));
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




//*******************Freq Line Edit *************************//

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


    BandList &blist = BandList::getBandList();
    BandInfo bi;
    bool bandOK = false;

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

    if (ok)
    {
        if (direction)
        {
            freq += tuneStep;
            bandOK = blist.findBand(freq, bi);
            if (!bandOK)
            {
                freq -= tuneStep;
            }
        }
        else
        {
            freq -= tuneStep;
            bandOK = blist.findBand(freq, bi);
            if (!bandOK)
            {
                freq += tuneStep;
            }
        }


        sfreq = convertFreqString(freq);
        trace(QString("Change Freq: Freq Tuning = %1").arg(freq));
        if (bandOK)
        {
            setText(sfreq);
            emit newFreq();
        }
        else
        {
            setText(QString("%1 %2 %3").arg("<font color='Red'>", sfreq, "</font>"));
        }

        setCursorPosition(pos);
   }
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
