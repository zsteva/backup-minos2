/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#include "logger_pch.h"

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

TSendDM *SendDM = 0;
//---------------------------------------------------------------------------
TSendDM::TSendDM( QWidget* Owner )
      : QObject( Owner )
{
    QSettings config("./Configuration/MinosConfig.ini", QSettings::IniFormat);
    QString circleOfHell = config.value( "CircleOfHell/Name", "No_name_in_config" ).toString().trimmed();
    serverName = circleOfHell;

    QString rigServer = config.value( "RigControl/Server", "localhost" ).toString().trimmed();
    rigServerName = rigServer;

    QString keyerServer = config.value( "Keyer/Server", "localhost" ).toString().trimmed();
    keyerServerName = keyerServer;

    QString bandMapServer = config.value( "BandMap/Server", "localhost" ).toString().trimmed();
    bandMapServerName = bandMapServer;

    QString rotatorServer = config.value( "Rotator/Server", "localhost" ).toString().trimmed();
    rotatorServerName = rotatorServer;

    MinosRPC *rpc = MinosRPC::getMinosRPC();
    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_response(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(clientCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_request (bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    rpc->setAppName("Logger");
    rpc->subscribeRemote( rigServerName, rpcConstants::RigControlCategory );
    rpc->subscribeRemote( keyerServerName, rpcConstants::KeyerCategory );
    rpc->subscribeRemote( bandMapServerName, rpcConstants::BandMapCategory );
    rpc->subscribeRemote( rotatorServerName, rpcConstants::RotatorCategory );
}
TSendDM::~TSendDM()
{
   SendDM = 0;
}
//---------------------------------------------------------------------------
void TSendDM::logMessage( QString s )
{
   trace( s );
}
//---------------------------------------------------------------------------
/*static*/ void TSendDM::sendKeyerPlay(  int fno )
{
    if (SendDM)
        SendDM->doSendKeyerPlay(fno);
}

void TSendDM::doSendKeyerPlay(  int fno )
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sName(new RPCStringParam( rpcConstants::keyerPlayFile ));
   QSharedPointer<RPCParam>iValue(new RPCIntParam( fno ));
   st->addMember( sName, "Name" );
   st->addMember( iValue, "Value" );
   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerName );
}
/*static*/ void TSendDM::sendKeyerRecord(  int fno )
{
    if (SendDM)
        SendDM->doSendKeyerRecord(fno);
}

void TSendDM::doSendKeyerRecord(  int fno )
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sName(new RPCStringParam( "RecordFile" ));
   QSharedPointer<RPCParam>iValue(new RPCIntParam( fno ));
   st->addMember( sName, "Name" );
   st->addMember( iValue, "Value" );
   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerName );
}

/*static*/ void TSendDM::sendKeyerTone()
{
    if (SendDM)
        SendDM->doSendKeyerTone();
}
void TSendDM::doSendKeyerTone()
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);
    QSharedPointer<RPCParam>sName(new RPCStringParam( "Tone" ));
    QSharedPointer<RPCParam>iValue(new RPCIntParam( 0 ));
    st->addMember( sName, "Name" );
    st->addMember( iValue, "Value" );
    rpc.getCallArgs() ->addParam( st );
    rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerName );
}
/*static*/ void TSendDM::sendKeyerTwoTone()
{
    if (SendDM)
        SendDM->doSendKeyerTwoTone();
}

void TSendDM::doSendKeyerTwoTone()
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);
    QSharedPointer<RPCParam>sName(new RPCStringParam( "TwoTone" ));
    QSharedPointer<RPCParam>iValue(new RPCIntParam( 0 ));
    st->addMember( sName, "Name" );
    st->addMember( iValue, "Value" );
    rpc.getCallArgs() ->addParam( st );
    rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerName );
}
/*static*/ void TSendDM::sendKeyerStop()
{
    if (SendDM)
        SendDM->doSendKeyerStop();
}

void TSendDM::doSendKeyerStop()
{
    RPCGeneralClient rpc(rpcConstants::keyerMethod);
    QSharedPointer<RPCParam>st(new RPCParamStruct);
    QSharedPointer<RPCParam>sName(new RPCStringParam( "Stop" ));
    QSharedPointer<RPCParam>iValue(new RPCIntParam( 0 ));
    st->addMember( sName, "Name" );
    st->addMember( iValue, "Value" );
    rpc.getCallArgs() ->addParam( st );
    rpc.queueCall( rpcConstants::keyerApp + "@" + keyerServerName );
}
//---------------------------------------------------------------------------
void TSendDM::sendBandMap(  const QString &freq,   const QString &call,   const QString &utc,   const QString &loc,   const QString &qth )
{
    if (SendDM)
        SendDM->doSendBandMap(freq, call, utc, loc, qth);
}

/*static*/ void TSendDM::doSendBandMap(  const QString &freq,   const QString &call,   const QString &utc,   const QString &loc,   const QString &qth )
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
   rpc.queueCall( rpcConstants::bandmapApp + "@" + bandMapServerName );
}

/*static*/ void TSendDM::sendRotator(rpcConstants::RotateDirection direction, int angle )
{
    if (SendDM)
        SendDM->doSendRotator(direction, angle);
}

void TSendDM::doSendRotator( rpcConstants::RotateDirection direction,  int angle )
{
    RPCGeneralClient rpc(rpcConstants::rotatorMethod);
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   st->addMember( static_cast<int> (direction), rpcConstants::rotatorParamDirection );
   st->addMember( angle, rpcConstants::rotatorParamAngle );
   rpc.getCallArgs() ->addParam( st );

   rpc.queueCall( rpcConstants::rotatorApp + "@" + rotatorServerName );
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
      if ( an.getCategory() == rpcConstants::KeyerCategory && an.getKey() == "Report" )
      {
         LogContainer->setKeyerLoaded();
         LogContainer->setCaption( an.getValue() );
         logMessage( "KeyerReport " + an.getValue() );
      }
      if ( an.getCategory() == rpcConstants::RigControlCategory && an.getKey() == "Mode" )
      {
         LogContainer->setMode( an.getValue() );
         logMessage( "RigMode " + an.getValue() );
      }
      if ( an.getCategory() == rpcConstants::RigControlCategory && an.getKey() == "Frequency" )
      {
         LogContainer->setFreq( an.getValue() );
      }
      if ( an.getCategory() == rpcConstants::BandMapCategory && an.getKey() == "Loaded" )
      {
         LogContainer->setBandMapLoaded();
      }
      if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == "State")
      {
         LogContainer->setRotatorLoaded();
         LogContainer->setRotatorState(an.getValue());
      }
      if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == "Bearing")
      {

         LogContainer->setRotatorBearing(an.getValue());
      }
      if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == "MaxAzimuth")
      {

         LogContainer->setRotatorMaxAzimuth(an.getValue());
      }
      if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == "MinAzimuth")
      {

         LogContainer->setRotatorMinAzimuth(an.getValue());
      }
      if ( an.getCategory() == rpcConstants::RotatorCategory && an.getKey() == "AntennaName")
      {

         LogContainer->setRotatorAntennaName(an.getValue());
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

