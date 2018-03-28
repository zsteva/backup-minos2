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

// this publishes the list of radios configured in rigcontrol

void RigControlRpc::publishRadioNames(QStringList radios)
{
    MinosRPC *rpc = MinosRPC::getMinosRPC();

    QString nameList = radios.join(":");
    rigCache.addRigList(nameList);
    rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlRadioList, nameList, psPublished );
}

void RigControlRpc::publishBandNames(QStringList bands)
{
    MinosRPC *rpc = MinosRPC::getMinosRPC();
    QString bandList = bands.join(":");
    //rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlBandList, bandList, psPublished );
}



// this publishes the selected radio name

void RigControlRpc::publishRadioName(const QString &radioName)
{
    static QString old;
    if ( radioName != old )
    {
       trace(QString("Rig RPC: Publish radioName = %1").arg(radioName));
       old = radioName;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       //rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlRadioName, radioName, psPublished );
    }
}


void RigControlRpc::publishState(const QString &state)
{
    static QString old;
    if ( state != old )
    {
        trace(QString("Rig RPC: Publish State = %1").arg(state));
       old = state;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       //rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlStatus, state, psPublished );
    }
}

void RigControlRpc::publishErrorMsg(const QString &errorMsg)
{
    static QString old;
    if ( errorMsg != old )
    {
        trace(QString("Rig RPC: Publish Error Message = %1").arg(errorMsg));
       old = errorMsg;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       //rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlErrorMsg, errorMsg, psPublished );
    }
}


void RigControlRpc::publishFreq(const QString &freq)
{
    static QString old;
    if (freq != old)
    {
        trace(QString("Rig RPC: Publish Freq = %1").arg(freq));
        old = freq;
        MinosRPC *rpc = MinosRPC::getMinosRPC();
        //rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlFreq, freq, psPublished );
    }
}



void RigControlRpc::publishMode(const QString &mode)
{
    static QString old;
    if ( mode != old )
    {
        trace(QString("Rig RPC: Publish Mode = %1").arg(mode));
       old = mode;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       //rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlMode, mode, psPublished );
    }
}





void RigControlRpc::publishTransVertStatus(const QString &status)
{
    static QString old;
    if ( status != old )
    {
        trace(QString("Rig RPC: Publish TransVert Status = %1").arg(status));
       old = status;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       //rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlTxVertStatus, status, psPublished );
    }
}


// sends transvert offset freq in the form "+:xxxxxxxx" or "-:xxxxxxx"
// where + is positive offset, - is negative offset and xxxxx is the offset freq.

void RigControlRpc::publishTransVertOffSetFreq(bool offSet, const QString &freq)
{
    static QString old;
    static bool offSign;
    QString msg;
    if ( freq != old || offSign != offSet)
    {
       if (offSet)
       {
           // negative offset
           msg = QString("-:");
       }
       else
       {
           // positve offset
           msg = QString("+:");
       }
       msg = msg.append(freq);
       trace(QString("Rig RPC: Publish TransVert OffsetFreq = %1").arg(msg));
       old = freq;
       offSign = offSet;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       //rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlTxVertOffsetFreq, msg, psPublished );
    }
}


void RigControlRpc::publishTransVertSwitch(const QString &swNum)
{
    static QString old;
    if ( swNum != old )
    {
        trace(QString("Rig RPC: Publish TransVert Switch Number = %1").arg(swNum));
       old = swNum;
       MinosRPC *rpc = MinosRPC::getMinosRPC();
       //rpc->publish( rpcConstants::rigControlCategory, rpcConstants::rigControlTxVertSwitch, swNum, psPublished );
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
        QSharedPointer<RPCParam> psName;
        QSharedPointer<RPCParam> psSelect;

        RPCArgs *args = mro->getCallArgs();
        if ( args->getStructArgMember( 0, rpcConstants::rigControlFreq, psFreq ))
        {
            QString freq;


            if ( psFreq->getString( freq ) )
            {
                // here you handle what the logger has sent to us
                trace(QString("Rig RPC: Freq Command From Logger = %1").arg(freq));
                emit (setFreq(freq));
            }
        }
        else if (args->getStructArgMember(0, rpcConstants::rigControlRadioName, psName))
        {
            QString name;
            if (psName->getString(name))
            {
                QString mode;
                if ( args->getStructArgMember( 0, rpcConstants::rigControlMode, psMode ) )
                {
                     if ( psMode->getString( mode ) )
                     {
                         // here you handle what the logger has sent to us
                        trace(QString("Rig RPC: Mode Command From Logger = %1").arg(mode));
                     }
                }
                QString sel;
                if ( args->getStructArgMember( 0, rpcConstants::selected, psSelect ) )
                {
                         if ( psSelect->getString( sel ) )
                         {
                             // here you handle what the logger has sent to us
                            PubSubName psn(name);
                            rigCache.setSelected(psn, sel);
                            trace(QString("Rig RPC: select Command From Logger = %1 psn=%2, sel=%3").arg(sel).arg(psn.toString()).arg(sel));
                         }
                }
                emit selectLoggerRadio(name, mode);
            }
        }
        else if ( args->getStructArgMember( 0, rpcConstants::rigControlMode, psMode ) )
        {
                 QString mode;

                 if ( psMode->getString( mode ) )
                 {
                     // here you handle what the logger has sent to us
                    trace(QString("Rig RPC: Mode Command From Logger = %1").arg(mode));
                    emit (setMode(mode));
                 }
        }
    }
}
