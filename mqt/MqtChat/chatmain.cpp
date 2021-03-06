#include "base_pch.h"
#include "MinosRPC.h"

#include "chatmain.h"
#include "ui_chatmain.h"

TMinosChatForm::TMinosChatForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

    connect(&SyncTimer, SIGNAL(timeout()), this, SLOT(SyncTimerTimer()));
    SyncTimer.start(100);

    MinosRPC *rpc = MinosRPC::getMinosRPC(rpcConstants::chatApp, false);    // DO NOT use the environment variable - use "Chat" everywhere

    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_serverCall(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    rpc->subscribe(rpcConstants::LocalStationCategory);
}

TMinosChatForm::~TMinosChatForm()
{
    delete ui;
}
void TMinosChatForm::onStdInRead(QString cmd)
{
    trace("Command read from stdin: " + cmd);
    if (cmd.indexOf("ShowServers", Qt::CaseInsensitive) >= 0)
        setShowServers(true);
    if (cmd.indexOf("HideServers", Qt::CaseInsensitive) >= 0)
        setShowServers(false);
}

//---------------------------------------------------------------------------

void TMinosChatForm::logMessage( QString s )
{
   trace( s );
}

void TMinosChatForm::closeEvent(QCloseEvent *event)
{
    // and tidy up all loose ends

    SyncTimerTimer( );

    QWidget::closeEvent(event);
}
void TMinosChatForm::moveEvent(QMoveEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QWidget::moveEvent(event);
}
void TMinosChatForm::resizeEvent(QResizeEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QWidget::resizeEvent(event);
}
void TMinosChatForm::changeEvent( QEvent* e )
{
    if( e->type() == QEvent::WindowStateChange )
    {
        QSettings settings;
        settings.setValue("geometry", saveGeometry());
    }
}
void TMinosChatForm::SyncTimerTimer(  )
{
   syncStations();
   syncChat();

   static bool closed = false;
   if ( !closed )
   {
      if ( checkCloseEvent() )
      {
         closed = true;
         close();
      }
   }
}
//---------------------------------------------------------------------------

static bool syncstat = false;
static QVector<QString> chatQueue;
static QString stateIndicator[] =
{
   "Available",
   "NotAvailable",
   "NoContact"
};
void TMinosChatForm::syncStations()
{
   if ( syncstat )
   {
      syncstat = false;
      ui->StationList->clear();
      for ( QVector<Server>::iterator i = serverList.begin(); i != serverList.end(); i++ )
      {
         // should we link an object with the state, and owner draw
          QString state = stateIndicator[(*i).state] + " " + (*i).name;
         ui->StationList->addItem( state );
         trace("syncStations " + state);
      }
   }
}
void TMinosChatForm::addChat(const QString &mess)
{
   QDateTime dt = QDateTime::currentDateTime();
   QString sdt = dt.toString( "HH:mm:ss " ) + mess;
   chatQueue.push_back(sdt);
}
void TMinosChatForm::syncChat()
{
   for ( QVector<QString>::iterator i = chatQueue.begin(); i != chatQueue.end(); i++ )
   {
      ui->ChatMemo->append( (*i) );
      trace("syncChat " + (*i));
   }
   chatQueue.clear();
}
//---------------------------------------------------------------------------
static QString stateList[] =
{
   "Available",
   "Not Available",
   "No Contact"
};
void TMinosChatForm::on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &/*from*/ )
{
    AnalysePubSubNotify an( err, mro );

    if ( an.getOK() )
    {
      if ( an.getCategory() == rpcConstants::LocalStationCategory)
      {
          QString s = an.getKey();
          RPCPubSub::publish(rpcConstants::ChatCategory, rpcConstants::ChatServer, s, psPublished);
          RPCPubSub::subscribe(rpcConstants::StationCategory);
      }
      if (an.getCategory() == rpcConstants::StationCategory)
      {
          QString key = an.getKey();
          RPCPubSub::subscribeRemote(key, rpcConstants::ChatCategory);
      }

      if ( an.getCategory() == rpcConstants::ChatCategory )
      {
         logMessage( QString(stateIndicator[an.getState()]) + " " + an.getKey() + " " + an.getValue() );
         QVector<Server>::iterator stat;
         for ( stat = serverList.begin(); stat != serverList.end(); stat++ )
         {
            if ((*stat).name == an.getPublisherServer())
            {
               if ((*stat).state != an.getState())
               {
                  (*stat).state = an.getState();
                  QString mess = an.getPublisherServer() + " changed state to " + stateList[an.getState()];
                  addChat( mess );
                  syncstat = true;
               }
               break;
            }
         }

         if ( stat == serverList.end() )
         {
            // We have received notification from a previously unknown station - so report on it
            Server s;
            s.name = an.getPublisherServer();
            s.ip = an.getValue();
            s.state = an.getState();
            serverList.push_back( s );
            QString mess = an.getPublisherServer() + " changed state to " + stateList[an.getState()];
            addChat( mess );
            syncstat = true;
         }
      }
   }
}
//---------------------------------------------------------------------------
void TMinosChatForm::on_serverCall(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
   trace( "chat callback from " + from + ( err ? ":Error" : ":Normal" ) );

   // Should we use QMap to give a list of name/value pairs?
   // BUT the value isn't always the same type - should it be?
   // We could use QVariant, of course...

   if ( !err )
   {
      RPCArgs *args = mro->getCallArgs();

      if (args)
      {
            QSharedPointer<RPCParam> psMess;
            if (args->getStructArgMember(0, rpcConstants::SendChatMessage, psMess))
            {
                QString pmess;
                if (psMess->getString(pmess))
                {
                   // add to chat window
                   QString mess = from + " : " + pmess;
                   addChat( mess );
                }
            }
      }
   }
}

void TMinosChatForm::on_SendButton_clicked()
{
    // We need to send the message to all connected stations
    for ( QVector<Server>::iterator i = serverList.begin(); i != serverList.end(); i++ )
    {
        RPCGeneralClient rpc(rpcConstants::chatMethod);
        QSharedPointer<RPCParam>st(new RPCParamStruct);
        st->addMember( ui->ChatEdit->text(), rpcConstants::SendChatMessage );
        rpc.getCallArgs() ->addParam( st );
        rpc.queueCall( rpcConstants::chatApp + "@" + ( (*i).name ) );
    }
    ui->ChatEdit->clear(); // otherwise it is a pain!
    ui->ChatEdit->setFocus();
}
void TMinosChatForm::keyPressEvent( QKeyEvent* event )
{
    int Key = event->key();

    if (Key == Qt::Key_Return || Key == Qt::Key_Enter)
    {
        ui->SendButton->clicked();
    }
    if (Key == Qt::Key_Escape)
    {
        ui->ChatEdit->clear();
    }
}
