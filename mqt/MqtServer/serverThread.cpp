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

//==============================================================================
//==============================================================================
MinosServerConnection::MinosServerConnection() : srv( 0 ), resubscribed( false )
{}
bool MinosServerConnection::initialise(bool conn)
{
    QHostAddress h = sock->peerAddress();
    connectHost = h.toString();
    connect(sock.data(), SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    connect(sock.data(), SIGNAL(disconnected()), this, SLOT(on_disconnected()));

    connect(&resubscribeTimer, SIGNAL(timeout()), this, SLOT(sendKeepAlive()));
    resubscribeTimer.start(1000);

   return conn;   // already initialised
}

MinosServerConnection::~MinosServerConnection()
{
}
void MinosServerConnection::closeDown()
{
   logMessage( "Server Link", "Closing" );

   if (PubSubMain)
       PubSubMain->disconnectServer(makeJid());
   if (srv)
      TZConf::getZConf()->publishDisconnect(srv->station);
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

void MinosServerConnection::mConnect( Server *psrv )
{
   srv = psrv;
   clientServer = srv->station;

   logMessage( "Server", QString( "Connecting to " ) + srv->station + " host " + srv->host );

   // connect to endpoint
   // We need to connect out to the end point - looks much like a client connection!
    sock = QSharedPointer<QTcpSocket>(new QTcpSocket);

    connect(sock.data(), SIGNAL(connected()), this, SLOT(on_connected()));
    connect(sock.data(), SIGNAL(disconnected()), this, SLOT(on_disconnected()));
    connect(sock.data(), SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    sock->connectToHost(srv->host, srv->port);
    txConnection = true;
}
void MinosServerConnection::on_connected()
{
    connected = true;
    logMessage( "Server", QString( "Connected OK to " ) + srv->station + " host " + srv->host );
    RPCRequest *rpa = new RPCRequest( clientServer, MinosServer::getMinosServer() ->getServerName(), "ServerSetFromId" );   // for our local server, this one MUST have a from
    rpa->addParam( MinosServer::getMinosServer() ->getServerName() );
    rpa->addParam( TZConf::getZConf()->getZConfString(true ) );
    sendAction( rpa );
    delete rpa;
}

//==============================================================================
void MinosServerConnection::setFromId( MinosId &id, RPCRequest *req )
{
   resubscribed = false;
   // and we need to check that the originator is who we think they ought to be
   if ( !id.server.size() )
   {
      logMessage( "ServerSetFromId", "No \"from\" from server " + srv->station );
      return;
   }
   if ( srv && srv->station.compare( id.server, Qt::CaseInsensitive) != 0 )
   {
      logMessage( "ServerSetFromId", "Mismatch from server " + srv->station + " we received \"" + id.server + "\"" );
      return;
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
            TZConf::getZConf()->processZConfString(message, connectHost);
         }
      }
   }
   else
   {
      logMessage( "ServerSetFromId", QString( "server " ) + id.server + " connected to us - srv already set up as " + srv->station );
   }
   clientServer = id.server;
}
//==============================================================================
void MinosServerConnection::sendAction( XStanza *a )
{
   // use the stanza to send itself
   a->setNextId();   // only happens if no Id already

   TIXML_STRING s = a->getActionMessage();
   sendRaw( s );
}
//==============================================================================
void MinosServerConnection::sendKeepAlive( )
{
    if (srv)
    {
        if (!checkLastRx())
        {
            // abort the connection
            trace("MinosServerConnection - checkLastRx failed, removing socket");
            remove_socket = true;
            return;
        }
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
        sendRaw("<keepAlive />");
    }
}
bool MinosServerConnection::checkLastRx()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - lastRx > resubscribeTimer.interval() * 5)
    {
        return false;
    }

    return true;
}
//==============================================================================

