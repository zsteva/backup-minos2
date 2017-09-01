#include "logger_pch.h"

#include "MatchThread.h"
#include "BandList.h"
#include "tqsoeditdlg.h"
#include "tentryoptionsform.h"
#include "tsinglelogframe.h"
#include "ui_tsinglelogframe.h"

#include "SendRPCDM.h"
#include "tlogcontainer.h"
#include "focuswatcher.h"
#include "htmldelegate.h"
#include "enqdlg.h"

TSingleLogFrame::TSingleLogFrame(QWidget *parent, BaseContestLog * contest) :
    QFrame(parent),
    ui(new Ui::TSingleLogFrame),
    contest( contest ),
    splittersChanged(false),
    currFreq( 0 ), oldFreq( 0 ),
    lastStanzaCount( 0 ),
    xferTree( 0 ),
    rotatorLoaded(false),
    bandMapLoaded(false),
    keyerLoaded(false),
    radioLoaded(false)

{
    ui->setupUi(this);

    ui->Controlsplitter->setVisible(false);

#ifdef Q_OS_ANDROID
    splitterHandleWidth = 20;
#else
    splitterHandleWidth = 6;
#endif

    LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );

    sendDM = new TSendDM( this, ct );

    qsoModel.initialise(contest);
    ui->QSOTable->setModel(&qsoModel);
    ui->QSOTable->setItemDelegate( new HtmlDelegate );

    ui->QSOTable->resizeColumnsToContents();
    ui->QSOTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ui->ThisMatchTree->setModel(&thisMatchModel);
    ui->ThisMatchTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->ThisMatchTree->setItemDelegate( new HtmlDelegate );

    ui->OtherMatchTree->setModel(&otherMatchModel);
    ui->OtherMatchTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->OtherMatchTree->setItemDelegate( new HtmlDelegate );

    ui->ArchiveMatchTree->setModel(&archiveMatchModel);
    ui->ArchiveMatchTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->ArchiveMatchTree->setItemDelegate( new HtmlDelegate );

    restoreColumns();

    ui->GJVQSOLogFrame->initialise( contest, false );

    connect(&MinosLoggerEvents::mle, SIGNAL(ContestPageChanged()), this, SLOT(on_ContestPageChanged()));
    connect(&MinosLoggerEvents::mle, SIGNAL(XferPressed()), this, SLOT(on_XferPressed()));

    //connect(&MinosLoggerEvents::mle, SIGNAL(BandMapPressed()), this, SLOT(on_BandMapPressed()));



    connect(&MinosLoggerEvents::mle, SIGNAL(TimerDistribution()), this, SLOT(NextContactDetailsTimerTimer()));
    connect(&MinosLoggerEvents::mle, SIGNAL(TimerDistribution()), this, SLOT(PublishTimerTimer()));
    connect(&MinosLoggerEvents::mle, SIGNAL(TimerDistribution()), this, SLOT(HideTimerTimer()));
    connect(&MinosLoggerEvents::mle, SIGNAL(MatchStarting(BaseContestLog*)), this, SLOT(on_MatchStarting(BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(MakeEntry(BaseContestLog*)), this, SLOT(on_MakeEntry(BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(AfterSelectContact(QSharedPointer<BaseContact>, BaseContestLog *)), this, SLOT(on_AfterSelectContact(QSharedPointer<BaseContact>, BaseContestLog *)));
    connect(&MinosLoggerEvents::mle, SIGNAL(AfterLogContact(BaseContestLog *)), this, SLOT(on_AfterLogContact(BaseContestLog *)));


    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceThisLogList(TMatchCollection*,BaseContestLog*)), this, SLOT(on_ReplaceThisLogList(TMatchCollection*,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceOtherLogList(TMatchCollection*,BaseContestLog*)), this, SLOT(on_ReplaceOtherLogList(TMatchCollection*,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceListList(TMatchCollection*,BaseContestLog*)), this, SLOT(on_ReplaceListList(TMatchCollection*,BaseContestLog*)), Qt::QueuedConnection);

    doNextContactDetailsOnLeftClick( true);  // but the sizes are zero...
    getSplitters();

    connect(&MinosLoggerEvents::mle, SIGNAL(LogColumnsChanged()), this, SLOT(onLogColumnsChanged()));
    connect(&MinosLoggerEvents::mle, SIGNAL(SplittersChanged()), this, SLOT(onSplittersChanged()));
    connect(&MinosLoggerEvents::mle, SIGNAL(NextContactDetailsOnLeft()), this, SLOT(on_NextContactDetailsOnLeft()));
    connect(&MinosLoggerEvents::mle, SIGNAL(NextUnfilled(BaseContestLog*)), this, SLOT(on_NextUnfilled(BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(GoToSerial(BaseContestLog*)), this, SLOT(on_GoToSerial(BaseContestLog*)));

    // BandMap Updates

    connect(sendDM, SIGNAL(setBandMapLoaded()), this, SLOT(on_BandMapLoaded()));


    // RigControl Updates
    // From rig controller
    connect(sendDM, SIGNAL(setRadioLoaded()), this, SLOT(on_RadioLoaded()));
    connect(sendDM, SIGNAL(setMode(QString)), this, SLOT(on_SetMode(QString)));
    connect(sendDM, SIGNAL(setFreq(QString)), this, SLOT(on_SetFreq(QString)));
    connect(sendDM, SIGNAL(setRadioName(QString)), this, SLOT(on_SetRadioName(QString)));
    connect(sendDM, SIGNAL(setRadioState(QString)), this, SLOT(on_SetRadioState(QString)));

    // To rig controller
    connect(ui->FKHRigControlFrame, SIGNAL(sendFreqControl(QString)), this, SLOT(sendRadioFreq(QString)));
    connect(ui->GJVQSOLogFrame, SIGNAL(sendModeControl(QString)), this , SLOT(sendRadioMode(QString)));
    connect(ui->FKHRigControlFrame, SIGNAL(sendPassBandStateToControl(int)), this, SLOT(sendRadioPassBandState(int)));

    // Rotator updates
    // From rotator controller
    connect(sendDM, SIGNAL(RotatorLoaded()), this, SLOT(on_RotatorLoaded()));
    connect(sendDM, SIGNAL(RotatorState(QString)), this, SLOT(on_RotatorState(QString)));
    connect(sendDM, SIGNAL(RotatorBearing(QString)), this, SLOT(on_RotatorBearing(QString)));
    connect(sendDM, SIGNAL(RotatorMaxAzimuth(QString)), this, SLOT(on_RotatorMaxAzimuth(QString)));
    connect(sendDM, SIGNAL(RotatorMinAzimuth(QString)), this, SLOT(on_RotatorMinAzimuth(QString)));
    connect(sendDM, SIGNAL(RotatorAntennaName(QString)), this, SLOT(on_RotatorAntennaName(QString)));

    // To rotator controller
    connect(ui->FKHRotControlFrame, SIGNAL(sendRotator(rpcConstants::RotateDirection , int  )),
            this, SLOT(sendRotator(rpcConstants::RotateDirection , int  )));


    connect(sendDM, SIGNAL(setKeyerLoaded()), this, SLOT(on_KeyerLoaded()));


    connect( ui->QSOTable->horizontalHeader(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));

    connect( ui->ThisMatchTree->header(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));
    connect( ui->OtherMatchTree->header(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));
    connect( ui->ArchiveMatchTree->header(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));


    OtherMatchTreeFW = new FocusWatcher(ui->OtherMatchTree);
    connect(OtherMatchTreeFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(onOtherMatchTreeFocused(QObject *, bool, QFocusEvent *)));
    ArchiveMatchTreeFW = new FocusWatcher(ui->ArchiveMatchTree);
    connect(ArchiveMatchTreeFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(onArchiveMatchTreeFocused(QObject *, bool, QFocusEvent *)));

    connect(LogContainer, SIGNAL(sendKeyerPlay( int )), this, SLOT(sendKeyerPlay(int)));
    connect(LogContainer, SIGNAL(sendKeyerRecord( int)), this, SLOT(sendKeyerRecord(int)));
    connect(LogContainer, SIGNAL(sendKeyerTone()), this, SLOT(sendKeyerTone()));
    connect(LogContainer, SIGNAL(sendKeyerTwoTone()), this, SLOT(sendKeyerTwoTone()));
    connect(LogContainer, SIGNAL(sendKeyerStop()), this, SLOT(sendKeyerStop()));

/*
    connect(ui->FKHBandMapFrame, SIGNAL(sendBandMap( QString, QString, QString, QString, QString )),
            this, SLOT(sendBandMap(QString,QString,QString,QString,QString)));
*/


}

TSingleLogFrame::~TSingleLogFrame()
{
    delete ui;
    delete sendDM;

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
      EntryDlg.setWindowTitle("Save imported log as a .minos file");
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
       RPCPubSub::publish( rpcConstants::monitorLogCategory, contest->publishedName, QString::number( 0 ), psRevoked );
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
    ui->stackedInfoFrame->setContest(ct);

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
    doNextContactDetailsOnLeftClick( false );
    MinosLoggerEvents::SendShowOperators();

    updateQSODisplay();

}
void TSingleLogFrame::doNextContactDetailsOnLeftClick(bool keepSizes )
{
    QList<int> sizes = ui->CribSplitter->sizes();

    bool isOnLeft = (ui->CribSplitter->widget(0) == ui->CribSheet);

    bool conleft = LogContainer->isNextContactDetailsOnLeft();

    if (conleft)
    {
        ui->CribSplitter->insertWidget(0, ui->CribSheet);
    }
    else
    {
        ui->CribSplitter->insertWidget(1, ui->CribSheet);
    }

    bool nowOnLeft = (ui->CribSplitter->widget(0) == ui->CribSheet);
    if (isOnLeft != nowOnLeft)
    {
        //they have flipped
        std::reverse(sizes.begin(), sizes.end());
    }
    if (!keepSizes)
    {
        ui->CribSplitter->setSizes(sizes);
    on_CribSplitter_splitterMoved(0, 0);    // preserve the splitter position
    }
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
            QString snBuff = QString("%1").arg( contest->maxSerial + 1, 3, 10, QChar('0') );
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
void TSingleLogFrame::PublishTimerTimer(  )
{
   LoggerContestLog * ct = dynamic_cast<LoggerContestLog *>( contest );
   if ( ct && ct->isMinosFile() && !ct->isUnwriteable() && !ct->isProtected())
   {
      int stanzaCount = contest->getCtStanzaCount();
      if ( lastStanzaCount != stanzaCount )
      {
         // publish this contest details - what to use?
         // category LoggerContestLog
         // name filename(?)
         // value stanzaCount
         RPCPubSub::publish( rpcConstants::monitorLogCategory, contest->publishedName, QString::number( stanzaCount ), psPublished );
         lastStanzaCount = stanzaCount;
      }
   }
}
void TSingleLogFrame::HideTimerTimer(  )
{
    bool controlsLoaded = isBandMapLoaded() || isRadioLoaded() || isRotatorLoaded();

    ui->Controlsplitter->setVisible(controlsLoaded);
}
void TSingleLogFrame::on_NextContactDetailsOnLeft()
{
    doNextContactDetailsOnLeftClick(false);
}
void TSingleLogFrame::updateQSODisplay()
{
   ui->GJVQSOLogFrame->updateQSODisplay();
}
void TSingleLogFrame::on_XferPressed()
{
   // transfer from current match
   if (!contest || contest->isReadOnly() )
      return ;

   // copy relevant parts of match contact to screen contact
   if ( archiveTreeClickIndex.isValid() && ( xferTree == 0 || xferTree == ui->ArchiveMatchTree ) )
   {
      MatchTreeItem * MatchTreeIndex = static_cast< MatchTreeItem * >(archiveTreeClickIndex.internalPointer());

      transferDetails( MatchTreeIndex );

   }
   else
   {
      if ( otherTreeClickIndex.isValid() && ( xferTree == 0 || xferTree == ui->OtherMatchTree ) )
      {
         MatchTreeItem * MatchTreeIndex = static_cast< MatchTreeItem * > (otherTreeClickIndex.internalPointer());

         transferDetails( MatchTreeIndex );
      }
   }
}
//==============================================================================
void TSingleLogFrame::transferDetails(MatchTreeItem *MatchTreeIndex )
{
    if ( !contest  )
    {
       return ;
    }
   // needs to be transferred into QSOLogFrame.cpp
   QSharedPointer<MatchContact> mc = MatchTreeIndex->getMatchContact();

   if (mc)
   {
       QSharedPointer<BaseContact> bct = mc->getBaseContact();

       if ( bct )
       {
          BaseContestLog *matct = mc->getContactLog();
          ui->GJVQSOLogFrame->transferDetails( bct, matct );
       }
       else
       {
           ListContact *lct = mc->getListContact();
           if (lct)
           {
               ContactList *matct = mc->getContactList();
               ui->GJVQSOLogFrame->transferDetails( lct, matct );
           }
       }
   }
}
//---------------------------------------------------------------------------

void TSingleLogFrame::QSOTreeSelectContact( QSharedPointer<BaseContact> lct )
{
   if (lct)
   {
      EditContact( lct );
   }
}
void TSingleLogFrame::on_QSOTable_doubleClicked(const QModelIndex &index)
{
    QSOTreeSelectContact(contest->pcontactAt( index.row() ));
}
void TSingleLogFrame::EditContact( QSharedPointer<BaseContact> lct )
{
   TQSOEditDlg qdlg( this, false, false );
   qdlg.selectContact( contest, lct );

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

      thisMatchModel.currentModel = false;
      otherMatchModel.currentModel = false;
      archiveMatchModel.currentModel = true;

      //ui->ArchiveMatchTree->setBackgroundColor(Qt::lightGray);

      matchTreeClickIndex = QModelIndex();
      otherTreeClickIndex = QModelIndex();
      archiveTreeClickIndex = QModelIndex();

      ui->GJVQSOLogFrame->setXferEnabled(false);
    }
}
//---------------------------------------------------------------------------
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
    int rc = otherMatchModel.rowCount();
    for(int i = 0; i < rc; i++)
    {
        ui->OtherMatchTree->setFirstColumnSpanned( i, QModelIndex(), true );
    }

    if (otherMatchModel.firstIndex.isValid())
    {
        QItemSelectionModel *m = ui->OtherMatchTree->selectionModel();
        m->select(otherMatchModel.firstIndex, QItemSelectionModel::Select|QItemSelectionModel::Rows);
    }
    connect(ui->OtherMatchTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(on_OtherMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &)), Qt::UniqueConnection);
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
    if (archiveMatchModel.firstIndex.isValid())
    {
        QItemSelectionModel *m = ui->ArchiveMatchTree->selectionModel();
        m->select(archiveMatchModel.firstIndex, QItemSelectionModel::Select|QItemSelectionModel::Rows);
    }
    connect(ui->ArchiveMatchTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(on_ArchiveMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &)), Qt::UniqueConnection);
}
//---------------------------------------------------------------------------
ScreenContact &TSingleLogFrame::getScreenEntry()
{
    ui->GJVQSOLogFrame->getScreenEntry();

    ui->GJVQSOLogFrame->calcLoc();

    return ui->GJVQSOLogFrame->screenContact;
}
//---------------------------------------------------------------------------
int TSingleLogFrame::getBearingFrmQSOLog()
{
    return ui->FKHRotControlFrame->getAngle(ui->GJVQSOLogFrame->getBearing());
}

//---------------------------------------------------------------------------
void TSingleLogFrame::on_ReplaceThisLogList( TMatchCollection *matchCollection, BaseContestLog* )
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        showThisMatchQSOs( matchCollection );
}
void TSingleLogFrame::on_ReplaceOtherLogList( TMatchCollection *matchCollection, BaseContestLog* )
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        showOtherMatchQSOs( matchCollection );
}

