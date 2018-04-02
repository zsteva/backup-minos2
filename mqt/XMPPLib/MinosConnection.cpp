/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "LogEvents.h"
#include "MTrace.h"
#include "PortIds.h"
#include "ServerEvent.h"
#include "MinosConnection.h" 
#include "XMPPStanzas.h"
#include "XMPPEvents.h"
#include "RPCPubSub.h"
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

   The top level node is either <request> or <event>.

   In practice, requests all come from content from minos.
   events come from either side.


*///---------------------------------------------------------------------------
MinosAppConnection *MinosAppConnection::minosAppConnection = nullptr;

bool connected = false;

bool XMPPInitialise( const QString &pmyId )
{
   if ( pmyId.size() == 0 )
   {
      logMessage( "XMPP", "No user ID set" );
      return false;
   }

   QString myId = QString( pmyId ) + "@localhost";

   if ( MinosAppConnection::minosAppConnection )
   {
      logMessage( "Minos", "XMPP Already initialised" );
      return false;
   }
   logMessage( "Minos", "XMPP minosAppConnection initialised with " + myId );
   MinosAppConnection::minosAppConnection = new MinosAppConnection( myId );

   MinosAppConnection::minosAppConnection->startConnection();

   return true;
}
//---------------------------------------------------------------------------
MinosAppConnection::MinosAppConnection( const QString &myid ) : myId(myid)
  , user_data( this )
  , sock( new QTcpSocket )
{
    connect(&waitConnectTimer, SIGNAL(timeout()), this, SLOT(on_waitConnectTimeout()));
    connect(sock.data(), SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    connect(sock.data(), SIGNAL(connected()), this, SLOT(on_connected()));
    connect(sock.data(), SIGNAL(disconnected()), this, SLOT(on_disconnected()));
}
MinosAppConnection::~MinosAppConnection()
{
}
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
    sock->connectToHost("localhost", MinosClientPort);
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

   if (minosAppConnection)  // cleared before closedown
        startConnection();
}
bool MinosAppConnection::closeConnection()
{
   // close down the socket connection
    sock->close();

    return true;
}
//---------------------------------------------------------------------------
void MinosAppConnection::onLog ( const TIXML_STRING &data, int is_incoming )
{
   QString logbuff;
   if ( is_incoming )
      logbuff += "RECV";
   else
      logbuff += "SEND";
   logbuff += "[";
   logbuff += data.c_str();
   logbuff += "]";

   logMessage( "XMPP", logbuff );
}//---------------------------------------------------------------------------

void MinosAppConnection::on_readyRead()
{
   // read from the connection; buffer until a complete packet received

    const int RXBUFFLEN = 4096;
    char rxbuff[ RXBUFFLEN + 1 ];

    while (sock->bytesAvailable() > 0)
    {
        qint64 rxlen = sock->read(rxbuff, RXBUFFLEN);

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
                    int ptlen = static_cast<int>(strlen( &rxbuff[ rxpt ]) );
                    if ( ptlen )
                    {
                        onLog( &rxbuff[ rxpt ], true );
                        packetbuff += &rxbuff[ rxpt ];   // which will strip out any nulls
                    }
                    rxpt += ptlen + 1;
                }

                while ( packetbuff.size() > 2 && packetbuff.substr( 0, 2 ) == "&&" )
                {
                    size_t packetoffset = packetbuff.find( '<' );
                    if ( packetoffset > 0 )    // length field should always be followed by XML
                    {
                        char * ec;
                        int packetlen = strtol( packetbuff.c_str() + 2, &ec, 10 );
                        if ( *ec == '<' && packetlen <= static_cast<int> (strlen( ec )) + 2 && packetbuff.find( ">&&" ) != std::string::npos )
                        {
                            unsigned int upacketlen = static_cast<unsigned int>(packetlen);
                            TIXML_STRING packet = packetbuff.substr( packetoffset, upacketlen );
                            packetbuff = packetbuff.substr( packetoffset + upacketlen + 2, strlen( ec + upacketlen ) );

#ifdef TRACE_PACKETS

                            TraceIncoming( packet.c_str() );
#endif

                            analyseNode( user_data, packet );
                            // and go round again...
                        }
                        else
                        {
                            // partial message, keep receiving until we get more
                            break;
                        }
                    }
                }
            }
    }
}

void MinosAppConnection::sendAction( XStanza *a )
{
   if ( connected )
   {
      a->setNextId();   // only happens if no Id already
      TIXML_STRING xmlstr = a->getActionMessage();

      size_t xmllen = xmlstr.length();

      if (xmllen)
      {
          char * xmlbuff = new char[ 10 + 1 + xmllen + 1 ];
          sprintf( xmlbuff, "&&%lu%s&&", static_cast<unsigned long>(xmllen), xmlstr.c_str() );
          xmllen = strlen( xmlbuff );
          qint64 ret = sock->write ( xmlbuff, xmllen );
          if (ret >= 0)
              onLog ( xmlbuff, false );
          delete [] xmlbuff;
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
}
//---------------------------------------------------------------------------

