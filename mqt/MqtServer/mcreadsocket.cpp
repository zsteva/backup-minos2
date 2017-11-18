/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include <qglobal.h>
#ifdef Q_OS_WIN
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCKET int
#endif

#include "minos_pch.h"
#include "mcreadsocket.h"
#include "MServerZConf.h"

#include <QListIterator>
//==============================================================================
class WSAGuard
{
   public:
      WSAGuard( void );
      ~WSAGuard();
};
WSAGuard::WSAGuard( void )
{
#ifdef Q_OS_WIN
   // class to ensure that WinSock is opened and closed correctly
   WSADATA wsaData;
   const int major = 1;
   const int minor = 1;

   WORD wVersionRequested = MAKEWORD( major, minor );
   volatile int WSAerr = WSAStartup( wVersionRequested, &wsaData );
   if ( WSAerr != 0 )
   {
      exit( 1 );
   }
   else
   {

      if ( LOBYTE( wsaData.wVersion ) != major || HIBYTE( wsaData.wVersion ) != minor )
      {
         exit( 1 );
      }
   }
#endif
}
//---------------------------------------------------------------------------

WSAGuard::~WSAGuard()
{
#ifdef Q_OS_WIN
   WSACleanup();
#endif
}

WSAGuard *wsag = 0;
//==============================================================================
class UPnPDataObject
{
   public:
      UPnPDataObject(): addrlen(0), AddressList(0), AddressListLength(0),
               NOTIFY_RECEIVE_sock(0)
      {
         memset(&addr, 0, sizeof(struct sockaddr_in));
         memset(&mreq, 0, sizeof(struct ip_mreq));
         // and most of  TZConf::initialise should be in here...
      }
      ~UPnPDataObject()
      {
      }
      struct sockaddr_in addr;
      socklen_t addrlen;

      struct ip_mreq mreq;
      char message[ 4096 ];
      QVector<QHostAddress> AddressList;
      int AddressListLength;

      SOCKET NOTIFY_RECEIVE_sock;
};

MCReadSocket::MCReadSocket(QObject *parent) : QObject(parent)
{
    wsag = new WSAGuard();
}
MCReadSocket::~MCReadSocket()
{
    delete wsag;
}

void MCReadSocket::onTimeout()
{
    bool dataRead = true;

    while (dataRead)
    {
        fd_set readset;

        FD_ZERO( &readset );

        struct timeval tv;
        tv.tv_sec = 0;  // never wait
        tv.tv_usec = 0;

        // and we need to put in the sockets we are waiting on
        FD_SET( state->NOTIFY_RECEIVE_sock, &readset );

        int slct = select( FD_SETSIZE, &readset, NULL, NULL, &tv );
        if ( slct >= 1 )    // timeout...
        {
           if ( FD_ISSET( state->NOTIFY_RECEIVE_sock, &readset ) != 0 )
           {
              char message[ 4096 ];
              int cnt = recvfrom( state->NOTIFY_RECEIVE_sock, message, 4095, 0,
                                  reinterpret_cast< /*struct*/ sockaddr * > (& ( state->addr )), &( state->addrlen ) );
              if ( cnt < 0 )
              {
                 return ;     // end of thread?
              }
              else
                 if ( cnt > 0 )
                 {
                    // and we have a message, so we need to parse it and do any necessary publishing
                    message[ cnt ] = 0;
                    QString recvHost = QString( inet_ntoa( state->addr.sin_addr ) );
                    trace( "UDP received from " + recvHost + " : " + message );

                    emit readyRead(QString(message), recvHost);
                    dataRead = true;
                 }
           }
        }
        else
        {
            dataRead = false;
        }
    }
}

int GetLocalIPAddressList( QVector<QHostAddress> &addrList )
{
    QList<QNetworkInterface> mListIfaces = QNetworkInterface::allInterfaces();

    for (int i = 0; i < mListIfaces.length(); ++i)
    {
        if (!mListIfaces.at(i).flags().testFlag( QNetworkInterface::IsUp))
            continue;
        if (!mListIfaces.at(i).flags().testFlag(QNetworkInterface::IsRunning))
            continue;

        QListIterator< QNetworkAddressEntry > networkAddressEntry(mListIfaces.at(i).addressEntries());

        while (networkAddressEntry.hasNext())
        {
            QNetworkAddressEntry netEntry(networkAddressEntry.next());

            if (netEntry.ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                addrList.push_back(netEntry.ip());
            }
        }

    }
    return addrList.size();
}

bool MCReadSocket::setupRO()
{
    state = new UPnPDataObject();

    struct sockaddr_in addr;
    struct ip_mreq mreq;

    // Complete State Reset

    memset( reinterpret_cast< char *> (&( state->addr )), 0, sizeof( state->addr ) );
    state->addr.sin_family = AF_INET;
    state->addr.sin_addr.s_addr = htonl( INADDR_ANY );
    state->addr.sin_port = static_cast<unsigned short>(htons( UPNP_PORT ));
    state->addrlen = sizeof( state->addr );


    // Set up socket
    state->AddressListLength = GetLocalIPAddressList( state->AddressList );

    state->NOTIFY_RECEIVE_sock = socket( AF_INET, SOCK_DGRAM, 0 );
    memset( reinterpret_cast<char *>(&( addr )), 0, sizeof( addr ) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( INADDR_ANY );
    addr.sin_port = static_cast<unsigned short>(htons( UPNP_PORT ));
    int ra = 1;
#if defined(Q_OS_WIN) | defined(Q_OS_ANDROID)
#define SO_REUSEPORT 0
#endif
    if ( setsockopt( state->NOTIFY_RECEIVE_sock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, reinterpret_cast<char*>(&ra), sizeof( ra ) ) < 0 )
    {
       trace( "Setting SockOpt SO_REUSEADDR failed" );
       return false;
    }
    if ( bind( state->NOTIFY_RECEIVE_sock, reinterpret_cast<struct sockaddr *>(&( addr )), sizeof( addr ) ) < 0 )
    {
       trace( "Could not bind to UPnP Listen Port" );
       return false;
    }

    //
    // Iterate through all the current IP Addresses
    //
    for ( int i = 0; i < state->AddressListLength; ++i )
    {
        memset( reinterpret_cast<char *> (& ( addr )), 0, sizeof( addr ) );
        addr.sin_family = AF_INET;
        QHostAddress &address = state->AddressList[ i ];
        addr.sin_addr.s_addr = htonl(address.toIPv4Address());
        addr.sin_port = static_cast<unsigned short>(htons( UPNP_PORT ));

        mreq.imr_multiaddr.s_addr = inet_addr( UPNP_GROUP );
        mreq.imr_interface.s_addr = htonl(address.toIPv4Address());
        //
        // Join the multicast group
        //
        if ( setsockopt( state->NOTIFY_RECEIVE_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char*>(&mreq), sizeof( mreq ) ) < 0 )
        {
            trace("Join multicast failed "+ address.toString());
        /* Does not matter */
        }
        else
        {
            trace("RO bound to multicast " + address.toString());
        }
    }
    return true;
}
