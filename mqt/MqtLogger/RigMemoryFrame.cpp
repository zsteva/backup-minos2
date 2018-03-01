#include <QGridLayout>
#include <QToolButton>

#include "logger_pch.h"
#include "tlogcontainer.h"
#include "tsinglelogframe.h"
#include "rigmemdialog.h"
#include "rigutils.h"
#include "RigMemoryFrame.h"
#include "htmldelegate.h"
#include "ui_RigMemoryFrame.h"

enum eRigMemGridCols {ermLocator, ermBearing, ermFreq, ermTime, ermWorked,
                    ermMaxCol
                   };
static GridColumn RigMemoryColumns[ ermMaxCol ] =
   {
      GridColumn( ermLocator, "MM00MM00", "Locator", taLeftJustify ),
      GridColumn( ermBearing, "BRGXXX", "Brg", taCenter ),
      GridColumn( ermFreq, "144.000.000", "Freq", taLeftJustify ),
      GridColumn( ermTime, "XX:XX", "Time", taLeftJustify ),
      GridColumn( ermWorked, "Wk CtX", "Wkd", taCenter ),
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

    ui->rigMemTable->setObjectName( "rigMemTable" );
    ui->rigMemTable->setItemDelegate( new HtmlDelegate );
    ui->rigMemTable->horizontalHeader() ->setSectionsMovable( true );
    ui->rigMemTable->horizontalHeader() ->setSectionsClickable( true );
    ui->rigMemTable->horizontalHeader() ->setSectionResizeMode( QHeaderView::Interactive );
    ui->rigMemTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->rigMemTable->verticalHeader()->setContextMenuPolicy( Qt::CustomContextMenu );
/*
    ui->rigMemTable->setContextMenuPolicy( Qt::CustomContextMenu );

    connect( ui->rigMemTable, SIGNAL( customContextMenuRequested( const QPoint& ) ),
             this, SLOT( rigMemTable_Hdr_customContextMenuRequested( const QPoint& ) ) );

    connect( ui->rigMemTable->verticalHeader(), SIGNAL( customContextMenuRequested( const QPoint& ) ),
             this, SLOT( on_rigMemTable_customContextMenuRequested( const QPoint& ) ) );
*/
    connect(&MinosLoggerEvents::mle, SIGNAL(TimerDistribution()), this, SLOT(checkTimerTimer()));
    connect(&MinosLoggerEvents::mle, SIGNAL(RigFreqChanged(QString,BaseContestLog*)), this, SLOT(onRigFreqChanged(QString,BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(RotBearingChanged(int,BaseContestLog*)), this, SLOT(onRotBearingChanged(int,BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(AfterLogContact(BaseContestLog *)), this, SLOT(on_AfterLogContact(BaseContestLog *)));

    reloadColumns();
    //ui->rigMemTable->resizeColumnsToContents();

    connect( ui->rigMemTable->horizontalHeader(), SIGNAL(sectionMoved(int, int , int)),
             this, SLOT( on_sectionMoved(int, int , int)));
    connect( ui->rigMemTable->horizontalHeader(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));
    connect( ui->rigMemTable->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
             this, SLOT( on_sortIndicatorChanged(int, Qt::SortOrder)));

    connect( ui->rigMemTable->verticalHeader(), SIGNAL( sectionClicked(int)),
             this, SLOT( vsectionClicked( int ) ) );

    memoryMenu = new QMenu(ui->flushMemoriesButton);

    ui->flushMemoriesButton->setFocusPolicy(Qt::NoFocus);

    readAction = new QAction("&Read", this);
    writeAction = new QAction("&Write",this);
    editAction = new QAction("&Edit", this);
    clearAction = new QAction("&Clear",this);

    clearAllAction = new QAction("Clear &All",this);
    clearWorkedAction = new QAction("Clear Wor&ked",this);

    memoryMenu->addAction(readAction);
    memoryMenu->addAction(writeAction);
    memoryMenu->addAction(editAction);
    memoryMenu->addAction(clearAction);
    memoryMenu->addAction(clearAllAction);
    memoryMenu->addAction(clearWorkedAction);

    ui->flushMemoriesButton->setMenu(memoryMenu);

   // connect(ui->flushMemoriesButton, SIGNAL(clicked(bool)), this, SLOT(readActionSelected()));
    connect( readAction, SIGNAL( triggered() ), this, SLOT(readActionSelected()) );
    connect( writeAction, SIGNAL( triggered() ), this, SLOT(writeActionSelected()) );
    connect( editAction, SIGNAL( triggered() ), this, SLOT(editActionSelected()) );
    connect( clearAction, SIGNAL( triggered() ), this, SLOT(clearActionSelected()) );
    connect( clearAllAction, SIGNAL( triggered() ), this, SLOT(clearAllActionSelected()) );
    connect( clearWorkedAction, SIGNAL( triggered() ), this, SLOT(clearWorkedActionSelected()) );

}

RigMemoryFrame::~RigMemoryFrame()
{
    delete ui;
}
void RigMemoryFrame::saveAllColumnWidthsAndPositions()
{
    QSettings settings;
    QByteArray state;

    state = ui->rigMemTable->horizontalHeader()->saveState();
    settings.setValue("RigMemoryTable/state", state);

    //And we need to send this out to all other instances

    sendUpdateMemories();
}
void RigMemoryFrame::reloadColumns()
{
    QSettings settings;
    QByteArray state = settings.value("RigMemoryTable/state").toByteArray();
    if (state.size())
    {
        // this will fire signals, so... don't save at the same time
        ui->rigMemTable->horizontalHeader()->restoreState(state);
    }
}
void RigMemoryFrame:: on_sectionMoved(int /*logicalIndex*/, int /*oldVisualIndex*/, int /*newVisualIndex*/)
{
    saveAllColumnWidthsAndPositions();
}

void RigMemoryFrame::on_sectionResized(int /*logicalIndex*/, int /*oldSize*/, int /*newSize*/)
{
    saveAllColumnWidthsAndPositions();
}

void RigMemoryFrame::on_sortIndicatorChanged(int /*logicalIndex*/, Qt::SortOrder /*order*/)
{
    saveAllColumnWidthsAndPositions();
}
void RigMemoryFrame::setContest( BaseContestLog *pct )
{
    ct = dynamic_cast<LoggerContestLog *>( pct);
    on_AfterLogContact(ct);
    doMemoryUpdates();

    model.ct = pct;
    model.frame = this;

    proxyModel.ct = pct;
    if (pct)
    {
        proxyModel.setSourceModel(&model);
        ui->rigMemTable->setModel(&proxyModel);
        connect( ui->rigMemTable->horizontalHeader(), SIGNAL(sectionResized(int, int , int)),
                 this, SLOT( on_sectionResized(int, int , int)), Qt::UniqueConnection);

    }

}

void RigMemoryFrame::setRigMemoryData(int memoryNumber, memoryData::memData m)
{
    model.beginResetModel();
    ct->saveRigMemory(memoryNumber, m);
    model.endResetModel();
}

void RigMemoryFrame::doMemoryUpdates()
{
    // called from minosLoggerEvents following sendUpdateMemories
    model.beginResetModel();

    model.endResetModel();

//    ui->rigMemTable->resizeColumnsToContents();


    // clear all the "old" buttons
/*
    QString tTipStr = "Callsign: " + m.callsign + "\n"
            + "Freq: " + convertFreqStrDisp(m.freq) + "\n"
            + "Mode: " + m.mode + "\n"
            + "Locator: " + m.locator + "\n"
            + "Bearing: " + QString::number(m.bearing) + "\n"
            + "Time: " + m.time;
*/
    suppressSendUpdate = true;
    reloadColumns();
    suppressSendUpdate = false;
}

void RigMemoryFrame::checkTimerTimer()
{
    if (!isVisible())
        return;

    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();
    if (!ct || !tslf)
        return;

    memoryData::memData logData;
    tslf->getCurrentDetails(logData);

    double rigFreq = convertStrToFreq(logData.freq);
    int bearing = logData.bearing;

    if (!doTimer && (rigFreq == lastRigFreq && bearing == lastBearing))
        return;

    lastRigFreq = rigFreq;
    lastBearing = bearing;

    doTimer = false;

    int mcount = ct->rigMemories.size();
    int firstMatch = -1;

    for (int buttonNumber = 0; buttonNumber < mcount; buttonNumber ++)
    {

        memoryData::memData m = ct->getRigMemoryData(buttonNumber);
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

        QString ht = m.callsign;
        QColor colour( Qt::black);

        if (m.worked)
        {
            ht = "(" + ht + ")";
            colour = Qt::darkGray;
        }

        if (onfreq == rtsOn || onbearing == rtsOn)
        {
            if (onfreq == rtsOn && onbearing == rtsOn)
            {
                ht = "FB " + ht;
                colour = Qt::darkGreen;
                if (firstMatch == -1)
                    firstMatch = m.memno;
            }
            else if (onfreq == rtsOn)
            {
                ht = "F  " + ht;
                colour = Qt::red;
            }
            else
            {
                ht = "B  " + ht;
                colour = Qt::blue;
            }
        }
        headerVal[m.callsign].text = ht;
        headerVal[m.callsign].colour = colour;
    }
    if (firstMatch >= 0)
    {
        ui->rigMemTable->scrollTo(model.index(firstMatch, 0), QAbstractItemView::PositionAtCenter);
    }
    proxyModel.headerDataChanged(Qt::Vertical, 0, model.rowCount() - 1);
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
    if (!suppressSendUpdate)
        MinosLoggerEvents::sendUpdateMemories(ct);

    doTimer = true;
}
//======================================================================================

// responses to button actions

void RigMemoryFrame::on_newMemoryButton_clicked()
{
    int n = -1;
    int mcount = ct->rigMemories.size();
    for (int i = 0; i <= mcount; i ++)
    {
        memoryData::memData m = ct->getRigMemoryData(i);

        if ( m.callsign == memDefData::DEFAULT_CALLSIGN)
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

    writeMemory(n); // which creates the button as well
}
void RigMemoryFrame::on_AfterLogContact( BaseContestLog *c)
{
      if (c && ct == c)
      {
          int mcount = ct->rigMemories.size();
          for (int buttonNumber = 0; buttonNumber < mcount; buttonNumber ++)
          {
              memoryData::memData m = ct->getRigMemoryData(buttonNumber);

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

int RigMemoryFrame::getSelectedLine()
{
    QItemSelectionModel *ism = ui->rigMemTable->selectionModel();
    QModelIndexList mil = ism->selectedRows();
    if (mil.size() == 0)
        return -1;
    const QModelIndex index = proxyModel.mapToSource( mil[0] );
    if (!index.isValid())
        return -1;
    int buttonNumber = index.row();
    return buttonNumber;
}

void RigMemoryFrame::readActionSelected()
{
    int buttonNumber = getSelectedLine();
    if (buttonNumber < 0)
        return;

    traceMsg(QString("Memory Read Selected = %1").arg(QString::number(buttonNumber +1)));
    memoryData::memData m = ct->getRigMemoryData(buttonNumber);

    // send detail to rotator control frame, locator will give bearing
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();
    tslf->transferDetails(m);

}
void RigMemoryFrame::writeMemory(int buttonNumber)
{
    traceMsg(QString("Memory Write Selected %1 = ").arg(QString::number(buttonNumber +1)));

    // get contest information
    TSingleLogFrame *tslf = LogContainer->getCurrentLogFrame();

    memoryData::memData logData;
    tslf->getDetails(logData);

    RigMemDialog memDialog(this);
    memDialog.setLogData(&logData, buttonNumber, ct);
    memDialog.setWindowTitle(QString("M%1 - Write").arg(QString::number(buttonNumber + 1)));
   if ( memDialog.exec() == QDialog::Accepted)
   {
       setRigMemoryData(buttonNumber, logData);

       sendUpdateMemories();
   }
}

void RigMemoryFrame::writeActionSelected()
{
    int buttonNumber = getSelectedLine();
    if (buttonNumber < 0)
        return;

    writeMemory(buttonNumber);
}
void RigMemoryFrame::editActionSelected()
{
    model.beginResetModel();
    int buttonNumber = getSelectedLine();
    if (buttonNumber < 0)
        return;

    traceMsg(QString("Memory Edit Selected = %1").arg(QString::number(buttonNumber + 1)));

    memoryData::memData logData = ct->getRigMemoryData(buttonNumber);

    RigMemDialog memDialog(this);

    memDialog.setLogData(&logData, buttonNumber, ct);
    memDialog.setWindowTitle(QString("M%1 - Edit").arg(QString::number(buttonNumber + 1)));

    if ( memDialog.exec() == QDialog::Accepted)
    {
        setRigMemoryData(buttonNumber, logData);

        sendUpdateMemories();
    }
    model.endResetModel();
}


void RigMemoryFrame::clearActionSelected()
{
    model.beginResetModel();
    int buttonNumber = getSelectedLine();
    if (buttonNumber < 0)
        return;

    traceMsg(QString("Memory Clear Selected = %1").arg(QString::number(buttonNumber + 1)));

    memoryData::memData m;
    setRigMemoryData(buttonNumber, m);

    sendUpdateMemories();
    model.endResetModel();
}
void RigMemoryFrame::clearAllActionSelected()
{
    model.beginResetModel();
    int mcount = ct->rigMemories.size();
    for (int buttonNumber = 0; buttonNumber < mcount; buttonNumber ++)
    {
        memoryData::memData mn;
        setRigMemoryData(buttonNumber, mn);
    }
    sendUpdateMemories();
    model.endResetModel();

}
void RigMemoryFrame::clearWorkedActionSelected()
{
    // scan memories and see if they have been worked
    model.beginResetModel();
    int mcount = ct->rigMemories.size();
    for (int buttonNumber = 0; buttonNumber < mcount; buttonNumber ++)
    {
        memoryData::memData m = ct->getRigMemoryData(buttonNumber);

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
    model.endResetModel();
}

void RigMemoryFrame::vsectionClicked(int /*logicalIndex*/)
{
    readActionSelected();
}


RigMemoryGridModel::RigMemoryGridModel():ct(0)
{}
RigMemoryGridModel::~RigMemoryGridModel()
{
}
void RigMemoryGridModel::reset()
{
    beginResetModel();

    endResetModel();
}

void RigMemoryGridModel::initialise( )
{
   beginResetModel();

   // pick up the correct RigMemory list

   endResetModel();
}
QVariant RigMemoryGridModel::data( const QModelIndex &index, int role ) const
{
    int row = index.row();
    int col = index.column();
    if (ct)
    {
        if ( role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::UserRole )
            return QVariant();

        if (role == Qt::DisplayRole || role == Qt::UserRole)
        {
            LoggerContestLog *c = dynamic_cast<LoggerContestLog *>( ct );
            memoryData::memData m = c->getRigMemoryData(row);
            QString disp;
            switch(col)
            {
            case ermWorked:
                disp = m.worked?"Y":"N";
                break;
            case ermLocator:
                disp = m.locator;
                break;
            case ermBearing:
                disp = QString("%1").arg( m.bearing, 3, 10, QChar('0'));
                break;
            case ermFreq:
                {
                    QString newfreq = m.freq.trimmed().remove('.');
                    double dfreq = convertStrToFreq(newfreq);
                    dfreq = dfreq/1000000.0;  // MHz

                    disp = QString::number(dfreq, 'f', 3); //MHz to 3 decimal places
                    break;
                }
            case ermTime:
                {
                    disp = m.time;
                    break;
                }
            }
            return disp;
        }
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignLeft;
    }
    return QVariant();
}
QVariant RigMemoryGridModel::headerData( int section, Qt::Orientation orientation,
                     int role ) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            QString cell;

            cell = RigMemoryColumns[section].title;

            return cell;
        }
        else if (role == Qt::TextAlignmentRole)
            return Qt::AlignLeft;
    }
    else if (orientation == Qt::Vertical)
    {
        if (role == Qt::DisplayRole)
        {
            QString disp;
            LoggerContestLog *c = dynamic_cast<LoggerContestLog *>( ct );
            if (c)
            {
                memoryData::memData m = c->getRigMemoryData(section);
                disp = frame->headerVal[m.callsign].text;
                if (disp.isEmpty())
                {
                    // This appears to be the line that defines the width
                    // of the vertical header
                    disp = "  " + m.callsign + "  ";
                }
            }
            return disp;
        }
        else if (role == Qt::ForegroundRole)
        {
            LoggerContestLog *c = dynamic_cast<LoggerContestLog *>( ct );
            if (c)
            {
                memoryData::memData m = c->getRigMemoryData(section);
                QColor colour = frame->headerVal[m.callsign].colour;
                return colour;
            }
        }
        else if (role == Qt::TextAlignmentRole)
            return Qt::AlignLeft;
    }
    return QVariant();
}

QModelIndex RigMemoryGridModel::index( int row, int column, const QModelIndex &/*parent*/) const
{
    if ( row < 0 || row >= rowCount()  )
        return QModelIndex();

    return createIndex( row, column );
}

QModelIndex RigMemoryGridModel::parent( const QModelIndex &/*index*/ ) const
{
    return QModelIndex();
}

int RigMemoryGridModel::rowCount( const QModelIndex &/*parent*/ ) const
{
    LoggerContestLog *c = dynamic_cast<LoggerContestLog *>( ct );
    if (c)
        return c->rigMemories.size();
    return 0;
}

int RigMemoryGridModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return ermMaxCol;
}
bool RigMemorySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &/*sourceParent*/) const
{
    LoggerContestLog *c = dynamic_cast<LoggerContestLog *>( ct );
    bool ret = false;
    if (c)
    {
        memoryData::memData m = c->getRigMemoryData(sourceRow);
        if (m.callsign != memDefData::DEFAULT_CALLSIGN)
            ret = true;
    }
    return ret;
}
bool RigMemorySortFilterProxyModel::lessThan(const QModelIndex &left,
                      const QModelIndex &right) const
{
    //Model Indices are to the SOURCE model

    int lrow = left.row();
    int rrow = right.row();

    RigMemoryGridModel *gridModel = dynamic_cast<RigMemoryGridModel *>(this->sourceModel());
    if (lrow >= gridModel->rowCount())
        return false;
    if (rrow >= gridModel->rowCount())
        return false;

    QString ws1;
    QString ws2;
    ws1 = gridModel->data(left, Qt::UserRole).toString();
    ws2 = gridModel->data(right, Qt::UserRole).toString();

    return ws1 < ws2;
}
