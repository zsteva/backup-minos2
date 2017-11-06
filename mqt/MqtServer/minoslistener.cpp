#include "minos_pch.h"

#include "minoslistener.h"
#include "clientThread.h"
#include "serverThread.h"

MinosServerListener *MinosServerListener::MSL = 0;
MinosClientListener *MinosClientListener::MCL = 0;

//=============================================================================
void xperror( int test_val, QString s, bool endit = true )
{
//   int lerrno = GetLastError();
   if ( test_val )
   {
//      QString s = QString( mesg ); // + " : error number " + QString::number( lerrno );
      logMessage( "xperror", s );

      if ( endit )
      {
         throw Exception( ("JServer error: " + s ).toStdString().c_str());
      }
   }
}
//=============================================================================
MinosListener::MinosListener():sock(new QTcpServer(this))
{
    connect(&CheckTimer, SIGNAL(timeout()), this, SLOT(on_timeout()));
    CheckTimer.start(100);
}
MinosListener::~MinosListener()
{
   clearSockets();
}
//-----------------------------------------------------------------------------
bool MinosListener::initialise( QString type, int port )
{

    QHostAddress::SpecialAddress addr = QHostAddress::Any;

    if (type == "Client")
    {
        addr = QHostAddress::LocalHost;
    }
    connect(sock.data(), SIGNAL(newConnection()), this, SLOT(on_newConnection()));
   bool res = sock->listen(addr, port); // signals newConnection()
   xperror( res == false ,type + " listen"  );

   return true;
}
bool MinosListener::acceptFreeSlot( MinosCommonConnection *il )
{
    // Create a new end point
    QHostAddress h = il->sock->peerAddress();
    il->connectHost = h.toString();

    logMessage( "acceptFreeSlot", "from " + il->connectHost );
    i_array.push_back( il );

    il->initialise(false);
    return true;
}
// add a connected socket (we started the connection)
bool MinosListener::connectFreeSlot( MinosCommonConnection *il )
{
  /* Create a new end point */
    QHostAddress h = il->sock->peerAddress();
    il->connectHost = h.toString();

    logMessage( "connectFreeSlot", "from " + il->connectHost );
  i_array.push_back( il );

  il->initialise(true);
  return true;
}
int MinosListener::getConnectionCount()
{
    return i_array.size();
}
//==============================================================================
// Check for activity on the sockets

// Predicate function for remove_if
bool nosock( MinosCommonConnection *ip )
{
   if ( ip == 0 )
      return true;
   else
      return false;
}
void MinosListener::on_newConnection()
{
    QTcpSocket *s = sock->nextPendingConnection();
    if (s)
    {
        MinosCommonConnection *cc = makeConnection(s);

        acceptFreeSlot(cc);
    }
}
void MinosListener::on_timeout()
{
    bool clearup = false;
    for ( CommonIterator i = i_array.begin(); i != i_array.end(); i++ )
    {
        if ( ( *i ) ->remove_socket )
        {
            // process says to finish off
            logMessage( "MinosListener::on_timeout", "deleting socket : " + ( *i ) ->getIdentity() );
            (*i)->closeDown();
            delete ( *i );
            *i = 0;
            clearup = true;
        }
    }
    if ( clearup )
    {
        i_array.erase( std::remove_if( i_array.begin(), i_array.end(), nosock ), i_array.end() );
    }

}

void MinosListener::clearSockets()
{
      for ( CommonIterator i = i_array.begin(); i != i_array.end(); i++ )
      {
          delete ( *i );
      }
      i_array.clear();
}
//==============================================================================

MinosCommonConnection *MinosServerListener::makeConnection(QTcpSocket *s)
{
    MinosServerConnection *c = new MinosServerConnection();
    c->sock = QSharedPointer<QTcpSocket>(s);

    return c;
}

