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
#include "MinosLink.h"

class Server;

class MinosServerConnection: public MinosCommonConnection
{
    Q_OBJECT
   private:
      Server *srv;
      QDateTime lastEventTick;
      bool resubscribed;
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
      virtual bool setFromId( MinosId &from, RPCRequest *req );

      virtual bool mConnect( Server *srv );
      virtual void sendAction( XStanza *a );
      virtual QString getIdentity()
      {
         return "MinosServerConnection " + makeJid();
      }
      virtual void sendKeepAlive( );
      void closeDown() override;
private slots:
      void on_connected();
};
//==============================================================================
#endif
