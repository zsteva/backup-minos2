/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2017
//
//
//
/////////////////////////////////////////////////////////////////////////////




#include "base_pch.h"
#include "rigcontrolmainwindow.h"
#include "rigcontrolrpc.h"


RigControlRpc *rigControlRpc;

RigControlRpc::RigControlRpc(RigControlMainWindow *parent) : QObject(parent), parent(parent)
{

    rigControlRpc = this;


    MinosRPC *rpc = MinosRPC::getMinosRPC(rpcConstants::rigControlApp);

    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_serverCall(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    // we aren't subscribing to anything!


}

//--------------------------------------------------------------------------------------------------//



void RigControlRpc::publishState(const QString &state)
{
    static QString old;
    trace(QString("Rig RPC: Publish State = %1").arg(state));
    if ( state != old )
    {
       old = state;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlKeyState, state, psPublished );
    }
}


void RigControlRpc::publishRadioName(const QString &radioName)
{
    static QString old;
    trace(QString("Rig RPC: Publish Name = %1").arg(radioName));
    if ( radioName != old )
    {
       old = radioName;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlKeyRadioName, radioName, psPublished );
    }
}


void RigControlRpc::publishFreq(const QString &freq)
{
    trace(QString("Rig RPC: Publish Freq = %1").arg(freq));
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlKeyFreq, freq, psPublished );
}


void RigControlRpc::publishMode(const QString &mode)
{
    static QString old;
    trace(QString("Rig RPC: Publish Mode = %1").arg(mode));
    if ( mode != old )
    {
       old = mode;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlKeyMode, mode, psPublished );
    }
}






void RigControlRpc::publishRxPbFlag(const QString &flag)
{
    static QString old;
    trace(QString("Rig RPC: Publish RxPBFlag = %1").arg(flag));
    if (flag != old)
    {
        old = flag;
        MinosRPC *rpc = MinosRPC::getMinosRPC();
        rpc->publish(rpcConstants::rigControlCategory, rpcConstants::rigControlKeyRxPbFlag, flag, psPublished);
    }
}


void RigControlRpc::publishPbandState(const QString &state)
{
    static QString old;
    trace(QString("Rig RPC: Publish PassBand State = %1").arg(state));
    if (state != old)
    {
        old = state;
        MinosRPC *rpc = MinosRPC::getMinosRPC();
        rpc->publish(rpcConstants::rigControlCategory, rpcConstants::rigControlKeyPBandState, state, psPublished);
    }
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
void RigControlRpc::on_serverCall( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
    trace("Rig RPC: Rigcontrol callback from " + from + ( err ? ":Error" : ":Normal" ) );

    if ( !err )
    {
        QSharedPointer<RPCParam> psFreq;
        QSharedPointer<RPCParam> psMode;
        QSharedPointer<RPCParam> psPBandState;
        RPCArgs *args = mro->getCallArgs();
        if ( args->getStructArgMember( 0, rpcConstants::rigControlKeyFreq, psFreq ))
        {
            QString freq;


            if ( psFreq->getString( freq ) )
            {
                // here you handle what the logger has sent to us
                trace(QString("Rig RPC: Freq Command From Logger = $1").arg(freq));
                emit (setFreq(freq));
            }
        }
        else if ( args->getStructArgMember( 0, rpcConstants::rigControlKeyMode, psMode ) )
        {
                 QString mode;

                 if ( psMode->getString( mode ) )
                 {
                     // here you handle what the logger has sent to us
                    trace(QString("Rig RPC: Mode Command From Logger = %1").arg(mode));
                    emit (setMode(mode));
                 }
        }
        else if ( args->getStructArgMember( 0, rpcConstants::rigControlKeyPBandState, psPBandState ) )
        {
                 int passBandState;

                 if ( psPBandState->getInt( passBandState ) )
                 {
                     // here you handle what the logger has sent to us
                    trace(QString("Rig RPC: PassBandState Command From Logger = %1").arg(QString::number(passBandState)));
                    emit (setPassBand(passBandState));
                 }
        }
    }
}
