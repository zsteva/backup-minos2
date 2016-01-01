#include "minos_pch.h"

#include "servermain.h"
#include "ui_mainwindow.h"

extern int GetSubscribedCount();
extern int GetPublishedCount();

ServerMain *MinosMainForm = 0;

bool closeApp = false;

ServerMain::ServerMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&LogTimer, SIGNAL(timeout()), this, SLOT(LogTimerTimer()));
    connect(&ScanTimer, SIGNAL(timeout()), this, SLOT(ScanTimerTimer()));

    enableTrace( "./TraceLog", "MinosServer_" );
    MinosServer::getMinosServer();
    clientThread = new GJV_thread( "client", &runClientThread, ( void * ) 0 );
    serverThread = new GJV_thread( "server", &runServerThread, ( void * ) 0 );
    PubSubMain = new TPubSubMain();
    ZConf = new TZConf();
    ZConf->setName( MinosServer::getMinosServer() ->getServerName() );

    LogTimer.start(100);
    ScanTimer.start(20000);
}

ServerMain::~ServerMain()
{
    delete ui;
}
void ServerMain::logMessage( const QString &s )
{
   trace( s );
}
void ServerMain::LogTimerTimer( )
{

   static int lastServerCount = 0;
   static int lastClientCount = 0;
   static int lastSubCount = 0;
   static int lastPubCount = 0;

   MinosServerListener *msl = MinosServerListener::getListener();
   MinosClientListener *mcl = MinosClientListener::getListener();
   int serverCount = msl ? msl->getConnectionCount() : 0;
   int clientCount = mcl ? mcl->getConnectionCount() : 0;
   int pubCount = GetPublishedCount();
   int subCount = GetSubscribedCount();

   if ( lastServerCount != serverCount )
   {
      lastServerCount = serverCount;
      ui->ServerLabel->setText(QString::number(serverCount));
   }
   if ( lastClientCount != clientCount )
   {
      lastClientCount = clientCount;
      ui->ClientLabel->setText(QString::number(clientCount));
   }

   if ( lastSubCount != subCount )
   {
      lastSubCount = subCount;
      ui->SubLabel->setText(QString::number(subCount));
   }
   if ( lastPubCount != pubCount )
   {
      lastPubCount = pubCount;
      ui->PubLabel->setText(QString::number(pubCount));
   }

   bool show = getShowServers();
   if ( !isVisible() && show )
   {
      setVisible(true);
   }
   if ( isVisible() && !show )
   {
      setVisible(false);
   }

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
   if ( closeApp )
      close();

   static bool closed = false;
   if ( !closed )
   {
      if ( checkCloseEvent() )
      {
         closeApp = true;
         closed = true;
         on_CloseButton_clicked( );
      }
   }
}
void ServerMain::ScanTimerTimer( )
{
   // default is every 20 secs
   CsGuard g;
   // scan the server list, and try to connect to all that we know of and we don't
   // have a server connection for
   ZConf->ServerScan();

}

void ServerMain::on_CloseButton_clicked()
{
    logMessage("Server close requested");
    closeApp = true;
    LogTimerTimer( );}
