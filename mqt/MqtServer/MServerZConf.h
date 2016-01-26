/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

//#include "minos_pch.h"

#ifndef MServerZConfH
#define MServerZConfH 

#include <QApplication>
#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>

#include <QSharedPointer>
#include <QHostAddress>
#include <QHostInfo>

#include <QSettings>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QDateTime>

#include "mcreadsocket.h"

//---------------------------------------------------------------------------
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
      virtual ~Server(){}
};
extern std::vector<Server *> serverList;
extern Server *findStation( const QString s );

class UDPSocket: public QObject
{
    Q_OBJECT
    QSharedPointer<QUdpSocket> qus;
    QString ifaceName;
    QNetworkInterface qui;

public:
    UDPSocket();
    virtual ~UDPSocket() override;
    bool setup(QNetworkInterface &intr, QNetworkAddressEntry &addr);
//    bool setupRO(QNetworkInterface &intr, QNetworkAddressEntry &addr);

    bool sendMessage(const QString &mess );


private slots:
      void onSocketStateChange(QAbstractSocket::SocketState);
//      void onSocketStateChangeRO(QAbstractSocket::SocketState);
//      void onError(QAbstractSocket::SocketError socketError);

};
class TZConf: public QObject
{
    Q_OBJECT
   private:  	// User declarations

      static void publishServer(const QString &uuid, const QString &name,
                        const QString &hosttarget, int PortAsNumber, bool autoReconnect );
      void readServerList();
      bool waitNameReply;
      QString localName;

      std::vector<QSharedPointer<UDPSocket> > TxSocks;
//      std::vector<QSharedPointer<UDPSocket> > RxSocks;

      QSharedPointer<MCReadSocket> rxSocket;

      //QSharedPointer<UDPSocket>  rxSocket;

      QTimer beaconTimer;
      int iPort;

      bool sendMessage(bool beaconReq );

      static TZConf *ZConf;

      unsigned int beaconInterval;   // once a minute
      QDateTime lastTick;



   public:  		// User declarations

      TZConf( );
      virtual ~TZConf( );
//      static void publishServer( const QString &name );
      static  TZConf *getZConf()
       {
           return ZConf;
       }

      void runThread(const QString &name);

     // void setName(  );
      QString getName()
      {
         return localName;
      }

      void ServerScan();
      bool sendBeaconResponse;
      QHostAddress groupAddress;

      QString getZConfString(bool beaconreq);
      bool processZConfString(const QString &message,const  QString &recvAddress);
      void publishDisconnect(const QString &name);
      void closeDown();
private slots:
      void onReadyRead(QString datagram, QString sender);
      void onTimeout();
};
#endif