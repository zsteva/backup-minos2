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
#include "BandMapRPCObj.h"
#include "KeyerRPCObj.h"
#include "LoggerRPCObj.h"
#include "RPCPubSub.h"

#include "SendRPCDM.h"
#include "tsinglelogframe.h"
#include "tlogcontainer.h"

TSendDM *SendDM = 0;
//---------------------------------------------------------------------------
void TSendDM::makeRPCObjects()
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

   RPCPubSub::initialisePubSub( new TRPCCallback <TSendDM> ( SendDM, &TSendDM::notifyCallback ) );

   MinosRPCObj::addObj( new RPCBandMapClient( new TRPCCallback <TSendDM> ( SendDM, &TSendDM::bandMapClientCallback ) ) );
   MinosRPCObj::addObj( new RPCBandMapServer( new TRPCCallback <TSendDM> ( SendDM, &TSendDM::bandMapServerCallback ) ) );
   MinosRPCObj::addObj( new RPCKeyerControlClient( new TRPCCallback <TSendDM> ( SendDM, &TSendDM::keyerCallback ) ) );

   TRPCFunctor *lccb = new TRPCCallback <TSendDM> ( SendDM, &TSendDM::loggerServerCallback );
   MinosRPCObj::addObj( new RPCLogSubscribeServer( lccb ) );

}
//---------------------------------------------------------------------------
/*static*/ void TSendDM::sendKeyerPlay(  int fno )
{
    if (SendDM)
        SendDM->doSendKeyerPlay(fno);
}

void TSendDM::doSendKeyerPlay(  int fno )
{
   RPCKeyerControlClient rpc( 0 );
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sName(new RPCStringParam( "PlayFile" ));
   QSharedPointer<RPCParam>iValue(new RPCIntParam( fno ));
   st->addMember( sName, "Name" );
   st->addMember( iValue, "Value" );
   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( "Keyer@" + keyerServerName );
}
/*static*/ void TSendDM::sendKeyerRecord(  int fno )
{
    if (SendDM)
        SendDM->doSendKeyerRecord(fno);
}

void TSendDM::doSendKeyerRecord(  int fno )
{
   RPCKeyerControlClient rpc( 0 );
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sName(new RPCStringParam( "RecordFile" ));
   QSharedPointer<RPCParam>iValue(new RPCIntParam( fno ));
   st->addMember( sName, "Name" );
   st->addMember( iValue, "Value" );
   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( "Keyer@" + keyerServerName );
}
//---------------------------------------------------------------------------
void TSendDM::sendBandMap(  const QString &freq,   const QString &call,   const QString &utc,   const QString &loc,   const QString &qth )
{
    if (SendDM)
        SendDM->doSendBandMap(freq, call, utc, loc, qth);
}

