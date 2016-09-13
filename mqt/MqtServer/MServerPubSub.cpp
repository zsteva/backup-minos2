/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "minos_pch.h"

#include "MinosLink.h"
#include "clientThread.h"
#include "serverThread.h"

extern bool closeApp;

//---------------------------------------------------------------------------
TPubSubMain *PubSubMain = 0;
//---------------------------------------------------------------------------
static void makeRPCObjects()
{
   MinosRPCObj::addServerObj( QSharedPointer<MinosRPCObj>(new RPCPublishServer( new TRPCCallback <TPubSubMain> ( PubSubMain, &TPubSubMain::publishCallback ) ) ) );
   MinosRPCObj::addServerObj( QSharedPointer<MinosRPCObj>(new RPCSubscribeServer( new TRPCCallback <TPubSubMain> ( PubSubMain, &TPubSubMain::subscribeCallback ) ) ) );
   MinosRPCObj::addServerObj( QSharedPointer<MinosRPCObj>(new RPCRemoteSubscribeServer( new TRPCCallback <TPubSubMain> ( PubSubMain, &TPubSubMain::remoteSubscribeCallback ) ) ) );
   MinosRPCObj::addServerObj( QSharedPointer<MinosRPCObj>(new RPCRServerSubscribeServer( new TRPCCallback <TPubSubMain> ( PubSubMain, &TPubSubMain::serverSubscribeCallback ) ) ) );
   MinosRPCObj::addServerObj( QSharedPointer<MinosRPCObj>(new RPCServerNotifyServer( new TRPCCallback <TPubSubMain> ( PubSubMain, &TPubSubMain::serverNotifyCallback ) ) ) );

   RPCServerPubSub::initialisePubSub( new TRPCCallback <TPubSubMain> ( PubSubMain, &TPubSubMain::notifyCallback ) );
}
//---------------------------------------------------------------------------
class Subscriber;
typedef std::vector <Subscriber *> SubscriberList;
typedef std::vector <Subscriber *>::iterator SubscriberListIterator;
//---------------------------------------------------------------------------
class RemoteSubscriber;
typedef std::vector <RemoteSubscriber *> RemoteSubscriberList;
typedef std::vector <RemoteSubscriber *>::iterator RemoteSubscriberListIterator;
//---------------------------------------------------------------------------
class ServerSubscriber;
typedef std::vector <ServerSubscriber *> ServerSubscriberList;
typedef std::vector <ServerSubscriber *>::iterator ServerSubscriberListIterator;
//---------------------------------------------------------------------------
class PublishedCategory;
typedef std::vector <PublishedCategory *> PublishedCategoryList;
typedef std::vector <PublishedCategory *>::iterator PublishedCategoryListIterator;
//---------------------------------------------------------------------------
class PublishedKey;
typedef std::vector <PublishedKey *> PublishedKeyList;
typedef std::vector <PublishedKey *>::iterator PublishedKeyListIterator;
//---------------------------------------------------------------------------
// normal subscriber; client prog of this server
class Subscriber
{
      QString sjid;
   public:

      Subscriber( const QString &sjid )
            : sjid( sjid )
      {}
      Subscriber()
      {}
      ~Subscriber()
      {}
      QString getSjid() const
      {
         return sjid;
      }
      void SendTo( const PublishedKey &pk );
};
//---------------------------------------------------------------------------
// normal subscriber; client prog of this server, subscribed to remote server value
class RemoteSubscriber
{
      QString sjid;
      QString server;
   public:

      RemoteSubscriber( const QString &sjid, const QString &server )
            : sjid( sjid ), server( server )
      {}
      RemoteSubscriber()
      {}
      ~RemoteSubscriber()
      {}
      QString getSjid() const
      {
         return sjid;
      }

      QString getServer() const
      {
         return server;
      }
      void SendTo( const PublishedKey &pk );
};
//---------------------------------------------------------------------------
// remote server subscribed to this server (on bahalf of its client)
class ServerSubscriber
{
      QString sjid;
      QString server;
   public:

      ServerSubscriber( const QString &sjid, const QString &server )
            : sjid( sjid ), server( server )
      {}
      ServerSubscriber()
      {}
      ~ServerSubscriber()
      {}
      QString getSjid() const
      {
         return sjid;
      }

