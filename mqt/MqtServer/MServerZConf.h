/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef MServerZConfH
#define MServerZConfH 
//---------------------------------------------------------------------------
#include <QObject>
#include <QUdpSocket>
#include <QSharedPointer>
class Server
{
   public:
      QString uuid;
      QString host;
      QString station;
      int port;
      bool autoReconnect;

//      bool available;
      bool zconf;
      bool local;

      Server( const QString &uuid, const QString &h, const QString &s, int p )
            : /*available( false ),*/ zconf( true ), local( false ),
            uuid(uuid), host( h ), station( s ), port( p ), autoReconnect(false)
      {}
      Server( const QString &s )
            : /*available( false ),*/ zconf( false ), local( false ),
            station( s ), port( -1 ), autoReconnect(false)
      {}
};
extern std::vector<Server *> serverList;
extern Server *findStation( const QString s );

class TZConf: public QObject
{
    Q_OBJECT
   private:  	// User declarations

      static void publishServer(const QString &uuid, const QString &name,
                        QHostAddress hosttarget, int PortAsNumber, bool autoReconnect );
      void readServerList();
      bool waitNameReply;
      QString localName;

      QSharedPointer<QUdpSocket> qus;
      QHostAddress groupAddress;
      int iPort;

      bool initialise( );
      bool sendMessage(const QString &mess );

   public:  		// User declarations

      TZConf( );
      ~TZConf( );
//      static void publishServer( const QString &name );

      void runThread();

      void setName( const QString &name );
      QString getName()
      {
         return localName;
      }
      void ServerScan();

      QString getZConfString(bool beacon = false);
      bool setZConfString(const QString &message, QHostAddress recvAddress);
      void publishDisconnect(const QString &name);
};
extern TZConf *ZConf;
#endif
