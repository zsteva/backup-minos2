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

#include <QHostAddress>
#include <QHostInfo>

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
bool isHostLocal(const QString &host)
{
    QHostInfo connaddr = QHostInfo::fromName( host );
    if ( connaddr.addresses().contains(QHostInfo::fromName( "127.0.0.1" ).addresses().first() ))
    {
       return true;             // connection address is 127.0.0.1
    }
    return false;
}
//==============================================================================
/*
   class MinosId
   {
      public:
         QString user;
         QString server;
         QString fullId;
*/

bool MinosId::empty()
{
   return fullId.size() == 0;
}

MinosId::MinosId()
{
}
MinosId::MinosId( const QString &s )
{
   setId( s );
}
MinosId::~MinosId()
{
}

void MinosId::setId( const QString &s )
{
   fullId = s;
   int atpos = s.indexOf( "@" );
   if ( atpos < 0 )
   {
      server = s;
   }
   else
   {
      user = s.left(atpos );
      server = s.right( atpos + 1 );
   }
}

//==============================================================================
bool MinosSocket::newConnection = false;
MinosSocket::MinosSocket() : //sock( INVALID_SOCKET ),
   remove( false ),
   txConnection(false)

{}
MinosSocket::~MinosSocket()
{
   closeSocket();
}
void MinosSocket::on_connected()
{

}

void MinosSocket::on_disconnected()
{

}

void MinosSocket::on_error(QAbstractSocket::SocketError socketError)
{

}

//=============================================================================
MinosListener::MinosListener():sock(new QTcpServer(this))
{}
MinosListener::~MinosListener()
{
   clearSockets();
}
//-----------------------------------------------------------------------------
bool MinosListener::initialise( QString type, int port )
{

   bool res = sock->listen(QHostAddress::Any, port); // signals newConnection()
   // need an event loop or waitForNewConnection()
   xperror( res == false ,type + " listen"  );

   //i_array.push_back( this );

   return true;
}
bool MinosListener::acceptFreeSlot( MinosCommonConnection *il )
{
    /*
   if ( i_array.size() < MAXIPSLOTS )
   {
      // Create a new end point
      i_array.push_back( il );
      il->initialise();

      SOCKADDR_IN sin;
      int in_addr_len = sizeof( sin );

      SOCKET s = accept( sock, ( LPSOCKADDR ) & sin, &in_addr_len );
      il->connectHost = QString( inet_ntoa( sin.sin_addr ) );
      logMessage( "acceptFreeSlot", "from " + il->connectHost );
      if ( s == INVALID_SOCKET )
      {
         return false;
      }

      //il ->setSocket( s );
      return true;
   }
   else
   {
      logMessage( "acceptFreeSlot", "Socket table full" );
      SOCKET tmp = accept( sock, NULL, NULL );
      closesocket( tmp );
      delete il;
   }
*/
   return false;
}
// add a connected socket (we started the connection)
bool MinosListener::connectFreeSlot( MinosCommonConnection *il )
{
   if ( i_array.size() < MAXIPSLOTS )
   {
      /* Create a new end point */
      il->newConnection = true;
      i_array.push_back( il );
      il->initialise();
      return true;
   }
   else
   {
      logMessage( "connectFreeSlot", "Socket table full" );
      delete il;
   }
   return false;
}
int MinosListener::getConnectionCount()
{
    return i_array.size();
    /*
   int cnt = 0;
   for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      if ( ( *i ).isValid() )
      {
         cnt++;
      }
   }
   int cnt2 = ( int ) i_array.size();
   if ( cnt != cnt2 )
   {
      cnt = -1;
   }
   return cnt;
   */
}
bool MinosListener::isServerConnection( const MinosId &s )
{
   for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      // worry about the details
      if ( ( *i ) ->checkServer( s ) )
      {
         return false;
      }
      if ( ( *i ) -> checkServer( s ) )
      {
         return true;
      }
   }
   return false;
}
bool MinosListener::isClientConnection( const MinosId &s )
{
   for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      // worry about the details
      if ( ( *i ) ->checkUser( s ) )
      {
         return true;
      }
      if ( ( *i ) -> checkServer( s ) )
      {
         return false;
      }
   }
   return false;
}
//==============================================================================
/* */
/* Check for activity on the sockets */
/* */