void TSingleLogFrame::on_ReplaceListList(TMatchCollection *matchCollection , BaseContestLog *)
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        showMatchList( matchCollection );
}
//---------------------------------------------------------------------------


void TSingleLogFrame::on_MakeEntry(BaseContestLog *ct)
{
    if (ct == contest)
    {
       makeEntry( false );
    }
}
void TSingleLogFrame::on_AfterSelectContact( QSharedPointer<BaseContact>lct, BaseContestLog *ct)
{
    if (ct == contest && !lct)
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
    ui->stackedInfoFrame->refreshMults();
}

void TSingleLogFrame::updateTrees()
{
   qsoModel.reset();
   refreshMults();
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

    ui->stackedInfoFrame->getSplitters();
}
void TSingleLogFrame::onSplittersChanged()
{
    splittersChanged = true;
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
    otherMatchModel.currentModel = true;
    archiveMatchModel.currentModel = false;

    otherTreeClickIndex = selected.indexes().at(0);
    ui->ArchiveMatchTree->repaint();
}
void TSingleLogFrame::onOtherMatchTreeFocused(QObject *, bool in, QFocusEvent * )
{
    if (!in)
    {
        ui->ArchiveMatchTree->viewport()->repaint();
        ui->OtherMatchTree->viewport()->repaint();
        return;
    }

    xferTree = ui->OtherMatchTree;
    otherMatchModel.currentModel = true;
    archiveMatchModel.currentModel = false;

    ui->ArchiveMatchTree->viewport()->repaint();
}