/*static*/ void TSendDM::doSendBandMap(  const QString &freq,   const QString &call,   const QString &utc,   const QString &loc,   const QString &qth )
{
   RPCBandMapClient rpc( 0 );
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   QSharedPointer<RPCParam>sName(new RPCStringParam( "BandMap" ));
   st->addMember( sName, "Name" );

   QSharedPointer<RPCParam>sValue(new RPCStringParam( freq ));
   st->addMember( sValue, "Freq" );

   QSharedPointer<RPCParam>sCall(new RPCStringParam( call ));
   st->addMember( sCall, "Callsign" );

   QSharedPointer<RPCParam>sLoc(new RPCStringParam( loc ));
   st->addMember( sLoc, "Locator" );

   QSharedPointer<RPCParam>dValue(new RPCDtgParam( utc ));
   st->addMember( dValue, "UTC" );

   QSharedPointer<RPCParam>sQTH(new RPCStringParam( qth ));
   st->addMember( sQTH, "QTH" );

   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( "BandMap@" + bandMapServerName );
}
//---------------------------------------------------------------------------
TSendDM::TSendDM( QWidget* Owner )
      : QObject( Owner ), connected( false ), subscribed( false )
{
    connect(&ConnectTimer, SIGNAL(timeout()), this, SLOT(ConnectTimerTimer()));
    connect(&SubscribeTimer, SIGNAL(timeout()), this, SLOT(SubscribeTimerTimer()));
    ConnectTimer.start(1000);
}
TSendDM::~TSendDM()
{
   MinosRPCObj::clearRPCObjects();
   XMPPClosedown();

   SendDM = 0;
}
//---------------------------------------------------------------------------
void TSendDM::logMessage( QString s )
{
   trace( s );
}
//---------------------------------------------------------------------------
void TSendDM::ConnectTimerTimer(  )
{
   if ( checkServerReady() )
   {
      ConnectTimer.stop();
      makeRPCObjects();
      XMPPInitialise( "Logger" );
      connected = true;
      SubscribeTimer.start(1000);
   }
}
//---------------------------------------------------------------------------
void TSendDM::SubscribeTimerTimer( )
{
   subscribeAll(); // may need to be repeated... won't do it too often, though
}
//---------------------------------------------------------------------------
void TSendDM::bandMapClientCallback( bool err, MinosRPCObj * /*mro*/, const QString &from )
{
   logMessage( "BandMap client callback from " + from + ( err ? ":Error" : ":Normal" ) );
   // transfer from contact to bandmap
}
//---------------------------------------------------------------------------
void TSendDM::bandMapServerCallback( bool err, MinosRPCObj * /*mro*/, const QString &from )
{
   logMessage( "BandMap server callback from " + from + ( err ? ":Error" : ":Normal" ) );
   // transfer from bandmap to new contact
}
//---------------------------------------------------------------------------
void TSendDM::keyerCallback( bool err, MinosRPCObj * /*mro*/, const QString &from )
{
   logMessage( "Keyer callback from " + from + ( err ? ":Error" : ":Normal" ) );
}
//---------------------------------------------------------------------------
void TSendDM::subscribeAll()
{
   if ( SendDM && SendDM->connected && !SendDM->subscribed )
   {
      SendDM->subscribed = true;

      RPCPubSub::subscribeRemote( rigServerName, "RigControl" );

      RPCPubSub::subscribeRemote( keyerServerName, "Keyer" );

      RPCPubSub::subscribeRemote( bandMapServerName, "BandMap" );
   }
}
//---------------------------------------------------------------------------
void TSendDM::notifyCallback( bool err, MinosRPCObj *mro, const QString &from )
{
   // PubSub notifications
   logMessage( "Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
   AnalysePubSubNotify an( err, mro );

   // called whenever frequency changes
   if ( an.getOK() )
   {
      if ( an.getCategory() == "Keyer" && an.getKey() == "Report" )
      {
         LogContainer->setCaption( an.getValue() );
         logMessage( "KeyerReport " + an.getValue() );
      }
      if ( an.getCategory() == "RigControl" && an.getKey() == "Mode" )
      {
         LogContainer->setMode( an.getValue() );
         logMessage( "RigMode " + an.getValue() );
      }
      if ( an.getCategory() == "RigControl" && an.getKey() == "Frequency" )
      {
         LogContainer->setFreq( an.getValue() );
      }
      if ( an.getCategory() == "BandMap" && an.getKey() == "Loaded" )
      {
         LogContainer->setBandMapLoaded();
      }
   }

}
//---------------------------------------------------------------------------
void TSendDM::zconfCallback( bool err, MinosRPCObj * /*mro*/, const QString &from )
{
   logMessage( "RPC Response callback from " + from + ( err ? ":Error" : ":Normal" ) );
}
//---------------------------------------------------------------------------
void TSendDM::loggerServerCallback( bool err, MinosRPCObj *mro, const QString &from )
{
   // responds to pull calls from the monitoring client
   logMessage( "RPC loggerServerCallback from " + from + ( err ? ":Error" : ":Normal" ) );
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

