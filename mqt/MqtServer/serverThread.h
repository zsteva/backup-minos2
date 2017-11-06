/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifndef serverThreadH
#define serverThreadH

#include <QDateTime>
#include <QTimer>
#include "MinosLink.h"

class Server;

class MinosServerConnection: public MinosCommonConnection
{
    Q_OBJECT
   private:
      Server *srv;
      bool resubscribed;

      QTimer resubscribeTimer;
   protected:
   public:
      MinosServerConnection();
      virtual bool initialise(bool conn) override;
      ~MinosServerConnection();
      virtual bool checkFrom( TiXmlElement *pak );
      virtual bool isServer()
      {
         return true;
      }
      virtual void setFromId( MinosId &from, RPCRequest *req );

      virtual void mConnect( Server *srv );
      virtual void sendAction( XStanza *a );
      virtual QString getIdentity()
      {
         return "MinosServerConnection " + makeJid();
      }
      void closeDown() override;
private slots:
      void on_connected();
      virtual void sendKeepAlive( );
};
//==============================================================================
#endif