// Predicate function for remove_if
bool nosock( MinosSocket *ip )
{
   if ( ip == 0 )
      return true;
   else
      return false;
}
void MinosListener::processSockets( void )
{
    /*
   try
   {
      int nsocks = i_array.size();
      std::auto_ptr<WSAPOLLFD> fds(new WSAPOLLFD [nsocks]);

      {
         CsGuard g;
         int n = 0;
         for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++, n++ )
         {
            if ( ( *i ) && ( *i ) ->getSocket() != INVALID_SOCKET )
            {
               fds.get()[n].fd = ( *i ) ->getSocket();
               fds.get()[n].events = POLLIN;
               fds.get()[n].revents = 0;
            }
            else
            {
               fds.get()[n].fd = -1;
               fds.get()[n].events = 0;
               fds.get()[n].revents = 0;
            }
         }
      }
      int nevents = WSAPoll( fds.get(), nsocks, 50 );   // 50 ms timeout

      if (nevents < 0)
      {
         // timeout
         for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
         {
            try
            {
               if ( ( *i ) )
               {
                  ( *i ) ->sendKeepAlive();
               }
            }
            catch ( ... )
            {
               logMessage( "process_sockets", "Unknown Exception in sendKeepAlive" );
               closeApp = true;
               break;
            }
         }
         return ;
      }
      if (nevents == 0)
      {
         return;
      }
      if (closeApp)
      {
         return;
      }

      //        Check for each of the sockets which could have fired for read

      bool clearup = false;
      int n = 0;
      for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++, n++ )
      {
         if ( ( *i ) && (fds.get()[n].revents & POLLIN != 0) )
         {
            try
            {
               MinosSocket * ip = ( *i );
               ip ->process();  // This may add an element...  in which case the iterator is screwed
               if ( ip->newConnection )
               {
                  ip->newConnection = false;
                  break;
               }
            }
            catch ( Exception & e )
            {
               logMessage( "process_sockets", QString( "Exception processing socket : " ) + e.Message.c_str() );
               closeApp = true;
               break;
            }
            catch ( ... )
            {
               logMessage( "process_sockets", "Unknown Exception processing socket" );
               closeApp = true;
               break;
            }
         }
      }
      for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
      {
         try
         {
            if ( ( *i ) ->remove
               )
            {
               // process says to finish off
               logMessage( "process_sockets", QString( "deleting socket : " ) + ( *i ) ->getIdentity() );
               delete ( *i );
               *i = 0;
               clearup = true;
            }
         }
         catch ( Exception & e )
         {
            logMessage( "process_sockets", QString( "Exception tidying socket : " ) + e.Message.c_str() );
            closeApp = true;
            break;
         }
         catch ( ... )
         {
            logMessage( "process_sockets", "Unknown Exception tidying socket" );
            closeApp = true;
            break;
         }
      }
      if ( clearup )
      {
         i_array.erase( std::remove_if( i_array.begin(), i_array.end(), nosock ), i_array.end() );
      }
   }
   catch ( Exception & e )
   {
      logMessage( "process_sockets", QString( "Exception while processing: " ) + e.Message.c_str() );
      closeApp = true;
   }
   */
}
#ifdef RUBBISH
void MinosListener::processSockets( void )
{
#warning change server over to socket polling - no 64 connection limit!
   try
   {
      int ready_cnt = 0;

      fd_set in_socks;

      FD_ZERO( &in_socks );

      /*
           Add the active sockets to the list to be checked
      */
      {
         CsGuard g;
         for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
         {
            if ( ( *i ) && ( *i ) ->getSocket() != INVALID_SOCKET )
            {
               FD_SET( ( *i ) ->getSocket(), &in_socks );
            }
         }
      }

      // When we get going, then we also want to queue output on each socket
      // and send when the socket is ready - i.e. when a send is likely to
      // succeed

      timeval timeout;
      timeout.tv_sec = 0;
      timeout.tv_usec = 50000;

      ready_cnt = select( 0, &in_socks, NULL, NULL, &timeout );

      if (closeApp)
      {
         return;
      }

      /*
         Pick up on the timeout
      */
      CsGuard g;
      if ( ready_cnt < 1 )
      {
         for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
         {
            try
            {
               if ( ( *i ) )
               {
                  ( *i ) ->sendKeepAlive();
               }
            }
            catch ( ... )
            {
               logMessage( "process_sockets", "Unknown Exception in sendKeepAlive" );
               closeApp = true;
               break;
            }
         }
         return ;
      }
      /*
              Check for each of the sockets which could have fired for read
      */
      bool clearup = false;
      for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
      {
         if ( ( *i ) && FD_ISSET( ( *i ) ->getSocket(), &in_socks ) )
         {
            try
            {
               MinosSocket * ip = ( *i );
               ip ->process();  // This may add an element...  in which case the iterator is screwed
               if ( ip->newConnection )
               {
                  ip->newConnection = false;
                  break;
               }
            }
            catch ( Exception & e )
            {
               logMessage( "process_sockets", QString( "Exception processing socket : " ) + e.Message.c_str() );
               closeApp = true;
               break;
            }
            catch ( ... )
            {
               logMessage( "process_sockets", "Unknown Exception processing socket" );
               closeApp = true;
               break;
            }
         }
      }
      for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
      {
         try
         {
            if ( ( *i ) ->remove
               )
            {
               // process says to finish off
               logMessage( "process_sockets", QString( "deleting socket : " ) + ( *i ) ->getIdentity() );
               delete ( *i );
               *i = 0;
               clearup = true;
            }
         }
         catch ( Exception & e )
         {
            logMessage( "process_sockets", QString( "Exception tidying socket : " ) + e.Message.c_str() );
            closeApp = true;
            break;
         }
         catch ( ... )
         {
            logMessage( "process_sockets", "Unknown Exception tidying socket" );
            closeApp = true;
            break;
         }
      }
      if ( clearup )
      {
         i_array.erase( std::remove_if( i_array.begin(), i_array.end(), nosock ), i_array.end() );
      }
   }
   catch ( Exception & e )
   {
      logMessage( "process_sockets", QString( "Exception while processing: " ) + e.Message.c_str() );
      closeApp = true;
   }
}
#endif
void MinosListener::clearSockets()
{
   CsGuard lck;
   try
   {
      for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
      {
          delete ( *i );
      }
      //      i_array.erase( std::remove_if( i_array.begin(), i_array.end(), nosock ), i_array.end() );
      i_array.clear();
   }
   catch ( Exception & e )
   {
      logMessage( "clear_sockets", QString( "Exception while closing sockets: " ) + e.what() );
   }
}
bool MinosListener::checkServerConnection( const QString &sname )
{
   for ( std::vector<MinosSocket *>::iterator i = i_array.begin(); i != i_array.end(); i++ )
   {
      if ( ( *i ) ->checkServer( sname ) )
      {
         return true;               // we only need ONE that is right...
      }
   }
   return false;
}
void MinosListener::on_NewConnection()
{

}

