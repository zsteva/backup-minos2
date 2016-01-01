/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef minosConnectionH
#define minosConnectionH
//---------------------------------------------------------------------------
#include "XMPPClient_pch.h"

#include <QObject>
#include <QSharedPointer>

//---------------------------------------------------------------------------

class MinosAppConnection:public QObject
{
    Q_OBJECT
#define RXBUFFLEN 4096
   private:
      RPCDispatcher *user_data;
      QSharedPointer<QTcpSocket> sock;
      char rxbuff[ RXBUFFLEN ];
      TIXML_STRING packetbuff;
      void io_close ();
      bool io_connect ( const char *server, int port );
      bool io_send ( const char *data, size_t len );
      int io_recv ( char *buffer, size_t buf_len, int timeout );

   public:
      MinosAppConnection( RPCDispatcher *ud );
      ~MinosAppConnection();
      bool startConnection();
      bool runConnection();
      bool closeConnection();
      bool minos_send( const TIXML_STRING &data );
      bool minos_send( TiXmlElement *data );
};
#endif
