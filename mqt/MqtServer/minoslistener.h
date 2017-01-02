#ifndef MINOSLISTENER_H
#define MINOSLISTENER_H

/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#include <QTcpServer>
#include <QTimer>

#include "MinosLink.h"
class Server;

extern bool closeApp;

class MinosListener:public QObject
{
    Q_OBJECT
   protected:

     QTimer CheckTimer;
      QSharedPointer<QTcpServer> sock;
      QVector<MinosCommonConnection *>i_array;

      virtual MinosCommonConnection *makeConnection(QTcpSocket *s) = 0;
    public:

      void clearSockets();
      bool connectFreeSlot( MinosCommonConnection * );
      bool acceptFreeSlot( MinosCommonConnection * );
      int getConnectionCount();
      MinosListener();
      virtual ~MinosListener();
      virtual bool initialise( QString type, int port );
      virtual QString getIdentity() = 0;

      bool isServerConnection( const MinosId &s );
      bool isClientConnection( const MinosId &s );
      bool checkServerConnection( const QString &sname );

      virtual void closeDown() = 0;

private slots:
      void on_newConnection();
      void on_timeout();
};
typedef QVector<MinosCommonConnection *>::iterator CommonIterator;
//==============================================================================
class MinosServerListener: public MinosListener
{
    //Q_OBJECT
   private:
      static MinosServerListener *MSL;
   protected:
      virtual MinosCommonConnection *makeConnection(QTcpSocket *s) override;
   public:
      static MinosServerListener *getListener()
      {
         return MSL;
      }
      MinosServerListener()
      {
         MSL = this;
      }
      ~MinosServerListener() override
      {
         MSL = 0;
      }
//      virtual void process() override;
      bool sendServer( MinosCommonConnection *il, TiXmlElement *pak );
      void checkServerConnected( Server *s, bool force );
      bool checkStillServerConnection( const QString &s );
      virtual QString getIdentity() override
      {
         return "MinosServerListener";
      }

      void closeDown() override;
};
//==============================================================================
class MinosClientListener: public MinosListener
{
    Q_OBJECT
   private:
      static MinosClientListener *MCL;
   protected:
      virtual MinosCommonConnection *makeConnection(QTcpSocket *s) override;

   public:
      static MinosClientListener *getListener()
      {
         return MCL;
      }
      MinosClientListener();
      ~MinosClientListener() override;
      bool sendClient( MinosCommonConnection *il, TiXmlElement *pak );
      virtual QString getIdentity()
      {
         return "MinosClientListener";
      }
      bool checkStillClientConnection( const QString &s );
      virtual void closeDown() override;
};
//==============================================================================
#endif // MINOSLISTENER_H
