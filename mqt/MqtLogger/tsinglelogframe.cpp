#include "logger_pch.h"

#include "tentryoptionsform.h"
#include "tsinglelogframe.h"
#include "ui_tsinglelogframe.h"

#include "focuswatcher.h"
#include "htmldelegate.h"

TSingleLogFrame::TSingleLogFrame(QWidget *parent, BaseContestLog * contest) :
    QFrame(parent),
    ui(new Ui::TSingleLogFrame),
    contest( contest ),
    logColumnsChanged( false ),
    splittersChanged(false),
    currFreq( 0 ), oldFreq( 0 ),
    lastStanzaCount( 0 )
    // matchTreeClickNode( 0 ),
    //otherTreeClickNode( 0 ),
    //archiveTreeClickNode( 0 ),
    //xferTree( 0 ),

{
    ui->setupUi(this);
    qsoModel.initialise(contest);
    ui->QSOTable->setModel(&qsoModel);
    ui->QSOTable->setItemDelegate( new HtmlDelegate );

    ui->QSOTable->resizeColumnsToContents();
    //ui->QSOTable->resizeRowsToContents();
    ui->QSOTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ui->LogAreaSplitter->setClosingWidget(ui->ArchiveSplitter);
    ui->CribSplitter->setClosingWidget(ui->CribSheet);
    ui->MultSplitter->setClosingWidget(ui->MultTabs);
    ui->TopSplitter->setClosingWidget(ui->MultSplitter);

//    LogMonitor->initialise( contest );
    ui->GJVQSOLogFrame->initialise( contest, false );

}

TSingleLogFrame::~TSingleLogFrame()
{
    delete ui;
}

QString TSingleLogFrame::makeEntry( bool saveMinos )
{
   LoggerContestLog * ct = dynamic_cast<LoggerContestLog *>( contest );
   if ( !ct )
   {
      return "";
   }
   TEntryOptionsForm EntryDlg( this, ct, saveMinos  );
   if ( saveMinos )
   {
      EntryDlg.setWindowTitle("Save imported log as a .Minos file");
   }
   if ( EntryDlg.exec() == QDialog::Accepted )
   {
      ct->commonSave( false );
      QString expName = EntryDlg.doFileSave( );
      return expName;
   }
   return "";
}
BaseContestLog * TSingleLogFrame::getContest()
{
   return contest;
}

void TSingleLogFrame::closeContest()
{
    if ( TContestApp::getContestApp() )
    {
//       RPCPubSub::publish( "ContestLog", contest->publishedName, makeStr( 0 ), psRevoked );
//       GJVQSOLogFrame->closeContest();
       TContestApp::getContestApp() ->closeFile( contest );
       contest = 0;
    }
}
QSOGridModel::QSOGridModel()
{}
QSOGridModel::~QSOGridModel()
{}
void QSOGridModel::initialise( BaseContestLog * pcontest )
{
   contest = pcontest;
}
QVariant QSOGridModel::data( const QModelIndex &index, int role ) const
{
    int row = index.row();
    int column = index.column();

    if ( row >= rowCount() )
        return QVariant();

    BaseContact * ct = contest->pcontactAt( row);

    if (role == Qt::BackgroundRole)
    {
        if ( ct->contactFlags.getValue() & FORCE_LOG )
        {
           return ( QColor ) ( 0x00FF80C0 );        // Pink(ish)
        }
        else
        {
           if ( ct->getModificationCount() > 1 )
           {
               return ( QColor ) ( 0x00C0DCC0 );    // "money green"
           }
        }
        return QVariant();
    }
    if ( role != Qt::DisplayRole && role != Qt::EditRole )
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if ( ct && column >= 0 && column < columnCount())
        {
           QString line = ct->getField( QSOTreeColumns[ column ].fieldId, contest );
           QColor multhighlight = Qt::red;
           bool setHighlight = false;
           switch ( QSOTreeColumns[ column ].fieldId )
           {
              case egTime:
                 if (!contest->checkTime(ct->time))
                 {
                    setHighlight = true;
                 }
                 break;
              case egCall:
                 if ( contest->countryMult.getValue() && ct->newCtry )
                     setHighlight = true;
                 break;
              case egExchange:
                 if ( contest->districtMult.getValue() && ct->newDistrict )
                     setHighlight = true;
                 break;
              case egLoc:
                 if ( (contest->locMult.getValue() && ct->locCount > 0) || (contest->UKACBonus.getValue() && ct->bonus > 0))
                 {
                     setHighlight = true;
                 }
                 break;
           }
           if (setHighlight)
               line = HtmlFontColour(multhighlight) + line;
           return line;
        }
    }
    return QVariant();
}
QVariant QSOGridModel::headerData( int section, Qt::Orientation orientation,
                     int role ) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        QString h = QSOTreeColumns[ section ].title;
        return h;
    }
    return QVariant();
}

QModelIndex QSOGridModel::index( int row, int column, const QModelIndex &parent) const
{
    if ( row < 0 || row >= rowCount() || ( parent.isValid() && parent.column() != 0 ) )
        return QModelIndex();

    return createIndex( row, column );
}

QModelIndex QSOGridModel::parent( const QModelIndex &index ) const
{
    return QModelIndex();
}

int QSOGridModel::rowCount( const QModelIndex &parent ) const
{
    return contest->ctList.size();
}

int QSOGridModel::columnCount( const QModelIndex &parent ) const
{
    return  LOGTREECOLS;
}