bool MinosServerListener::sendServer( TiXmlElement *tix )
{
    MinosId to( getAttribute( tix, "to" ) );

    if ( to.server.size() == 0 )
        return false;
    if ( to.server.compare( MinosServer::getMinosServer() ->getServerName(), Qt::CaseInsensitive) == 0 )
        return false;
    if ( to.server.compare( DEFAULT_SERVER_NAME, Qt::CaseInsensitive ) == 0 )
        return false;

    // OK, it is not for us... look at connected servers

    bool connectSocket = false;
    for ( CommonIterator i = i_array.begin(); i != i_array.end(); i++ )
    {
        if ( ( *i ) ->checkServer( to ) && (*i) ->isTxConnection() )
        {
            if ( !( *i ) ->tryForwardStanza( tix ) )
            {
                connectSocket = false;
                break;
            }
            return true;
        }
    }
    // send failed; stash the message and initiate a server connection
    // (but some stanza types should be ignored?)
    if ( connectSocket && MinosServer::getMinosServer() ->getServerName() != DEFAULT_SERVER_NAME )
    {
        // We need to look at the servers vector, and try to find the relevant one
        // If we can't find it, we refuse anyway

        Server * srv = findStation( to.server );
        if ( srv )
        {
            // set ourselves up to connect
            MinosServerConnection * s = new MinosServerConnection();
            s->mConnect( srv );
            connectFreeSlot( s );
            // and we need to TRY to resend
            if (!s ->tryForwardStanza( tix ))
            {
                connectSocket = false;
            }
            return true;

            // and continue to refuse the message
        }
    }

// server is not connected; ignore the message.
// The originator should retry sometime later to send message again

return true;   // don't pass it on - either we have dealt with it, or its not useful
}
void MinosServerListener::checkServerConnected( Server *srv, bool force )
{
   if ( srv->local )
   {
      return ;
   }
   {
       for ( CommonIterator i = i_array.begin(); i != i_array.end(); i++ )
       {
          if ( ( *i ) && ( *i ) ->checkServer( srv->station ) && (*i)->isTxConnection() )
          {
             return ;
          }
       }
   }
   if (force || srv->autoReconnect)
   {
      MinosServerConnection * s = new MinosServerConnection();
      s->mConnect( srv );
      connectFreeSlot( s );
   }
}

bool MinosServerListener::checkStillServerConnection( const QString &s )
{
   for ( CommonIterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      if ( ( *i ) && ( *i ) ->checkServer( s ) )
      {
         return true;
      }
   }
   return false;
}
void MinosServerListener::closeDown()
{
   MSL = 0;
}
//==============================================================================
//==============================================================================
MinosClientListener::MinosClientListener()
{
   MCL = this;
}
MinosClientListener::~MinosClientListener()
{
   MCL = 0;
}
//==============================================================================
MinosCommonConnection *MinosClientListener::makeConnection(QTcpSocket *s)
{
    MinosClientConnection *c = new MinosClientConnection();
    c->sock = QSharedPointer<QTcpSocket>(s);

    return c;
}
//==============================================================================
bool MinosClientListener::sendClient( TiXmlElement *tix )
{
   MinosId from( getAttribute( tix, "from" ) );
   MinosId to( getAttribute( tix, "to" ) );

   if ( to.user.size() == 0 )
   {
      // invalid to servername (still default) or no "to" user
      return false;
   }
   bool fromLocal = false;
   bool fromLocalHost = false;
   bool toLocal = false;
   bool toLocalHost = false;

   bool addressOK = false;

   if ( from.empty() || from.server.compare( MinosServer::getMinosServer() ->getServerName(), Qt::CaseInsensitive ) == 0 )
      fromLocal = true;

   if ( from.empty() && from.server.compare( DEFAULT_SERVER_NAME, Qt::CaseInsensitive ) == 0 )
      fromLocalHost = true;

   if ( to.server.compare( MinosServer::getMinosServer() ->getServerName(), Qt::CaseInsensitive ) == 0 )
      toLocal = true;

   if ( to.server.compare( DEFAULT_SERVER_NAME, Qt::CaseInsensitive ) == 0 )
      toLocalHost = true;

   if ( toLocal )
      addressOK = true;      // proper "to" server, from anywhere

   if ( ( fromLocal || fromLocalHost ) && toLocalHost )      // "to" localhost OK if "from" local app
      addressOK = true;

   if ( fromLocal && !toLocal && !toLocalHost )      // from proper address, to remote address
      addressOK = true;

   if ( addressOK && !toLocal && !toLocalHost )
      return false;

   /* BUT (and we don't need to check...
   if (fromLocalHost && !(toLocal || toLocalHost))
      addressOK = false;
   */

   if ( !addressOK )
   {
      return true;   // don't pass it on - its not useful
   }

   // OK, it is for what might be one of our clients

   for ( CommonIterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      // worry about the details
      if ((*i) && ( *i ) ->checkUser( to ) )
      {
         if ( !( *i ) ->tryForwardStanza( tix ) )
         {
            // send failed; stash the message
            // (but some stanza types should be ignored?)
            break;
         }
         return true;
      }
   }
   // client is not connected; we have to ignore it

   return true;   // don't pass it on - either we have dealt with it, or its not useful
}
bool MinosClientListener::checkStillClientConnection( const QString &s )
{
   MinosId id( s );
   for ( CommonIterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      // worry about the details
      if ( ( *i ) ->checkUser( id ) )
      {
         return true;
      }
   }
   return false;
}
void MinosClientListener::closeDown()
{

}
