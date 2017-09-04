
#include "MatchTreesFrame.h"
#include "ui_MatchTreesFrame.h"
#include "focuswatcher.h"
#include "htmldelegate.h"
#include "MatchThread.h"

MatchTreesFrame::MatchTreesFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MatchTreesFrame),
  xferTree( 0 )
{
    ui->setupUi(this);

#ifdef Q_OS_ANDROID
    splitterHandleWidth = 20;
#else
    splitterHandleWidth = 6;
#endif

    ui->ThisMatchTree->setModel(&thisMatchModel);
    ui->ThisMatchTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->ThisMatchTree->setItemDelegate( new HtmlDelegate );

    ui->OtherMatchTree->setModel(&otherMatchModel);
    ui->OtherMatchTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->OtherMatchTree->setItemDelegate( new HtmlDelegate );

    ui->ArchiveMatchTree->setModel(&archiveMatchModel);
    ui->ArchiveMatchTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    ui->ArchiveMatchTree->setItemDelegate( new HtmlDelegate );

    connect(&MinosLoggerEvents::mle, SIGNAL(MatchStarting(BaseContestLog*)), this, SLOT(on_MatchStarting(BaseContestLog*)));
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceThisLogList(TMatchCollection*,BaseContestLog*)), this, SLOT(on_ReplaceThisLogList(TMatchCollection*,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceOtherLogList(TMatchCollection*,BaseContestLog*)), this, SLOT(on_ReplaceOtherLogList(TMatchCollection*,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceListList(TMatchCollection*,BaseContestLog*)), this, SLOT(on_ReplaceListList(TMatchCollection*,BaseContestLog*)), Qt::QueuedConnection);

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

}

MatchTreesFrame::~MatchTreesFrame()
{
    delete ui;
}
void MatchTreesFrame::setContest(BaseContestLog *ct)
{
    contest = ct;
}
void MatchTreesFrame::restoreColumns()
{
    QSettings settings;
    QByteArray state;

    state = settings.value("ThisMatchTree/state").toByteArray();
    ui->ThisMatchTree->header()->restoreState(state);

    state = settings.value("OtherMatchTree/state").toByteArray();
    ui->OtherMatchTree->header()->restoreState(state);

    state = settings.value("ArchiveMatchTree/state").toByteArray();
    ui->ArchiveMatchTree->header()->restoreState(state);
}
MatchTreeItem * MatchTreesFrame::getXferItem()
{
   // transfer from current match

   // copy relevant parts of match contact to screen contact
   if ( archiveTreeClickIndex.isValid() && ( xferTree == 0 || xferTree == ui->ArchiveMatchTree ) )
   {
      MatchTreeItem * MatchTreeIndex = static_cast< MatchTreeItem * >(archiveTreeClickIndex.internalPointer());

      return MatchTreeIndex;

   }
   else
   {
      if ( otherTreeClickIndex.isValid() && ( xferTree == 0 || xferTree == ui->OtherMatchTree ) )
      {
         MatchTreeItem * MatchTreeIndex = static_cast< MatchTreeItem * > (otherTreeClickIndex.internalPointer());

         return  MatchTreeIndex;
      }
   }
   return 0;
}
void MatchTreesFrame::on_MatchStarting(BaseContestLog *ct)
{
      // clear down match trees
    if (contest == ct)
    {
      xferTree = 0;

      thisMatchModel.currentModel = false;
      otherMatchModel.currentModel = false;
      archiveMatchModel.currentModel = true;

      matchTreeClickIndex = QModelIndex();
      otherTreeClickIndex = QModelIndex();
      archiveTreeClickIndex = QModelIndex();

      emit setXferEnabled(false);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MatchTreesFrame::showThisMatchQSOs( TMatchCollection *matchCollection )
{
    thisMatchModel.initialise(ThisMatch, matchCollection);
    ui->ThisMatchTree->setModel(&thisMatchModel);
    ui->ThisMatchTree->setFirstColumnSpanned( 0, QModelIndex(), true );
    ui->ThisMatchTree->expandAll();
    restoreColumns();
}
void MatchTreesFrame::showOtherMatchQSOs( TMatchCollection *matchCollection )
{
    if (matchCollection->contactCount())
        emit setXferEnabled(true);
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
void MatchTreesFrame::showMatchList( TMatchCollection *matchCollection )
{
    if (matchCollection->contactCount())
        emit setXferEnabled(true);
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
void MatchTreesFrame::on_ReplaceThisLogList( TMatchCollection *matchCollection, BaseContestLog* )
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        showThisMatchQSOs( matchCollection );
}
void MatchTreesFrame::on_ReplaceOtherLogList( TMatchCollection *matchCollection, BaseContestLog* )
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        showOtherMatchQSOs( matchCollection );
}

void MatchTreesFrame::on_ReplaceListList(TMatchCollection *matchCollection , BaseContestLog *)
{
    if (contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        showMatchList( matchCollection );
}
void MatchTreesFrame::getSplitters()
{
    QSettings settings;
    QByteArray state;

    state = settings.value("ArchiveSplitter/state").toByteArray();
    ui->ArchiveSplitter->restoreState(state);
    ui->ArchiveSplitter->setHandleWidth(splitterHandleWidth);
}
void MatchTreesFrame::on_ArchiveSplitter_splitterMoved(int /*pos*/, int /*index*/)
{
    QByteArray state = ui->ArchiveSplitter->saveState();
    QSettings settings;
    settings.setValue("ArchiveSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}
void MatchTreesFrame::on_sectionResized(int, int, int)
{
    QSettings settings;
    QByteArray state;

    state = ui->ThisMatchTree->header()->saveState();
    settings.setValue("ThisMatchTree/state", state);

    state = ui->OtherMatchTree->header()->saveState();
    settings.setValue("OtherMatchTree/state", state);

    state = ui->ArchiveMatchTree->header()->saveState();
    settings.setValue("ArchiveMatchTree/state", state);

    MinosLoggerEvents::SendLogColumnsChanged();
}
void MatchTreesFrame::on_OtherMatchTreeSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    xferTree = ui->OtherMatchTree;
    otherMatchModel.currentModel = true;
    archiveMatchModel.currentModel = false;

    otherTreeClickIndex = selected.indexes().at(0);
    ui->ArchiveMatchTree->repaint();
}
void MatchTreesFrame::onOtherMatchTreeFocused(QObject *, bool in, QFocusEvent * )
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

void MatchTreesFrame::on_ArchiveMatchTreeSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    xferTree = ui->ArchiveMatchTree;
    archiveMatchModel.currentModel = true;
    otherMatchModel.currentModel = false;

    archiveTreeClickIndex = selected.indexes().at(0);
     ui->OtherMatchTree->viewport()->repaint();
}
void MatchTreesFrame::onArchiveMatchTreeFocused(QObject *, bool in, QFocusEvent * )
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

void MatchTreesFrame::on_ThisMatchTree_doubleClicked(const QModelIndex &index)
{
    MatchTreeItem * MatchTreeIndex = static_cast< MatchTreeItem *>(index.internalPointer());

    QSharedPointer<MatchContact> mc = MatchTreeIndex->getMatchContact();
    QSharedPointer<BaseContact> bct = mc->getBaseContact();

    if ( bct )
    {
       emit editContact( bct );
    }
}

void MatchTreesFrame::on_OtherMatchTree_doubleClicked(const QModelIndex &/*index*/)
{
    emit xferPressed();;
}

void MatchTreesFrame::on_ArchiveMatchTree_doubleClicked(const QModelIndex &/*index*/)
{
    emit xferPressed();;
}
