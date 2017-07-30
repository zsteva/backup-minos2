#include "base_pch.h"
#include "rigcontrolmainwindow.h"
#include "rigcontrolrpc.h"


RigControlRpc *rigControlRpc;

RigControlRpc::RigControlRpc(RigControlMainWindow *parent) : QObject(parent), parent(parent)
{

    rigControlRpc = this;


    MinosRPC *rpc = MinosRPC::getMinosRPC(rpcConstants::RigControlCategory);

    connect(rpc, SIGNAL(clientCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_response(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_request(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    // we aren't subscribing to anything!


}






void RigControlRpc::on_notify( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
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
void RigControlRpc::on_response(bool /*err*/, QSharedPointer<MinosRPCObj> /*mro*/, const QString &/*from*/ )
{
   // call back says OK/not OK, and we ignore it
}
//---------------------------------------------------------------------------
void RigControlRpc::on_request( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
    //trace( "rotator callback from " + from + ( err ? ":Error" : ":Normal" ) );

    if ( !err )
    {
        QSharedPointer<RPCParam> psDirection;
        QSharedPointer<RPCParam> psAngle;
        RPCArgs *args = mro->getCallArgs();
        if ( args->getStructArgMember( 0, rpcConstants::rotatorParamDirection, psDirection )
             && args->getStructArgMember( 0, rpcConstants::rotatorParamAngle, psAngle ) )
        {
            int direction;
            int angle;

            if ( psDirection->getInt( direction ) && psAngle->getInt( angle ) )
            {
                // here you handle what the logger has sent to us

               // emit (setRotation(direction, angle));

                QSharedPointer<RPCParam>st(new RPCParamStruct);
                st->addMember( true, rpcConstants::rotatorResult );
                mro->clearCallArgs();
                mro->getCallArgs() ->addParam( st );
                mro->queueResponse( from );
            }
        }
    }
}