      QString getServer() const
      {
         return server;
      }
      void SendTo( const PublishedKey &pk );
};
//---------------------------------------------------------------------------
// we need to extend this... we need a list of categories
// each of which has a list of keys, and a subscriber can subscribe at either level
class Published
{
   protected:
      QString pubId;
      bool localOnly;

   public:
      static PublishedCategoryList publist;  // base list of categories

      Published( const QString &pubId, bool local );
      virtual ~Published();
      QString getPubId() const
      {
         return pubId;
      }

      static void clearPublist();
      static int GetSubscribedCount();
      static int GetPublishedCount() ;
};
class PublishedCategory: public Published
{
   private:
      PublishedCategory();
      QString category;
   public:
      SubscriberList subscribedLocal;
      RemoteSubscriberList subscribedRemote;
      ServerSubscriberList subscribedServer;
      PublishedKeyList pubkeylist;              // per cat list of key/value pairs
      int GetSubscribedCount();

      static bool publish( const QString &pubId, const QString &category, const QString &key, const QString &value, PublishState state );
      void publish( const QString &pubId, const QString &key, const QString &value , PublishState state );

      static bool serverPublish( const QString &pubId, const QString &svr, const QString &category, const QString &key, const QString &value, PublishState state );
      void serverPublish( const QString &pubId, const QString &svr, const QString &key, const QString &value, PublishState state );

      static void clientSubscribe( const QString &subId, const QString &category );
      static void remoteSubscribe( const QString &subId, const QString &server, const QString &category );
      static void serverSubscribe( const QString &subId, const QString &server, const QString &category );

      // find a published item
      static PublishedCategoryListIterator findPubCategory( const QString &category );

      // find a published item
      PublishedKeyListIterator findPubKey( const QString &svr, const QString &key );

      PublishedCategory( const QString &pubId, const QString &category );
      ~PublishedCategory();

      QString getCategory()
      {
         return category;
      }

      Subscriber * getClientSubscribed( const QString &subId );
      RemoteSubscriber * getRemoteSubscribed( const QString &subId );
      ServerSubscriber * getServerSubscribed( const QString &subId );

};
class PublishedKey: public Published
{
   private:
      PublishedKey();

      QString server;  // published from server
      QString client;  // published from client - should we combine client and server into MinosId?
                           // or is this Published::pubId ?
      QString key;
      QString value;
      PublishState state;
      PublishedCategory *cat;

   public:

      PublishedKey( bool local, const QString &pubId, const QString &svr, PublishedCategory *pcat, const QString &key, PublishState state );
      ~PublishedKey();

