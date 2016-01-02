#include "base_pch.h"

#include "chatmain.h"
#include "ui_chatmain.h"

TMinosChatForm *MinosChatForm = 0;

TMinosChatForm::TMinosChatForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    enableTrace( "./TraceLog", "MinosChat_" );

    MinosChatForm = this;

    connect(&LogTimer, SIGNAL(timeout()), this, SLOT(LogTimerTimer()));
    connect(&ConnectTimer, SIGNAL(timeout()), this, SLOT(ConnectTimerTimer()));
    LogTimer.start(100);
    ConnectTimer.start(1000);
}

TMinosChatForm::~TMinosChatForm()
{
    delete ui;
}
void TMinosChatForm::logMessage( QString s )
{
   trace( s );
}
/*static*/
void TMinosChatForm::makeRPCObjects()
{
   RPCPubSub::initialisePubSub( new TRPCCallback <TMinosChatForm> ( MinosChatForm, &TMinosChatForm::notifyCallback ) );
   MinosRPCObj::addObj( new RPCChatControlClient( new TRPCCallback <TMinosChatForm> ( MinosChatForm, &TMinosChatForm::chatClientCallback ) ) );
   MinosRPCObj::addObj( new RPCChatControlServer( new TRPCCallback <TMinosChatForm> ( MinosChatForm, &TMinosChatForm::chatServerCallback ) ) );
}

void TMinosChatForm::closeEvent(QCloseEvent *event)
{
    MinosRPCObj::clearRPCObjects();
    XMPPClosedown();
    LogTimerTimer( );
    // and tidy up all loose ends
    CsGuard::ClearDown();

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

void TMinosChatForm::LogTimerTimer(  )
{
   syncStations();
   syncChat();
   // check log queue; if anything on it then log to main window
   while ( true )
   {
      LogEvent * ev = deQueueLog();
      if ( ev )
      {
         logMessage( "Log : " + ev->eMessageType + " " + ev->eMessage );
         delete ev;
      }
      else
         break;
   }

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
   CsGuard g;
   if ( syncstat )
   {
      syncstat = false;
      ui->StationList->clear();
      for ( std::vector<Server>::iterator i = serverList.begin(); i != serverList.end(); i++ )
      {
         // should we link an object with the state, and owner draw
         ui->StationList->addItem( stateIndicator[(*i).state] + " " + (*i).name );
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
   CsGuard g;
   for ( std::vector<QString>::iterator i = chatQueue.begin(); i != chatQueue.end(); i++ )
   {
      ui->ChatMemo->append( (*i) );
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
void TMinosChatForm::notifyCallback( bool err, MinosRPCObj *mro, const QString &from )
{
   logMessage( "Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
   AnalysePubSubNotify an( err, mro );

   if ( an.getOK() )
   {
      if ( an.getCategory() == "Station" )
      {
         CsGuard g;
         QString key = an.getKey();
         QString value = an.getValue();
         PublishState state = an.getState();
         logMessage( QString(stateIndicator[state]) + " " + key + " " + value );
         std::vector<Server>::iterator stat;
         for ( stat = serverList.begin(); stat != serverList.end(); stat++ )
         {
            if ((*stat).name == key)
            {
               if ((*stat).state != state)
               {
                  (*stat).state = state;
                  QString mess = key + " changed state to " + stateList[state];
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
            s.name = key;
            s.ip = value;
            s.state = state;
            serverList.push_back( s );
            QString mess = key + " changed state to " + stateList[state];
            addChat( mess );
            syncstat = true;
         }
      }
   }
}
//---------------------------------------------------------------------------

void TMinosChatForm::ConnectTimerTimer( )
{
   if ( !connected && checkServerReady() )
   {
      makeRPCObjects();
      XMPPInitialise( "Chat" );
      connected = true;
      RPCPubSub::subscribe( "Station" );
   }
}
//---------------------------------------------------------------------------
void TMinosChatForm::chatClientCallback( bool /*err*/, MinosRPCObj */*mro*/, const QString &/*from*/ )
{
   // call back says OK/not OK
}
//---------------------------------------------------------------------------
void TMinosChatForm::chatServerCallback( bool err, MinosRPCObj *mro, const QString &from )
{
   trace( "chat callback from " + from + ( err ? ":Error" : ":Normal" ) );

   if ( !err )
   {
      boost::shared_ptr<RPCParam> psName;
      boost::shared_ptr<RPCParam>psValue;
      RPCArgs *args = mro->getCallArgs();
      if ( args->getStructArgMember( 0, "Name", psName ) && args->getStructArgMember( 0, "Value", psValue ) )
      {
         QString Name;
         QString Value;

         if ( psName->getString( Name ) && psValue->getString( Value ) )
         {
            boost::shared_ptr<RPCParam>st(new RPCParamStruct);

            if ( Name == "SendChatMessage" )
            {
               CsGuard g;
               // add to chat window
               QString mess = from + " : " + Value;
               addChat( mess );
            }
            st->addMember( true, "ChatResult" );
            mro->clearCallArgs();
            mro->getCallArgs() ->addParam( st );
            mro->queueResponse( from );
         }
      }
   }
}

void TMinosChatForm::on_SendButton_clicked()
{
    // We need to send the message to all connected stations
    for ( std::vector<Server>::iterator i = serverList.begin(); i != serverList.end(); i++ )
    {
       RPCChatControlClient rpc( 0 );
       boost::shared_ptr<RPCParam>st(new RPCParamStruct);
       boost::shared_ptr<RPCParam>sName(new RPCStringParam( "SendChatMessage" ));
       boost::shared_ptr<RPCParam>sValue(new RPCStringParam( ui->ChatEdit->text() ));
       st->addMember( sName, "Name" );
       st->addMember( sValue, "Value" );
       rpc.getCallArgs() ->addParam( st );
       rpc.queueCall( "chat@" + ( (*i).name ) );
    }
    ui->ChatEdit->clear(); // otherwise it is a pain!
    ui->ChatEdit->setFocus();
}
