#include <QGridLayout>
#include <QToolButton>

#include "logger_pch.h"
#include "tlogcontainer.h"
#include "tsinglelogframe.h"
#include "rigmemdialog.h"
#include "rigutils.h"
#include "RigMemoryFrame.h"
#include "ui_RigMemoryFrame.h"

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

void RigMemoryFrame::traceMsg(QString msg)
{
    trace("RigMemoryFrame: " + msg);
}

RigMemoryFrame::RigMemoryFrame(QWidget *parent) :
    QFrame(parent),
    ct(0),
    ui(new Ui::RigMemoryFrame)
{
    ui->setupUi(this);
    ui->memFrame->setStyleSheet("background-color:white;");
    connect(&MinosLoggerEvents::mle, SIGNAL(TimerDistribution()), this, SLOT(checkTimerTimer()));
    connect(&MinosLoggerEvents::mle, SIGNAL(RigFreqChanged(QString,BaseContestLog*)), this, SLOT(onRigFreqChanged(QString,BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(RotBearingChanged(int,BaseContestLog*)), this, SLOT(onRotBearingChanged(int,BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(AfterLogContact(BaseContestLog *)), this, SLOT(on_AfterLogContact(BaseContestLog *)));
}

RigMemoryFrame::~RigMemoryFrame()
{
    delete ui;
}
void RigMemoryFrame::setContest( BaseContestLog *pct )
{
    ct = dynamic_cast<LoggerContestLog *>( pct);
    on_AfterLogContact(ct);
    doMemoryUpdates();
}
memoryData::memData RigMemoryFrame::getRigMemoryData(int memoryNumber)
{
    memoryData::memData m;
    if (ct && ct->rigMemories.size() > memoryNumber)
    {
        m = ct->rigMemories[memoryNumber].getValue();
        Locator loc;
        loc.loc.setValue(m.locator);
        loc.validate();
        double lon = 0.0;
        double lat = 0.0;

        if ( lonlat( loc.loc.getValue(), lon, lat ) == LOC_OK )
        {
            double dist;
            int brg;
            ct->disbear( lon, lat, dist, brg );
            m.bearing = brg;
        }
    }
    return m;
}

bool freqSortcompare(const memoryData::memData &mem1, const memoryData::memData &mem2)
{
    if (mem1.freq != mem2.freq)
    {
        return mem1.freq.compare(mem2.freq, Qt::CaseInsensitive) < 0;
    }

    return mem1.callsign.compare(mem2.callsign, Qt::CaseInsensitive) < 0;
}
void RigMemoryFrame::setRigMemoryData(int memoryNumber, memoryData::memData m)
{
    ct->saveRigMemory(memoryNumber, m);
}

void RigMemoryFrame::doMemoryUpdates()
{
    // called from minosLoggerEvents following sendUpdateMemories

    // clear all the "old" buttons

    memButtonMap.clear();
    QGridLayout *gl = qobject_cast<QGridLayout *>(ui->memFrame->layout());

    delete gl;

    // and recreate the layout

    gl = new QGridLayout(ui->memFrame);
    ui->memFrame->setLayout(gl);

    // get all the info from the contest

    if (ct)
    {
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(false);

        QVector<memoryData::memData >sortedData;
        int mcount = ct->rigMemories.size();
        for (int i = 0; i < mcount; i++)
        {
            memoryData::memData m = getRigMemoryData(i);
            sortedData.push_back(m);
        }

        qSort(sortedData.begin(), sortedData.end(), freqSortcompare);

        int bpos = 0;
        for (int buttonNumber = 0; buttonNumber < mcount; buttonNumber ++)
        {
            memoryData::memData m = sortedData[buttonNumber];

            if ( m.callsign != memDefData::DEFAULT_CALLSIGN)
            {
                // create button

                int row = bpos/2;
                int col = bpos%2;
                bpos++;

                QSharedPointer<RigMemoryButton> rmbb(new RigMemoryButton(ui->scrollArea, this, m.memno));
                memButtonMap[m.memno] = rmbb;
                connect( rmbb.data(), SIGNAL( clearActionSelected(int)) , this, SLOT(clearActionSelected(int)), Qt::QueuedConnection );

                gl->addWidget(rmbb->memButton, row, col);

                // get the button to be a sensible size
                // This works - recipe from internet - but not sure why!

                rmbb->memButton->setSizePolicy(sizePolicy);

                rmbb->memButton->setText("M" + QString::number(m.memno + 1) + ": " + m.callsign);

                QString tTipStr = "Callsign: " + m.callsign + "\n"
                        + "Freq: " + convertFreqStrDisp(m.freq) + "\n"
                        + "Mode: " + m.mode + "\n"
                        + "Locator: " + m.locator + "\n"
                        + "Bearing: " + QString::number(m.bearing) + "\n"
                        + "Time: " + m.time;
                rmbb->memButton->setToolTip(tTipStr);
            }
        }
    }
}

void RigMemoryFrame::checkTimerTimer()
{
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();
    if (!ct || !tslf)
        return;

    memoryData::memData logData;
    tslf->getCurrentDetails(logData);

    double rigFreq = convertStrToFreq(logData.freq);
    int bearing = logData.bearing;


    int mcount = ct->rigMemories.size();
    for (int buttonNumber = 0; buttonNumber < mcount; buttonNumber ++)
    {
        QSharedPointer<RigMemoryButton> rmbb = memButtonMap[buttonNumber];
        if (!rmbb)
            continue;

        memoryData::memData m = getRigMemoryData(buttonNumber);
        if (m.callsign == memDefData::DEFAULT_CALLSIGN)
            continue;

        double memFreq = convertStrToFreq(m.freq);
        int memBearing = m.bearing;

        enum rTriState{rtsNotLoaded, rtsOn, rtsOff};
        rTriState onfreq = rtsNotLoaded;
        rTriState onbearing = rtsNotLoaded;

        if (tslf->isRadioLoaded())
        {
            if (abs(rigFreq - memFreq) < 2000.0)
            {
                onfreq = rtsOn;
            }
            else
            {
                onfreq = rtsOff;
            }
        }
        if (tslf->isRotatorLoaded())
        {
            if (abs(bearing - memBearing) < 15)
                onbearing = rtsOn;
            else
                onbearing = rtsOff;
        }

        QString bgcolour = "background-color: ";
        if (m.worked)
        {
            bgcolour += "#AFAFAF";
        }
        else
        {
            bgcolour += "#DFDFDF";
        }

        if (onfreq == rtsOn || onbearing == rtsOn)
        {
            if (onfreq == rtsOn && onbearing == rtsOn)
                rmbb->memButton-> setStyleSheet("border: 1px solid green; color: green ;" + bgcolour);
            else if (onfreq == rtsOn)
                rmbb->memButton-> setStyleSheet("border: 1px solid red; color: red ;" + bgcolour);
            else
                rmbb->memButton-> setStyleSheet("border: 1px solid blue; color: blue ;" + bgcolour);
        }
        else
        {
            rmbb->memButton-> setStyleSheet("border: 1px solid black; color: black ;" + bgcolour);
        }
    }
}
void RigMemoryFrame::onRigFreqChanged(QString /*f*/, BaseContestLog *c)
{
    if (ct == c)
    {
        checkTimerTimer();
    }
}
void RigMemoryFrame::onRotBearingChanged(int/*f*/, BaseContestLog *c)
{
    if (ct == c)
    {
        checkTimerTimer();
    }
}
//======================================================================================
void RigMemoryFrame::sendUpdateMemories()
{
    // go through the signal/slot mechanism so all auxiliary displays are updated
    MinosLoggerEvents::sendUpdateMemories(ct);
}
//======================================================================================

// responses to button actions

void RigMemoryFrame::on_newMemoryButton_clicked()
{
    int n = -1;

    for (int i = 0; i < memButtonMap.size() + 1; i++)
    {
        if  (!memButtonMap.contains(i) || !memButtonMap[i])
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
void RigMemoryFrame::on_flushMemoriesButton_clicked()
{
    // scan memories and see if they have been worked
    int mcount = ct->rigMemories.size();
    for (int buttonNumber = 0; buttonNumber < mcount; buttonNumber ++)
    {
        memoryData::memData m = getRigMemoryData(buttonNumber);

        if ( m.callsign != memDefData::DEFAULT_CALLSIGN)
        {
            Callsign mcs(m.callsign);
            mcs.validate();

            for ( LogIterator i = ct->ctList.begin(); i != ct->ctList.end(); i++ )
            {
                if ((*i).wt->cs == mcs)
                {
                    memoryData::memData mn;
                    setRigMemoryData(buttonNumber, mn);
                    break;
                }
            }
        }
    }
    sendUpdateMemories();
}
void RigMemoryFrame::on_AfterLogContact( BaseContestLog *c)
{
      if (c && ct == c)
      {
          int mcount = ct->rigMemories.size();
          for (int buttonNumber = 0; buttonNumber < mcount; buttonNumber ++)
          {
              memoryData::memData m = getRigMemoryData(buttonNumber);

              if ( m.callsign != memDefData::DEFAULT_CALLSIGN)
              {
                  Callsign mcs(m.callsign);
                  mcs.validate();

                  for ( LogIterator i = ct->ctList.begin(); i != ct->ctList.end(); i++ )
                  {
                      if ((*i).wt->cs == mcs)
                      {
                          m.worked = true;
                          setRigMemoryData(buttonNumber, m);
                          break;
                      }
                  }
              }
          }
          sendUpdateMemories();
      }
}

void RigMemoryFrame::readActionSelected(int buttonNumber)
{
    traceMsg(QString("Memory Read Selected = %1").arg(QString::number(buttonNumber +1)));
    memoryData::memData m = getRigMemoryData(buttonNumber);

    // send detail to rotator control frame, locator will give bearing
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();
    tslf->transferDetails(m);

}

void RigMemoryFrame::writeActionSelected(int buttonNumber)
{
    traceMsg(QString("Memory Write Selected %1 = ").arg(QString::number(buttonNumber +1)));

    // get contest information
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();
    ScreenContact sc = tslf->getScreenEntry();

    memoryData::memData logData;
    tslf->getDetails(logData);

    RigMemDialog memDialog(this);
    memDialog.setLogData(&logData, buttonNumber);
    memDialog.setWindowTitle(QString("M%1 - Write").arg(QString::number(buttonNumber + 1)));
   if ( memDialog.exec() == QDialog::Accepted)
   {
       setRigMemoryData(buttonNumber, logData);

       sendUpdateMemories();
   }
}


void RigMemoryFrame::editActionSelected(int buttonNumber)
{
    traceMsg(QString("Memory Edit Selected = %1").arg(QString::number(buttonNumber + 1)));

    memoryData::memData logData = getRigMemoryData(buttonNumber);

    RigMemDialog memDialog(this);

    memDialog.setLogData(&logData, buttonNumber);
    memDialog.setWindowTitle(QString("M%1 - Edit").arg(QString::number(buttonNumber + 1)));

    if ( memDialog.exec() == QDialog::Accepted)
    {
        setRigMemoryData(buttonNumber, logData);

        sendUpdateMemories();
    }

}


void RigMemoryFrame::clearActionSelected(int buttonNumber)
{

    traceMsg(QString("Memory Clear Selected = %1").arg(QString::number(buttonNumber + 1)));

    memoryData::memData m;
    setRigMemoryData(buttonNumber, m);

    sendUpdateMemories();
}

//============================================================================================
RigMemoryButton::RigMemoryButton(QWidget *parent, RigMemoryFrame *rcf, int no)
{
    memNo = no;
    rigMemoryFrame = rcf;

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
    delete memButton;
}
void RigMemoryButton::memoryShortCutSelected()
{
    memButton->showMenu();
}
void RigMemoryButton::readActionSelected()
{
    rigMemoryFrame->readActionSelected(memNo);
}
void RigMemoryButton::editActionSelected()
{
    rigMemoryFrame->editActionSelected(memNo);
}
void RigMemoryButton::writeActionSelected()
{
    rigMemoryFrame->writeActionSelected(memNo);
}
void RigMemoryButton::clearActionSelected()
{
    emit clearActionSelected(memNo);
}