      void setPubCat( PublishedCategory *pcat )
      {
         cat = pcat;
      }
      void setPubId( const QString &p )
      {
         pubId = p;
      }
      void setPubValue( const QString &v )
      {
         value = v;
      }
      void setPubState( PublishState pState )
      {
         state = pState;
      }
      PublishedCategory *getPubCat() const
      {
         return cat;
      }
      QString getServer() const
      {
         return server;
      }
      QString getPubKey() const
      {
         return key;
      }
      QString getPubValue() const
      {
         return value;
      }
      PublishState getPubState() const
      {
         return state;
      }
};
//---------------------------------------------------------------------------
void Subscriber::SendTo ( const PublishedKey &pk )
{
   // Build the stanza, and send it to the subid
   RPCClientNotifyClient rnc( 0 );
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   // local - no server
   QSharedPointer<RPCParam>sServer(new RPCStringParam( "" ));
   QSharedPointer<RPCParam>sCategory(new RPCStringParam( pk.getPubCat() ->getCategory() ));
   QSharedPointer<RPCParam>sKey(new RPCStringParam( pk.getPubKey() ));
   QSharedPointer<RPCParam>sValue(new RPCStringParam( pk.getPubValue() ));
   QSharedPointer<RPCParam>sState(new RPCIntParam( pk.getPubState() ));

   st->addMember( sServer, "Server" );
   st->addMember( sCategory, "Category" );
   st->addMember( sKey, "Key" );
   st->addMember( sValue, "Value" );
   st->addMember( sState, "State" );

   rnc.getCallArgs() ->addParam( st );
   rnc.queueCall( getSjid() );
}
//---------------------------------------------------------------------------
void RemoteSubscriber::SendTo ( const PublishedKey &pk )
{
   // Build the stanza, and send it to the subid
   RPCClientNotifyClient rnc( 0 );
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   // server is remote server name (as published)
   QSharedPointer<RPCParam>sServer(new RPCStringParam( pk.getServer() ));
   QSharedPointer<RPCParam>sCategory(new RPCStringParam( pk.getPubCat() ->getCategory() ));
   QSharedPointer<RPCParam>sKey(new RPCStringParam( pk.getPubKey() ));
   QSharedPointer<RPCParam>sValue(new RPCStringParam( pk.getPubValue() ));
   QSharedPointer<RPCParam>sState(new RPCIntParam( pk.getPubState() ));

   st->addMember( sServer, "Server" );
   st->addMember( sCategory, "Category" );
   st->addMember( sKey, "Key" );
   st->addMember( sValue, "Value" );
   st->addMember( sState, "State" );

   rnc.getCallArgs() ->addParam( st );
   rnc.queueCall( getSjid() );
}
//---------------------------------------------------------------------------
void ServerSubscriber::SendTo ( const PublishedKey &pk )
{
   // Build the stanza, and send it to the subid
   RPCServerNotifyClient rnc( 0 );
   QSharedPointer<RPCParam>st(new RPCParamStruct);

   //server is OUR server name
   QSharedPointer<RPCParam>sServer(new RPCStringParam( MinosServer::getMinosServer() ->getServerName() ));
   QSharedPointer<RPCParam>sCategory;
   PublishedCategory *pc = 0;
   QString cat;
   try
   {
      pc = pk.getPubCat();
      cat = pc ->getCategory();
      sCategory = QSharedPointer<RPCParam>(new RPCStringParam( cat ));
   }
   catch(...)
   {
      // sCategory = 0;
   }
   QSharedPointer<RPCParam>sKey(new RPCStringParam( pk.getPubKey() ));
   QSharedPointer<RPCParam>sValue(new RPCStringParam( pk.getPubValue() ));
   QSharedPointer<RPCParam>sState(new RPCIntParam( pk.getPubState() ));

   st->addMember( sServer, "Server" );
   st->addMember( sCategory, "Category" );
   st->addMember( sKey, "Key" );
   st->addMember( sValue, "Value" );
   st->addMember( sState, "State" );

   rnc.getCallArgs() ->addParam( st );
   rnc.queueCall( getSjid() );
}
//---------------------------------------------------------------------------
/*static*/
PublishedCategoryList Published::publist;
//---------------------------------------------------------------------------
Published::Published( const QString &pubId, bool loc ) : pubId( pubId ), localOnly( loc )
{}
//---------------------------------------------------------------------------
void Published::clearPublist()
{
   for ( PublishedCategoryListIterator i = Published::publist.begin(); i != Published::publist.end(); i++ )
   {
      delete ( *i );
      (*i) = 0;
   }
   Published::publist.clear();
}
int Published::GetSubscribedCount()
{
   int scount = 0;
   for ( PublishedCategoryListIterator i = Published::publist.begin(); i != Published::publist.end(); i++ )
   {
      scount += ( *i ) ->GetSubscribedCount();
   }
   return scount;
}
int Published::GetPublishedCount()
{
   int pcount = 0;
   for ( PublishedCategoryListIterator i = Published::publist.begin(); i != Published::publist.end(); i++ )
   {
      pcount += ( *i ) ->pubkeylist.size();
   }
   return pcount;
}
bool noCategoryConnection( PublishedCategory *pc )
{
   if ( pc == 0 )
      return true;
   else
      return false;
}
Published::~Published()
{}
//---------------------------------------------------------------------------
int PublishedCategory::GetSubscribedCount()
{
   int scount = subscribedLocal.size();
   scount += subscribedRemote.size();
   scount += subscribedServer.size();

   return scount;
}
//---------------------------------------------------------------------------
/*static*/ void PublishedCategory::clientSubscribe(  const QString &subId,  const QString &category  )
{
   PublishedCategoryListIterator f = PublishedCategory::findPubCategory( category );
   if ( f == publist.end() )
   {
      PublishedCategory * p = new PublishedCategory( "", category );
      publist.push_back( p );
      f = findPubCategory( category );
   }
   if ( f != publist.end() )
   {
      // we subscribe to ALL keys under the category
      Subscriber * s = ( *f ) ->getClientSubscribed( subId );
      if ( !s )
      {
         s = new Subscriber( subId );
         ( *f ) ->subscribedLocal.push_back( s );
      }

      // and we now need to send them all...
      for ( PublishedKeyListIterator i = ( *f ) ->pubkeylist.begin(); i != ( *f ) ->pubkeylist.end(); i++ )
      {
         // Here we are sending all the already published values
         if ( ( *i ) ->getServer().size() == 0 || ( *i ) ->getServer() == "localhost" || ( *i ) ->getServer() == MinosServer::getMinosServer() ->getServerName() )
         {
            s->SendTo( *( *i ) );
         }
      }
   }
}
//---------------------------------------------------------------------------
/*static*/ void PublishedCategory::remoteSubscribe(  const QString &subId,  const QString &server,  const QString &category  )
{
   PublishedCategoryListIterator f = PublishedCategory::findPubCategory( category );
   if ( f == publist.end() )
   {
      PublishedCategory * p = new PublishedCategory( "", category );
      publist.push_back( p );
      f = findPubCategory( category );
   }
   if ( f != publist.end() )
   {
      // we subscribe to ALL keys under the category
      RemoteSubscriber * s = ( *f ) ->getRemoteSubscribed( subId );
      if ( !s )
      {
         // not yet published
         s = new RemoteSubscriber( subId, server );
         ( *f ) ->subscribedRemote.push_back( s );

         //**** and we need to subscribe to the remote server
      }

      // and we now need to send them all...
      for ( PublishedKeyListIterator i = ( *f ) ->pubkeylist.begin(); i != ( *f ) ->pubkeylist.end(); i++ )
      {
         // Here we are sending all the already published values
         if ( ( *i ) ->getServer() == server )
         {
            s->SendTo( *( *i ) );
         }
      }
      RPCServerPubSub::serverSubscribeRemote( server, category );
   }
}

