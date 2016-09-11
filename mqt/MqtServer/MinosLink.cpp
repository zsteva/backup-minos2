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

#include "minoslistener.h"

extern bool closeApp;


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
      server = s.right( s.size() - atpos - 1 );
   }
}

//==============================================================================
MinosCommonConnection::MinosCommonConnection()
    : fromIdSet( false ),
    connchecked( false ),
    remove_socket( false ),
    txConnection(false),
    connected(false)
{
}
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

void MinosCommonConnection::on_readyRead()
{
   // select says we have data, so read it
   // and send the data through the parser
   logMessage ( "XMPP test", "MinosCommonConnection::on_readyRead called to receive data from " + connectHost );

   // documntation says this may occasionally fail on Windows
   while (sock->bytesAvailable() > 0)
   {
       int rxlen = sock->read(rxbuff, 4096 - 1);
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

          while ( packetbuff.size() > 2 && packetbuff.left( 2 ) == "&&" )
          {
             int packetoffset = packetbuff.indexOf( '<' );
             if ( packetoffset > 0 )    // length field should always be followed by XML
             {
                 QStringRef slen = packetbuff.midRef(2, packetoffset - 2);
                 int packetlen = slen.toInt();
                if ( packetlen <= ( int ) packetbuff.size() - 2 && packetbuff.indexOf( ">&&" ) )
                {
                   QString packet = packetbuff.mid( packetoffset, packetlen );
                   packetbuff = packetbuff.right(  packetbuff.size() - 2 - packetlen - packetoffset );

                   TiXmlBase::SetCondenseWhiteSpace( false );
                   TiXmlDocument xdoc;
                   TIXML_STRING p = packet.toStdString();
                   xdoc.Parse( p.c_str(), 0 );
                   TiXmlElement *tix = xdoc.RootElement();
                   analyseNode( tix );
                }
                else
                {
                   // partial message, keep receiving until we get more
                   break ;
                }
             }
          }
       }
       else if (rxlen < 0)
       {
           trace("Bad read in MinosCommonConnection::on_readyRead; remove_socket = true");
          remove_socket = true;
       }
       // rxlen == 0 is valid
   }
}
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
         trace("Bad checkFrom in MinosCommonConnection::analyseNode; remove_socket = true");
         remove_socket = true;
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
void MinosCommonConnection::on_disconnected()
{
    trace("MinosCommonConnection::on_disconnected()" + clientServer + "; remove_socket = true");
    remove_socket = true;
}
