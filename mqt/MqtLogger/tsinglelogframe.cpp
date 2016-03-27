#include "logger_pch.h"

#include <QSignalMapper>

#include "MatchThread.h"
#include "BandList.h"
#include "tqsoeditdlg.h"
#include "tentryoptionsform.h"
#include "tsinglelogframe.h"
#include "ui_tsinglelogframe.h"

#include "tlogcontainer.h"
#include "focuswatcher.h"
#include "htmldelegate.h"
#include "enqdlg.h"

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
    splittersChanged(false),
    currFreq( 0 ), oldFreq( 0 ),
    lastStanzaCount( 0 ),
    xferTree( 0 ),
    filterClickEnabled( false )

{
    ui->setupUi(this);

    splitterHandleWidth = ui->ArchiveSplitter->handleWidth();
#ifdef Q_OS_ANDROID
    splitterHandleWidth = 20;
#else
    splitterHandleWidth = 6;
#endif
    initFilters();

    qsoModel.initialise(contest);
    ui->QSOTable->setModel(&qsoModel);
    ui->QSOTable->setItemDelegate( new HtmlDelegate );

    ui->QSOTable->resizeColumnsToContents();
    ui->QSOTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    //ui->LogAreaSplitter->setClosingWidget(ui->ArchiveSplitter);
    //ui->CribSplitter->setClosingWidget(ui->CribSheet);
    //ui->MultSplitter->setClosingWidget(ui->MultDisp);
    //ui->TopSplitter->setClosingWidget(ui->MultSplitter);

    ui->ThisMatchTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->ThisMatchTree->setItemDelegate( new HtmlDelegate );

    ui->OtherMatchTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->OtherMatchTree->setItemDelegate( new HtmlDelegate );

    ui->ArchiveMatchTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->ArchiveMatchTree->setItemDelegate( new HtmlDelegate );


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

    connect(&MinosLoggerEvents::mle, SIGNAL(LogColumnsChanged()), this, SLOT(onLogColumnsChanged()));
    connect(&MinosLoggerEvents::mle, SIGNAL(SplittersChanged()), this, SLOT(onSplittersChanged()));
    connect(&MinosLoggerEvents::mle, SIGNAL(FiltersChanged()), this, SLOT(onFiltersChanged()));
    connect(&MinosLoggerEvents::mle, SIGNAL(NextContactDetailsOnLeft()), this, SLOT(on_NextContactDetailsOnLeft()));
    connect(&MinosLoggerEvents::mle, SIGNAL(NextUnfilled(BaseContestLog*)), this, SLOT(on_NextUnfilled(BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(GoToSerial(BaseContestLog*)), this, SLOT(on_GoToSerial(BaseContestLog*)));

    connect(&MinosLoggerEvents::mle, SIGNAL(SetMode(QString, BaseContestLog*)), this, SLOT(on_SetFreq(QString, BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(SetFreq(QString, BaseContestLog*)), this, SLOT(on_SetMode(QString, BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(RotatorState(QString,BaseContestLog*)), this, SLOT(on_RotatorState(QString, BaseContestLog*)));


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

    connect( ui->QSOTable->horizontalHeader(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));

    connect( ui->ThisMatchTree->header(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));
    connect( ui->OtherMatchTree->header(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));
    connect( ui->ArchiveMatchTree->header(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));

    ui->dxccFrame->setContest(contest);
    ui->districtFrame->setContest(contest);
    ui->StatsFrame->setContest(contest);
    ui->locFrame->setContest(contest);
    ui->StatsButton->setChecked(true);
    ui->StackedMults->setCurrentIndex(4);
}

TSingleLogFrame::~TSingleLogFrame()
{
    delete ui;
    ui = nullptr;
    contest = nullptr;
}
void TSingleLogFrame::keyPressEvent( QKeyEvent* event )
{
    ui->GJVQSOLogFrame->doKeyPressEvent(event);
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
void TSingleLogFrame::restoreColumns()
{
    QSettings settings;
    QByteArray state;

    state = settings.value("QSOTable/state").toByteArray();
    ui->QSOTable->horizontalHeader()->restoreState(state);

    state = settings.value("ThisMatchTree/state").toByteArray();
    ui->ThisMatchTree->header()->restoreState(state);

    state = settings.value("OtherMatchTree/state").toByteArray();
    ui->OtherMatchTree->header()->restoreState(state);

    state = settings.value("ArchiveMatchTree/state").toByteArray();
    ui->ArchiveMatchTree->header()->restoreState(state);

    logColumnsChanged = false;

}

void TSingleLogFrame::showQSOs()
{

   NextContactDetailsTimerTimer( );

   restoreColumns();

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

    refreshMults();

    ui->GJVQSOLogFrame->selectField(0);
    ui->GJVQSOLogFrame->logTabChanged();

//    MultDispFrame->setContest( contest );
    doNextContactDetailsOnLeftClick( );
    MinosLoggerEvents::SendShowOperators();

    updateQSODisplay();

}
void TSingleLogFrame::doNextContactDetailsOnLeftClick( )
{
    bool conleft = LogContainer->isNextContactDetailsOnLeft();

    if (conleft)
    {
        ui->CribSplitter->insertWidget(0, ui->CribSheet);
    }
    else
    {
        ui->CribSplitter->insertWidget(1, ui->CribSheet);
    }
    on_CribSplitter_splitterMoved(0, 0);    // preserve the splitter position
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
            QString snBuff = QString::number( contest->maxSerial + 1 );
            QString locBuff;
            if (contest->location.getValue().size())
            {
                locBuff = "<br>" + contest->location.getValue();
            }
            ui->NextContactDetailsLabel->setText( "<b><center><nobr><p><big>"
                                                  + cb + "</p><h1>"
                                                  + contest->mycall.fullCall.getValue() + "<br>"
                                                  + snBuff + "<br>"
                                                  + contest->myloc.loc.getValue()
                                                  + locBuff);
        }
    }
}
void TSingleLogFrame::on_NextContactDetailsOnLeft()
{
    doNextContactDetailsOnLeftClick();
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
      MatchTreeItem * MatchTreeIndex = ( MatchTreeItem * ) archiveTreeClickIndex.internalPointer();

      transferDetails( MatchTreeIndex );

   }
   else
   {
      if ( otherTreeClickIndex.isValid() && ( xferTree == 0 || xferTree == ui->OtherMatchTree ) )
      {
         MatchTreeItem * MatchTreeIndex = ( MatchTreeItem * ) otherTreeClickIndex.internalPointer();

         transferDetails( MatchTreeIndex );
      }
   }
}
//==============================================================================
void TSingleLogFrame::transferDetails(MatchTreeItem *MatchTreeIndex )
{
    if ( !contest )
    {
       return ;
    }
   // needs to be transferred into QSOLogFrame.cpp
   MatchContact *mc = MatchTreeIndex->getMatchContact();
   BaseContact *bct = mc->getBaseContact();

   if ( bct )
   {
      BaseContestLog * matct = mc->getContactLog();
      ui->GJVQSOLogFrame->transferDetails( bct, matct );
   }
   else
   {
       ListContact *lct = mc->getListContact();
       if (lct)
       {
           ContactList * matct = mc->getContactList();
           ui->GJVQSOLogFrame->transferDetails( lct, matct );
       }
   }
}
//---------------------------------------------------------------------------

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
   refreshMults();
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
//---------------------------------------------------------------------------
void TSingleLogFrame::showThisMatchQSOs( TMatchCollection *matchCollection )
{
    thisMatchModel.initialise(ThisMatch, matchCollection);
    ui->ThisMatchTree->setModel(&thisMatchModel);
    ui->ThisMatchTree->setFirstColumnSpanned( 0, QModelIndex(), true );
    ui->ThisMatchTree->expandAll();
    restoreColumns();

}
void TSingleLogFrame::showOtherMatchQSOs( TMatchCollection *matchCollection )
{
    if (matchCollection->contactCount())
        ui->GJVQSOLogFrame->setXferEnabled(true);
    otherMatchModel.initialise(OtherMatch, matchCollection);
    ui->OtherMatchTree->setModel(&otherMatchModel);
    ui->OtherMatchTree->expandAll();
    restoreColumns();
    for(int i = 0; i < otherMatchModel.rowCount(); i++)
    {
        ui->OtherMatchTree->setFirstColumnSpanned( i, QModelIndex(), true );
    }
    connect(ui->OtherMatchTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(on_OtherMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &)));
}
void TSingleLogFrame::showMatchList( TMatchCollection *matchCollection )
{
    if (matchCollection->contactCount())
        ui->GJVQSOLogFrame->setXferEnabled(true);
    archiveMatchModel.initialise(ArchiveMatch, matchCollection);
    ui->ArchiveMatchTree->setModel(&archiveMatchModel);
    ui->ArchiveMatchTree->expandAll();
    restoreColumns();
    for(int i = 0; i < archiveMatchModel.rowCount(); i++)
    {
        ui->ArchiveMatchTree->setFirstColumnSpanned( i, QModelIndex(), true );
    }
    connect(ui->ArchiveMatchTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(on_ArchiveMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &)));
}
//---------------------------------------------------------------------------
void TSingleLogFrame::on_ReplaceThisLogList( TMatchCollection *matchCollection, BaseContestLog* )
{
    if (contest == TContestApp::getContestApp() ->getCurrentContest())
        showThisMatchQSOs( matchCollection );
}
void TSingleLogFrame::on_ReplaceOtherLogList( TMatchCollection *matchCollection, BaseContestLog* )
{
    if (contest == TContestApp::getContestApp() ->getCurrentContest())
        showOtherMatchQSOs( matchCollection );
}

void TSingleLogFrame::on_ReplaceListList(TMatchCollection *matchCollection , BaseContestLog *)
{
    if (contest == TContestApp::getContestApp() ->getCurrentContest())
        showMatchList( matchCollection );
}
//---------------------------------------------------------------------------

void TSingleLogFrame::on_ScrollToDistrict( const QString &qth, BaseContestLog* )
{
    DistrictEntry * dist = MultLists::getMultLists() ->searchDistrict( qth );
    if ( dist )
    {
       unsigned int district_ind = MultLists::getMultLists() ->getDistListIndexOf( dist );
       ui->districtFrame->scrollToDistrict( district_ind, true );
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
       ui->dxccFrame->scrollToCountry( ctry_ind, true );
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
    ui->dxccFrame->reInitialiseCountries();
    ui->districtFrame->reInitialiseDistricts();
}

void TSingleLogFrame::updateTrees()
{
   qsoModel.reset();
   refreshMults();
}
void TSingleLogFrame::initFilters()
{
   filterClickEnabled = false;  // stop them being saved while we are setting up

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
void TSingleLogFrame::onFiltersChanged()
{
    if (contest)
    {
        initFilters();
        ui->dxccFrame->reInitialiseCountries();
        ui->districtFrame->reInitialiseDistricts();
        ui->locFrame->reInitialiseLocators();
        ui->StatsFrame->reInitialiseStats();
    }
}

void TSingleLogFrame::saveFilters()
{
    if ( filterClickEnabled )
    {
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
    ui->LogAreaSplitter->setHandleWidth(splitterHandleWidth);

    state = settings.value("ArchiveSplitter/state").toByteArray();
    ui->ArchiveSplitter->restoreState(state);
    ui->ArchiveSplitter->setHandleWidth(splitterHandleWidth);

    state = settings.value("TopSplitter/state").toByteArray();
    ui->TopSplitter->restoreState(state);
    ui->TopSplitter->setHandleWidth(splitterHandleWidth);

    state = settings.value("CribSplitter/state").toByteArray();
    ui->CribSplitter->restoreState(state);
    ui->CribSplitter->setHandleWidth(splitterHandleWidth);

    state = settings.value("MultSplitter/state").toByteArray();
    ui->MultSplitter->restoreState(state);
    ui->MultSplitter->setHandleWidth(splitterHandleWidth);
}
void TSingleLogFrame::onSplittersChanged()
{
    splittersChanged = true;
}

void TSingleLogFrame::on_StackedMults_currentChanged(int /*arg1*/)
{
    ui->StatsFrame->reInitialiseStats();
}

void TSingleLogFrame::on_LogAreaSplitter_splitterMoved(int /*pos*/, int /*index*/)
{
    QByteArray state = ui->LogAreaSplitter->saveState();
    QSettings settings;
    settings.setValue("LogAreaSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}

void TSingleLogFrame::on_ArchiveSplitter_splitterMoved(int /*pos*/, int /*index*/)
{
    QByteArray state = ui->ArchiveSplitter->saveState();
    QSettings settings;
    settings.setValue("ArchiveSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}

void TSingleLogFrame::on_TopSplitter_splitterMoved(int /*pos*/, int /*index*/)
{
    QByteArray state = ui->TopSplitter->saveState();
    QSettings settings;
    settings.setValue("TopSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}

void TSingleLogFrame::on_CribSplitter_splitterMoved(int /*pos*/, int /*index*/)
{
    QByteArray state = ui->CribSplitter->saveState();
    QSettings settings;
    settings.setValue("CribSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}

void TSingleLogFrame::on_MultSplitter_splitterMoved(int /*pos*/, int /*index*/)
{
    QByteArray state = ui->MultSplitter->saveState();
    QSettings settings;
    settings.setValue("MultSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}
void TSingleLogFrame::on_sectionResized(int, int, int)
{
    QSettings settings;
    QByteArray state;

    state = ui->QSOTable->horizontalHeader()->saveState();
    settings.setValue("QSOTable/state", state);

    state = ui->ThisMatchTree->header()->saveState();
    settings.setValue("ThisMatchTree/state", state);

    state = ui->OtherMatchTree->header()->saveState();
    settings.setValue("OtherMatchTree/state", state);

    state = ui->ArchiveMatchTree->header()->saveState();
    settings.setValue("ArchiveMatchTree/state", state);

    MinosLoggerEvents::SendLogColumnsChanged();
}
void TSingleLogFrame::onLogColumnsChanged()
{
    logColumnsChanged = true;
}
void TSingleLogFrame::on_OtherMatchTreeSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    xferTree = ui->OtherMatchTree;
    otherTreeClickIndex = selected.indexes().at(0);
}

void TSingleLogFrame::on_ArchiveMatchTreeSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    xferTree = ui->ArchiveMatchTree;
    archiveTreeClickIndex = selected.indexes().at(0);
}
void TSingleLogFrame::on_ThisMatchTree_doubleClicked(const QModelIndex &index)
{
    MatchTreeItem * MatchTreeIndex = ( MatchTreeItem * ) index.internalPointer();

    MatchContact *mc = MatchTreeIndex->getMatchContact();
    BaseContact *bct = mc->getBaseContact();

    if ( bct )
    {
       EditContact( bct );
    }
}

void TSingleLogFrame::on_OtherMatchTree_doubleClicked(const QModelIndex &/*index*/)
{
    MinosLoggerEvents::SendXferPressed();;
}

void TSingleLogFrame::on_ArchiveMatchTree_doubleClicked(const QModelIndex &/*index*/)
{
    MinosLoggerEvents::SendXferPressed();
}
void TSingleLogFrame::goNextUnfilled()
{
   BaseContact * nuc = contest->findNextUnfilledContact( );
   if ( nuc )
   {
      TQSOEditDlg qdlg(this, false, true );
      qdlg.setContest( contest );
      qdlg.setFirstContact( nuc );
      qdlg.exec();
      contest->scanContest();

      //LogMonitor->QSOTree->Invalidate();
      refreshMults();
      //LogMonitor->QSOTree->Repaint();
      ui->GJVQSOLogFrame->startNextEntry();
   }
   else
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "No unfilled contacts" );
   }

}
void TSingleLogFrame::on_NextUnfilled(BaseContestLog *ct)
{
    if (ct == contest)
    {
       goNextUnfilled();
    }
}

void TSingleLogFrame::goSerial( )
{
    static int serial = 0;
    do
    {
       if ( serial == -1 )
          serial = 0;
       if ( !enquireDialog( this, "Please give serial wanted", serial ) )
          return ;
    }
    while ( serial == -1 );

    DisplayContestContact *cfu = 0;
    for ( LogIterator i = contest->ctList.begin(); i != contest->ctList.end(); i++ )
    {
        bool ok;
        int s = ( *i ) ->serials.getValue().toInt(&ok );
       if ( ok && serial == s )
       {
          cfu = dynamic_cast<DisplayContestContact *>( *i );
          break;
       }
    }

    if ( cfu )
    {
       EditContact( cfu );
    }
    else
       MinosParameters::getMinosParameters() ->mshowMessage( "Serial number " + QString::number( serial ) + " not found" );
}

void TSingleLogFrame::on_GoToSerial(BaseContestLog *ct)
{
    if (ct == contest)
    {
       goSerial();
    }
}

void TSingleLogFrame::on_SetMode(QString,BaseContestLog*)
{

}

void TSingleLogFrame::on_SetFreq(QString,BaseContestLog*)
{

}

void TSingleLogFrame::on_RotatorState(QString s, BaseContestLog *ct)
{
    ui->GJVQSOLogFrame->setRotatorState(s);
}

//=============================================================================
QSOGridModel::QSOGridModel():contest(0)
{}
QSOGridModel::~QSOGridModel()
{}
void QSOGridModel::reset()
{
    beginResetModel();
    endResetModel();
}

void QSOGridModel::initialise(BaseContestLog * pcontest )
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
//=============================================================================
static GridColumn ThisMatchTreeColumns[ THISMATCHTREECOLS ] =
   {
      GridColumn( egTime, "XXXXXXXXXX", "UTC", taLeftJustify ),               // time
      GridColumn( egCall, "MMMMMMMMMMM", "Callsign", taLeftJustify ),         // call
      GridColumn( egRSTTx, "599XXX", "RepTx", taLeftJustify ),                 // RST
      GridColumn( egSNTx, "1234X", "SnTx", taLeftJustify /*taRightJustify*/ ),   // serial
      GridColumn( egRSTRx, "599XXX", "RepRx", taLeftJustify ),                 // RST
      GridColumn( egSNRx, "1234X", "SnRx", taLeftJustify /*taRightJustify*/ ),   // Serial
      GridColumn( egLoc, "MM00MM00X", "Loc", taLeftJustify ),            // LOC
      GridColumn( egBrg, "3601X", "brg", taLeftJustify ),                // bearing
      GridColumn( egScore, "12345XX", "dist", taLeftJustify /*taRightJustify*/ ),  // score
      GridColumn( egExchange, "XXXXXXXXXXXXXXXX", "Exchange", taLeftJustify ),    // QTH
      GridColumn( egComments, "XXXXXXXXXXXXXXXX", "Comments", taLeftJustify )     // comments
   };
//---------------------------------------------------------------------------
static GridColumn OtherMatchTreeColumns[ OTHERMATCHTREECOLS ] =
   {
      GridColumn( egCall, "MMMMMMMMMMM", "Callsign", taLeftJustify ),         // call
      GridColumn( egLoc, "MM00MM00X", "Loc", taLeftJustify ),            // LOC
      GridColumn( egBrg, "3601X", "brg", taLeftJustify ),                // bearing
      GridColumn( egScore, "12345XX", "dist", taLeftJustify /*taRightJustify*/ ),  // score
      GridColumn( egExchange, "XXXXXXXXXXXXXXXX", "Exchange", taLeftJustify ),    // QTH
      GridColumn( egComments, "XXXXXXXXXXXXXXXX", "Comments", taLeftJustify )     // comments
   };
//---------------------------------------------------------------------------
static GridColumn ArchiveMatchTreeColumns[ ARCHIVEMATCHTREECOLS ] =
   {
      GridColumn( egCall, "MMMMMMMMMMM", "Callsign", taLeftJustify ),         // call
      GridColumn( egLoc, "MM00MM00X", "Loc", taLeftJustify ),            // LOC
      GridColumn( egBrg, "3601X", "brg", taLeftJustify ),                // bearing
      GridColumn( egScore, "12345XX", "dist", taLeftJustify /*taRightJustify*/ ),  // score
      GridColumn( egExchange, "XXXXXX", "Exchange", taLeftJustify ),     // exchange
      GridColumn( egComments, "XXXX", "Comments", taLeftJustify )     // comments
   };
//---------------------------------------------------------------------------

MatchTreeItem::MatchTreeItem(MatchTreeItem *parent, BaseMatchContest *matchContest, MatchContact *matchContact)
    :parent(parent), matchContest(matchContest), matchContact(matchContact), row(-1)
{

}

MatchTreeItem::~MatchTreeItem()
{
    for (unsigned int i = 0; i < children.size(); i++)
    {
        delete children[i];
        children[i] = 0;
    }
}

bool MatchTreeItem::isMatchLine()
{
    return matchContact != 0;
}
MatchTreeItem *MatchTreeItem::getParent()
{
    return parent;
}

MatchContact *MatchTreeItem::getMatchContact()
{
    return matchContact;
}
BaseMatchContest *MatchTreeItem::getMatchContest()
{
    return matchContest;
}


QSOMatchGridModel::QSOMatchGridModel():rootItem(0), match(0), type(ThisMatch)
{}
QSOMatchGridModel::~QSOMatchGridModel()
{
    delete rootItem;
    delete match;
}

void QSOMatchGridModel::initialise(MatchType t, TMatchCollection *pmatch )
{
   beginResetModel();
   type = t;

   if (match)
   {
       delete match;
       match = 0;
   }
   if (rootItem)
   {
       delete rootItem;
       rootItem = 0;
   }

   if (pmatch == 0 || pmatch->contactCount() == 0)
   {
       endResetModel();
       delete pmatch;
       return;
   }

   match = pmatch;  // preserve all the tree
   rootItem = new MatchTreeItem(0, 0, 0);
   rootItem->setRow(0);
   for (ContestMatchIterator i = pmatch->matchList.begin(); i != pmatch->matchList.end(); i++)
   {
       MatchTreeItem *ci = new MatchTreeItem(rootItem, (*i), 0);
       rootItem->addChild(ci); // also sets row
       //(*i) is *BaseMatchContest
       for (int j = 0; j < (*i)->getContactCount(); j++)
       {
           MatchTreeItem *mi = new MatchTreeItem(ci, (*i), (*i)->pcontactAt(j));
           ci->addChild(mi);
       }
   }
   endResetModel();
}
QVariant QSOMatchGridModel::data( const QModelIndex &index, int role ) const
{
    QModelIndex p = index.parent();

    MatchTreeItem *thisItem = static_cast<MatchTreeItem*>(index.internalPointer());

    MatchTreeItem *parentItem;
    if (p.isValid())
        parentItem = static_cast<MatchTreeItem*>(p.internalPointer());
    else
        parentItem = rootItem;

    int row = index.row();
    int column = index.column();

    if ( row >= parentItem->childCount() )
        return QVariant();

    BaseMatchContest *matchContest = thisItem->getMatchContest();
    MatchContact * mct = thisItem->getMatchContact();
    BaseContact *ct = 0;
    ListContact *lct = 0;

    if (mct)
    {
        ct = mct->getBaseContact();
        lct = mct->getListContact();
    }

    if (role == Qt::BackgroundRole)
    {
        if (ct)
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
        }
        return QVariant();
    }
    if ( role != Qt::DisplayRole && role != Qt::EditRole )
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (type == ArchiveMatch)
        {
            ContactList *contest = matchContest->getContactList();
            if (lct)
            {
                if( column >= 0 && column < columnCount(p))
                {
                    BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
                    QString cell;
                    contest->getMatchField( lct, ArchiveMatchTreeColumns[ column ].fieldId, cell, ct );     // col 0 is the tree lines
                    return cell;
                }
            }
            else
            {
                if (column == 0)
                {
                    return contest->name;
                }
            }
        }
        else
        {
            BaseContestLog *contest = matchContest->getContactLog();
            if (ct)
            {
                if( column >= 0 && column < columnCount(p))
                {

                    QString line;
                    BaseContestLog * act = TContestApp::getContestApp() ->getCurrentContest();


                    if (type == ThisMatch)
                        contest->getMatchField( ct, ThisMatchTreeColumns[ column ].fieldId, line, act );     // col 0 is the tree lines
                    else
                        contest->getMatchField( ct, OtherMatchTreeColumns[ column ].fieldId, line, act );     // col 0 is the tree lines

                    if (type == ThisMatch)
                    {
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
                    }
                    return line;
                }
            }
            else
            {
                if (column == 0)
                {
                    if (type == ThisMatch)
                        return "Current contest" + TMatchThread::getThisMatchStatus();
                    if (type == OtherMatch)
                        return contest->name.getValue();
                }
            }
        }
    }
    return QVariant();
}
QVariant QSOMatchGridModel::headerData( int section, Qt::Orientation orientation,
                     int role ) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        QString cell;
        switch (type)
        {
        case ThisMatch:
            cell = ThisMatchTreeColumns[ section ].title;
            break;

        case OtherMatch:
            cell = OtherMatchTreeColumns[ section ].title;
            break;

        case ArchiveMatch:
            cell = ArchiveMatchTreeColumns[ section ].title;
            break;
        }

        return cell;
    }
    return QVariant();
}

QModelIndex QSOMatchGridModel::index( int row, int column, const QModelIndex &parent) const
{

    if (!hasIndex(row, column, parent))
        return QModelIndex();

    MatchTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<MatchTreeItem*>(parent.internalPointer());

    MatchTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex QSOMatchGridModel::parent( const QModelIndex &index ) const
{
    MatchTreeItem *parentItem = (static_cast<MatchTreeItem*>(index.internalPointer()))->getParent();
    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->getRow(), 0, parentItem);
}

int QSOMatchGridModel::rowCount( const QModelIndex &parent ) const
{
    if ( !rootItem)
        return 0;

    if (!parent.isValid())
        return rootItem->childCount();

    MatchTreeItem *parentItem = static_cast<MatchTreeItem*>(parent.internalPointer());
    return parentItem->childCount();
}

int QSOMatchGridModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    int cols =0;
    switch (type)
    {
    case ThisMatch:
        cols = THISMATCHTREECOLS;
        break;

    case OtherMatch:
        cols = OTHERMATCHTREECOLS;
        break;

    case ArchiveMatch:
        cols = ARCHIVEMATCHTREECOLS;
        break;
    }
    return cols;
}

