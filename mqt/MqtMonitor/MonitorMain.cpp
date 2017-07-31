#include "base_pch.h"
#include <QAbstractItemModel>
#include <QModelIndex>

#include "MinosRPC.h"

#include "MonitoredLog.h"
#include "MonitoringFrame.h"
#include "MonitorMain.h"
#include "ui_MonitorMain.h"

//=============================================================================================
class MonitorParameters : public MinosParametersAdapter
{
      int p1;
      int p2;
   public:
      MonitorParameters() : p1( 10 ), p2( 60 )
      {}
      ~MonitorParameters()
      {}
      virtual int getStatsPeriod1();
      virtual int getStatsPeriod2();
      virtual void setStatsPeriod1( int );
      virtual void setStatsPeriod2( int );
      virtual bool yesNoMessage( QWidget* Owner, const QString &mess ) override;
      virtual void mshowMessage(const QString &mess, QWidget* Owner = 0 ) override;
};
static MonitorParameters mp;

int MonitorParameters::getStatsPeriod1()
{
   return p1;
}
int MonitorParameters::getStatsPeriod2()
{
   return p2;
}
void MonitorParameters::setStatsPeriod1( int p )
{
   p1 = p;
}
void MonitorParameters::setStatsPeriod2( int p )
{
   p2 = p;
}
bool MonitorParameters::yesNoMessage( QWidget* Owner, const QString &mess )
{
   return mShowYesNoMessage( Owner, mess );
}
void MonitorParameters::mshowMessage( const QString &mess, QWidget* Owner )
{
   mShowMessage( mess, Owner );
}
//=============================================================================================
enum NodeType{entRoot, entServer, entLog};
class TreeNode
{
    NodeType ntype;
protected:
    QString NodeName;
    int childNo;
    TreeNode *parentItem;

public:
    TreeNode(NodeType sn, TreeNode *parent, QString name):ntype(sn), NodeName(name), parentItem(parent)
    {
        if (parent)
            parent->nodes.push_back(this);
    }
    virtual ~TreeNode()
    {
        clear();
    }

    virtual NodeType GetNodeType()
    {
        return ntype;
    }
    QVector<TreeNode *> nodes;
    int childNumber() const;

    int find( const TreeNode *t ) const;

    void clear();

    virtual QString Name ( void )
    {
        return NodeName;
    }
    virtual QString data( int column ) = 0;

    TreeNode *parent();
    TreeNode *child( int number );
    int childCount() const;

};
class RootTreeNode:public TreeNode
{
public:
    RootTreeNode(QString name):TreeNode(entRoot, 0, "Root")
    {

    }
    virtual QString data( int column );
};
class ServerTreeNode:public TreeNode
{
public:
    ServerTreeNode(TreeNode *parent, QString name):TreeNode(entServer, parent, name)
    {

    }
    virtual QString data( int column );
};
class LogTreeNode:public TreeNode
{
public:
    LogTreeNode(TreeNode *parent, QString name):TreeNode(entLog, parent, name)
    {

    }
    virtual QString data( int column );
};
void TreeNode::clear()
{
    for ( QVector<TreeNode *>::iterator tn = nodes.begin(); tn != nodes.end(); tn++ )
    {
        delete ( *tn );
    }
    nodes.clear();
}

int TreeNode::find( const TreeNode *t ) const
{
    int i = 0;
    for ( auto v : nodes )
    {
        if ( v == t )
            return i;
        i++;
    }
    return 0;
}

TreeNode *TreeNode::parent()
{
    return parentItem;
}

TreeNode *TreeNode::child( int number )
{
    return nodes[ number ];
}

