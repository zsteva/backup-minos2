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
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void
MinosAppConnection::io_close ()
{
   sock->close();
}

bool
MinosAppConnection::io_connect ( const char *server, int port )
{
    sock->connectToHost(server, port);
    // and we have to wire things up to wait for conected() or error()

   return sock->waitForConnected(10000);
}

bool
MinosAppConnection::io_send ( const char *data, size_t len )
{
    if (sock->write(data, len) == -1)
      return false;
   return sock->waitForBytesWritten(100);
}

int
MinosAppConnection::io_recv ( char *buffer, size_t buf_len, int timeout )
{
    int len = 0;
    if (sock->waitForReadyRead(timeout))
    {
        // documntation says this may occasionally fail on Windows
        len = sock->read(buffer, buf_len);
    }
    return len;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MinosAppConnection::MinosAppConnection( RPCDispatcher *ud ) : sock( new QTcpSocket ), user_data( ud )
{}
MinosAppConnection::~MinosAppConnection()
{}
bool MinosAppConnection::minos_send( const TIXML_STRING &xmlstr )
{
   if ( xmlstr.size() )
   {
      unsigned int xmllen = ( unsigned int ) xmlstr.size();
      char *xmlbuff = new char[ 10 + 1 + xmllen + 1 ];
      sprintf( xmlbuff, "&&%d%s&&", xmllen, xmlstr.c_str() );
      io_send ( xmlbuff, strlen( xmlbuff ) + 1 );  // always include the zero terminator
      MinosThread::minosThread->onLog( xmlbuff, strlen( xmlbuff ), false );
      delete [] xmlbuff;
   }
   return false;
}
bool MinosAppConnection::minos_send( TiXmlElement *data )
{
   // serialise the data node, send it to the socket in a packet
   TIXML_STRING xmlstr;
   xmlstr << ( *data );
   return minos_send( xmlstr );
}
bool MinosAppConnection::startConnection()
{
   // open up the socket connection to the minos
   return io_connect ( "localhost", ClientPort );
}
bool MinosAppConnection::runConnection()
{
   // read from the connection; buffer until a complete packet received

   int rxlen = io_recv( rxbuff, RXBUFFLEN, 1 );
   if ( rxlen < 0 || sock->state() != QAbstractSocket::ConnectedState)
   {
      closeDaemonThread();
      return false;
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
               MinosThread::minosThread->onLog( &rxbuff[ rxpt ], ptlen, true );
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
                  return true;
               }
            }
         }
      }
   return true;
}
bool MinosAppConnection::closeConnection()
{
   // close down the socket connection
   io_close();
   return false;
}