void MinosListener::on_acceptError(QAbstractSocket::SocketError socketError)
{

}

//==============================================================================

//==============================================================================
MinosCommonConnection::MinosCommonConnection()
: fromIdSet( false ), connchecked( false )
{}
MinosCommonConnection::~MinosCommonConnection()
{}
bool MinosCommonConnection::CheckLocalConnection()
{
   if ( !connchecked )
   {
      QString sname = MinosServer::getMinosServer() ->getServerName();
      if ( ( clientServer.compare( "localhost", Qt::CaseInsensitive ) != 0 ) &&
           ( clientServer.compare( sname, Qt::CaseInsensitive) != 0 ) )
      {
         return false;        // not a good host name in from
      }
      if ( isHostLocal(connectHost))
      {
         connchecked = true;
         return true;             // connection address is 127.0.0.1
      }
      Server *serv = findStation( sname );
      if ( serv )
      {
         connchecked = true;
         return true;          // connection address is one we own
      }
      return false;
   }
   return true;
}
QString MinosCommonConnection::makeJid()
{
   QString id;
   if ( clientUser.length() )
      id = clientUser + "@";
   id += clientServer;

   return id;
}
bool MinosCommonConnection::sendRaw ( const char *xmlstr )
{
   unsigned int xmllen = strlen( xmlstr );
   if ( xmllen )
   {
      char * xmlbuff = new char[ 10 + 1 + xmllen + 1 ];
      sprintf( xmlbuff, "&&%d%s&&", xmllen, xmlstr );
      xmllen = strlen( xmlbuff );
      int ret = sock->write ( xmlbuff, xmllen );
      onLog ( xmlbuff, xmllen, 0 );
      if ( ret == -1 )
         return false;
      delete [] xmlbuff;
   }
   return true;
}
void MinosCommonConnection::onLog ( const char *data, size_t /*size*/, int is_incoming )
{
   QString logbuff;
   if ( is_incoming )
      logbuff += "RECV";
   else
      logbuff += "SEND";

   logbuff += "[";
   logbuff += data;
   logbuff += "]";

   logMessage( "MinosCommonConnection", logbuff );
}
//---------------------------------------------------------------------------
bool MinosCommonConnection::tryForwardStanza( TiXmlElement *tix )
{
   TIXML_STRING s;
   s << *tix;
   bool res = sendRaw( s.c_str() );
   return res;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void MinosCommonConnection::sendError( TiXmlElement *tix,const  char * /*type*/, const char *defined_condition )
{
   /*
   <stanza-kind to='sender' type='error'>
     [RECOMMENDED to include sender XML here]
     <error type='error-type'>
       <defined-condition xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
       <text xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'
             xml:lang='langcode'>
         OPTIONAL descriptive text
       </text>
       [OPTIONAL application-specific condition element]
     </error>
   </stanza-kind>
   */

   // to a server, we might have to include our own "from" - or it might reject
   // as no "from"

   // we need to check if this is an error response already...
   // and if so, we cannot send an error(again)

   QString from = getAttribute( tix, "from" );
   QString to = getAttribute( tix, "to" );
   QString id = getAttribute( tix, "id" );
   QString subtype = getAttribute( tix, "type" );

   if ( subtype.compare("error", Qt::CaseInsensitive ) == 0 )
      return ;

   TiXmlElement x( "iq" );

   x.SetAttribute( "type", "error" );
   x.SetAttribute ( "id", id.toStdString() );

   TiXmlElement condition( "error_condition" );
   TiXmlText t( defined_condition );
   condition.InsertEndChild( t );
   x.InsertEndChild( condition );

   TIXML_STRING s;
   s << x;

   sendRaw ( s.c_str() );
}
//---------------------------------------------------------------------------
void MinosCommonConnection::sendAction( XStanza *a )
{
   // use the stanza to send itself
   a->setNextId();   // only happens if no Id already
   TIXML_STRING s = a->getActionMessage().toStdString();
   sendRaw( s.c_str() );
}
//=============================================================================
void sendAction( XStanza *a )
{
   // stanza has a "to" - but this is internal, so we need to dispatch it
   QString mess = a->getActionMessage();
   //int err;

   // convert from a RPCParam structure to a DOM

   TiXmlBase::SetCondenseWhiteSpace( false );
   TiXmlDocument xdoc;
   xdoc.Parse( mess.toStdString().c_str(), 0 );
   TiXmlElement *x = xdoc.RootElement();

   if ( a->getFrom().size() == 0 )
   {
      // insert a from of ourselves.

      QString from = MinosServer::getMinosServer() ->getServerName();
      if ( from.length() )
      {
         x->SetAttribute( "from", from.toStdString().c_str() );
      }
   }
   QString to = a->getTo();
   if ( to.size() != 0 )
   {
      x->SetAttribute( "to", to.toStdString().c_str() );
   }
   // and now dispatch to its destination

   if ( !MinosServer::getMinosServer() ->forwardStanza( 0, x ) )              // our own services
   {
      if ( !MinosClientListener::getListener() ->sendClient( 0, x ) )         // look at real and potential clients
      {
         if ( !MinosServerListener::getListener() ->sendServer( 0, x ) )         // look at real and potential servers
         {
            // or no valid destination found
            //sendError( pak, "cancel", "item-not-found" );
            return ;
         }
      }
   }
   // or no valid destination found
}
//=============================================================================
/*
void MinosCommonConnection::process()
{
   // select says we have data, so read it
   // and send the data through the parser
   logMessage ( "XMPP test", "MinosCommonConnection::process called to receive data from " + clientUser );

   int rxlen = recv ( getSocket(), rxbuff, 4096 - 1, 0 );
   if ( rxlen > 0 )
   {
      rxbuff[ rxlen ] = '\0';

      // We might have embedded nulls between message parts - so strip them
      int rxpt = 0;
      while ( rxpt < rxlen )
      {
         int ptlen = ( int ) strlen( &rxbuff[ rxpt ] );
         if ( ptlen )
         {
            onLog ( &rxbuff[ rxpt ], ptlen, 1 );  // but this ignores the wrapper
            packetbuff += &rxbuff[ rxpt ];   // which will strip out any nulls
         }
         rxpt += ptlen + 1;
      }

      while ( packetbuff.size() > 2 && packetbuff.substr( 0, 2 ) == "&&" )
      {
         int packetoffset = ( int ) packetbuff.find( '<' );
         if ( packetoffset > 0 )    // length field should always be followed by XML
         {
            char * ec;
            int packetlen = strtol( packetbuff.c_str() + 2, &ec, 10 );
            if ( *ec == '<' && packetlen <= ( int ) strlen( ec ) + 2 && packetbuff.find( ">&&" ) )
            {
               QString packet = packetbuff.substr( packetoffset, packetlen );
               packetbuff = packetbuff.substr( packetoffset + packetlen + 2, strlen( ec + packetlen ) );

               TiXmlBase::SetCondenseWhiteSpace( false );
               TiXmlDocument xdoc;
               xdoc.Parse( packet.c_str(), 0 );
               TiXmlElement *tix = xdoc.RootElement();
               analyseNode( tix );
            }
            else
            {
               // partial message, keep receiving until we get more
               return ;
            }
         }
      }
   }
   else
   {
      remove
         = true;
   }
}
*/
//==============================================================================
void MinosCommonConnection::analyseNode( TiXmlElement *tix )
{
   // response factory - look at the node, and build the correct response stanza object

   // checkFrom looks at the "from" address, and validates it against the connection
   // A server connection has to have a "from" (but it isn't necessarily correct, if its been proxied)
   // A client must either have a from address, or nothing - when checkFrom will insert it

   if ( !checkFrom( tix ) )
   {
      if ( isServer() )
      {
         closeSocket();
         remove = true;
      }
      else
      {
         sendError( tix, "cancel", "invalid-from" );
      }
      return ;
   }
   // Dispatch to its destination

   // we should first look for "built in" clients - initially this means PubSub
   // ZConf is a possibility, but it (currently) works via PubSub
   // Actually, we could build these in to the server as RPC calls
   // - don't need a decent "to", just the server name
   if ( !MinosServer::getMinosServer() ->forwardStanza( this, tix ) )              // our own services
   {
      if ( !MinosClientListener::getListener() ->sendClient( this, tix ) )         // look at real and potential clients
      {
         if ( !MinosServerListener::getListener() ->sendServer( this, tix ) )         // look at real and potential servers
         {
            // or no valid destination found
            sendError( tix, "cancel", "item-not-found" );
            return ;
         }
      }
   }

}
//=============================================================================
