#include "base_pch.h"
#include "rotatormainwindow.h"
#include "rotatorlogic.h"



RotatorLogic *rotatorLogic;

RotatorLogic::RotatorLogic(RotatorMainWindow *parent) : QObject(parent), parent(parent)
{
    rotatorLogic = this;
    connect(&ConnectTimer, SIGNAL(timeout()), this, SLOT(ConnectTimerTimer()));
    ConnectTimer.start(1000);
}
//---------------------------------------------------------------------------

/*static*/
void RotatorLogic::makeRPCObjects()
{
   MinosRPCObj::addObj( new RPCRotatorClient( new TRPCCallback <RotatorLogic> ( rotatorLogic, &RotatorLogic::rotatorClientCallback ) ) );
   MinosRPCObj::addObj( new RPCRotatorServer( new TRPCCallback <RotatorLogic> ( rotatorLogic, &RotatorLogic::rotatorServerCallback ) ) );

}
//---------------------------------------------------------------------------

void RotatorLogic::publishState(const QString &state)
{
    if (connected && subscribed)
    {
        static QString old;

        if ( state != old )
        {
           old = state;
           RPCPubSub::publish( "Rotator", "State", state, psPublished );
        }
    }
}

void RotatorLogic::ConnectTimerTimer( )
{
   if ( !connected && checkServerReady() )
   {
      makeRPCObjects();
      XMPPInitialise( "Rotator" );
      connected = true;
   }
   if ( connected && !subscribed )
   {
      RPCPubSub::initialisePubSub( new TRPCCallback <RotatorLogic> ( rotatorLogic, &RotatorLogic::notifyCallback ) );
      subscribed = true;
   }
}
void RotatorLogic::notifyCallback( bool err, MinosRPCObj *mro, const QString &from )
{
   trace( "Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
   AnalysePubSubNotify an( err, mro );

   // called whenever soemthing we subscribe to changes
   if ( an.getOK() )
   {
// example from elsewhere
//      if ( an.getCategory() == "LineControl" )
//      {
//         lineStates[ an.getKey() ] = ( ( an.getValue() == "set" ) ? true : false );
//         trace( "LineControl" + an.getKey() + ":" + an.getValue() );
//      }
   }
}
//---------------------------------------------------------------------------
void RotatorLogic::rotatorClientCallback( bool /*err*/, MinosRPCObj * /*mro*/, const QString &/*from*/ )
{
   // call back says OK/not OK, and we ignore it
}
//---------------------------------------------------------------------------
void RotatorLogic::rotatorServerCallback( bool err, MinosRPCObj *mro, const QString &from )
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
                // here you handle what the logger has sent to us

                emit (setRotation(direction, angle));

                QSharedPointer<RPCParam>st(new RPCParamStruct);
                st->addMember( true, "RotatorResult" );
                mro->clearCallArgs();
                mro->getCallArgs() ->addParam( st );
                mro->queueResponse( from );
            }
        }
    }
}
