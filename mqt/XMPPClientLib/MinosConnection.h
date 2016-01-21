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
#include <QTimer>

//---------------------------------------------------------------------------
extern bool connected;
bool XMPPInitialise( const QString &myId );
bool XMPPClosedown();
//---------------------------------------------------------------------------


class MinosAppConnection:public QObject, RPCDispatcher
{
    Q_OBJECT
   private:
      QString jabberId;

      RPCDispatcher *user_data;
      QSharedPointer<QTcpSocket> sock;
      TIXML_STRING packetbuff;

      QTimer waitConnectTimer;

      void onLog (const QString &data, int is_incoming );

      bool closeConnection();

   public:
      MinosAppConnection( const QString &jid );
      ~MinosAppConnection();

      static MinosAppConnection *minosAppConnection;

      void startConnection();
      void closeDaemonThread();

      virtual void dispatchResponse( XStanza *a );

      void sendAction( XStanza *a );
      void setJid( const QString &jid )
      {
         jabberId = jid;
      }
private slots:
      void on_waitConnectTimeout();
      void on_readyRead();
      void on_connected();
      void on_disconnected();

};
#endif
