#include "base_pch.h"
#include "rotatormainwindow.h"
#include "rotatorlogic.h"

RotatorLogic *rotatorLogic;

RotatorLogic::RotatorLogic(RotatorMainWindow *parent) : QObject(parent), parent(parent)
{
    rotatorLogic = this;

    MinosRPC *rpc = MinosRPC::getMinosRPC();

    connect(rpc, SIGNAL(clientCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_response(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_request(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    rpc->setAppName("Minos:Rotator");

    // we aren't subscribing to anything!

}
//---------------------------------------------------------------------------

void RotatorLogic::publishState(const QString &state)
{
    static QString old;

    if ( state != old )
    {
       old = state;
       RPCPubSub::publish( "Rotator", "State", state, psPublished );
    }
}

void RotatorLogic::on_notify( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
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
void RotatorLogic::on_response(bool /*err*/, QSharedPointer<MinosRPCObj> /*mro*/, const QString &/*from*/ )
{
   // call back says OK/not OK, and we ignore it
}
//---------------------------------------------------------------------------
void RotatorLogic::on_request( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
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
