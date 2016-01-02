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

GJV_thread *serverThread = 0;
MinosServerListener *MinosServerListener::MSL = 0;
//---------------------------------------------------------------------------
void runServerThread( void * )
{
//   WSAGuard guardian;

   if ( MinosServerListener::getListener() )
      return ;
   try
   {
      /*
         Next open listener for Servers to connect to
      */
      new MinosServerListener();
      MinosServerListener::getListener() ->initialise( "Server", ServerPort );
   }
   catch ( Exception & e )
   {
      logMessage( "Exception", QString( "Exception while setting up Server listener: " ) + e.what() );
      return ;
   }
   try
   {
      while ( !closeApp )
      {
         MinosServerListener::getListener() ->processSockets();
      }
   }
   catch ( Exception & e )
   {
      logMessage( "Exception", QString( "Exception while running Server listener: " ) + e.what() );
      return ;
   }
   try
   {
      //      MinosServerListener::getListener() ->clearSockets();
      delete MinosServerListener::getListener();
   }
   catch ( Exception & e )
   {
      logMessage( "Exception", QString( "Exception while closing Server listener: " ) + e.what() );
      return ;
   }
}
//==============================================================================
MinosCommonConnection *MinosServerListener::makeConnection(QTcpSocket *s)
{
    MinosServerConnection *c = new MinosServerConnection();
    c->sock = QSharedPointer<QTcpSocket>(s);

    return c;
}

//==============================================================================
bool MinosServerListener::sendServer( MinosCommonConnection *il, TiXmlElement *tix )
{
   MinosId to( getAttribute( tix, "to" ) );

   if ( to.server.size() == 0 )
      return false;
   if ( to.server.compare( MinosServer::getMinosServer() ->getServerName(), Qt::CaseInsensitive) == 0 )
      return false;
   if ( to.server.compare( DEFAULT_SERVER_NAME, Qt::CaseInsensitive ) == 0 )
      return false;

   // OK, it is not for us... look at connected servers

   //   bool connect = true;
   bool connect = false;
   for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      if ( ( *i ) ->checkServer( to ) && (*i) ->isTxConnection() )
      {
         if ( !( *i ) ->tryForwardStanza( tix ) )
         {
            connect = false;
            break;
         }
         return true;
      }
   }
   // send failed; stash the message and initiate a server connection
   // (but some stanza types should be ignored?)
   if ( connect && MinosServer::getMinosServer() ->getServerName() != DEFAULT_SERVER_NAME )
   {
      // We need to look at the servers vector, and try to find the relevant one
      // If we can't find it, we refuse anyway

      Server * srv = findStation( to.server );
      if ( srv )
      {
         // set ourselves up to connect
         MinosServerConnection * s = new MinosServerConnection();
         if ( s->mConnect( srv ) )
         {
            connectFreeSlot( s );
            // and we need to TRY to resend
            if (!s ->tryForwardStanza( tix ))
            {
               connect = false;
            }
            return true;
         }
         else
         {
            delete s;

            // and continue to refuse the message
         }
      }
   }

   // server is not connected; REFUSE the message.

   // The originator should retry sometime later to send message again

   if ( il )
      il->sendError( tix, "retry", "item-not-found" );  // deletes pak
   return true;   // don't pass it on - either we have dealt with it, or its rubbish
}
void MinosServerListener::checkServerConnected( Server *srv, bool force )
{
   if ( srv->local )
   {
      return ;
   }
   for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      if ( ( *i ) && ( *i ) ->checkServer( srv->station ) && (*i)->isTxConnection() )
      {
         return ;
      }
   }
   if (force || srv->autoReconnect)
   {
      MinosServerConnection * s = new MinosServerConnection();
      if ( s->mConnect( srv ) )
         connectFreeSlot( s );
      else
         delete s;
   }
}

bool MinosServerListener::checkStillServerConnection( const QString &s )
{
   for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      if ( ( *i ) && ( *i ) ->checkServer( s ) )
      {
         return true;
      }
   }
   return false;
}
//==============================================================================
//==============================================================================
MinosServerConnection::MinosServerConnection() : srv( 0 ), resubscribed( false )
{}
bool MinosServerConnection::initialise()
{
   return true;   // already initialised
}

