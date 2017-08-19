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

    connect(rpc, SIGNAL(clientCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_response(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_request(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    // we aren't subscribing to anything!


}

//--------------------------------------------------------------------------------------------------//



void RigControlRpc::publishState(const QString &state)
{
    static QString old;

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

    if ( radioName != old )
    {
       old = radioName;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlKeyRadioName, radioName, psPublished );
    }
}


void RigControlRpc::publishFreq(const QString &freq)
{
    static QString old;

    if ( freq != old )
    {
       old = freq;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlKeyFreq, freq, psPublished );
    }
}


void RigControlRpc::publishMode(const QString &mode)
{
    static QString old;

    if ( mode != old )
    {
       old = mode;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlKeyMode, mode, psPublished );
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
void RigControlRpc::on_response(bool /*err*/, QSharedPointer<MinosRPCObj> /*mro*/, const QString &/*from*/ )
{
   // call back says OK/not OK, and we ignore it
}
//---------------------------------------------------------------------------
void RigControlRpc::on_request( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
    trace( "rigcontrol callback from " + from + ( err ? ":Error" : ":Normal" ) );

    if ( !err )
    {
        QSharedPointer<RPCParam> psFreq;
        QSharedPointer<RPCParam> psMode;
        RPCArgs *args = mro->getCallArgs();
        if ( args->getStructArgMember( 0, rpcConstants::rigControlKeyFreq, psFreq ))
        {
            QString freq;


            if ( psFreq->getString( freq ) )
            {
                // here you handle what the logger has sent to us

                emit (setFreq(freq));

                QSharedPointer<RPCParam>st(new RPCParamStruct);
                st->addMember( true, rpcConstants::rotatorResult );
                mro->clearCallArgs();
                mro->getCallArgs() ->addParam( st );
                mro->queueResponse( from );
            }
        }
        else if ( args->getStructArgMember( 0, rpcConstants::rigControlKeyMode, psMode ) )
             {
                 QString mode;

                 if ( psMode->getString( mode ) )
                 {
                     // here you handle what the logger has sent to us

                    emit (setMode(mode));

                     QSharedPointer<RPCParam>st(new RPCParamStruct);
                     st->addMember( true, rpcConstants::rotatorResult );
                     mro->clearCallArgs();
                     mro->getCallArgs() ->addParam( st );
                     mro->queueResponse( from );
                 }
             }
    }
}