void TSingleLogFrame::on_ArchiveMatchTreeSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    xferTree = ui->ArchiveMatchTree;
    archiveMatchModel.currentModel = true;
    otherMatchModel.currentModel = false;

    archiveTreeClickIndex = selected.indexes().at(0);
     ui->OtherMatchTree->viewport()->repaint();
}
void TSingleLogFrame::onArchiveMatchTreeFocused(QObject *, bool in, QFocusEvent * )
{
    if (!in)
    {
        ui->ArchiveMatchTree->viewport()->repaint();
        ui->OtherMatchTree->viewport()->repaint();
        return;
    }

    xferTree = ui->ArchiveMatchTree;
    archiveMatchModel.currentModel = true;
    otherMatchModel.currentModel = false;

     ui->OtherMatchTree->viewport()->repaint();
}

void TSingleLogFrame::on_ThisMatchTree_doubleClicked(const QModelIndex &index)
{
    MatchTreeItem * MatchTreeIndex = static_cast< MatchTreeItem *>(index.internalPointer());

    QSharedPointer<MatchContact> mc = MatchTreeIndex->getMatchContact();
    QSharedPointer<BaseContact> bct = mc->getBaseContact();

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
   QSharedPointer<BaseContact> nuc = contest->findNextUnfilledContact( );
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

    QSharedPointer<BaseContact> cfu;
    for ( LogIterator i = contest->ctList.begin(); i != contest->ctList.end(); i++ )
    {
        bool ok;
        int s = i->wt->serials.getValue().toInt(&ok );
       if ( ok && serial == s )
       {
          cfu = i->wt;
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

//---------------------------------------------------------------------------
void TSingleLogFrame::on_KeyerLoaded()
{
   keyerLoaded = true;
   ui->GJVQSOLogFrame->setKeyerLoaded();
}
bool TSingleLogFrame::isKeyerLoaded()
{
   return keyerLoaded;
}


void TSingleLogFrame::sendKeyerPlay( int fno )
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendKeyerPlay(fno);
}

void TSingleLogFrame::sendKeyerRecord( int fno )
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendKeyerRecord(fno);
}

void TSingleLogFrame::sendBandMap( QString freq, QString call, QString utc, QString loc, QString qth )
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendBandMap(freq, call, utc, loc, qth);
}