//---------------------------------------------------------------------------
/*static*/ void PublishedCategory::serverSubscribe(  const QString &subId,  const QString &server,  const QString &category  )
{
   PublishedCategoryListIterator f = PublishedCategory::findPubCategory( category );
   if ( f == publist.end() )
   {
      PublishedCategory * p = new PublishedCategory( "", category );
      publist.push_back( p );
      f = findPubCategory( category );
   }
   if ( f != publist.end() )
   {
      // we subscribe to ALL keys under the category
      ServerSubscriber * s = ( *f ) ->getServerSubscribed( subId );
      if ( !s )
      {
         s = new ServerSubscriber( subId, server );
         ( *f ) ->subscribedServer.push_back( s );
      }

      // and we now need to send them all...
      for ( PublishedKeyListIterator i = ( *f ) ->pubkeylist.begin(); i != ( *f ) ->pubkeylist.end(); i++ )
      {
         // make sure that the key is one published by THIS server; we don't
         // want to re-publish
         // BUT it can publish as blank
         QString pserver = ( *i ) ->getServer();
         if ( pserver.size() == 0 || pserver == "localhost" || pserver == MinosServer::getMinosServer() ->getServerName() )
         {
            // Here we are sending all the already published values

            // THIS DOES NASTY LOCAL THINGS - it DOESN't just send!
            s->SendTo( *( *i ) );
         }
      }
   }
}


//---------------------------------------------------------------------------
// return entry for this Name
/*static*/ PublishedCategoryListIterator PublishedCategory::findPubCategory(  const QString &category  )
{
   for ( PublishedCategoryListIterator i = publist.begin(); i != publist.end(); i++ )
   {
      if ( category == ( *i ) ->category )
         return i;
   }
   return publist.end();
}

