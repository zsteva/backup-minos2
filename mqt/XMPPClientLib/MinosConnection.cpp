/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "XMPPClient_pch.h"

#include "MinosConnection.h" 
//---------------------------------------------------------------------------
/*
   The minos protocol is related to the XMPP version, but only vaguely, in that
   the payload is again XML/RPC, and made up exactly as for the XMPP version.

   The "wrapper" is a decimal length string at the start (length of the XML payload
   only), with a null ternminator on the end. The body between these two
   elements must be a well formed XML document.

   The whole message is enclosed in double amphersands; these should never
   appear in "real" XML/RPC data, as they should always be escaped (no CData allowed!)

   &&length<XML tag>...</XML tag>&&\0

   <?xml version="1.0" encoding="UTF-8"?> is allowed to be present
   but it will be assumed anyway.

   WE ONLY WORK HERE WITH UTF-8 ENCODINGS.

   The top level node is either <request>, <response>, or <event>.

   In practice, requests all come from content, responses from minos.
   events come from either side.

   Requests and responses are tied together by the "id" attribute; it is the
   responsability of the requester to provide a unique id string (we normally
   use numbers, but this is not required.

   <request requestName='name of request' id='1234'>
      <XML/RPC parameter body/>
   </request>

   <event eventName='name of event'>
      <XML/RPC parameter body/>
   </event>

   <response id='1234'>
      <XML/RPC parameter body/>
   </response>

*///---------------------------------------------------------------------------
MinosAppConnection *MinosAppConnection::minosAppConnection = 0;
QString myId;
bool connected = false;
static bool terminated = false;

