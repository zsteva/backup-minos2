/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#include "logger_pch.h"

#include "ConfigFile.h"

#include "ServerEvent.h"
#include "LogEvents.h"
#include "XMPPRPCParams.h"
#include "XMPPStanzas.h"
#include "Dispatcher.h"
#include "XMPPEvents.h"
#include "XMPPRPCObj.h"
#include "RPCPubSub.h"

#include "SendRPCDM.h"
#include "tsinglelogframe.h"
#include "tlogcontainer.h"

//---------------------------------------------------------------------------
TSendDM::TSendDM(QWidget* Owner , LoggerContestLog *ct)
      : QObject( Owner )
{
    resetConnectables(ct);

    MinosRPC *rpc = MinosRPC::getMinosRPC(rpcConstants::loggerApp);
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_serverCall(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(clientCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_clientCall (bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

}
TSendDM::~TSendDM()
{
}

void TSendDM::resetConnectables(LoggerContestLog *ct)
{
    MinosConfig *config = MinosConfig::getMinosConfig();

    rigServerConnectable = config->getApp(ct->appRigControl.getValue());
    keyerServerConnectable = config->getApp(ct->appVoiceKeyer.getValue());
    bandMapServerConnectable = config->getApp(ct->appBandMap.getValue());
    rotatorServerConnectable = config->getApp(ct->appRotator.getValue());

}

//---------------------------------------------------------------------------
void TSendDM::logMessage( QString s )
{
   trace( s );
}
//---------------------------------------------------------------------------
void TSendDM::sendKeyerPlay(  int fno )
{
   RPCGeneralClient rpc(rpcConstants::keyerMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sName(new RPCStringParam( rpcConstants::keyerPlayFile ));
   QSharedPointer<RPCParam>iValue(new RPCIntParam( fno ));
   st->addMember( sName, "Name" );
   st->addMember( iValue, "Value" );
   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( keyerServerConnectable.remoteAppName + "@" + keyerServerConnectable.serverName );
}
void TSendDM::sendKeyerRecord(  int fno )
{
   RPCGeneralClient rpc(rpcConstants::keyerMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sName(new RPCStringParam( "RecordFile" ));
   QSharedPointer<RPCParam>iValue(new RPCIntParam( fno ));
   st->addMember( sName, "Name" );
   st->addMember( iValue, "Value" );
   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( keyerServerConnectable.remoteAppName + "@" + keyerServerConnectable.serverName );
}

void TSendDM::sendKeyerTone()
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);
    QSharedPointer<RPCParam>sName(new RPCStringParam( "Tone" ));
    QSharedPointer<RPCParam>iValue(new RPCIntParam( 0 ));
    st->addMember( sName, "Name" );
    st->addMember( iValue, "Value" );
    rpc.getCallArgs() ->addParam( st );
    rpc.queueCall( keyerServerConnectable.remoteAppName + "@" + keyerServerConnectable.serverName );
}
void TSendDM::sendKeyerTwoTone()
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);
    QSharedPointer<RPCParam>sName(new RPCStringParam( "TwoTone" ));
    QSharedPointer<RPCParam>iValue(new RPCIntParam( 0 ));
    st->addMember( sName, "Name" );
    st->addMember( iValue, "Value" );
    rpc.getCallArgs() ->addParam( st );
    rpc.queueCall( keyerServerConnectable.remoteAppName + "@" + keyerServerConnectable.serverName );
}
void TSendDM::sendKeyerStop()
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);
    QSharedPointer<RPCParam>sName(new RPCStringParam( "Stop" ));
    QSharedPointer<RPCParam>iValue(new RPCIntParam( 0 ));
    st->addMember( sName, "Name" );
    st->addMember( iValue, "Value" );
    rpc.getCallArgs() ->addParam( st );
    rpc.queueCall( keyerServerConnectable.remoteAppName + "@" + keyerServerConnectable.serverName );
}
//---------------------------------------------------------------------------
void TSendDM::sendBandMap(  const QString &freq,   const QString &call,   const QString &utc,   const QString &loc,   const QString &qth )
{
   RPCGeneralClient rpc(rpcConstants::bandmapMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   //st->addMember( rpcConstants::bandmapApp, rpcConstants::bandmapParamName );
   st->addMember( freq, rpcConstants::bandmapParamFreq );
   st->addMember( call, rpcConstants::bandmapParamCallsign );
   st->addMember( loc, rpcConstants::bandmapParamLocator );
   st->addDtgMember( utc, rpcConstants::bandmapParamUTC );
   st->addMember( qth, rpcConstants::bandmapParamQTH );

   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( bandMapServerConnectable.remoteAppName + "@" + bandMapServerConnectable.serverName );
}

void TSendDM::sendRotator(rpcConstants::RotateDirection direction, int angle )
{
   RPCGeneralClient rpc(rpcConstants::rotatorMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   st->addMember( static_cast<int> (direction), rpcConstants::rotatorParamDirection );
   st->addMember( angle, rpcConstants::rotatorParamAngle );
   rpc.getCallArgs() ->addParam( st );

   rpc.queueCall( rotatorServerConnectable.remoteAppName + "@" + rotatorServerConnectable.serverName );
}

//---------------------------------------------------------------------------
void TSendDM::on_notify( bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{

    // Does each SendDm get a notification??

    // PubSub notifications
    logMessage( "Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
    AnalysePubSubNotify an( err, mro );

    if ( an.getOK() )
    {
        if (an.getPublisherProgram() == keyerServerConnectable.remoteAppName && an.getPublisherServer() == keyerServerConnectable.serverName)
        {
            if ( an.getCategory() == rpcConstants::KeyerCategory && an.getKey() == rpcConstants::keyerKeyReport )
            {
                emit setKeyerLoaded();
                LogContainer->setCaption( an.getValue() );
                logMessage( "KeyerReport " + an.getValue() );
            }
        }
        if (an.getPublisherProgram() == rigServerConnectable.remoteAppName && an.getPublisherServer() == rigServerConnectable.serverName)
        {
            if ( an.getCategory() == rpcConstants::RigControlCategory && an.getKey() == rpcConstants::rigControlKeyMode )
            {
                emit setMode( an.getValue() );
            }
            if ( an.getCategory() == rpcConstants::RigControlCategory && an.getKey() == rpcConstants::rigControlKeyFreq )
            {
                emit setFreq( an.getValue() );
            }
        }
        if (an.getPublisherProgram() == bandMapServerConnectable.remoteAppName && an.getPublisherServer() == bandMapServerConnectable.serverName)
        {
            if ( an.getCategory() == rpcConstants::BandMapCategory && an.getKey() == rpcConstants::bandmapKeyLoaded )
            {
                emit setBandMapLoaded();
            }
        }
        if (an.getPublisherProgram() == rotatorServerConnectable.remoteAppName && an.getPublisherServer() == rotatorServerConnectable.serverName)
        {
            if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == rpcConstants::rotatorKeyState)
            {
                emit RotatorLoaded();
                emit RotatorState(an.getValue());
            }
            if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == rpcConstants::rotatorBearing)
            {

                emit RotatorBearing(an.getValue());
            }
            if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == "MaxAzimuth")
            {

                emit RotatorMaxAzimuth(an.getValue());
            }
            if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == "MinAzimuth")
            {

                emit RotatorMinAzimuth(an.getValue());
            }
            if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == "AntennaName")
            {

                emit RotatorAntennaName(an.getValue());
            }
        }
    }

}
//---------------------------------------------------------------------------
void TSendDM::on_clientCall( bool err, QSharedPointer<MinosRPCObj> /*mro*/, const QString &from )
{
   logMessage( "response callback from " + from + ( err ? ":Error" : ":Normal" ) );
}