MinosServerConnection::~MinosServerConnection()
{
   logMessage( "Server Link", "Closing" );
   // here we need to mark all of this servers published keys as disconnected
   // but we leave them in place - server connections are to be treated as fragile
   // When it reconnects what do we do with those that aren't republished?
   // after a time they should be retired - server may have been restarted
   PubSubMain->disconnectServer(makeJid());
   if (srv)
      ZConf->publishDisconnect(srv->station);
}
bool MinosServerConnection::checkFrom( TiXmlElement *tix )
{
   // No "from" on a server link - kill the link
   const char * from = tix->Attribute( "from" );

   if ( !from )
      return false;

   // but "from" doesn't actually need to be correct - it could have been proxied...

   return true;
}
bool MinosServerConnection::ioConnect ( const QString &server, int port )
{
    /*
   int sock = -1;
   int tmp;
   struct hostent *host;
   struct sockaddr_in sin;

   host = gethostbyname ( server.c_str() );
   if ( !host )
      return false;

   memcpy ( &sin.sin_addr, host->h_addr, host->h_length );
   sin.sin_family = host->h_addrtype;
   sin.sin_port = htons ( port );

   sock = socket ( host->h_addrtype, SOCK_STREAM, 0 );
   if ( sock == -1 )
      return false;

   tmp = connect ( sock, ( struct sockaddr * ) & sin, sizeof ( struct sockaddr_in ) );
   if ( tmp != 0 )
   {
      closesocket( sock );
      return false;
   }

//   setSocket( sock );
   txConnection = true;
   return true;
*/
   return false;
}

bool MinosServerConnection::connectWith ( const QString &host, int port )
{
   // We need to connect out to the end point - looks much like a client connection!
   bool ret = ioConnect ( host, port );
   return ret;
}

bool MinosServerConnection::mConnect( Server *psrv )
{
   srv = psrv;
   clientServer = srv->station;

   logMessage( "Server", QString( "Connecting to " ) + srv->station + " host " + srv->host );

   // connect to endpoint
   bool ret = connectWith ( srv->host, srv->port );  // server port
   if (ret)
   {
      RPCRequest *rpa = new RPCRequest( clientServer, MinosServer::getMinosServer() ->getServerName(), "ServerSetFromId" );   // for our local server, this one MUST have a from
      rpa->addParam( MinosServer::getMinosServer() ->getServerName() );
      rpa->addParam( ZConf->getZConfString( ) );
      sendAction( rpa );               // This wants to be a queue response...
      delete rpa;
   }
   else
   {
      logMessage( "Server", QString( "Connection to " ) + srv->station + " host " + srv->host + " failed");
   }
   return ret;
}
//==============================================================================
bool MinosServerConnection::setFromId( MinosId &id, RPCRequest *req )
{
   resubscribed = false;
   // and we need to check that the originator is who we think they ought to be
   if ( !id.server.size() )
   {
      logMessage( "ServerSetFromId", "No \"from\" from server " + srv->station );
      return false;
   }
   if ( srv && srv->station.compare( id.server, Qt::CaseInsensitive) != 0 )
   {
      logMessage( "ServerSetFromId", "Mismatch from server " + srv->station + " we received \"" + id.server + "\"" );
      return false;
   }

   if ( !srv )
   {
      // we need to find who is connecting to us
      srv = findStation( id.server );
      if ( srv )
      {
         logMessage( "ServerSetFromId", "server " + srv->station + " connected to us" );
      }
      else
      {
         logMessage( "ServerSetFromId", "server " + QString( id.server ) + " tried to connect to us - not recognised" );
         // SO we need to set up a server

         QString message;
         if (req->getStringArg(1, message))
         {
            ZConf->setZConfString(message, QHostAddress(connectHost));
         }
      }
   }
   else
   {
      logMessage( "ServerSetFromId", QString( "server " ) + id.server + " connected to us - srv already set up as " + srv->station );
   }
   clientServer = id.server;

   return true;
}
//==============================================================================
void MinosServerConnection::sendAction( XStanza *a )
{
   // use the stanza to send itself
   a->setNextId();   // only happens if no Id already

   QString s = a->getActionMessage();
   sendRaw( s.toStdString().c_str() );
}
//==============================================================================
void MinosServerConnection::sendKeepAlive( )
{
   // Every ??? send a heartbeat to make sure the link stays open
   if ( !resubscribed && srv )
   {
      if ( clientServer.size() && clientServer.compare( "localhost", Qt::CaseInsensitive ) != 0 &&
            clientServer.compare( MinosServer::getMinosServer() ->getServerName(), Qt::CaseInsensitive) != 0 )
      {
         RPCServerPubSub::serverReconnectRemotePubSub( srv->station );
         resubscribed = true;
         return ;
      }
   }
   QDateTime tickCount = QDateTime::currentDateTimeUtc();
   if ( srv && lastEventTick.msecsTo(tickCount) > 15000 )  // tickCount is milliseconds
   {
      lastEventTick = tickCount;
      //      RPCRequest *rpa = new RPCRequest( /*to*/srv->station, /*from*/ MinosServer::getMinosServer() ->getServerName(), "KeepAlive" );  // for local server
      // we need to set our "from" here
      //      sendAction( rpa );
   }
}
//==============================================================================

