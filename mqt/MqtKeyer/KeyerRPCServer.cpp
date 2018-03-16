/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#include "base_pch.h"

#include "MinosRPC.h"

#include "keyctrl.h"
#include "KeyerRPCServer.h"

KeyerServer *KS = 0;
//---------------------------------------------------------------------------
KeyerServer::KeyerServer()
{
    MinosRPC *rpc = MinosRPC::getMinosRPC(rpcConstants::keyerApp);

    connect(rpc, SIGNAL(serverCall(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_serverCall(bool,QSharedPointer<MinosRPCObj>,QString)));
    connect(rpc, SIGNAL(notify(bool,QSharedPointer<MinosRPCObj>,QString)), this, SLOT(on_notify(bool,QSharedPointer<MinosRPCObj>,QString)));

    RPCPubSub::subscribe( "LineControl" );
}
//---------------------------------------------------------------------------
KeyerServer::~KeyerServer()
{
}
//---------------------------------------------------------------------------
/*static*/void KeyerServer::checkConnection()
{
   if ( !KS )
      KS = new KeyerServer;
}
//---------------------------------------------------------------------------
void KeyerServer::doPublishState( const QString &state )
{
  static QString old;

  if ( state != old )
  {
     old = state;
     RPCPubSub::publish( rpcConstants::KeyerCategory, rpcConstants::keyerReport, state, psPublished );
  }
}
/*static*/void KeyerServer::publishState(  const QString &state )
{
   checkConnection();

   KS->doPublishState( state );
}
//---------------------------------------------------------------------------
void KeyerServer::doPublishCommand( const QString &cmd )
{
      static QString old;

      if ( cmd != old )
      {
         old = cmd;
         RPCPubSub::publish( "Keyer", "Command", cmd, psPublished );
      }
}
/*static*/void KeyerServer::publishCommand(  const QString &cmd )
{
    checkConnection();

    KS->doPublishCommand( cmd );
}
//---------------------------------------------------------------------------
void KeyerServer::on_serverCall( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
   trace( "Keyer callback from " + from + ( err ? ":Error" : ":Normal" ) );

   if ( !err )
   {
      QSharedPointer<RPCParam> psName;
      QSharedPointer<RPCParam>piValue;
      RPCArgs *args = mro->getCallArgs();
      if ( args->getStructArgMember( 0, "Name", psName ) && args->getStructArgMember( 0, "Value", piValue ) )
      {
         QString Name;
         int Value;

         if ( psName->getString( Name ) && piValue->getInt( Value ) )
         {
            if ( Value >= 1 && Value <= 12 )
            {
               if ( Name == "PlayFile" )
               {
                  playKeyerFile( Value, true );    // do actual transmit, and repeat as required
               }
               else
                  if ( Name == "RecordFile" )
                  {
                     startRecordDVPFile( Value );
                  }
           }
               if ( Name == "Tone" )
               {
                  sendTone1();
               }
            else
               if ( Name == "TwoTone" )
               {
                  sendTone2();
               }
            else
               if ( Name == "Stop" )
               {
                  stopKeyer();
               }

            mro->clearCallArgs();
            QSharedPointer<RPCParam>st(new RPCParamStruct);
         }
      }
   }
}
//---------------------------------------------------------------------------
void KeyerServer::on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
   trace( "Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
   AnalysePubSubNotify an( err, mro );

   // called whenever line changes
   if ( an.getOK() )
   {
      if ( an.getCategory() == "LineControl" )
      {
         lineStates[ an.getKey() ] = ( ( an.getValue() == "set" ) ? true : false );
         trace( "LineControl" + an.getKey() + ":" + an.getValue() );
      }
   }
}
//---------------------------------------------------------------------------
bool KeyerServer::getState( const QString &line )
{
   QMap<QString, bool>::iterator l = lineStates.find( line );
   if ( l != lineStates.end() )
   {
      return l.value();
   }
   return false;
}
