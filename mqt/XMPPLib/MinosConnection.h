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
#include "Dispatcher.h"
#include <QObject>
#include <QSharedPointer>
#include <QTimer>
#include <QTcpSocket>

//---------------------------------------------------------------------------
extern bool connected;
bool XMPPInitialise( const QString &myId );
//---------------------------------------------------------------------------


class MinosAppConnection:public QObject, RPCDispatcher
{
    Q_OBJECT
   private:
      QString myId;

      RPCDispatcher *user_data;
      QSharedPointer<QTcpSocket> sock;
      TIXML_STRING packetbuff;

      QTimer waitConnectTimer;

      void onLog (const TIXML_STRING &data, int is_incoming );

      bool closeConnection();

   public:
      MinosAppConnection( const QString &myid );
      ~MinosAppConnection() override;

      static MinosAppConnection *minosAppConnection;

      void startConnection();
      void closeDaemonThread();

      virtual void dispatchResponse( XStanza *a ) override;

      void sendAction( XStanza *a );

private slots:
      void on_waitConnectTimeout();
      void on_readyRead();
      void on_connected();
      void on_disconnected();

};
#endif
