/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
//
//
/////////////////////////////////////////////////////////////////////////////




#include "base_pch.h"
#include "rotatormainwindow.h"
#include "rotatorRpc.h"



RotatorRpc *rotatorRpc;

RotatorRpc::RotatorRpc(RotatorMainWindow *parent) : QObject(parent), parent(parent)
{
    rotatorRpc = this;

    MinosRPC *rpc = MinosRPC::getMinosRPC();

    connect(rpc, SIGNAL(clientCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_response(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_request(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    rpc->setAppName(rpcConstants::rotatorApp);

    // we aren't subscribing to anything!

}
//---------------------------------------------------------------------------

void RotatorRpc::publishState(const QString &state)
{
    static QString old;

    if ( state != old )
    {
       old = state;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorKeyState, state, psPublished );
    }
}


void RotatorRpc::publishAntennaName(const QString &antennaName)
{
    static QString old;

    if ( antennaName != old )
    {
       old = antennaName;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorAntennaName, antennaName, psPublished );
    }
}


void RotatorRpc::publishMaxAzimuth(const QString maxAzimuth)
{
    static QString old;

    if ( maxAzimuth != old )
    {
       old = maxAzimuth;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorMaxAzimuth, maxAzimuth, psPublished );
    }
}

void RotatorRpc::publishMinAzimuth(const QString minAzimuth)
{
    static QString old;

    if ( minAzimuth != old )
    {
       old = minAzimuth;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorMinAzimuth, minAzimuth, psPublished );
    }
}



void RotatorRpc::publishBearing(const QString bearing)
{
    static QString old;

    if ( bearing != old )
    {
       old = bearing;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorBearing, bearing, psPublished );
    }
}







void RotatorRpc::on_notify( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
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
void RotatorRpc::on_response(bool /*err*/, QSharedPointer<MinosRPCObj> /*mro*/, const QString &/*from*/ )
{
   // call back says OK/not OK, and we ignore it
}
//---------------------------------------------------------------------------
void RotatorRpc::on_request( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
    trace( "rotator callback from " + from + ( err ? ":Error" : ":Normal" ) );

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

                emit (setRotation(direction, angle));

                QSharedPointer<RPCParam>st(new RPCParamStruct);
                st->addMember( true, rpcConstants::rotatorResult );
                mro->clearCallArgs();
                mro->getCallArgs() ->addParam( st );
                mro->queueResponse( from );
            }
        }
    }
}

