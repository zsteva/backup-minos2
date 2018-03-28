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

    MinosRPC *rpc = MinosRPC::getMinosRPC(rpcConstants::rotatorApp);

    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_serverCall(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    // we aren't subscribing to anything!

}
//---------------------------------------------------------------------------

void RotatorRpc::publishState(const QString &name, const QString &state)
{
    static QString old;
    trace(QString("Rot RPC: Publish State = %1").arg(state));
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    if ( state != old )
    {
       old = state;
       //rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorState, state, psPublished );
    }
}

void RotatorRpc::publishAntennaList(QString ants)
{
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    rotatorCache.addRotList(ants);
    rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorList, ants, psPublished );
}

void RotatorRpc::publishMaxAzimuth(const QString &name, const QString maxAzimuth)
{
    static QString old;
    trace(QString("Rot RPC: Publish MaxAzimuth = %1").arg(maxAzimuth));
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    if ( maxAzimuth != old )
    {
       old = maxAzimuth;
       //rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorMaxAzimuth, maxAzimuth, psPublished );
    }

}

void RotatorRpc::publishMinAzimuth(const QString &name, const QString minAzimuth)
{
    static QString old;
    trace(QString("Rot RPC: Publish MinAzimuth = %1").arg(minAzimuth));
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    if ( minAzimuth != old )
    {
       old = minAzimuth;
       //rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorMinAzimuth, minAzimuth, psPublished );
    }
}

void RotatorRpc::publishBearing(const QString &name, const QString bearing)
{
    static QString old;
    trace(QString("Rot RPC: Publish Bearing = %1").arg(bearing));
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    if ( bearing != old )
    {
       old = bearing;
       //rpc->publish( rpcConstants::RotatorCategory, rpcConstants::rotatorBearing, bearing, psPublished );
    }

}

void RotatorRpc::on_notify( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
   trace( "Rot Rpc: Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
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
void RotatorRpc::on_serverCall( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
    trace( "Rot RPC: rotator callback from " + from + ( err ? ":Error" : ":Normal" ) );

    if ( !err )
    {
        QSharedPointer<RPCParam> psDirection;
        QSharedPointer<RPCParam> psAngle;
        QSharedPointer<RPCParam> psAntName;
        QSharedPointer<RPCParam> psSelect;
        RPCArgs *args = mro->getCallArgs();
        if ( args->getStructArgMember( 0, rpcConstants::rotatorParamDirection, psDirection )
             && args->getStructArgMember( 0, rpcConstants::rotatorParamAngle, psAngle ) )
        {
            int direction;
            int angle;

            if ( psDirection->getInt( direction ) && psAngle->getInt( angle ) )
            {
                // here you handle what the logger has sent to us
                trace(QString("Rot RPC: Direction = %1, Angle = %2").arg(QString::number(direction), QString::number(angle)));
                emit (setRotation(direction, angle));
            }
        }
        else if (args->getStructArgMember(0, rpcConstants::rotatorAntennaName, psAntName))
        {
            QString name;
            if (psAntName->getString(name))
            {
                QString sel;
                if ( args->getStructArgMember( 0, rpcConstants::selected, psSelect ) )
                {
                     if ( psSelect->getString( sel ) )
                     {
                         // here you handle what the logger has sent to us
                        trace(QString("Rig RPC: select Command From Logger = %1").arg(sel));
                        PubSubName psn(name);
                        rotatorCache.setSelected(psn, sel);
                     }
                }
                emit selectAntenna(name);
            }
        }
    }
}