void TSingleLogFrame::sendKeyerTone()
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendKeyerTone();
}

void TSingleLogFrame::sendKeyerTwoTone()
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendKeyerTwoTone();
}

void TSingleLogFrame::sendKeyerStop()
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendKeyerStop();
}


//---------------------------------------------------------------------------

// Bandmap

void TSingleLogFrame::on_BandMapLoaded()
{
   bandMapLoaded = true;
   ui->GJVQSOLogFrame->setBandMapLoaded();
}

bool TSingleLogFrame::isBandMapLoaded()
{
   return bandMapLoaded;
}

//---------------------------------------------------------------------------

// RigControl

void TSingleLogFrame::on_SetMode(QString m)
{
    ui->FKHRigControlFrame->setMode(m);
    ui->GJVQSOLogFrame->modeSentFromRig(m);
}

void TSingleLogFrame::on_SetFreq(QString f)
{
    ui->FKHRigControlFrame->setFreq(f);
}


void TSingleLogFrame::on_RadioLoaded()
{
    ui->FKHRigControlFrame->setRadioLoaded();
}

bool TSingleLogFrame::isRadioLoaded()
{
   return radioLoaded;
}

void TSingleLogFrame::on_SetRadioName(QString n)
{

    ui->FKHRigControlFrame->setRadioName(n);

}

