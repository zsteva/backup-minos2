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
    MinosConfig *config = MinosConfig::getMinosConfig(Owner);

    rigServerConnectable = config->getApp(atRigControl, ct?ct->appRigControl.getValue():QString());
    keyerServerConnectable = config->getApp(atKeyer, ct?ct->appVoiceKeyer.getValue():QString());
    bandMapServerConnectable = config->getApp(atBandMap, ct?ct->appBandMap.getValue():QString());
    rotatorServerConnectable = config->getApp(atRotator, ct?ct->appRotator.getValue():QString());

    MinosRPC *rpc = MinosRPC::getMinosRPC(rpcConstants::loggerApp);
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_response(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(clientCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_request (bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

}
TSendDM::~TSendDM()
{
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
   rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerConnectable.serverName );
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
   rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerConnectable.serverName );
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
    rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerConnectable.serverName );
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
    rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerConnectable.serverName );
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
    rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerConnectable.serverName );
}
//---------------------------------------------------------------------------
void TSendDM::sendBandMap(  const QString &freq,   const QString &call,   const QString &utc,   const QString &loc,   const QString &qth )
{
   RPCGeneralClient rpc(rpcConstants::bandmapMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   st->addMember( rpcConstants::bandmapApp, rpcConstants::bandmapParamName );
   st->addMember( freq, rpcConstants::bandmapParamFreq );
   st->addMember( call, rpcConstants::bandmapParamCallsign );
   st->addMember( loc, rpcConstants::bandmapParamLocator );
   st->addDtgMember( utc, rpcConstants::bandmapParamUTC );
   st->addMember( qth, rpcConstants::bandmapParamQTH );

   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( rpcConstants::bandmapApp + "@" + bandMapServerConnectable.serverName );
}

void TSendDM::sendRotator(rpcConstants::RotateDirection direction, int angle )
{
   RPCGeneralClient rpc(rpcConstants::rotatorMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   st->addMember( static_cast<int> (direction), rpcConstants::rotatorParamDirection );
   st->addMember( angle, rpcConstants::rotatorParamAngle );
   rpc.getCallArgs() ->addParam( st );

   rpc.queueCall( rpcConstants::rotatorApp + "@" + rotatorServerConnectable.serverName );
}

//---------------------------------------------------------------------------
void TSendDM::on_notify( bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
   // PubSub notifications
   logMessage( "Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
   AnalysePubSubNotify an( err, mro );

   // called whenever frequency changes
   if ( an.getOK() )
   {
      if ( an.getCategory() == rpcConstants::KeyerCategory && an.getKey() == rpcConstants::keyerKeyReport )
      {
         emit setKeyerLoaded();
         LogContainer->setCaption( an.getValue() );
         logMessage( "KeyerReport " + an.getValue() );
      }
      if ( an.getCategory() == rpcConstants::RigControlCategory && an.getKey() == rpcConstants::rigControlKeyMode )
      {
         emit setMode( an.getValue() );
      }
      if ( an.getCategory() == rpcConstants::RigControlCategory && an.getKey() == rpcConstants::rigControlKeyFrequency )
      {
         emit setFreq( an.getValue() );
      }
      if ( an.getCategory() == rpcConstants::BandMapCategory && an.getKey() == rpcConstants::bandmapKeyLoaded )
      {
         emit setBandMapLoaded();
      }
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
//---------------------------------------------------------------------------
void TSendDM::on_response( bool err, QSharedPointer<MinosRPCObj> /*mro*/, const QString &from )
{
   logMessage( "response callback from " + from + ( err ? ":Error" : ":Normal" ) );
}

//---------------------------------------------------------------------------
void TSendDM::on_request(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
   // responds to pull calls from the monitoring client
   logMessage( "request callback from " + from + ( err ? ":Error" : ":Normal" ) );

   // need to check "from" is correct
   if ( !err )
   {
      QSharedPointer<RPCParam> psLogName;
      QSharedPointer<RPCParam>psStanza;
      RPCArgs *args = mro->getCallArgs();
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