//---------------------------------------------------------------------------
void TSendDM::on_serverCall(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
   // responds to pull calls from the monitoring client
   logMessage( "request callback from " + from + ( err ? ":Error" : ":Normal" ) );
   logMessage("method is " + mro->getMethodName());

   // need to check "from" is correct
   if ( !err )
   {
      QSharedPointer<RPCParam> psLogName;
      QSharedPointer<RPCParam>psStanza;
      RPCArgs *args = mro->getCallArgs();

      //QSharedPointer<RPCParam> psMess;
      //if (args->getStructArgMember(0, rpcConstants::loggerStanzaRequest, psMess))

      if ( args->getStructArgMember( 0, "LogName", psLogName )
           && args->getStructArgMember( 0, "Stanza", psStanza ) )
      {
         QString LogName;
         int Stanza;
         if ( psLogName->getString( LogName ) && psStanza->getInt( Stanza ) )
         {
            mro->clearCallArgs();
            QSharedPointer<RPCParam>st(new RPCParamStruct);


            bool callOK = true;
            // Find Value as a published log
            // and then send requested stanza from the log
            // we publish the stanza count; it is up to the monitor to ensure
            // it has a full set
            st->addMember( LogName, "LogName" );
            st->addMember( Stanza, "Stanza" );
            if ( callOK )
            {
               // we need to start pushing stanzas from the logfile - we can
               // only really process a log sequentially
               // Once it has all been pushed then later stanzas need
               // to go to all subscribers.

               // SO it is more in the nature of a "subscribe" but no
               // get the stanza data from the log and add it as a string
               TSingleLogFrame * lf = LogContainer ->findContest( LogName );
               if ( lf )
               {
                  QString StanzaData;
                  callOK = lf->getStanza( Stanza, StanzaData );
                  if ( callOK )
                  {
                     st->addMember( StanzaData, "StanzaData" );
                  }
               }
               else
               {
                  callOK = false;
               }
            }

            st->addMember( callOK, "LoggerResult" );
            mro->getCallArgs() ->addParam( st );
            mro->queueResponse( from );
         }
      }
   }
}
//---------------------------------------------------------------------------