//---------------------------------------------------------------------------
// return entry for this Name
PublishedKeyListIterator PublishedCategory::findPubKey( const QString &svr, const QString &key )
{
   for ( PublishedKeyListIterator i = pubkeylist.begin(); i != pubkeylist.end(); i++ )
   {
      PublishedKey *pk = (*i);
      if (pk && key == pk ->getPubKey() && pk ->getServer() == svr )
         return i;
   }
   return pubkeylist.end();
}
//---------------------------------------------------------------------------
/*static*/ bool PublishedCategory::publish(  const QString &pubId,   const QString &category,  const QString &key ,   const QString &value, PublishState pState )
{
   PublishedCategoryListIterator f = PublishedCategory::findPubCategory( category );
   if ( f == publist.end() )
   {
      // create it...
      PublishedCategory * p = new PublishedCategory( pubId, category );
      publist.push_back( p );
      f = PublishedCategory::findPubCategory( category );
   }
   ( *f ) ->publish( pubId, key, value, pState );
   // queue the response
   return true;
}
//---------------------------------------------------------------------------
void PublishedCategory::publish( const QString &pubId, const QString &k, const QString &v , PublishState pState)
{
   PublishedKeyListIterator kl = findPubKey( "", k );
   bool doPub = false;
   if ( kl == pubkeylist.end() && pState == psPublished)
   {
      PublishedKey * p = new PublishedKey( false, pubId, "", this, k, pState );
      pubkeylist.push_back( p );
      kl = findPubKey( "", k );
      doPub = true;
   }

   if ( ( kl != pubkeylist.end() ) && ( doPub || ( *kl ) ->getPubValue() != v || (*kl)->getPubState() != pState) )     // first time, force broadcast anyway
   {
      ( *kl ) ->setPubId( pubId );
      ( *kl ) ->setPubValue( v );
      ( *kl ) ->setPubState( pState );
      for ( SubscriberListIterator i = subscribedLocal.begin(); i != subscribedLocal.end(); i++ )
      {
         // send to all who have subscribed to the category
         ( *i ) ->SendTo( *( *kl ) );
      }
      for ( ServerSubscriberListIterator i = subscribedServer.begin(); i != subscribedServer.end(); i++ )
      {
         // send to all who have subscribed to the category
         ( *i ) ->SendTo( *( *kl ) );
      }
   }
}
//---------------------------------------------------------------------------
/*static*/ bool PublishedCategory::serverPublish(  const QString &pubId,   const QString &svr, const QString &category, const QString &key , const QString &value, PublishState pState )
{
   PublishedCategoryListIterator f = PublishedCategory::findPubCategory( category );
   if ( f == publist.end() )
   {
      // create it...
      PublishedCategory * p = new PublishedCategory( pubId, category );
      publist.push_back( p );
      f = PublishedCategory::findPubCategory( category );
   }
   ( *f ) ->serverPublish( pubId, svr, key, value, pState );
   // queue the response
   return true;
}
//---------------------------------------------------------------------------
void PublishedCategory::serverPublish( const QString &pubId, const QString &svr, const QString &k, const QString &v, PublishState pState )
{

//#warning What happens here? Why do remote contestlogs get reported with no server to the local monitor?
   PublishedKeyListIterator kl = findPubKey( svr, k );
   bool doPub = false;
   if ( kl == pubkeylist.end() )
   {
      PublishedKey * p = new PublishedKey( true, pubId, svr, this, k, pState );
      pubkeylist.push_back( p );
      kl = findPubKey( svr, k );
      doPub = true;
   }

   if ( ( kl != pubkeylist.end() ) && ( doPub || ( *kl ) ->getPubValue() != v || ( *kl) ->getPubState() != pState ) )     // first time, force broadcast anyway
   {
      ( *kl ) ->setPubId( pubId );
      ( *kl ) ->setPubValue( v );
      ( *kl ) ->setPubState( pState );
      for ( RemoteSubscriberListIterator i = subscribedRemote.begin(); i != subscribedRemote.end(); i++ )
      {
         // send to all who have subscribed to the category
         ( *i ) ->SendTo( *( *kl ) );
      }
   }
}
PublishedCategory::PublishedCategory( const QString &pubId, const QString &category ) :
      Published( pubId, false ), category( category )
{}
PublishedCategory::~PublishedCategory()
{
   for ( SubscriberListIterator i = subscribedLocal.begin(); i != subscribedLocal.end(); i++ )
   {
      delete ( *i );
      (*i) = 0;
   }
   subscribedLocal.clear();
   for ( RemoteSubscriberListIterator i = subscribedRemote.begin(); i != subscribedRemote.end(); i++ )
   {
      delete ( *i );
      (*i) = 0;
   }
   subscribedRemote.clear();
   for ( ServerSubscriberListIterator i = subscribedServer.begin(); i != subscribedServer.end(); i++ )
   {
      delete ( *i );
      (*i) = 0;
   }
   subscribedServer.clear();

   for ( PublishedKeyListIterator i = pubkeylist.begin(); i != pubkeylist.end(); i++ )
   {
      delete ( *i );
      (*i) = 0;
   }
   pubkeylist.clear();
}
//---------------------------------------------------------------------------
Subscriber * PublishedCategory::getClientSubscribed( const QString &subId )
{
   for ( SubscriberListIterator i = subscribedLocal.begin(); i != subscribedLocal.end(); i++ )
   {
      if ( ( *i ) ->getSjid() == subId )
         return ( *i );
   }
   return 0;
}
//---------------------------------------------------------------------------
RemoteSubscriber * PublishedCategory::getRemoteSubscribed( const QString &subId )
{
   for ( RemoteSubscriberListIterator i = subscribedRemote.begin(); i != subscribedRemote.end(); i++ )
   {
      if ( ( *i ) ->getSjid() == subId )
         return ( *i );
   }
   return 0;
}
//---------------------------------------------------------------------------
ServerSubscriber * PublishedCategory::getServerSubscribed( const QString &subId )
{
   for ( ServerSubscriberListIterator i = subscribedServer.begin(); i != subscribedServer.end(); i++ )
   {
      if ( ( *i ) ->getSjid() == subId )
         return ( *i );
   }
   return 0;
}
//---------------------------------------------------------------------------
PublishedKey::PublishedKey( bool local, const QString &pubId, const QString &svr, PublishedCategory *pcat, const QString &key, PublishState pState ) :
      Published( pubId, local ), server( svr ), cat( pcat ), key( key ), state ( pState )
{}
PublishedKey::~PublishedKey()
{}
//---------------------------------------------------------------------------
void TPubSubMain::logMessage( QString s )
{
   trace( s );
}
//---------------------------------------------------------------------------
TPubSubMain::TPubSubMain( )
{
   PubSubMain = this;
   makeRPCObjects();
}
//---------------------------------------------------------------------------
TPubSubMain::~TPubSubMain()
{
}
//---------------------------------------------------------------------------
const char *stateList[] =
{
   "P",
   "R",
   "NC"
};
bool TPubSubMain::publish( const QString &pubId, const QString &category, const QString &key, const QString &value, PublishState pState )
{
   if (closeApp)
   {
      return false;
   }
   logMessage("Publishing from <" + pubId + "> cat " + category + " key " + key + " state " + stateList[pState]);
   return PublishedCategory::publish( pubId, category, key, value, pState );
}
bool TPubSubMain::serverPublish( const QString &pubId, const QString &svr, const QString &category, const QString &key, const QString &value, PublishState pState )
{
   if (closeApp)
   {
      return false;
   }
   logMessage("Server Publishing from svr <" + svr + "> pubid <" + pubId + "> cat " + category + " key " + key+ " state " + stateList[pState]);
   return PublishedCategory::serverPublish( pubId, svr, category, key, value, pState );
}
int GetSubscribedCount()
{
   return Published::GetSubscribedCount();
}
int GetPublishedCount()
{
   return Published::GetPublishedCount();
}
//---------------------------------------------------------------------------

