#include "base_pch.h"

#include "rotatormainwindow.h"
#include "ui_rotatormainwindow.h"

RotatorMainWindow *MinosRotatorForm;

RotatorMainWindow::RotatorMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RotatorMainWindow)
{
    ui->setupUi(this);

    enableTrace( "./TraceLog", "MinosChat_" );

    createCloseEvent();

    MinosRotatorForm = this;

    connect(&LogTimer, SIGNAL(timeout()), this, SLOT(LogTimerTimer()));
    connect(&ConnectTimer, SIGNAL(timeout()), this, SLOT(ConnectTimerTimer()));
    LogTimer.start(100);
    ConnectTimer.start(1000);
}

RotatorMainWindow::~RotatorMainWindow()
{
    delete ui;
}
void RotatorMainWindow::logMessage( QString s )
{
   trace( s );
}
/*static*/
void RotatorMainWindow::makeRPCObjects()
{
   //RPCPubSub::initialisePubSub( new TRPCCallback <RotatorMainWindow> ( MinosRotatorForm, &RotatorMainWindow::notifyCallback ) );

   MinosRPCObj::addObj( new RPCRotatorClient( new TRPCCallback <RotatorMainWindow> ( MinosRotatorForm, &RotatorMainWindow::rotatorClientCallback ) ) );
   MinosRPCObj::addObj( new RPCRotatorServer( new TRPCCallback <RotatorMainWindow> ( MinosRotatorForm, &RotatorMainWindow::rotatorServerCallback ) ) );

}

void RotatorMainWindow::closeEvent(QCloseEvent *event)
{
    MinosRPCObj::clearRPCObjects();
    XMPPClosedown();
    LogTimerTimer( );
    // and tidy up all loose ends

    /*
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    */
    QWidget::closeEvent(event);
}
void RotatorMainWindow::resizeEvent(QResizeEvent * event)
{
    /*
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    */
    QWidget::resizeEvent(event);
}

void RotatorMainWindow::LogTimerTimer(  )
{

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
//---------------------------------------------------------------------------

void RotatorMainWindow::ConnectTimerTimer( )
{
   if ( !connected && checkServerReady() )
   {
      makeRPCObjects();
      XMPPInitialise( "Rotator" );
      connected = true;
   }
}
//==================================================================================================
void RotatorMainWindow::rotatorClientCallback( bool /*err*/, MinosRPCObj * /*mro*/, const QString &/*from*/ )
{
   // call back says OK/not OK
}
//---------------------------------------------------------------------------
void RotatorMainWindow::rotatorServerCallback( bool err, MinosRPCObj *mro, const QString &from )
{
   trace( "rotator callback from " + from + ( err ? ":Error" : ":Normal" ) );

   if ( !err )
   {
      QSharedPointer<RPCParam> psDirection;
      QSharedPointer<RPCParam> psAngle;
      RPCArgs *args = mro->getCallArgs();
      if ( args->getStructArgMember( 0, "RotatorDirection", psDirection ) && args->getStructArgMember( 0, "RotatorAngle", psAngle ) )
      {
         int direction;
         int angle;

         if ( psDirection->getInt( direction ) && psAngle->getInt( angle ) )
         {
            QSharedPointer<RPCParam>st(new RPCParamStruct);

           ui->direction->setText( QString::number(direction));
           ui->angle->setText( QString::number(angle));

           st->addMember( true, "RotatorResult" );
            mro->clearCallArgs();
            mro->getCallArgs() ->addParam( st );
            mro->queueResponse( from );
         }
      }
   }
}