void TSingleLogFrame::on_SetRadioState(QString s)
{
    ui->FKHRigControlFrame->setRadioState(s);
}


void TSingleLogFrame::sendRadioFreq(QString freq)
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendRigControlFreq(freq);
}

void TSingleLogFrame::sendRadioMode(QString mode)
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendRigControlMode(mode);
}

void TSingleLogFrame::sendRadioPassBandState(int state)
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendRigControlPassBandState(state);
}


//---------------------------------------------------------------------------

// RotatorControl



void TSingleLogFrame::on_RotatorLoaded()
{
   rotatorLoaded = true;
   ui->FKHRotControlFrame->setRotatorLoaded();
}

bool TSingleLogFrame::isRotatorLoaded()
{
   return rotatorLoaded;
}

void TSingleLogFrame::on_RotatorState(QString s)
{
    ui->FKHRotControlFrame->setRotatorState(s);
}

void TSingleLogFrame::on_RotatorBearing(QString s)
{
    ui->FKHRotControlFrame->setRotatorBearing(s);
}


void TSingleLogFrame::on_RotatorMaxAzimuth(QString s)
{
    ui->FKHRotControlFrame->setRotatorMaxAzimuth(s);
}

void TSingleLogFrame::on_RotatorMinAzimuth(QString s)
{
    ui->FKHRotControlFrame->setRotatorMinAzimuth(s);
}


