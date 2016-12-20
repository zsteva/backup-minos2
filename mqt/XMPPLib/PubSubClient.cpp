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
QVector<RPCSubscriber *> subscribeList;
QVector<RPCPublisher *> publishList;

bool RPCSubscriber::isEqual( const QString &pcategory )
{
   return ( pcategory == category );
}
bool RPCSubscriber::isRemoteEqual( const QString &/*pserver*/, const QString &/*pcategory*/ )
{
   return false;
}
void RPCSubscriber::testAndSubscribe( const QString &category )
{
   RPCSubscriber * sub = 0;
   for ( QVector<RPCSubscriber *>::iterator i = subscribeList.begin(); i != subscribeList.end(); i++ )
   {
      if ( ( *i ) ->isEqual( category ) )
      {
         sub = ( *i );
         break;
      }
   }
   if ( !sub )
   {
      sub = new RPCSubscriber( category );
      subscribeList.push_back( sub );
   }
   if ( RPCPubSub::isConnected() )
   {
      sub->reSubscribe();
   }
}
bool RPCRemoteSubscriber::isRemoteEqual( const QString &pServer, const QString &category )
{
   return server == pServer && isEqual( category );
}
void RPCRemoteSubscriber::testAndSubscribe( const QString &server, const QString &category )
{
   RPCRemoteSubscriber * sub = 0;
   for ( QVector<RPCSubscriber *>::iterator i = subscribeList.begin(); i != subscribeList.end(); i++ )
   {
      if ( ( *i ) ->isRemoteEqual( server, category ) )
      {
         sub = dynamic_cast<RPCRemoteSubscriber *>( *i );
         if (sub)
         {
            break;
         }
      }
   }
   if ( !sub )
   {
      sub = new RPCRemoteSubscriber( server, category );
      subscribeList.push_back( sub );
   }
   if ( RPCPubSub::isConnected() )
   {
      sub->reSubscribe();
   }
}
void RPCSubscriber::reSubscribe()
{
   RPCSubscribeClient rsc( 0 );
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sServer(new RPCStringParam( "localhost" ));
   QSharedPointer<RPCParam>sCat(new RPCStringParam( category ));
   st->addMember( sServer, "Server" );
   st->addMember( sCat, "Category" );
   rsc.getCallArgs() ->addParam( st );
   rsc.queueCall( "localhost" );
}
// client resubscribing to remote event
void RPCRemoteSubscriber::reSubscribe()
{
   RPCRemoteSubscribeClient rsc( 0 );
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sServer(new RPCStringParam( server ));
   QSharedPointer<RPCParam>sCat(new RPCStringParam( category ));
   st->addMember( sServer, "Server" );
   st->addMember( sCat, "Category" );
   rsc.getCallArgs() ->addParam( st );
   rsc.queueCall( "localhost" );       // localhost just causes the server to loop
}

void RPCPublisher::testAndPublish( const QString &category, const QString &key, const QString &value, PublishState pState )
{
   RPCPublisher * pub = 0;
   for ( QVector<RPCPublisher *>::iterator i = publishList.begin(); i != publishList.end(); i++ )
   {
      if ( ( *i ) ->category == category && ( *i ) ->key == key )
      {
         pub = ( *i );
      }
   }
   if ( !pub )
   {
      pub = new RPCPublisher( category, key, value, pState );
      publishList.push_back( pub );
   }
   else
   {
      pub->value = value;
      pub->state = pState;
   }
   if ( RPCPubSub::isConnected() )
   {
      pub->rePublish();
   }
}
void RPCPublisher::rePublish()
{
   RPCPublishClient rpc( 0 );
   QSharedPointer<RPCParam>st(new RPCParamStruct);
   QSharedPointer<RPCParam>sCat(new RPCStringParam( category ));
   QSharedPointer<RPCParam>sKey(new RPCStringParam( key ));
   QSharedPointer<RPCParam>sValue(new RPCStringParam( value ));
   QSharedPointer<RPCParam>sState(new RPCIntParam( state ));
   st->addMember( sCat, "Category" );
   st->addMember( sKey, "Key" );
   st->addMember( sValue, "Value" );
   st->addMember( sState, "State" );
   rpc.getCallArgs() ->addParam( st );
   rpc.queueCall( "localhost" );
}

//---------------------------------------------------------------------------