int TreeNode::childCount() const
{
    return nodes.size();
}
int TreeNode::childNumber() const
{
    if ( parentItem )
    {
        return parentItem->find( this );
    }
    return 0;
}
QString RootTreeNode::data(int column)
{
    return Name();
}
QString ServerTreeNode::data(int column)
{
    return Name();
}
QString LogTreeNode::data(int column)
{
    return Name();
}
/*
char *stateList[] =
{
   "P",
   "R",
   "NC"
};

if ( *( int * ) Sender->GetNodeData( Node ) == 0 )
   CellText = (stationList[ Node->Index ] ->stationName + " " + stateList[stationList[ Node->Index ] ->state]).c_str();
else
{
   CellText = (QString(stateList[stationList[ Node->Parent->Index ] ->slotList[ Node->Index ] ->getState()])
               + " " + stationList[ Node->Parent->Index ] ->slotList[ Node->Index ] ->getPublishedName()).c_str();
}
*/
class MonitorTreeModel: public QAbstractItemModel
{
    TreeNode *rootData;
public:
    MonitorTreeModel();
    ~MonitorTreeModel();
    void setRoot(  TreeNode *root );
    void clear();

    QVariant data( const QModelIndex &index, int role ) const Q_DECL_OVERRIDE;
    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    QModelIndex parent( const QModelIndex &index ) const Q_DECL_OVERRIDE;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    TreeNode *getItem( const QModelIndex &index ) const;
};
MonitorTreeModel::MonitorTreeModel()
        : QAbstractItemModel( 0 ), rootData( 0 )
{}
MonitorTreeModel::~MonitorTreeModel()
{
    delete rootData;
}
void MonitorTreeModel::clear()
{
    beginResetModel();

    delete rootData;
    rootData = 0;

    endResetModel();
}
void MonitorTreeModel::setRoot(  TreeNode *root )
{
    beginResetModel();
    delete rootData;
    rootData = root;
    // And we probably need to tell the view that everything has changed
    endResetModel();
}

int MonitorTreeModel::columnCount( const QModelIndex & /* parent */ ) const
{
    return 1;
}

QVariant MonitorTreeModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    TreeNode *tni =  static_cast< TreeNode * >( index.internalPointer());

    if ( role != Qt::DisplayRole && role != Qt::EditRole )
        return QVariant();

    TreeNode *item = getItem( index );

    return item->data( index.column() );
}


QModelIndex MonitorTreeModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( parent.isValid() && parent.column() != 0 )
        return QModelIndex();

    TreeNode *parentItem = getItem( parent );

    if ( parentItem && row < parentItem->childCount() )
    {
        TreeNode * childItem = parentItem->child( row );
        if ( childItem )
            return createIndex( row, column, childItem );
    }
    return QModelIndex();
}
QModelIndex MonitorTreeModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return QModelIndex();

    TreeNode *childItem = getItem( index );
    TreeNode *parentItem = childItem->parent();

    if ( parentItem == rootData )
        return QModelIndex();

    return createIndex( parentItem->childNumber(), 0, parentItem );
}
int MonitorTreeModel::rowCount( const QModelIndex &parent ) const
{
    TreeNode * parentItem = getItem( parent );

    if ( parentItem )
        return parentItem->childCount();

    return 0;
}
TreeNode *MonitorTreeModel::getItem( const QModelIndex &index ) const
{
    if ( index.isValid() )
    {
        TreeNode * item = static_cast<TreeNode *>( index.internalPointer() );
        if ( item )
            return item;
    }
    return rootData;
}

class MonitoredStation
{
   private:

      // Plus we want the control (keyer) state, frequency, ?bandmap etc

   public:
      QString stationName;
      PublishState state;
      QVector< MonitoredLog *> slotList;

      MonitoredStation()
      {}
      ~MonitoredStation()
      {}   // need to delete slots...
};
//=============================================================================================
struct MonitoredStationCmp
{
   QString cmpstr;
   MonitoredStationCmp( const QString &s ) : cmpstr( s )
   {}

   bool operator() ( MonitoredStation * &s1 ) const
   {
      return s1->stationName.compare( cmpstr, Qt::CaseInsensitive ) == 0;
   }
};
struct MonitoredLogCmp
{
   QString cmpstr;
   MonitoredLogCmp( const QString &s ) : cmpstr( s )
   {}

