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

#ifndef MinosLinkH
#define MinosLinkH 
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QSharedPointer>
#include "tinyxml.h"
//---------------------------------------------------------------------------
class XStanza;
class RPCRequest;

class MinosId
{
   public:
      QString user;
      QString server;
      QString fullId;

      bool empty();

      MinosId();
      MinosId( const QString & );
      virtual ~MinosId();

      void setId( const QString & );
};
class MinosSocket:public QObject
{
    Q_OBJECT
   protected:
      bool txConnection;      // set if we can transmit on this connection
      bool connected;
   public:
      MinosSocket();
      virtual ~MinosSocket();

      bool isConnected()
      {
          return connected;
      }

      QSharedPointer<QTcpSocket> sock;
      //virtual void process() = 0;

      bool remove_socket;

      virtual void process() = 0;

      void closeSocket()
      {
        if (sock)
            sock->close();
      }
      virtual bool tryForwardStanza( TiXmlElement * /*pak*/ )
      {
         return false;
      }
      virtual bool checkServer( const MinosId & /*s*/ )
      {
         return false;
      }
      virtual bool checkServer( const QString & /*s*/ )
      {
         return false;
      }
      virtual bool checkUser( const MinosId & /*s*/ )
      {
         return false;
      }
      virtual QString getIdentity() = 0;
      virtual void sendKeepAlive( )
      {}
      bool isTxConnection()
      {
         return txConnection;
      }
};
//==============================================================================
#define RXBUFFLEN 4096

extern bool isHostLocal(const QString &host);
class MinosCommonConnection: public MinosSocket
{
    Q_OBJECT
   private:
      char rxbuff[ RXBUFFLEN ];
      QString packetbuff;
      QString rxBuff;

   protected:

      // who is connected?
      QString clientServer;     // server name
      QString clientUser;       // client name (or empty)
      QString makeJid();        // return the Jabber ID

      void onLog ( const char *data, size_t size, int is_incoming );
      bool sendRaw ( const char *xmlstr );
   public:
      bool fromIdSet;
      bool connchecked;
      QString connectHost;

      MinosCommonConnection();
      virtual bool initialise(bool conn ) = 0;
      virtual ~MinosCommonConnection();

      virtual void process() override;
      virtual void analyseNode( TiXmlElement *pak );
      virtual bool tryForwardStanza( TiXmlElement *pak );
      virtual void sendError( TiXmlElement *pak, const char *type, const char *defined_condition );
      virtual void sendAction( XStanza *a );

      virtual bool isServer() = 0;
      virtual bool checkFrom( TiXmlElement *pak ) = 0;
      virtual bool setFromId( MinosId &from, RPCRequest *req ) = 0;

      bool CheckLocalConnection();

      virtual bool checkServer( const MinosId &s )
      {
         return ( clientServer.compare(s.server, Qt::CaseInsensitive) == 0 );
      }
      virtual bool checkServer( const QString &s )
      {
          return ( clientServer.compare(s, Qt::CaseInsensitive) == 0 );
      }
      virtual bool checkUser( const MinosId &u )
      {
         return ( clientUser.compare(u.user, Qt::CaseInsensitive) == 0 );
      }
      virtual QString getIdentity() = 0;
};
//==============================================================================
class MinosListener:public QObject
{
    Q_OBJECT
   protected:

      QSharedPointer<QTcpServer> sock;
      std::vector<MinosSocket *>i_array;
    protected:
      virtual MinosCommonConnection *makeConnection(QTcpSocket *s) = 0;
    public:
      void processSockets();
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

};
//==============================================================================
#endif
