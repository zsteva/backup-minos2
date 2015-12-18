#include "logger_pch.h"

#include "tqsoeditdlg.h"
#include "tentryoptionsform.h"
#include "tsinglelogframe.h"
#include "ui_tsinglelogframe.h"

#include "focuswatcher.h"
#include "htmldelegate.h"

TSingleLogFrame::TSingleLogFrame(QWidget *parent, BaseContestLog * contest) :
    QFrame(parent),
    ui(new Ui::TSingleLogFrame),
    contest( contest ),
//    logColumnsChanged( false ),
    splittersChanged(false),
    currFreq( 0 ), oldFreq( 0 ),
    lastStanzaCount( 0 ),
    xferTree( 0 )

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
    ui->GJVQSOLogFrame->initialise( contest, false, false );

    connect(&MinosLoggerEvents::mle, SIGNAL(ContestPageChanged()), this, SLOT(on_ContestPageChanged()));
    connect(&MinosLoggerEvents::mle, SIGNAL(XferPressed()), this, SLOT(on_XferPressed()));
    connect(&MinosLoggerEvents::mle, SIGNAL(BandMapPressed()), this, SLOT(on_BandMapPressed()));

}

TSingleLogFrame::~TSingleLogFrame()
{
    delete ui;
    ui = nullptr;
}

void TSingleLogFrame::keyPressEvent( QKeyEvent* event )
{
    ui->GJVQSOLogFrame->keyPressEvent(event);
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
       qsoModel.initialise(0);
       TContestApp::getContestApp() ->closeFile( contest );
       ui->GJVQSOLogFrame->closeContest();
       contest = 0;
    }
}
void TSingleLogFrame::showQSOs()
{

//   NextContactDetailsTimerTimer( this );


   logColumnsChanged = false;

   ui->GJVQSOLogFrame->clearCurrentField();
   ui->GJVQSOLogFrame->startNextEntry();

}
void TSingleLogFrame::on_ContestPageChanged ()
{
    if (!ui)
        return;

    if ( this != LogContainer->getCurrentLogFrame() )
    {
       ui->GJVQSOLogFrame->savePartial();  // we kill it on (re) entry, so not needed
       return ;
    }

    BaseContestLog * ct = getContest();
    TContestApp::getContestApp() ->setCurrentContest( ct );

    if ( logColumnsChanged )
    {
       ui->GJVQSOLogFrame->killPartial();
       showQSOs();             // this does a restorePartial
       logColumnsChanged = false;
    }

//    if (splittersChanged)
//    {
//       getSplitters();
//    }


//    OnShowTimer->Enabled = true;
    ui->GJVQSOLogFrame->selectField(0);

//    MultDispFrame->setContest( contest );
//    doNextContactDetailsOnLeftClick( this );
//    MinosLoggerEvents::SendShowOperators();

    // Supress the tabstops we weren't able to manage in the form designed
    // to discover where they went, uncomment the top block in
    // TSingleLogFrame::NextContactDetailsTimerTimer

//    MultDispFrame->TabStop = false;
//    LogMonitor->TabStop = false;
//    GJVQSOLogFrame->TabStop = false;

    updateQSODisplay();

}
void TSingleLogFrame::updateQSODisplay()
{
    /*
   BandCombo->Clear();

   std::string cb = trim(contest->band.getValue());
   BandList &blist = BandList::getBandList();
   BandInfo bi;
   bool bandOK = blist.findBand(cb, bi);
   if (bandOK)
   {
      BandCombo->Items->Add(bi.uk.c_str());
   }
   else
   {
      BandCombo->Items->Add(contest->band.getValue().c_str());
   }
   BandCombo->ItemIndex = 0;
*/
   ui->GJVQSOLogFrame->updateQSODisplay();
}
void TSingleLogFrame::on_XferPressed()
{
   // transfer from current match
   if ( contest->isReadOnly() )
      return ;

   // copy relevant parts of match contact to screen contact
   if ( archiveTreeClickIndex.isValid() && ( xferTree == 0 || xferTree == ui->ArchiveMatchTable ) )
   {
      MatchNodeListData * MatchTreeIndex = ( MatchNodeListData * ) archiveTreeClickIndex.internalPointer();
      ContactList *clp = MatchTreeIndex->matchedList;
      if ( MatchTreeIndex && clp && MatchTreeIndex->matchedContact )
      {
         transferDetails( MatchTreeIndex );
      }
   }
   else
   {
      if ( otherTreeClickIndex.isValid() && ( xferTree == 0 || xferTree == ui->OtherMatchTable ) )
      {
         MatchNodeData * MatchTreeIndex = ( MatchNodeData * ) otherTreeClickIndex.internalPointer();
         BaseContestLog *clp = MatchTreeIndex->matchedContest;
         if ( MatchTreeIndex && clp && MatchTreeIndex->matchedContact )
         {
            transferDetails( MatchTreeIndex );
         }
      }
   }
}
//==============================================================================
void TSingleLogFrame::transferDetails( MatchNodeData *MatchTreeIndex )
{
   // needs to be transferred into QSOLogFrame.cpp
   const BaseContact * lct = MatchTreeIndex->matchedContact;
   if ( !contest )
   {
      return ;
   }

   if ( lct )
   {
      BaseContestLog * matct = MatchTreeIndex->matchedContest;
      ui->GJVQSOLogFrame->transferDetails( lct, matct );
   }
}
//---------------------------------------------------------------------------
void TSingleLogFrame::transferDetails( MatchNodeListData *MatchTreeIndex )
{
   // needs to be transferred into QSOLogFrame.cpp
   const ListContact * lct = MatchTreeIndex->matchedContact;
   if ( !contest )
   {
      return ;
   }

   if ( lct )
   {
      ContactList * matct = MatchTreeIndex->matchedList;
      ui->GJVQSOLogFrame->transferDetails( lct, matct );
   }
}
void TSingleLogFrame::on_BandMapPressed()
{
    ui->GJVQSOLogFrame->getScreenEntry();

    QString call = ui->GJVQSOLogFrame->screenContact.cs.fullCall.getValue();
    QString loc = ui->GJVQSOLogFrame->screenContact.loc.loc.getValue();
    QString utc = dtg( true ).getIsoDTG();
    QString qth = ui->GJVQSOLogFrame->screenContact.extraText;
    QString freq = sCurrFreq;

    if ( call.size() || loc.size() || qth.size() )
    {
       // Transfer to band map app
//       TSendDM::sendBandMap( freq, call, utc, loc, qth );
       ui->GJVQSOLogFrame->doGJVCancelButton_clicked();
    }
}
void TSingleLogFrame::QSOTreeSelectContact( BaseContact * lct )
{
   if (lct)
   {
      EditContact( lct );
   }
}
void TSingleLogFrame::EditContact( BaseContact *lct )
{
   TQSOEditDlg qdlg( this, false, false );
   ContestContact *ct = dynamic_cast<ContestContact *>( lct );
   qdlg.selectContact( contest, ct );

   qdlg.exec();

   contest->scanContest();

   ui->GJVQSOLogFrame->refreshOps();
   //LogMonitor->Invalidate();
   //MultDispFrame->refreshMults();
   //LogMonitor->Repaint();
   ui->GJVQSOLogFrame->startNextEntry();

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
    if (!ct)
        return QVariant();

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

QModelIndex QSOGridModel::parent( const QModelIndex &/*index*/ ) const
{
    return QModelIndex();
}

int QSOGridModel::rowCount( const QModelIndex &/*parent*/ ) const
{
    if (!contest)
        return 0;

    return contest->ctList.size() + 1;
}

int QSOGridModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return  LOGTREECOLS;
}



