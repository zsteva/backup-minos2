/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "XMPP_pch.h"
#include "ConfigFile.h"

//---------------------------------------------------------------------------
bool RPCPubSub::connected = false;
//---------------------------------------------------------------------------
RPCPubSub::~RPCPubSub()
{}
//---------------------------------------------------------------------------
/*static*/
void RPCPubSub::initialisePubSub( TRPCFunctor *notifycb )
{
   static bool objAdded = false;
   if ( !objAdded )
   {
      objAdded = true;
      MinosRPCObj::addServerObj( QSharedPointer<MinosRPCObj>(new RPCNotifyServer( notifycb )) );
   }
}
void RPCPubSub::publish( const QString &category, const QString &key, const QString &value, PublishState pState )
{
   RPCPublisher::testAndPublish( category, key, value, pState );
}

void RPCPubSub::subscribe( const QString &category )
{
   RPCSubscriber::testAndSubscribe( category );
}
void RPCPubSub::subscribeRemote( const QString &server, const QString &category )
{
   RPCRemoteSubscriber::testAndSubscribe( server, category );
}
void RPCPubSub::reconnectPubSub( )
{
   // iterate the publish and subscribe objects and re-do them
   for ( QVector<RPCPublisher *>::iterator i = publishList.begin(); i != publishList.end(); i++ )
   {
      if ( *i )
      {
         ( *i ) ->rePublish();
      }
   }
   for ( QVector<RPCSubscriber *>::iterator i = subscribeList.begin(); i != subscribeList.end(); i++ )
   {
      if ( *i )
      {
         ( *i ) ->reSubscribe();
      }
   }
   connected = true;
}

void RPCPubSub::close( )
{
   for ( QVector<RPCPublisher *>::iterator i = publishList.begin(); i != publishList.end(); i++ )
   {
      delete ( *i );
      ( *i ) = nullptr;
   }
   publishList.clear();
   for ( QVector<RPCSubscriber *>::iterator i = subscribeList.begin(); i != subscribeList.end(); i++ )
   {
      delete ( *i );
      ( *i ) = nullptr;
   }
   subscribeList.clear();
}
//---------------------------------------------------------------------------