bool XMPPInitialise( const QString &pmyId )
{
   if ( pmyId.size() == 0 )
   {
      logMessage( "XMPP", "No user ID set" );
      return false;
   }

   myId = QString( pmyId ) + "@localhost";

   if ( MinosAppConnection::minosAppConnection )
   {
      logMessage( "Minos", "XMPP Already initialised" );
      return false;
   }
   MinosAppConnection::minosAppConnection = new MinosAppConnection( myId );

   MinosAppConnection::minosAppConnection->startConnection();

   return true;
}
//---------------------------------------------------------------------------
bool XMPPClosedown()
{
   // signal to close down all boilerplate threads

   terminated = true;

   if ( MinosAppConnection::minosAppConnection )
   {
      // wait for XMPP and Request threads to finish
      MinosAppConnection::minosAppConnection->closeDaemonThread();
      delete MinosAppConnection::minosAppConnection;
      MinosAppConnection::minosAppConnection = 0;
   }

   RPCPubSub::close();
   return true;
}
//---------------------------------------------------------------------------
MinosAppConnection::MinosAppConnection(const QString &jid ) : jabberId(jid), sock( new QTcpSocket ), user_data( this )
{
    connect(&waitConnectTimer, SIGNAL(timeout()), this, SLOT(on_waitConnectTimeout()));
    connect(sock.data(), SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    connect(sock.data(), SIGNAL(connected()), this, SLOT(on_connected()));
    connect(sock.data(), SIGNAL(disconnected()), this, SLOT(on_disconnected()));
}
MinosAppConnection::~MinosAppConnection()
{}
void MinosAppConnection::startConnection()
{
    waitConnectTimer.start(1000);
}
void MinosAppConnection::on_waitConnectTimeout()
{
    if (!checkServerReady())
    {
        return;
    }
    waitConnectTimer.stop();
    sock->connectToHost("localhost", ClientPort);
}
void MinosAppConnection::on_connected()
{
    connected = true;

    RPCRequest *rpa = new RPCRequest( "", myId, "ClientSetFromId" );   // for our local server, this one MUST have a from
    rpa->addParam( myId );
    sendAction( rpa );
    delete rpa;
    RPCPubSub::reconnectPubSub();
}

void MinosAppConnection::on_disconnected()
{
    trace("on_disconnected");
    closeDaemonThread();
}

void MinosAppConnection::closeDaemonThread()
{
   if ( connected )
   {
      connected = false;
   }
   closeConnection();

   startConnection();
}
bool MinosAppConnection::closeConnection()
{
   // close down the socket connection
    sock->close();
//    sock->waitForDisconnected();
    return true;
}
//---------------------------------------------------------------------------
void MinosAppConnection::onLog ( const QString &data, int is_incoming )
{
   QString logbuff;
   if ( is_incoming )
      logbuff += "RECV";
   else
      logbuff += "SEND";
   logbuff += "[";
   logbuff += data;
   logbuff += "]";

   logMessage( "XMPP", logbuff );
}//---------------------------------------------------------------------------

void MinosAppConnection::on_readyRead()
{
   // read from the connection; buffer until a complete packet received

    const int RXBUFFLEN = 4096;
    char rxbuff[ RXBUFFLEN + 1 ];

    int rxlen = sock->read(rxbuff, RXBUFFLEN);

   if ( rxlen < 0 || sock->state() != QAbstractSocket::ConnectedState)
   {
      closeDaemonThread();
      return;
   }
   else
      if ( rxlen > 0 )
      {
         rxbuff[ rxlen ] = 0;

         // We might have embedded nulls between message parts - so strip them
         int rxpt = 0;
         while ( rxpt < rxlen )
         {
            unsigned int ptlen = strlen( &rxbuff[ rxpt ] );
            if ( ptlen )
            {
               onLog( QString(&rxbuff[ rxpt ]), true );
               packetbuff += &rxbuff[ rxpt ];   // which will strip out any nulls
            }
            rxpt += ptlen + 1;
         }

         while ( packetbuff.size() > 2 && packetbuff.substr( 0, 2 ) == "&&" )
         {
            unsigned int packetoffset = packetbuff.find( '<' );
            if ( packetoffset > 0 )    // length field should always be followed by XML
            {
               char * ec;
               int packetlen = strtol( packetbuff.c_str() + 2, &ec, 10 );
               if ( *ec == '<' && packetlen <= ( int ) strlen( ec ) + 2 && packetbuff.find( ">&&" ) )
               {
                  TIXML_STRING packet = packetbuff.substr( packetoffset, packetlen );
                  packetbuff = packetbuff.substr( packetoffset + packetlen + 2, strlen( ec + packetlen ) );

#ifdef TRACE_PACKETS

                  TraceIncoming( packet.c_str() );
#endif

                  analyseNode( user_data, packet );
                  // and go round again...
               }
               else
               {
                  // partial message, keep receiving until we get more
                  return;
               }
            }
         }
      }
   return;
}

void sendAction( XStanza *a )
{
   if ( MinosAppConnection::minosAppConnection )
   {
      MinosAppConnection::minosAppConnection->sendAction( a );
   }
}
void MinosAppConnection::sendAction( XStanza *a )
{
   if ( connected )
   {
      a->setNextId();   // only happens if no Id already
      QString xmlstr = a->getActionMessage();

      int xmllen = xmlstr.size();
      if ( xmllen )
      {
          xmlstr = QString("&&%1%2&&").arg(xmllen).arg(xmlstr);

          std::string s = xmlstr.toStdString();

         if (sock->write(s.c_str(), s.size() + 1 ) < 0)  // always include the zero terminator
           return;

         onLog( xmlstr, false );
      }
   }
}
//---------------------------------------------------------------------------
void MinosAppConnection::dispatchResponse( XStanza *xs )
{
   RPCRequest * req = dynamic_cast<RPCRequest *>( xs );
   if ( req )
   {
      makeXMPPEvent( req );
   }
   else
   {
      RPCResponse * rr = dynamic_cast<RPCResponse *>( xs );
      if ( rr )
      {
         if ( rr->methodName == "ClientSetFromId" )
         {
            // server will return the REAL Jid
            QString ouraddr;
            QString from;
            if ( rr->getStringArg( 0, from ) )
            {
               // check that from is what connected to us...
               setJid( ouraddr );
            }
         }
         else
            makeXMPPEvent( rr );
      }
   }
}
//---------------------------------------------------------------------------

