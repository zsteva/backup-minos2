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
      //      MinosId(const QString &id);
      virtual ~MinosId();

      void setId( const QString & );
};
class MinosSocket:public QObject
{
    Q_OBJECT
   protected:
      QTcpSocket sock;
      bool txConnection;      // set if we can transmit on this connection
   public:
      MinosSocket();
      virtual ~MinosSocket();

      //virtual void process() = 0;

      bool remove;
      static bool newConnection;

      void closeSocket()
      {
         sock.close();
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
private slots:
      void on_connected();
      void on_disconnected();
      void on_error(QAbstractSocket::SocketError socketError);
};
//==============================================================================
#define RXBUFFLEN 4096

extern bool isHostLocal(const QString &host);
class MinosCommonConnection: public MinosSocket
{
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
      virtual bool initialise( ) = 0;
      virtual ~MinosCommonConnection();

//      virtual void process();
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
#define MAXIPSLOTS 64         // max for a single select call

      QTcpServer sock;
      std::vector<MinosSocket *>i_array;
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

private slots:
      void on_NewConnection();
      void on_acceptError(QAbstractSocket::SocketError socketError);
};
//==============================================================================
#endif