void TSingleLogFrame::on_RotatorAntennaName(QString s)
{
    ui->FKHRotControlFrame->setRotatorAntennaName(s);
}


void TSingleLogFrame::sendRotator(rpcConstants::RotateDirection direction, int angle )
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        sendDM->sendRotator(direction, angle);
}


//=============================================================================
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

MatchTreeItem::MatchTreeItem(MatchTreeItem *parent, BaseMatchContest *matchContest, QSharedPointer<MatchContact> matchContact)
    :parent(parent), matchContest(matchContest), matchContact(matchContact), row(-1)
{

}

MatchTreeItem::~MatchTreeItem()
{
    for (int i = 0; i < children.size(); i++)
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

QSharedPointer<MatchContact> MatchTreeItem::getMatchContact()
{
    return matchContact;
}
BaseMatchContest *MatchTreeItem::getMatchContest()
{
    return matchContest;
}


QSOMatchGridModel::QSOMatchGridModel():rootItem(0), match(0), type(ThisMatch), currentModel(false)
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
   firstIndex = QModelIndex();

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
   rootItem = new MatchTreeItem(0, 0, QSharedPointer<MatchContact>());
   rootItem->setRow(0);
   for (ContestMatchIterator i = pmatch->contestMatchList.begin(); i != pmatch->contestMatchList.end(); i++)
   {
       MatchTreeItem *ci = new MatchTreeItem(rootItem, i->wt.data(), QSharedPointer<MatchContact>());
       rootItem->addChild(ci); // also sets row
       //(*i) is *BaseMatchContest
       foreach(auto mct, i->wt->contactMatchList)
       {
           MatchTreeItem *mi = new MatchTreeItem(ci, i->wt.data(), mct.wt);
           ci->addChild(mi);
           if (!firstIndex.isValid())
           {
               firstIndex = createIndex(mi->getRow(), 0, mi);
           }
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
    QSharedPointer<MatchContact> mct = thisItem->getMatchContact();
    QSharedPointer<BaseContact> ct;
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
               return static_cast< QColor > ( 0x00FF80C0 );        // Pink(ish)
            }
            else
            {
               if ( ct->getModificationCount() > 1 )
               {
                   return static_cast< QColor > ( 0x00C0DCC0 );    // "money green"
               }
            }
        }
        return QVariant();
    }
    if ( role != Qt::DisplayRole && role != Qt::EditRole )
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        QColor lightRed = QColor(Qt::red).lighter(140);
        if (type == ArchiveMatch)
        {
            const ContactList *contactList = matchContest->getContactList();
            if (lct)
            {
                if( column >= 0 && column < columnCount(p))
                {
                    BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
                    QString cell;
                    contactList->getMatchField( lct, ArchiveMatchTreeColumns[ column ].fieldId, cell, ct );     // col 0 is the tree lines

                    if (currentModel)
                    {
                        cell = HtmlFontColour(lightRed) + "<b>" + cell;
                    }
                    return cell;
                }
            }
            else
            {
                if (column == 0)
                {
                    QString cell = contactList->name;
                    if (currentModel)
                    {
                        cell = HtmlFontColour(lightRed) + "<b>" + cell;
                    }
                    return cell;
                }
            }
        }
        else
        {
            const BaseContestLog *contest = matchContest->getContactLog();
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
                           if ( contest->locMult.getValue() && ct->locCount > 0)
                           {
                               setHighlight = true;
                           }
                           else if ( contest->usesBonus.getValue() && ct->bonus > 0)
                           {
                               switch (ct->bonus)
                               {
                               case 500:  //blue
                                   multhighlight = Qt::blue;
                                   break;
                               case 1000: //green
                                   multhighlight = Qt::darkGreen;
                                   break;
                               case 2000: //red
                                   multhighlight = Qt::red;
                                   break;
                               }

                               setHighlight = true;
                           }
                            break;
                        }
                        if (setHighlight)
                            line = HtmlFontColour(multhighlight) + "<b>" + line;
                    }
                    else
                    {
                        if (currentModel)
                        {
                            line = HtmlFontColour(lightRed) + "<b>" + line;
                        }

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
                    {
                        QString cell = contest->name.getValue();
                        if (currentModel)
                        {
                            cell = HtmlFontColour(lightRed) + "<b>" + cell;
                        }
                        return cell;
                    }
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
        if (section < columnCount())
        {
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


