#include "logger_pch.h"

#include <QSignalMapper>

#include "MatchThread.h"
#include "BandList.h"
#include "tqsoeditdlg.h"
#include "tentryoptionsform.h"
#include "tsinglelogframe.h"
#include "ui_tsinglelogframe.h"

#include "focuswatcher.h"
#include "htmldelegate.h"

const int CONTINENTS = 6;
struct ContList
{
   char continent[ 3 ];
   bool allow;
};

extern ContList contlist[ CONTINENTS ];
ContList contlist[ CONTINENTS ] =
   {
      {"EU", true},
      {"AS", false},
      {"AF", false},
      {"OC", false},
      {"SA", false},
      {"NA", false},
   };
bool showWorked = false;
bool showUnworked = false;

TSingleLogFrame::TSingleLogFrame(QWidget *parent, BaseContestLog * contest) :
    QFrame(parent),
    ui(new Ui::TSingleLogFrame),
    contest( contest ),
//    logColumnsChanged( false ),
    splittersChanged(false),
    currFreq( 0 ), oldFreq( 0 ),
    lastStanzaCount( 0 ),
    xferTree( 0 ),
    filterClickEnabled( false )

{
    ui->setupUi(this);
    qsoModel.initialise(contest);
    ui->QSOTable->setModel(&qsoModel);
    ui->QSOTable->setItemDelegate( new HtmlDelegate );

    ui->QSOTable->resizeColumnsToContents();
    ui->QSOTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ui->LogAreaSplitter->setClosingWidget(ui->ArchiveSplitter);
    ui->CribSplitter->setClosingWidget(ui->CribSheet);
    ui->MultSplitter->setClosingWidget(ui->MultDisp);
    ui->TopSplitter->setClosingWidget(ui->MultSplitter);

    ui->ThisMatchTree->header()->setSectionResizeMode(QHeaderView::Stretch);
    ui->OtherMatchTree->header()->setSectionResizeMode(QHeaderView::Stretch);
    ui->ArchiveMatchTree->header()->setSectionResizeMode(QHeaderView::Stretch);


    ui->GJVQSOLogFrame->initialise( contest, false );

    connect(&MinosLoggerEvents::mle, SIGNAL(ContestPageChanged()), this, SLOT(on_ContestPageChanged()));
    connect(&MinosLoggerEvents::mle, SIGNAL(XferPressed()), this, SLOT(on_XferPressed()));
    connect(&MinosLoggerEvents::mle, SIGNAL(BandMapPressed()), this, SLOT(on_BandMapPressed()));
    connect(&MinosLoggerEvents::mle, SIGNAL(TimerDistribution()), this, SLOT(NextContactDetailsTimerTimer()));
    connect(&MinosLoggerEvents::mle, SIGNAL(MatchStarting(BaseContestLog*)), this, SLOT(on_MatchStarting(BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(MakeEntry(BaseContestLog*)), this, SLOT(on_MakeEntry(BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(AfterSelectContact(BaseContact *, BaseContestLog *)), this, SLOT(on_AfterSelectContact(BaseContact *, BaseContestLog *)));
    connect(&MinosLoggerEvents::mle, SIGNAL(AfterLogContact(BaseContestLog *)), this, SLOT(on_AfterLogContact(BaseContestLog *)));


    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceThisLogList(TMatchCollection*,BaseContestLog*)), this, SLOT(on_ReplaceThisLogList(TMatchCollection*,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceOtherLogList(TMatchCollection*,BaseContestLog*)), this, SLOT(on_ReplaceOtherLogList(TMatchCollection*,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceListList(TMatchCollection*,BaseContestLog*)), this, SLOT(on_ReplaceListList(TMatchCollection*,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ScrollToCountry(QString,BaseContestLog*)), this, SLOT(on_ScrollToCountry(QString,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ScrollToDistrict(QString,BaseContestLog*)), this, SLOT(on_ScrollToDistrict(QString,BaseContestLog*)), Qt::QueuedConnection);

    connect(&MinosLoggerEvents::mle, SIGNAL(SplittersChanged()), this, SLOT(onSplittersChanged()));

    // Connect up the stats etc display
    QSignalMapper* sm = new QSignalMapper(this);

    // connect to `clicked' on all buttons
    connect(ui->DXCCButton, SIGNAL(clicked()), sm, SLOT(map()));
    connect(ui->FilterButton, SIGNAL(clicked()), sm, SLOT(map()));
    connect(ui->DistrictButton, SIGNAL(clicked()), sm, SLOT(map()));
    connect(ui->LocatorButton, SIGNAL(clicked()), sm, SLOT(map()));
    connect(ui->StatsButton, SIGNAL(clicked()), sm, SLOT(map()));

    // setMapping on each button to the QStackedWidget index we'd like to switch to
    // note: this affects the value passed via QSignalMapper::mapped(int) signal
    sm->setMapping(ui->DXCCButton, 0);
    sm->setMapping(ui->FilterButton, 1);
    sm->setMapping(ui->DistrictButton, 2);
    sm->setMapping(ui->LocatorButton, 3);
    sm->setMapping(ui->StatsButton, 4);

    // finally, connect the mapper to the stacked widget
    connect(sm, SIGNAL(mapped(int)), ui->StackedMults, SLOT(setCurrentIndex(int)));

    ui->StatsFrame->setContest(contest);
    ui->locFrame->setContest(contest);
    ui->StatsButton->setChecked(true);
    ui->StackedMults->setCurrentIndex(4);
    initFilters();

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
       qsoModel.initialise(0);
       TContestApp::getContestApp() ->closeFile( contest );
       ui->GJVQSOLogFrame->closeContest();
       contest = 0;
    }
}
void TSingleLogFrame::showQSOs()
{

   NextContactDetailsTimerTimer( );


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

    if (splittersChanged)
    {
       getSplitters();
    }


    ui->GJVQSOLogFrame->selectField(0);
    ui->GJVQSOLogFrame->logTabChanged();

//    MultDispFrame->setContest( contest );
//    doNextContactDetailsOnLeftClick( this );
//    MinosLoggerEvents::SendShowOperators();

    updateQSODisplay();

}
void TSingleLogFrame::NextContactDetailsTimerTimer( )
{
   if ( contest )
   {
       QString cb = contest->band.getValue().trimmed();
       BandList &blist = BandList::getBandList();
       BandInfo bi;
       bool bandOK = blist.findBand(cb, bi);
       if (bandOK)
       {
          cb = bi.uk;
       }

      if ( contest->isReadOnly() )
      {
         ui->NextContactDetailsLabel->setText( "<b><center><nobr><p><big>"
                                            + cb + "</p><h1>"
                                            + contest->mycall.fullCall.getValue() + "<br>"
                                            + contest->myloc.loc.getValue() + "<br>"
                                            + contest->location.getValue());
      }
      else
      {
         QString buff = QString::number( contest->maxSerial + 1 );
         ui->NextContactDetailsLabel->setText( "<b><center><nobr><p><big>"
                                            + cb + "</p><h1>"
                                            + contest->mycall.fullCall.getValue() + "<br>"
                                            + buff + "<br>"
                                            + contest->myloc.loc.getValue() + "<br>"
                                            + contest->location.getValue());
      }
   }
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
   if ( archiveTreeClickIndex.isValid() && ( xferTree == 0 || xferTree == ui->ArchiveMatchTree ) )
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
      if ( otherTreeClickIndex.isValid() && ( xferTree == 0 || xferTree == ui->OtherMatchTree ) )
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
void TSingleLogFrame::on_QSOTable_doubleClicked(const QModelIndex &index)
{
    BaseContact * lct = dynamic_cast<BaseContact*>( contest->pcontactAt( index.row() ) );
    QSOTreeSelectContact(lct);
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
   refreshMults();
   //LogMonitor->Repaint();
   ui->GJVQSOLogFrame->startNextEntry();

}
void TSingleLogFrame::on_MatchStarting(BaseContestLog *ct)
{
      // clear down match trees
    if (contest == ct)
    {
      xferTree = 0;
      matchTreeClickIndex = QModelIndex();
      otherTreeClickIndex = QModelIndex();
      archiveTreeClickIndex = QModelIndex();
//      OtherMatchTree->Colors->UnfocusedSelectionColor = clBtnFace;
//      ArchiveMatchTree->Colors->UnfocusedSelectionColor = clBtnFace;
//      GJVQSOLogFrame->MatchXferButton->Font->Color = clBtnText;

      ui->GJVQSOLogFrame->setXferEnabled(false);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QTreeWidgetItem * TSingleLogFrame::addTreeRoot(QTreeWidget *tree, QString text)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(tree);

    treeItem->setText(0, text);

    return treeItem;
}

void TSingleLogFrame::addTreeChild(QTreeWidgetItem *parent, QString text)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();

    treeItem->setText(0, text);

    parent->addChild(treeItem);
}
void TSingleLogFrame::showThisMatchQSOs( TMatchCollection *matchCollection )
{
    ui->ThisMatchTree->clear();

    ui->GJVQSOLogFrame->setXferEnabled(false);

   if ( matchCollection->getContestCount() == 0 )
   {
      return ;
   }

   QTreeWidgetItem *root = addTreeRoot(ui->ThisMatchTree, "Current contest" + TMatchThread::getThisMatchStatus());
   root->setExpanded(true);

   for ( int i = 0; i < matchCollection->getContestCount(); i++ )
   {
       BaseMatchContest * clp = matchCollection->pcontestAt(i);

       for ( int j = 0; j < clp->getContactCount(); j++)
       {

           MatchContact *pc = clp->pcontactAt( j );

          QString text;
          pc->getText( text, clp->getContactLog() );
          addTreeChild(root, text);
       }
   }

}
void TSingleLogFrame::showOtherMatchQSOs( TMatchCollection *matchCollection )
{
   ui->OtherMatchTree->clear();

   ui->GJVQSOLogFrame->setXferEnabled(false);
   if ( matchCollection->getContestCount() == 0 )
   {
       return ;
   }
   ui->GJVQSOLogFrame->setXferEnabled(true);
   QTreeWidgetItem *lastTopNode = 0;

   BaseContestLog * cc = MinosParameters::getMinosParameters() ->getCurrentContest();

   for ( int i = 0; i < matchCollection->getContestCount(); i++ )
   {
       BaseMatchContest * clp = matchCollection->pcontestAt(i);

       BaseContestLog *bcl = clp->getContactLog();
       lastTopNode = addTreeRoot(ui->OtherMatchTree, bcl->band.getValue() + " " + bcl->name.getValue());
       lastTopNode->setExpanded(true);

       for ( int j = 0; j < clp->getContactCount(); j++)
       {

           MatchContact *pc = clp->pcontactAt( j );

           BaseContact *lc = pc->getBaseContact();

           QString text = lc->getField(egCall, cc) + " "
                           + lc->getField(egLoc, cc) + " "
                           + lc->getField(egBrg, cc) + " "
                           + lc->getField(egScore, cc);

           addTreeChild(lastTopNode, text);
       }
   }
}
//---------------------------------------------------------------------------
void TSingleLogFrame::showMatchList( TMatchCollection *matchCollection )
{
    ui->ArchiveMatchTree->clear();

    ui->GJVQSOLogFrame->setXferEnabled(false);
    if ( matchCollection->getContestCount() == 0 )
    {
        return ;
    }
    ui->GJVQSOLogFrame->setXferEnabled(true);
    BaseContestLog * cc = MinosParameters::getMinosParameters() ->getCurrentContest();

    QTreeWidgetItem *lastTopNode = 0;
    for ( int i = 0; i < matchCollection->getContestCount(); i++ )
    {
        BaseMatchContest * clp = matchCollection->pcontestAt(i);

        ContactList *bcl = clp->getContactList();
        lastTopNode = addTreeRoot(ui->ArchiveMatchTree, bcl->name);
        lastTopNode->setExpanded(true);

        int contacts = clp->getContactCount();
        for ( int j = 0; j < contacts; j++)
        {

            MatchContact *pc = clp->pcontactAt( j );

            ListContact *lc = pc->getListContact();

            QString text = lc->getField(egCall, cc) + " "
                            + lc->getField(egLoc, cc) + " "
                            + lc->getField(egBrg, cc) + " "
                            + lc->getField(egScore, cc);
            addTreeChild(lastTopNode, text);
        }
    }
}
void TSingleLogFrame::on_ReplaceThisLogList( TMatchCollection *matchCollection, BaseContestLog* )
{
    showThisMatchQSOs( matchCollection );
}
void TSingleLogFrame::on_ReplaceOtherLogList( TMatchCollection *matchCollection, BaseContestLog* )
{
    showOtherMatchQSOs( matchCollection );
}

void TSingleLogFrame::on_ReplaceListList(TMatchCollection *matchCollection , BaseContestLog *)
{
    showMatchList( matchCollection );
}

void TSingleLogFrame::on_ScrollToDistrict( const QString &qth, BaseContestLog* )
{
    DistrictEntry * dist = MultLists::getMultLists() ->searchDistrict( qth );
    if ( dist )
    {
       unsigned int district_ind = MultLists::getMultLists() ->getDistListIndexOf( dist );
       //MultDispFrame->scrollToDistrict( district_ind, true );
    }
}

void TSingleLogFrame::on_ScrollToCountry( const QString &csCs, BaseContestLog* )
{
    callsign cs( csCs );
    cs.validate( );	// we don't use the result

    CountryEntry *ctryMult = findCtryPrefix( cs );
    if ( ctryMult )
    {
       unsigned int ctry_ind = MultLists::getMultLists() ->getCtryListIndexOf( ctryMult );
       //MultDispFrame->scrollToCountry( ctry_ind, true );
    }
}
void TSingleLogFrame::on_MakeEntry(BaseContestLog *ct)
{
    if (ct == contest)
    {
       makeEntry( false );
    }
}
void TSingleLogFrame::on_AfterSelectContact( BaseContact *lct, BaseContestLog *ct)
{
    if (ct == contest && lct == nullptr)
    {
        ui->QSOTable->scrollToBottom();
        int row = ui->QSOTable->model()->rowCount() - 1;
        if (row >= 0)
        {
            QModelIndex index = ui->QSOTable->model()->index( row, 0 );
            ui->QSOTable->setCurrentIndex(index);
        }
    }
}
void TSingleLogFrame::on_AfterLogContact( BaseContestLog *ct)
{
      if (ct == contest)
      {
         contest->scanContest();
         updateTrees();
         NextContactDetailsTimerTimer( );
      }
}
void TSingleLogFrame::refreshMults()
{
    ui->locFrame->reInitialiseLocators();
}

void TSingleLogFrame::updateTrees()
{
   qsoModel.reset();
   refreshMults();
}
void TSingleLogFrame::initFilters()
{
   filterClickEnabled = false;

   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentEU, contlist[ 0 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentAS, contlist[ 1 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentAF, contlist[ 2 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentOC, contlist[ 3 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentSA, contlist[ 4 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentNA, contlist[ 5 ].allow );

   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowWorked, showWorked );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowUnworked, showUnworked );

   ui->ContEU->setChecked(contlist[ 0 ].allow);
   ui->ContAS->setChecked(contlist[ 1 ].allow);
   ui->ContAF->setChecked(contlist[ 2 ].allow);
   ui->ContOC->setChecked(contlist[ 3 ].allow);
   ui->ContSA->setChecked(contlist[ 4 ].allow);
   ui->ContNA->setChecked(contlist[ 5 ].allow);
   ui->WorkedCB->setChecked(showWorked);
   ui->UnworkedCB->setChecked(showUnworked);

   filterClickEnabled = true;
}
void TSingleLogFrame::saveFilters()
{
    if ( filterClickEnabled )
    {
        ui->dxccFrame->reInitialiseCountries();
        ui->districtFrame->reInitialiseDistricts();
        ui->locFrame->reInitialiseLocators();
        ui->StatsFrame->reInitialiseStats();

        contlist[ 0 ].allow = ui->ContEU->isChecked();
        contlist[ 1 ].allow = ui->ContAS->isChecked();
        contlist[ 2 ].allow = ui->ContAF->isChecked();
        contlist[ 3 ].allow = ui->ContOC->isChecked();
        contlist[ 4 ].allow = ui->ContSA->isChecked();
        contlist[ 5 ].allow = ui->ContNA->isChecked();
        showWorked = ui->WorkedCB->isChecked();
        showUnworked = ui->UnworkedCB->isChecked();

        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentEU, contlist[ 0 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentAS, contlist[ 1 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentAF, contlist[ 2 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentOC, contlist[ 3 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentSA, contlist[ 4 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentNA, contlist[ 5 ].allow );

        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowWorked, showWorked );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowUnworked, showUnworked );
        MinosParameters::getMinosParameters() ->flushDisplayProfile();

        MinosLoggerEvents::SendFiltersChanged();
    }
}

void TSingleLogFrame::on_WorkedCB_clicked()
{
    saveFilters();
}

void TSingleLogFrame::on_UnworkedCB_clicked()
{
    saveFilters();
}

void TSingleLogFrame::on_ContEU_clicked()
{
    saveFilters();
}

void TSingleLogFrame::on_ContOC_clicked()
{
    saveFilters();
}

void TSingleLogFrame::on_ContAS_clicked()
{
    saveFilters();
}

void TSingleLogFrame::on_ContSA_clicked()
{
    saveFilters();
}

void TSingleLogFrame::on_ContAF_clicked()
{
    saveFilters();
}

void TSingleLogFrame::on_ContNA_clicked()
{
    saveFilters();
}
bool TSingleLogFrame::getStanza( unsigned int stanza, QString &stanzaData )
{
   return contest->getStanza( stanza, stanzaData );
}
void TSingleLogFrame::getSplitters()
{
    QSettings settings;
    QByteArray state;

    state = settings.value("LogAreaSplitter/state").toByteArray();
    ui->LogAreaSplitter->restoreState(state);

    state = settings.value("ArchiveSplitter/state").toByteArray();
    ui->ArchiveSplitter->restoreState(state);

    state = settings.value("TopSplitter/state").toByteArray();
    ui->TopSplitter->restoreState(state);

    state = settings.value("CribSplitter/state").toByteArray();
    ui->CribSplitter->restoreState(state);

    state = settings.value("MultSplitter/state").toByteArray();
    ui->MultSplitter->restoreState(state);
}
void TSingleLogFrame::onSplittersChanged()
{
    splittersChanged = true;
}

//=============================================================================
QSOGridModel::QSOGridModel()
{}
QSOGridModel::~QSOGridModel()
{}
void QSOGridModel::reset()
{
    beginResetModel();
    endResetModel();
}

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

void TSingleLogFrame::on_StackedMults_currentChanged(int /*arg1*/)
{
    ui->StatsFrame->reInitialiseStats();
}

void TSingleLogFrame::on_LogAreaSplitter_splitterMoved(int pos, int index)
{
    QByteArray state = ui->LogAreaSplitter->saveState();
    QSettings settings;
    settings.setValue("LogAreaSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}

void TSingleLogFrame::on_ArchiveSplitter_splitterMoved(int pos, int index)
{
    QByteArray state = ui->ArchiveSplitter->saveState();
    QSettings settings;
    settings.setValue("ArchiveSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}

void TSingleLogFrame::on_TopSplitter_splitterMoved(int pos, int index)
{
    QByteArray state = ui->TopSplitter->saveState();
    QSettings settings;
    settings.setValue("TopSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}

void TSingleLogFrame::on_CribSplitter_splitterMoved(int pos, int index)
{
    QByteArray state = ui->CribSplitter->saveState();
    QSettings settings;
    settings.setValue("CribSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}

void TSingleLogFrame::on_MultSplitter_splitterMoved(int pos, int index)
{
    QByteArray state = ui->MultSplitter->saveState();
    QSettings settings;
    settings.setValue("MultSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}
