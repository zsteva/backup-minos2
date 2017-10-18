
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
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceThisLogList(SharedMatchCollection,BaseContestLog*,QString)), this, SLOT(on_ReplaceThisLogList(SharedMatchCollection,BaseContestLog*,QString)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceOtherLogList(SharedMatchCollection,BaseContestLog*,QString)), this, SLOT(on_ReplaceOtherLogList(SharedMatchCollection,BaseContestLog*,QString)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ReplaceListList(SharedMatchCollection,BaseContestLog*,QString)), this, SLOT(on_ReplaceListList(SharedMatchCollection,BaseContestLog*,QString)), Qt::QueuedConnection);

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

    connect(this, SIGNAL(thisTreeClicked()), this, SLOT(afterThisTreeClicked()), Qt::QueuedConnection);
    connect(this, SIGNAL(matchTreeClicked()), this, SLOT(afterMatchTreeClicked()), Qt::QueuedConnection);
    connect(this, SIGNAL(archiveTreeClicked()), this, SLOT(afterArchiveTreeClicked()), Qt::QueuedConnection);
}

MatchTreesFrame::~MatchTreesFrame()
{
    delete ui;
}
void MatchTreesFrame::setBaseName(QString b)
{
    baseName = b;
}

void MatchTreesFrame::setContest(BaseContestLog *ct)
{
    contest = ct;
}
void MatchTreesFrame::restoreColumns()
{
    QSettings settings;
    QByteArray state;

    state = settings.value(baseName + "/ThisMatchTree/state").toByteArray();
    ui->ThisMatchTree->header()->restoreState(state);

    state = settings.value(baseName + "/OtherMatchTree/state").toByteArray();
    ui->OtherMatchTree->header()->restoreState(state);

    state = settings.value(baseName + "/ArchiveMatchTree/state").toByteArray();
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
void MatchTreesFrame::showThisMatchQSOs( SharedMatchCollection matchCollection )
{
    thisMatchModel.initialise(ThisMatch, matchCollection);
    ui->ThisMatchTree->setModel(&thisMatchModel);
    ui->ThisMatchTree->setFirstColumnSpanned( 0, QModelIndex(), true );
    ui->ThisMatchTree->expandAll();
    restoreColumns();
}
void MatchTreesFrame::showOtherMatchQSOs(SharedMatchCollection matchCollection )
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

    connect(ui->OtherMatchTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(on_OtherMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &)), Qt::UniqueConnection);

    if (otherMatchModel.firstIndex.isValid())
    {
        QItemSelectionModel *m = ui->OtherMatchTree->selectionModel();
        m->select(otherMatchModel.firstIndex, QItemSelectionModel::Select|QItemSelectionModel::Rows);
    }
}
void MatchTreesFrame::showMatchList( SharedMatchCollection matchCollection )
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

    connect(ui->ArchiveMatchTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(on_ArchiveMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &)), Qt::UniqueConnection);

    if (archiveMatchModel.firstIndex.isValid())
    {
        QItemSelectionModel *m = ui->ArchiveMatchTree->selectionModel();
        m->select(archiveMatchModel.firstIndex, QItemSelectionModel::Select|QItemSelectionModel::Rows);
    }
}
void MatchTreesFrame::on_ReplaceThisLogList(SharedMatchCollection matchCollection, BaseContestLog*, QString b )
{
    if (b == baseName && contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        showThisMatchQSOs( matchCollection );
}
void MatchTreesFrame::on_ReplaceOtherLogList( SharedMatchCollection matchCollection, BaseContestLog*, QString b )
{
    if (b == baseName && contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        showOtherMatchQSOs( matchCollection );
}

void MatchTreesFrame::on_ReplaceListList(SharedMatchCollection matchCollection , BaseContestLog *, QString b)
{
    if (b == baseName && contest && contest == TContestApp::getContestApp() ->getCurrentContest())
        showMatchList( matchCollection );
}
void MatchTreesFrame::getSplitters()
{
    QSettings settings;
    QByteArray state;

    state = settings.value(baseName + "/ArchiveSplitter/state").toByteArray();
    ui->ArchiveSplitter->restoreState(state);
    ui->ArchiveSplitter->setHandleWidth(splitterHandleWidth);
}
void MatchTreesFrame::on_ArchiveSplitter_splitterMoved(int /*pos*/, int /*index*/)
{
    QByteArray state = ui->ArchiveSplitter->saveState();
    QSettings settings;
    settings.setValue(baseName + "/ArchiveSplitter/state", state);
    MinosLoggerEvents::SendSplittersChanged();
}
void MatchTreesFrame::on_sectionResized(int, int, int)
{
    QSettings settings;
    QByteArray state;

    state = ui->ThisMatchTree->header()->saveState();
    settings.setValue(baseName + "/ThisMatchTree/state", state);

    state = ui->OtherMatchTree->header()->saveState();
    settings.setValue(baseName + "/OtherMatchTree/state", state);

    state = ui->ArchiveMatchTree->header()->saveState();
    settings.setValue(baseName + "/ArchiveMatchTree/state", state);

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
      GridColumn( egComments, "XXXXXXXXXXXXXXXX", "Comments", taLeftJustify ),     // comments
      GridColumn( egFrequency, "1.296.123.456XXX", "Freq", taLeftJustify ),
      GridColumn( egRotatorHeading, "XXXXXX", "Rot Heading", taLeftJustify ),
      GridColumn( egRigName, "XXXXXX", "Rig", taLeftJustify )
   };
//---------------------------------------------------------------------------
static GridColumn OtherMatchTreeColumns[ OTHERMATCHTREECOLS ] =
   {
      GridColumn( egTime, "XXXXXXXXXX", "UTC", taLeftJustify ),               // time
      GridColumn( egCall, "MMMMMMMMMMM", "Callsign", taLeftJustify ),         // call
      GridColumn( egLoc, "MM00MM00X", "Loc", taLeftJustify ),            // LOC
      GridColumn( egBrg, "3601X", "brg", taLeftJustify ),                // bearing
      GridColumn( egScore, "12345XX", "dist", taLeftJustify /*taRightJustify*/ ),  // score
      GridColumn( egExchange, "XXXXXXXXXXXXXXXX", "Exchange", taLeftJustify ),    // QTH
      GridColumn( egComments, "XXXXXXXXXXXXXXXX", "Comments", taLeftJustify ),     // comments
      GridColumn( egFrequency, "1.296.123.456XXX", "Freq", taLeftJustify ),
      GridColumn( egRotatorHeading, "XXXXXX", "Rot Heading", taLeftJustify ),
    GridColumn( egRigName, "XXXXXX", "Rig", taLeftJustify )
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
}

void QSOMatchGridModel::initialise(MatchType t, SharedMatchCollection pmatch )
{
   beginResetModel();
   type = t;
   firstIndex = QModelIndex();

   match.reset();

   if (rootItem)
   {
       delete rootItem;
       rootItem = 0;
   }

   if (pmatch->contactCount() == 0)
   {
       endResetModel();
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

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;       // but HtmlDelegate overrides


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
                        QString name = contest->name.getValue();
                        QString band = contest->band.getValue();
                        QString cell = "[" + band + "] " + name;
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
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;
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



void MatchTreesFrame::on_ThisMatchTree_clicked(const QModelIndex &index)
{
    matchTreeClickIndex = index;
    emit thisTreeClicked();
}

void MatchTreesFrame::on_OtherMatchTree_clicked(const QModelIndex &/*index*/)
{
    // let "selected" sort things out
    emit matchTreeClicked();
}

void MatchTreesFrame::on_ArchiveMatchTree_clicked(const QModelIndex &/*index*/)
{
    // let "selected" sort things out
    emit archiveTreeClicked();
}
void MatchTreesFrame::afterThisTreeClicked()
{
    if ( matchTreeClickIndex.isValid() )
    {
       MatchTreeItem * MatchTreeIndex = static_cast< MatchTreeItem * > (matchTreeClickIndex.internalPointer());

       QSharedPointer<MatchContact> mc = MatchTreeIndex->getMatchContact();
       QSharedPointer<BaseContact> bct = mc->getBaseContact();

       QString bearing = QString::number(bct->bearing);
       MinosLoggerEvents::SendBrgStrToRot(bearing);
    }
}
void MatchTreesFrame::afterMatchTreeClicked()
{
    if ( otherTreeClickIndex.isValid() )
    {
       MatchTreeItem * MatchTreeIndex = static_cast< MatchTreeItem * > (otherTreeClickIndex.internalPointer());

       QSharedPointer<MatchContact> mc = MatchTreeIndex->getMatchContact();
       QSharedPointer<BaseContact> bct = mc->getBaseContact();

       QString bearing = QString::number(bct->bearing);
       MinosLoggerEvents::SendBrgStrToRot(bearing);
    }
}
void MatchTreesFrame::afterArchiveTreeClicked()
{
    if ( archiveTreeClickIndex.isValid() )
    {
       MatchTreeItem * MatchTreeIndex = static_cast< MatchTreeItem * >(archiveTreeClickIndex.internalPointer());
       QSharedPointer<MatchContact> mc = MatchTreeIndex->getMatchContact();
       ListContact *lct = mc->getListContact();

       QString bearing = lct->getField(egBrg, contest);
       MinosLoggerEvents::SendBrgStrToRot(bearing);
    }
}
