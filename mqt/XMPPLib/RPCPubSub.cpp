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

//---------------------------------------------------------------------------
bool RPCPubSub::connected = false;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*static*/
void RPCPubSub::initialisePubSub( TRPCFunctor *notifycb )
{
   static bool objAdded = false;
   if ( !objAdded )
   {
      objAdded = true;
      MinosRPCObj::addClientObj( QSharedPointer<MinosRPCObj>(new RPCSubscribeClient( 0 ) ) );
      MinosRPCObj::addClientObj( QSharedPointer<MinosRPCObj>(new RPCPublishClient( 0 ) ) );
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
   for ( std::vector<RPCPublisher *>::iterator i = publishList.begin(); i != publishList.end(); i++ )
   {
      if ( *i )
      {
         ( *i ) ->rePublish();
      }
   }
   for ( std::vector<RPCSubscriber *>::iterator i = subscribeList.begin(); i != subscribeList.end(); i++ )
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
   for ( std::vector<RPCPublisher *>::iterator i = publishList.begin(); i != publishList.end(); i++ )
   {
      delete ( *i );
      ( *i ) = 0;
   }
   publishList.clear();
   for ( std::vector<RPCSubscriber *>::iterator i = subscribeList.begin(); i != subscribeList.end(); i++ )
   {
      delete ( *i );
      ( *i ) = 0;
   }
   subscribeList.clear();
}
//---------------------------------------------------------------------------
AnalysePubSubNotify::AnalysePubSubNotify(bool err, QSharedPointer<MinosRPCObj> mro ) :
      OK( false )
{
   if ( !err )
   {
      QSharedPointer<RPCParam> psServer;
      QSharedPointer<RPCParam> psCategory;
      QSharedPointer<RPCParam> psKey;
      QSharedPointer<RPCParam> psValue;
      QSharedPointer<RPCParam> psState;
      RPCArgs *args = mro->getCallArgs();
      if (
         args->getStructArgMember( 0, "Server", psServer )
         && args->getStructArgMember( 0, "Category", psCategory )
         && args->getStructArgMember( 0, "Key", psKey )
         && args->getStructArgMember( 0, "Value", psValue )
         && args->getStructArgMember( 0, "State", psState )
      )
      {
         int stemp;
         if (
            psServer->getString( server ) &&
            psCategory->getString( category ) &&
            psKey->getString( key ) &&
            psValue->getString( value ) &&
            psState->getInt( stemp )
         )
         {
            state = static_cast<PublishState>(stemp);
            OK = true;
         }
      }
   }
}

