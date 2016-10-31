#include "base_pch.h"

#include "MinosRPC.h"

#include "chatmain.h"
#include "ui_chatmain.h"

TMinosChatForm *MinosChatForm = 0;

TMinosChatForm::TMinosChatForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    enableTrace( "./TraceLog", "MinosChat_" );

    trace("Arguments");
    QCoreApplication *app = QCoreApplication::instance();
    foreach (QString arg, app->arguments())
    {
        trace(arg);
    }
    trace("End of Arguments");

    createCloseEvent();

    MinosChatForm = this;

    connect(&SyncTimer, SIGNAL(timeout()), this, SLOT(SyncTimerTimer()));
    SyncTimer.start(100);

    MinosRPC *rpc = MinosRPC::getMinosRPC();

    connect(rpc, SIGNAL(clientCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_response(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_request(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    rpc->setAppName(rpcConstants::chatApp);
    rpc->subscribe(rpcConstants::StationCategory);
}

TMinosChatForm::~TMinosChatForm()
{
    delete ui;
}
//---------------------------------------------------------------------------

void TMinosChatForm::logMessage( QString s )
{
   trace( s );
}

void TMinosChatForm::closeEvent(QCloseEvent *event)
{
    MinosRPCObj::clearRPCObjects();
    XMPPClosedown();
    SyncTimerTimer( );
    // and tidy up all loose ends

    /*
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    */
    QWidget::closeEvent(event);
}
void TMinosChatForm::resizeEvent(QResizeEvent * event)
{
    /*
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    */
    QWidget::resizeEvent(event);
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

bool syncstat = false;
std::vector<QString> chatQueue;
QString stateIndicator[] =
{
   "Av",
   "NA",
   "NC"
};
void TMinosChatForm::syncStations()
{
   if ( syncstat )
   {
      syncstat = false;
      ui->StationList->clear();
      for ( std::vector<Server>::iterator i = serverList.begin(); i != serverList.end(); i++ )
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
   QString sdt = dt.toString( "hh:MM:ss " ) + mess;
   chatQueue.push_back(sdt);
}
void TMinosChatForm::syncChat()
{
   for ( std::vector<QString>::iterator i = chatQueue.begin(); i != chatQueue.end(); i++ )
   {
      ui->ChatMemo->append( (*i) );
      trace("syncChat " + (*i));
   }
   chatQueue.clear();
}
//---------------------------------------------------------------------------
QString stateList[] =
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
      if ( an.getCategory() == rpcConstants::StationCategory )
      {
         logMessage( QString(stateIndicator[an.getState()]) + " " + an.getKey() + " " + an.getValue() );
         std::vector<Server>::iterator stat;
         for ( stat = serverList.begin(); stat != serverList.end(); stat++ )
         {
            if ((*stat).name == an.getKey())
            {
               if ((*stat).state != an.getState())
               {
                  (*stat).state = an.getState();
                  QString mess = an.getKey() + " changed state to " + stateList[an.getState()];
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
            s.name = an.getKey();
            s.ip = an.getValue();
            s.state = an.getState();
            serverList.push_back( s );
            QString mess = an.getKey() + " changed state to " + stateList[an.getState()];
            addChat( mess );
            syncstat = true;
         }
      }
   }
}
//---------------------------------------------------------------------------

void TMinosChatForm::on_response( bool /*err*/, QSharedPointer<MinosRPCObj> /*mro*/, const QString & /*from*/ )
{
   // call back says OK/not OK
}
//---------------------------------------------------------------------------
void TMinosChatForm::on_request(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
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

                   QSharedPointer<RPCParam>st(new RPCParamStruct);
                   st->addMember( true, rpcConstants::ChatResult );
                   mro->clearCallArgs();
                   mro->getCallArgs() ->addParam( st );
                   mro->queueResponse( from );
                }
            }
      }
   }
}

void TMinosChatForm::on_SendButton_clicked()
{
    // We need to send the message to all connected stations
    for ( std::vector<Server>::iterator i = serverList.begin(); i != serverList.end(); i++ )
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

    if (Key == Qt::Key_Return)
    {
        ui->SendButton->clicked();
    }
    if (Key == Qt::Key_Escape)
    {
        ui->ChatEdit->clear();
    }
}
