/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include <ws2tcpip.h>

#include "minos_pch.h"
#include "mcreadsocket.h"
#include "MServerZConf.h"

//==============================================================================
class WSAGuard
{
   public:
      WSAGuard( void );
      ~WSAGuard();
};
WSAGuard::WSAGuard( void )
{
   // class to ensure that WinSock is opened and closed correctly
   WSADATA wsaData;
   const int major = 1;
   const int minor = 1;

   WORD wVersionRequested = MAKEWORD( major, minor );
   volatile int WSAerr = WSAStartup( wVersionRequested, &wsaData );
   if ( WSAerr != 0 )
   {
      //      std::string csText = "Unable to initialise Winsock.DLL : error no ";
      //      csText += string_cast( WSAerr );
      //      ::logMessage(iKernelBaseAPI::iKernelAPIFramework,  csText );
      exit( 1 );
   }
   else
   {

      if ( LOBYTE( wsaData.wVersion ) != major || HIBYTE( wsaData.wVersion ) != minor )
      {
         //         const char * csText = "This Winsock.DLL doesn't support this application";
         //         ::logMessage(iKernelBaseAPI::iKernelAPIFramework,  csText );
         exit( 1 );
      }
   }
}
//---------------------------------------------------------------------------

WSAGuard::~WSAGuard()
{
   WSACleanup();
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
         free(AddressList);
      }
      struct sockaddr_in addr;
      int addrlen;

      struct ip_mreq mreq;
      char message[ 4096 ];
      int *AddressList;
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
                                  ( struct sockaddr * ) & ( state->addr ), &( state->addrlen ) );
              if ( cnt < 0 )
              {
                 trace( "recvfrom" );
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
int ILibGetLocalIPAddressList( int** pp_int )
{

   //
   // Use an Ioctl call to fetch the IPAddress list
   //
   int i;
   char buffer[ 16 * sizeof( SOCKET_ADDRESS_LIST ) ];
   DWORD bufferSize;
   SOCKET TempSocket = socket( AF_INET, SOCK_DGRAM, 0 );
   WSAIoctl( TempSocket, SIO_ADDRESS_LIST_QUERY, NULL, 0, buffer, 16 * sizeof( SOCKET_ADDRESS_LIST ), &bufferSize, NULL, NULL );
   *pp_int = ( int* ) malloc( sizeof( int ) * ( 1 + ( ( SOCKET_ADDRESS_LIST* ) buffer ) ->iAddressCount ) );
   for ( i = 0;i < ( ( SOCKET_ADDRESS_LIST* ) buffer ) ->iAddressCount;++i )
   {
      ( *pp_int ) [ i ] = ( ( struct sockaddr_in* ) ( ( ( SOCKET_ADDRESS_LIST* ) buffer ) ->Address[ i ].lpSockaddr ) ) ->sin_addr.s_addr;
   }
   ( *pp_int ) [ i ] = inet_addr( "127.0.0.1" );
   closesocket( TempSocket );
   return ( 1 + ( ( SOCKET_ADDRESS_LIST* ) buffer ) ->iAddressCount );
}
bool MCReadSocket::setupRO()
{
    state = new UPnPDataObject();

    struct sockaddr_in addr;
    struct ip_mreq mreq;

    // Complete State Reset

    memset( ( char * ) &( state->addr ), 0, sizeof( state->addr ) );
    state->addr.sin_family = AF_INET;
    state->addr.sin_addr.s_addr = htonl( INADDR_ANY );
    state->addr.sin_port = ( unsigned short ) htons( UPNP_PORT );
    state->addrlen = sizeof( state->addr );


    // Set up socket
    state->AddressListLength = ILibGetLocalIPAddressList( &( state->AddressList ) );

    state->NOTIFY_RECEIVE_sock = socket( AF_INET, SOCK_DGRAM, 0 );
    memset( ( char * ) &( addr ), 0, sizeof( addr ) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( INADDR_ANY );
    addr.sin_port = ( unsigned short ) htons( UPNP_PORT );
    int ra = 1;
    if ( setsockopt( state->NOTIFY_RECEIVE_sock, SOL_SOCKET, SO_REUSEADDR, ( char* ) &ra, sizeof( ra ) ) < 0 )
    {
       trace( "Setting SockOpt SO_REUSEADDR failed" );
       return false;
    }
    if ( bind( state->NOTIFY_RECEIVE_sock, ( struct sockaddr * ) &( addr ), sizeof( addr ) ) < 0 )
    {
       trace( "Could not bind to UPnP Listen Port" );
       return false;
    }

    //
    // Iterate through all the current IP Addresses
    //
    for ( int i = 0;i < state->AddressListLength;++i )
    {
        memset( ( char * ) & ( addr ), 0, sizeof( addr ) );
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = state->AddressList[ i ];
        addr.sin_port = ( unsigned short ) htons( UPNP_PORT );

        mreq.imr_multiaddr.s_addr = inet_addr( UPNP_GROUP );
        mreq.imr_interface.s_addr = state->AddressList[ i ];
        //
        // Join the multicast group
        //
        if ( setsockopt( state->NOTIFY_RECEIVE_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, ( char* ) &mreq, sizeof( mreq ) ) < 0 )
        {
        /* Does not matter */
        }
    }
    return true;
}