// callback to publish local server - this may get proxied when we have a remote server subscriber

void TPubSubMain::publishCallback( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
   logMessage( "Publish callback from " + from + ( err ? ":Error" : ":Normal" ) );

   if ( !err )
   {
      QSharedPointer<RPCParam> psCategory;
      QSharedPointer<RPCParam> psKey;
      QSharedPointer<RPCParam> psValue;
      QSharedPointer<RPCParam> psState;
      RPCArgs *args = mro->getCallArgs();
      if ( args->getStructArgMember( 0, "Category", psCategory )
           && args->getStructArgMember( 0, "Key", psKey )
           && args->getStructArgMember( 0, "Value", psValue )
           && args->getStructArgMember( 0, "State", psState )
           )
      {
         QString Category;
         QString Key;
         QString Value;
         PublishState State;
         int temps;

         if ( psCategory->getString( Category ) && psKey->getString( Key )
                  && psValue->getString( Value )  && psState->getInt( temps ))
         {
            State = static_cast<PublishState>(temps);
            QSharedPointer<RPCParam>st(new RPCParamStruct);
            if ( TPubSubMain::publish( from, Category, Key, Value, State ) )
            {
               st->addMember( true, "PublishResult" );
               mro->clearCallArgs();
               mro->getCallArgs() ->addParam( st );
               mro->queueResponse( from );
            }
            else
            {
               st->addMember( "RPC error", "PublishResult" );
               mro->clearCallArgs();
               mro->getCallArgs() ->addParam( st );
               mro->queueErrorResponse( from );

            }
         }
      }
   }
}
//---------------------------------------------------------------------------

