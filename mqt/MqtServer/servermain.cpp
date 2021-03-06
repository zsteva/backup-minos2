#include "minos_pch.h"

#include "MinosLink.h"
#include "clientThread.h"
#include "serverThread.h"

#include "servermain.h"
#include "ui_servermain.h"

extern int GetSubscribedCount();
extern int GetPublishedCount();

ServerMain *MinosMainForm = 0;

bool closeApp = false;

ServerMain::ServerMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(&stdinReader, SIGNAL(stdinLine(QString)), this, SLOT(onStdInRead(QString)));
    stdinReader.start();

    createCloseEvent();
    QSettings settings;
    QByteArray geometry = settings.value("geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    connect(&LogTimer, SIGNAL(timeout()), this, SLOT(LogTimerTimer()));
    connect(&ScanTimer, SIGNAL(timeout()), this, SLOT(ScanTimerTimer()));

    QString sname = MinosServer::getMinosServer()->getServerName();

    clientListener = QSharedPointer<MinosClientListener>(new MinosClientListener);
    clientListener ->initialise( "Client", MinosClientPort );

    serverListener = QSharedPointer<MinosServerListener>(new MinosServerListener);
    serverListener ->initialise( "Server", MinosServerPort );

    ZConf = QSharedPointer<TZConf>(new TZConf);

    PubSubMain = QSharedPointer<TPubSubMain>(new TPubSubMain);
    ZConf->startZConf (sname );

    LogTimer.start(100);
    ScanTimer.start(20000);

    makeServerEvent( true );
}

ServerMain::~ServerMain()
{
    delete ui;
}
void ServerMain::onStdInRead(QString cmd)
{
    trace("Command read from stdin: " + cmd);
    if (cmd.indexOf("ShowServers", Qt::CaseInsensitive) >= 0)
        setShowServers(true);
    if (cmd.indexOf("HideServers", Qt::CaseInsensitive) >= 0)
        setShowServers(false);
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

   // scan the server list, and try to connect to all that we know of and we don't
   // have a server connection for
   ZConf->ServerScan();

}

void ServerMain::on_CloseButton_clicked()
{
    trace("Server close requested");
    closeApp = true;
}
void ServerMain::closeEvent(QCloseEvent *event)
{
    static bool closeSeen = false;
    if (!closeSeen)
    {
        closeSeen = true;
        trace("Server close event seen");
        closeApp = true;
        PubSubMain->closeDown();
        ZConf->closeDown();

        clientListener ->closeDown();
        serverListener ->closeDown();

        LogTimerTimer( );

        QWidget::closeEvent(event);
    }
}
void ServerMain::resizeEvent(QResizeEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QWidget::resizeEvent(event);
}
void ServerMain::moveEvent(QMoveEvent * event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    QWidget::moveEvent(event);
}
void ServerMain::changeEvent( QEvent* e )
{
    if( e->type() == QEvent::WindowStateChange )
    {
        QSettings settings;
        settings.setValue("geometry", saveGeometry());
    }
}