   bool operator() ( MonitoredLog * &s1 ) const
   {
      return s1->getPublishedName().compare(cmpstr, Qt::CaseInsensitive ) == 0;
   }
};
//=============================================================================================


MonitorMain::MonitorMain(QWidget *parent) :
    QMainWindow(parent), syncstat(false),
    ui(new Ui::MonitorMain)
{
    ui->setupUi(this);
    connect(&stdinReader, SIGNAL(stdinLine(QString)), this, SLOT(onStdInRead(QString)));
    stdinReader.start();

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    createCloseEvent();

    QSettings settings;
    QByteArray geometry = settings.value("geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    MultLists::getMultLists(); // make sure everything is loaded

    treeModel = new MonitorTreeModel();
    ui->monitorTree->setModel(treeModel);

    monitorTimer = new QTimer();

    connect(monitorTimer, SIGNAL(timeout()), this, SLOT(on_monitorTimeout()));

    monitorTimer->start(100);


    MinosRPC *rpc = MinosRPC::getMinosRPC(rpcConstants::monitorApp, true);

    connect(rpc, SIGNAL(clientCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_response(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_request(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    rpc->subscribe(rpcConstants::StationCategory);
    rpc->subscribe(rpcConstants::LocalStationCategory);

}

MonitorMain::~MonitorMain()
{
    delete ui;
    for ( QVector<MonitoredStation *>::iterator i = stationList.begin(); i != stationList.end(); i++ )
    {
       for ( QVector< MonitoredLog *>::iterator j = ( *i ) ->slotList.begin(); j != ( *i ) ->slotList.end(); j++ )
       {
          delete ( *j );
       }
       ( *i ) ->slotList.clear();
       delete ( *i );
    }
    stationList.clear();
    delete MultLists::getMultLists();
}
void MonitorMain::closeEvent(QCloseEvent *event)
{
    // and tidy up all loose ends

    MinosRPCObj::clearRPCObjects();
    XMPPClosedown();

    QWidget::closeEvent(event);
}
void MonitorMain::moveEvent(QMoveEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QWidget::moveEvent(event);
}
void MonitorMain::resizeEvent(QResizeEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QWidget::resizeEvent(event);
}
void MonitorMain::changeEvent( QEvent* e )
{
    if( e->type() == QEvent::WindowStateChange )
    {
        QSettings settings;
        settings.setValue("geometry", saveGeometry());
    }
}

void MonitorMain::on_closeButton_clicked()
{
    close();
}
void MonitorMain::logMessage( const QString &s )
{
   trace( s );
}
void MonitorMain::on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
    // pubsub notify
    logMessage( "Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
    AnalysePubSubNotify an( err, mro );

    if ( an.getOK() )
    {
       PublishState state = an.getState();
       QString key = an.getKey();
       QString value = an.getValue();

       if ( an.getCategory() == rpcConstants::LocalStationCategory )
       {
          // This state better not be anything other than published!
          localServerName = an.getKey();
          RPCPubSub::subscribe( rpcConstants::StationCategory );  //want ALL keys - but do it once we know who WE are!
       }
       if ( an.getCategory() == rpcConstants::StationCategory )
       {
          logMessage( "Station " + key + " " + value );
          QVector<MonitoredStation *>::iterator stat = std::find_if( stationList.begin(), stationList.end(), MonitoredStationCmp( key ) );

          if (state != psRevoked)
          {
             if ( stat == stationList.end() )
             {
                MonitoredStation * ms = new MonitoredStation();
                ms->stationName = key;
                ms->state = state;
                stationList.push_back( ms );
                RPCPubSub::subscribeRemote( key, rpcConstants::monitorLogCategory);
             }
             else
             {
                (*stat)->state = state;
             }
          }
          else
          {
             (*stat)->state = state;
          }
       }
       if ( an.getCategory() == rpcConstants::monitorLogCategory )
       {
          QString server = an.getServer();
          if ( server.size() == 0 )
          {
             // it is for us...
             server = localServerName;
          }

          QString logval = server + " : " + key ;
          logMessage( "ContestLog " + logval + " " + value );

          QVector<MonitoredStation *>::iterator stat = std::find_if( stationList.begin(), stationList.end(), MonitoredStationCmp( server ) );
          if ( stat != stationList.end() )
          {
             QVector< MonitoredLog *>::iterator log = std::find_if( ( *stat ) ->slotList.begin(), ( *stat ) ->slotList.end(), MonitoredLogCmp( key ) );
             if (state == psPublished)
             {
                if ( log == ( *stat ) ->slotList.end() )
                {
                   MonitoredLog * ml = new MonitoredLog();
                   ml->initialise( server, key );
                   ml->setExpectedStanzaCount( value.toInt() );
                   ml->setState(state);
                   ( *stat ) ->slotList.push_back( ml );
                   syncstat = true;

                }
                else
                {
                   ( *log ) ->setExpectedStanzaCount( value.toInt() );
                   (*log)->setState(state);
                }
             }
             else
             {
                if ( log != ( *stat ) ->slotList.end() )
                {
                   (*log)->setState(state);
                }
             }
          }
       }
    }
}
//---------------------------------------------------------------------------

void MonitorMain::on_response( bool /*err*/, QSharedPointer<MinosRPCObj> /*mro*/, const QString & /*from*/ )
{
   // call back says OK/not OK
}
//---------------------------------------------------------------------------
void MonitorMain::on_request(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
    logMessage( "logger subscribe client callback from " + from + ( err ? ":Error" : ":Normal" ) );
    if ( !err )
    {
       // This will return stanza id, pubname, and stanza content
       QSharedPointer<RPCParam> psLogName;
       QSharedPointer<RPCParam> psStanzaData;
       QSharedPointer<RPCParam> psStanza;
       QSharedPointer<RPCParam> psResult;
       RPCArgs *args = mro->getCallArgs();
       if ( args->getStructArgMember( 0, "LogName", psLogName )
            && args->getStructArgMember( 0, "LoggerResult", psResult )
            && args->getStructArgMember( 0, "Stanza", psStanza )
            && args->getStructArgMember( 0, "StanzaData", psStanzaData )
          )
       {
          QString logName;
          QString stanzaData;
          bool result;
          int stanza;

          if ( psLogName->getString( logName ) && psStanzaData->getString( stanzaData )
               && psStanza->getInt( stanza ) && psResult->getBoolean( result )
             )
          {
             logMessage( "Name " + logName + " stanza " + QString::number( stanza ) );
             // Find the matching MonitoredLog and send the stanza their for processing
             for ( QVector<MonitoredStation *>::iterator i = stationList.begin(); i != stationList.end(); i++ )
             {
                // "from" is something like Logger@dev-station
                if ( "Logger@" + ( *i ) ->stationName == from )
                {
                   for ( QVector<MonitoredLog *>::iterator j = ( *i ) ->slotList.begin(); j != ( *i ) ->slotList.end(); j++ )
                   {
                      if ( ( *j ) ->getPublishedName() == logName )
                      {
                         logMessage( "||" + stanzaData + "||" );
                         ( *j ) ->processLogStanza( stanza, stanzaData );
                         return ;
                      }
                   }
                }
             }

          }
       }
    }
}
void MonitorMain::showContestScore( const QString &score )
{
   ui->scoreLabel->setText(score);
}


void MonitorMain::syncStations()
{
   // Here we want to subscribe to the loggers of the notified stations
   // Shouldn't matter if we end up doing it twice

   // Strictly I suppose we could ignore ourselves, but normally we won't run
   // a full monitor on a logging computer - we need a module that can
   // manage single stations for that.

   // And this module must make use of that single station monitor!

   // Probably it is basically the "MonitoredStation" class as a model, with a viewer
   // and maybe a controller...

  if ( syncstat )
   {
      syncstat = false;

      TreeNode *root = new RootTreeNode("Root");
      for ( QVector<MonitoredStation *>::iterator i = stationList.begin(); i != stationList.end(); i++ )
      {
          TreeNode *snode = new ServerTreeNode(root, (*i)->stationName);
          for ( QVector<MonitoredLog *>::iterator j = ( *i ) ->slotList.begin(); j != ( *i ) ->slotList.end(); j++ )
          {
              TreeNode *lnode = new LogTreeNode(snode, (*j)->getPublishedName());
          }
      }
      treeModel->setRoot(root);
   }
}
void MonitorMain::addSlot( MonitoredLog *ct )
{
   static int namegen = 0;
   QString baseFName = ExtractFileName( ct->getPublishedName() );

   MonitoringFrame *f = new MonitoringFrame( this );
   f->setObjectName( QString( "LogFrame" ) + QString::number(namegen++));

   int tno = ui->contestPageControl->addTab(f, baseFName);
   ui->contestPageControl->setCurrentWidget(ui->contestPageControl->widget(tno));
   ui->contestPageControl->setTabToolTip(tno, ct->getPublishedName());


   f->initialise( ct->getContest() );
   ct->setFrame( f );
   f->showQSOs();
}

MonitoringFrame *MonitorMain::findCurrentLogFrame()
{
    QWidget *w = ui->contestPageControl->currentWidget();
    MonitoringFrame *f = dynamic_cast<MonitoringFrame *>(w);
    return f;
}
MonitoringFrame *MonitorMain::findContestPage( BaseContestLog *ct )
{
   // we need to find the embedded frame...
   if ( !ui->contestPageControl->count() || !ct )
      return 0;
   int pc = ui->contestPageControl->count();
   for ( int i = 0; i < pc; i++ )
   {
       QWidget *tw = ui->contestPageControl->widget(i);
       MonitoringFrame *f = dynamic_cast<MonitoringFrame *>(tw);
       if (f)
       {
           if (f->getContest() == ct)
               return f;
       }
   }
   return 0;
}

bool nolog( MonitoredLog *ip )
{
   if ( ip == 0 )
      return true;
   else
      return false;
}

void MonitorMain::on_monitorTimeout()
{
    for ( QVector<MonitoredStation *>::iterator i = stationList.begin(); i != stationList.end(); i++ )
    {
       for ( QVector<MonitoredLog *>::iterator j = ( *i ) ->slotList.begin(); j != ( *i ) ->slotList.end(); j++ )
       {
          if ((*j)->getState() == psRevoked)
          {
             QWidget *cttab = findContestPage( (*j)->getContest() );
             if ( cttab )
             {
                delete cttab;
             }
             // take it out of the slot list and close it
             // and we need to redo the list
             delete (*j);
             (*i)->slotList.erase(j);
             syncstat = true;

          }
          else
          {
             ( *j ) ->checkMonitor();
          }
       }
       if (syncstat)
       {
          syncStations();
       }
    }
    MonitoringFrame *f = findCurrentLogFrame();
    QString statbuf;
    if ( f && f->getContest() )
    {
       f->getContest() ->setScore( statbuf );
    }
    showContestScore( statbuf );
}
void MonitorMain::on_monitorTree_doubleClicked(const QModelIndex &index)
{
    // apply double click to node MonitorTreeClickNode
    TreeNode * sel = static_cast< TreeNode *>(index.internalPointer());

    if (!sel)
    {
       return;
    }
    if ( sel->GetNodeType() != entLog )
    {
       // station
    }
    else
    {
       MonitoredStation *ms = stationList[ sel->parent()->childNumber() ];
       // log
      MonitoredLog * ml = ms ->slotList[ sel->childNumber()];
      if ( !ml->enabled() )
      {
         ml->startMonitor();
         addSlot( ml );
      }
      else
      {
         MonitoringFrame *cttab = findContestPage( ml->getContest() );
         if ( cttab )
         {
            ui->contestPageControl->setCurrentWidget(cttab);
         }
      }
    }
}

#ifdef RUBBISH
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TMonitorMain::MonitorTreeGetNodeDataSize(
   TBaseVirtualTree */*Sender*/, int &NodeDataSize )
{
   NodeDataSize = sizeof( int );
}
//---------------------------------------------------------------------------

void __fastcall TMonitorMain::MonitorTreeInitNode( TBaseVirtualTree *Sender,
      PVirtualNode ParentNode, PVirtualNode Node,
      TVirtualNodeInitStates &InitialStates )
{
   if ( ParentNode == 0 )
   {
      *( int * ) Sender->GetNodeData( Node ) = 0;
   }
   else
   {
      *( int * ) Sender->GetNodeData( Node ) = 1;
   }
   if ( ParentNode == 0 && stationList[ Node->Index ] ->slotList.size() )
   {
      InitialStates << ivsHasChildren;
   }
   else
   {}
}
//---------------------------------------------------------------------------

void __fastcall TMonitorMain::MonitorTreeInitChildren( TBaseVirtualTree */*Sender*/,
      PVirtualNode Node, DWORD &ChildCount )
{
   ChildCount = stationList[ Node->Index ] ->slotList.size();
}
//---------------------------------------------------------------------------
char *stateList[] =
{
   "P",
   "R",
   "NC"
};
void __fastcall TMonitorMain::MonitorTreeGetText( TBaseVirtualTree *Sender,
      PVirtualNode Node, TColumnIndex /*Column*/, TVSTTextType /*TextType*/,
      WideString &CellText )
{
   #warning if the state is not published we need to change the font, or colour
   if ( *( int * ) Sender->GetNodeData( Node ) == 0 )
      CellText = (stationList[ Node->Index ] ->stationName + " " + stateList[stationList[ Node->Index ] ->state]).c_str();
   else
   {
      CellText = (QString(stateList[stationList[ Node->Parent->Index ] ->slotList[ Node->Index ] ->getState()])
                  + " " + stationList[ Node->Parent->Index ] ->slotList[ Node->Index ] ->getPublishedName()).c_str();
   }
}
//---------------------------------------------------------------------------

void __fastcall TMonitorMain::MonitorTreeDblClick( TObject */*Sender*/ )
{
   // apply double click to node MonitorTreeClickNode
   if (!MonitorTreeClickNode)
   {
      return;
   }
   void *ndata = MonitorTree->GetNodeData( MonitorTreeClickNode );
   if ( *(int *)ndata == 0 )
   {
      // station
   }
   else
   {
      if (MonitorTreeClickNode->Parent->Index > stationList.size() )
      {
         return;
      }
      MonitoredStation *ms = stationList[ MonitorTreeClickNode->Parent->Index ];
      if (MonitorTreeClickNode->Index > ms->slotList.size() )
      {
         return;
      }
      // log
      if (ms->slotList.size() > MonitorTreeClickNode->Index)
      {
         MonitoredLog * ml = ms ->slotList[ MonitorTreeClickNode->Index ];
         if ( !ml->enabled() )
         {
            ml->startMonitor();
            addSlot( ml );
         }
         else
         {
            TTabSheet *cttab = findContestPage( ml->getContest() );
            if ( cttab )
            {
               ContestPageControl->ActivePage = cttab;
            }
         }
      }
   }

}
//---------------------------------------------------------------------------

void __fastcall TMonitorMain::MonitorTreeMouseDown( TObject */*Sender*/,
      TMouseButton /*Button*/, TShiftState /*Shift*/, int X, int Y )
{

   PVirtualNode Node = MonitorTree->GetNodeAt( X, Y );
   MonitorTreeClickNode = Node;
}
//---------------------------------------------------------------------------
TMonitoringFrame *TMonitorMain::findCurrentLogFrame()
{
   // we need to find the embedded frame...
   if ( !ContestPageControl->ActivePage )
      return 0;
   int cc = ContestPageControl->ActivePage->ControlCount;
   for ( int i = 0; i < cc; i++ )
   {
      if ( TMonitoringFrame * f = dynamic_cast<TMonitoringFrame *>( ContestPageControl->ActivePage->Controls[ i ] ) )
      {
         return f;
      }
   }
   return 0;
}
TTabSheet *TMonitorMain::findContestPage( BaseContestLog *ct )
{
   // we need to find the embedded frame...
   if ( !ContestPageControl->ActivePage || !ct )
      return 0;
   int pc = ContestPageControl->PageCount;
   for ( int i = 0; i < pc; i++ )
   {
      int cc = ContestPageControl->Pages[ i ] ->ControlCount;
      for ( int j = 0; j < cc; j++ )
      {
         if ( TMonitoringFrame * f = dynamic_cast<TMonitoringFrame *>( ContestPageControl->Pages[ i ] ->Controls[ j ] ) )
         {
            if ( f->getContest() == ct )
               return ContestPageControl->Pages[ i ];
         }
      }
   }
   return 0;
}

bool nolog( MonitoredLog *ip )
{
   if ( ip == 0 )
      return true;
   else
      return false;
}
void __fastcall TMonitorMain::MonitorTimerTimer( TObject */*Sender*/ )
{
   CsGuard g;
   for ( std::vector<MonitoredStation *>::iterator i = stationList.begin(); i != stationList.end(); i++ )
   {
      for ( std::vector<MonitoredLog *>::iterator j = ( *i ) ->slotList.begin(); j != ( *i ) ->slotList.end(); j++ )
      {
         if ((*j)->getState() == psRevoked)
         {
            TTabSheet *cttab = findContestPage( (*j)->getContest() );
            if ( cttab )
            {
               delete cttab;
            }
            // take it out of the slot list and close it
            // and we need to redo the list
            delete(*j);
            (*j) = 0;
            syncstat = true;

         }
         else
         {
            ( *j ) ->checkMonitor();
         }
      }
      if (syncstat)
      {
         ( *i ) ->slotList.erase(remove_if(( *i ) ->slotList.begin(), ( *i ) ->slotList.end(), nolog), ( *i ) ->slotList.end());
         syncStations();
      }
   }
   TMonitoringFrame *f = findCurrentLogFrame();
   QString statbuf;
   if ( f && f->getContest() )
   {
      f->getContest() ->setScore( statbuf );
   }
   showContestScore( statbuf );
}
//---------------------------------------------------------------------------

void __fastcall TMonitorMain::Close1Click( TObject */*Sender*/ )
{
   //
   TTabSheet * t = ContestPageControl->ActivePage;
   if ( t )
   {
      TMonitoringFrame * f = findCurrentLogFrame();
      BaseContestLog *ct = f->getContest();
      for ( std::vector<MonitoredStation *>::iterator i = stationList.begin(); i != stationList.end(); i++ )
      {
         for ( std::vector<MonitoredLog *>::iterator j = ( *i ) ->slotList.begin(); j != ( *i ) ->slotList.end(); j++ )
         {
            if ( ( *j ) ->getContest() == ct )
            {
               ( *j ) ->stopMonitor();
            }
         }
      }
      t->PageControl = 0;
      delete t;
   }
}
//---------------------------------------------------------------------------

void __fastcall TMonitorMain::FocusCurrentButtonClick(TObject */*Sender*/)
{
//
   TMonitoringFrame *f = findCurrentLogFrame();
   if (f)
   {
      PVirtualNode l = f->LogMonitorFrame->QSOTree->GetLastChild( f->LogMonitorFrame->QSOTree->RootNode );
      f->LogMonitorFrame->QSOTree->FocusedNode = l;
      f->LogMonitorFrame->QSOTree->Selected[ l ] = true;
   }
}
//---------------------------------------------------------------------------
#endif