// callback to subscribe client - local server (there should not be a server member)

void TPubSubMain::subscribeCallback(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
   logMessage( "Client Subscribe callback from " + from + ( err ? ":Error" : ":Normal" ) );
   if ( !err )
   {
      QSharedPointer<RPCParam>st(new RPCParamStruct);
      QSharedPointer<RPCParam> psCategory;
      RPCArgs *args = mro->getCallArgs();
      if ( args->getStructArgMember( 0, "Category", psCategory ) )
      {
         QString Category;
         bool resc = psCategory->getString( Category );
         if ( resc )
         {
            PublishedCategory::clientSubscribe( from, Category );

            st->addMember( true, "ClientSubscribeResult" );
            mro->clearCallArgs();
            mro->getCallArgs() ->addParam( st );
            mro->queueResponse( from );
         }
         else
         {
            st->addMember( "Bad Params", "ClientSubscribeResult" );
            mro->clearCallArgs();
            mro->getCallArgs() ->addParam( st );
            mro->queueErrorResponse( from );
         }
      }
      else
      {
         st->addMember( "ClientSubscribeResult", "Bad Params" );
         mro->clearCallArgs();
         mro->getCallArgs() ->addParam( st );
         mro->queueErrorResponse( from );
      }
   }
}
//---------------------------------------------------------------------------

// callback to subscribe client  - remote server

void TPubSubMain::remoteSubscribeCallback( bool err, QSharedPointer<MinosRPCObj>mro, const QString &from )
{
   logMessage( "Remote Subscribe callback from " + from + ( err ? ":Error" : ":Normal" ) );
   if ( !err )
   {
      QSharedPointer<RPCParam>st(new RPCParamStruct);
      QSharedPointer<RPCParam> psServer;
      QSharedPointer<RPCParam> psCategory;
      RPCArgs *args = mro->getCallArgs();
      if ( args->getStructArgMember( 0, "Server", psServer ) &&
           args->getStructArgMember( 0, "Category", psCategory ) )
      {
         QString Server;
         bool ress = psServer->getString( Server );
         QString Category;
         bool resc = psCategory->getString( Category );
         if ( ress && resc )
         {
            if ( !Server.size() || Server == "localhost" || Server == MinosServer::getMinosServer() ->getServerName() )
            {
               subscribeCallback( err, mro, from );     // actually, for local server
               return ;
            }

            PublishedCategory::remoteSubscribe( from, Server, Category );

            mro->clearCallArgs();
            st->addMember( true, "RemoteSubscribeResult" );
            mro->getCallArgs() ->addParam( st );
            mro->queueResponse( from );
         }
         else
         {
            mro->clearCallArgs();
            st->addMember( "Bad Params", "RemoteSubscribeResult" );
            mro->getCallArgs() ->addParam( st );
            mro->queueErrorResponse( from );
         }
      }
      else
      {
         st->addMember( "RemoteSubscribeResult", "Bad Params" );
         mro->getCallArgs() ->addParam( st );
         mro->queueErrorResponse( from );
      }
   }
}
//---------------------------------------------------------------------------

// callback to subscribe server  - remote server

void TPubSubMain::serverSubscribeCallback(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
   logMessage( "Server Subscribe callback from " + from + ( err ? ":Error" : ":Normal" ) );
   if ( !err )
   {
      QSharedPointer<RPCParam>st(new RPCParamStruct);
      QSharedPointer<RPCParam> psServer;
      QSharedPointer<RPCParam> psCategory;
      RPCArgs *args = mro->getCallArgs();
      if ( args->getStructArgMember( 0, "Server", psServer ) &&
           args->getStructArgMember( 0, "Category", psCategory ) )
      {
         QString Server;
         bool ress = psServer->getString( Server );
         QString Category;
         bool resc = psCategory->getString( Category );
         if ( ress && resc )
         {
            PublishedCategory::serverSubscribe( from, Server, Category );

            mro->clearCallArgs();
            st->addMember( true, "ServerSubscribeResult" );
            mro->getCallArgs() ->addParam( st );
            mro->queueResponse( from );
         }
         else
         {
            st->addMember( "Bad Params", "ServerSubscribeResult" );
            mro->clearCallArgs();
            mro->getCallArgs() ->addParam( st );
            mro->queueErrorResponse( from );
         }
      }
      else
      {
         st->addMember( "ServerSubscribeResult", "Bad Params" );
         mro->clearCallArgs();
         mro->getCallArgs() ->addParam( st );
         mro->queueErrorResponse( from );
      }
   }
}
//---------------------------------------------------------------------------

// callback for responses to notify messages

void TPubSubMain::notifyCallback( bool err, QSharedPointer<MinosRPCObj> /*mro*/, const QString &from )
{
   // response to pubsub calls
   logMessage( "Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
}
//---------------------------------------------------------------------------

// this we get when we get a subscribe notification from a remote server

void TPubSubMain::serverNotifyCallback(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from )
{
   // we need to pass it on to any of our subscribers who are interested
   // in this event from this server
   // But why aren't we sending a result?
   logMessage( "PubSub Notify callback from " + from + ( err ? ":Error" : ":Normal" ) );
   AnalysePubSubNotify an( err, mro );

   if ( an.getOK() )
   {
      QString server = an.getServer();
      QString category = an.getCategory();
      QString key = an.getKey();
      QString value = an.getValue();
      PublishState state = an.getState();
      serverPublish( from, server, category, key, value, state );       // but we mustn't publish this back to any remote servers
      // even if they ARE subscribed
      mro->clearCallArgs();
      QSharedPointer<RPCParam>st(new RPCParamStruct);
      st->addMember( true, "ServerNotifyResult" );
      mro->getCallArgs() ->addParam( st );
      mro->queueResponse( from );
   }
}
bool nopub( PublishedKey *ip )
{
   if ( ip == 0 )
      return true;
   else
      return false;
}
void TPubSubMain::revokeClient(const QString &pubId)
{
   if (closeApp)
   {
      return;
   }
   for ( PublishedCategoryListIterator f = Published::publist.begin(); f != Published::publist.end(); f++ )
   {
      for ( PublishedKeyListIterator i = ( *f ) ->pubkeylist.begin(); i != ( *f ) ->pubkeylist.end(); i++ )
      {
         if ((*i)->getPubId() == pubId)
         {
            // publish revoke
            TPubSubMain::publish( pubId, (*f)->getCategory(), (*i)->getPubKey(), "", psRevoked );
            delete (*i);
            (*i) = 0;
         }
      }
      (*f)->pubkeylist.erase( std::remove_if( (*f)->pubkeylist.begin(), (*f)->pubkeylist.end(), nopub ), (*f)->pubkeylist.end() );
      // and now clear up the published key list for the category
   }
}
void TPubSubMain::disconnectServer(const QString &pubId)
{
   if (closeApp)
   {
      return;
   }
   for ( PublishedCategoryListIterator f = Published::publist.begin(); f != Published::publist.end(); f++ )
   {
      for ( PublishedKeyListIterator i = ( *f ) ->pubkeylist.begin(); i != ( *f ) ->pubkeylist.end(); i++ )
      {
         if ((*i)->getPubId() == pubId)
         {
            // publish revoke
            TPubSubMain::serverPublish( pubId, MinosId(pubId).server, (*f)->getCategory(), (*i)->getPubKey(), "", psNotConnected );
         }
      }
   }
}

void TPubSubMain::closeDown()
{
    // need to clear all the lists
    Published::clearPublist();
    MinosRPCObj::clearRPCObjects();
    RPCServerPubSub::close( );
}
