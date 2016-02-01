/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "minos_pch.h"

//---------------------------------------------------------------------------
#include "minos_pch.h"

#include "MinosLink.h"
#include "clientThread.h"
#include "serverThread.h"

#include "minoslistener.h"

#include "MServerZConf.h"
#include "MServerPubSub.h"


TZConf *TZConf::ZConf = 0;
std::vector<Server *> serverList;

//---------------------------------------------------------------------------
TZConf::TZConf( )
      : waitNameReply( false )
{
   ZConf = this;
   sendBeaconResponse = false;
   beaconInterval = 60000;   // once a minute
   lastTick = QDateTime::currentDateTimeUtc().addSecs(-59); // force a beacon soon
}
TZConf::~TZConf( )
{
   for ( std::vector<Server *>::iterator i = serverList.begin(); i != serverList.end(); i++ )
   {
      delete ( *i );
   }
   serverList.clear();
}
void TZConf::closeDown()
{
    trace("closeDown");

}

/*
 * We seem to need one socket per interface. Our old QT code looped through the interfaces
 * and broadcast (not multicast) on all the IPV4 ones (NB not localhost...)
 *
 * It then bound on "any IPV4 address" so one socket managed all the receives
 *
 * BCB code creates a socket per interface, and adds an extra for localhost, binds each for
 * receive, and then loops through them for sending
 *
 */
void TZConf::runThread(const QString &name)
{
    trace("TZConf::runThread()");

    localName = name.trimmed();

    groupAddress = QHostAddress(UPNP_GROUP);

    QSharedPointer<MCReadSocket> mcrs(new MCReadSocket());
    mcrs->setupRO();
    rxSocket = mcrs;

    connect(rxSocket.data(), SIGNAL(readyRead(QString, QString )), this, SLOT(onReadyRead(QString, QString)), Qt::QueuedConnection);

    // Get network interfaces list

    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

    // Interfaces iteration
    for (int i = 0; i < ifaces.size(); i++)
    {
        if (!ifaces[i].flags().testFlag( QNetworkInterface::IsUp))
            continue;
        if (!ifaces[i].flags().testFlag(QNetworkInterface::IsRunning))
            continue;
        // Now get all IP addresses for the current interface
        QList<QNetworkAddressEntry> addrs = ifaces[i].addressEntries();

        // And for any IP address, if it is IPv4 and the interface is active, make a socket
        for (int j = 0; j < addrs.size(); j++)
        {
            if ((addrs[j].ip().protocol() == QAbstractSocket::IPv4Protocol)
                    && !addrs[j].ip().toString().isEmpty())
            {
                {
                    QSharedPointer<UDPSocket> qus(new UDPSocket());

                    bool res = qus->setup(ifaces[i], addrs[j]);

                    if (res)
                    {
                        TxSocks.push_back(qus);
                    }
                }
            }
        }
    }


   /*
      <minosServer UUID='xxx' name='name' port='port' request='true' />

      UUID is unique to this "machine" (more likely username)
      , and is to allow us to manage
      station name changes. So where should we keep it?
      Looks like registry may be favourite; we don't really
      want two serevrs running on a single machine.

      Actually we can use the GUID as the event name for servers...

      "request" true asks recipient to beacon immediately
      beacon messages do NOT force recipients to beacon

      ALL servers will periodically set "request", but if a
      request has been received "recently" then we don't... (?)

      This mechanism should allow for networks that fragment, and
      then the server comes online - once the network heals then
      we rely on the periodic beacon

      We also need to time out stations which we haven't heard from.
   */

   // and now we go and sit on a select on all the notify sockets
   // but also remember to beacon occasionally...

   connect (&beaconTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));


   beaconTimer.setInterval(100);
   beaconTimer.start();

   readServerList();
}
void TZConf::onTimeout()
{
   // this should be on a timer
   if (sendBeaconResponse || lastTick.msecsTo(QDateTime::currentDateTime()) > beaconInterval )
   {
      trace(QString("Timeout: Sending beacon, sendBeaconResponse = ") + (sendBeaconResponse?"true":"false"));
      lastTick = QDateTime::currentDateTime();

      sendMessage( sendBeaconResponse );   // timer requests beaconing, beacon just responds
      sendBeaconResponse = false;
   }
   rxSocket->onTimeout();
}


bool TZConf::sendMessage(bool beaconReq )
{
    for (std::vector<QSharedPointer<UDPSocket> >::iterator i = TxSocks.begin(); i != TxSocks.end(); i++)
    {
        QString mess = getZConfString(beaconReq);
        (*i)->sendMessage(mess);
    }
   return true;
}

void TZConf::onReadyRead(QString datagram, QString sender)
{
    // from MCReadSocket
    sendBeaconResponse = processZConfString(QString(datagram), sender);
}

//---------------------------------------------------------------------------


Server *findStation( const QString s )
{
   for ( std::vector<Server *>::iterator i = serverList.begin(); i != serverList.end(); i++ )
   {
      if ( ( *i ) ->station.compare( s, Qt::CaseInsensitive ) == 0 )
      {
         return ( *i );
      }
   }
   return 0;
}
void TZConf::ServerScan()
{
   for ( std::vector<Server *>::iterator i = serverList.begin(); !closeApp && i != serverList.end(); i++ )
   {
      trace("Server scan - checking " + (*i)->station);
      MinosServerListener::getListener() ->checkServerConnected( ( *i ), false );
      trace("Server scan - checked " + (*i)->station);
   }
}
void TZConf::readServerList()
{
    /*
   trace("Reading Server List File");
   // Read the server override file
   TIniFile * servers = new TIniFile( "./Configuration/Servers.ini" );
   TStringList *sl = new TStringList;
   TStringList *params = new TStringList;
   servers->ReadSections( sl );
   for ( int i = 0; i < sl->Count; i++ )
   {
      servers->ReadSectionValues( sl->Strings[ i ], params );

      String uuid = params->Values[ "Uuid" ];
      String host = params->Values[ "Host" ];
      String station = params->Values[ "Station" ];
      String port = params->Values[ "Port" ];

      if ( uuid.Length() == 0 )
      {
         servers->WriteString( sl->Strings[ i ], "Uuid", uuid.c_str() );
         uuid = makeUuid().c_str();
      }
      if ( host.Length() == 0 )
      {
         if ( station.Length() == 0 )
         {
            continue;
         }
         host = station;
      }
      else
         if ( station.Length() == 0 )
         {
            station = host;
         }

      if ( port.Length() == 0 )
      {
         port == String(MinosServerPort);
      }

      publishServer( uuid.c_str(), station.c_str(), host.c_str(), port.ToIntDef( MinosServerPort ), true );

   }
   delete params;
   delete servers;
   ServerScan();
   trace("Finished reading Server List File");
   */
}

//---------------------------------------------------------------------------
// uuid is the machines uuid (more unique than the server name!)
// name is server name
// hosttarget is the IP address/DNS name
// PortAsNumber is the remote port
// state is available/not available

// ONLY trouble is... clients will now address their servers by UUID!
// when they have subscribed to stations.

void TZConf::publishServer( const QString &uuid, const QString &name,
            const QString &hosttarget, int PortAsNumber, bool autoReconnect )
{
   trace( "publishServer Host " + hosttarget + " Station " + name +
            " Port " + QString::number( PortAsNumber ) + " uuid " + uuid + " auto " + ( autoReconnect ? "true" : "false" ) );
   Server *s = findStation( name );
   if ( s )
   {
      trace("Station " + name + " found zconf is " + ( s->zconf ? "true" : "false" ));
//      s->available = state;
      // we may not have all the details... we may need to add them
      if (!s->zconf)
      {
         s->uuid = uuid;
         s->host = hosttarget;
//      station should already be there
         s->port = PortAsNumber;

//         s->available = state;
         s->zconf = true;;
         if ( name == getZConf()->getName() )
         {
            s->local = true;
         }
      }
   }
   else
   {
      trace("Station " + name + " not found");
      s = new Server( uuid, hosttarget, name, PortAsNumber );
//      s->available = state;
      s->autoReconnect = autoReconnect;
      if ( name == getZConf()->getName() )
      {
         s->local = true;
      }
      serverList.push_back( s );
   }
   if ( s->local )
   {
      PubSubMain->publish( "", "LocalStation", name, hosttarget, psPublished );
   }
   else
   {
      MinosServerListener::getListener() ->checkServerConnected(s, true);
   }
   PubSubMain->publish( "", "Station", name, hosttarget, psPublished );
//   PubSubMain->publish( "", uuid, "Name", name );
//   PubSubMain->publish( "", uuid, "State", state?"true":"false" );
   trace("publishServer finished");
}
void TZConf::publishDisconnect(const QString &name)
{
    trace("publishDisconnect");
   Server *s = findStation( name );
   if ( s )
   {
      PubSubMain->publish( "", "Station", name, s->host, psNotConnected );
   }
}
//==============================================================================
QString TZConf::getZConfString(bool beaconreq)
{
    static int sequence = 0;
   QString Uuid = getServerId();
   return  QString("<minosServer ")
               + "seq='" + QString::number(sequence++)
               + "' UUID='" + Uuid
               + "' name='" + getName()
               + "' port='" + QString::number(MinosServerPort) + "'"
               + (beaconreq?"":" request='true'")
               + " />";
}
//==============================================================================
bool TZConf::processZConfString(const QString &message, const QString &recvHost)
{
   bool sendBeaconResponse = false;

   TiXmlDocument xdoc;
   std::string smessage = message.toStdString();
   xdoc.Parse( smessage.c_str(), 0 );
   TiXmlElement * tix = xdoc.RootElement();
   if ( tix && checkElementName( tix, "minosServer" ) )
   {
      //"<minosServer UUID='" + Uuid + "' name='" + getName() + "' port='7778' request='true' />";
      QString UUID = getAttribute( tix, "UUID" );
      QString station = getAttribute( tix, "name" );
      QString port = getAttribute( tix, "port" );
      QString request = getAttribute( tix, "request" );
      //if ( UUID != Uuid && station != getName() )
      {
         // publish what came in - possibly we should publish the XML string
         // against the UUID?
         bool ok;
         iPort = port.toInt(&ok);
         if (!ok)
             iPort = MinosServerPort;
         publishServer( UUID, station, recvHost, iPort, false );
         if ( request.size() && UUID != getServerId())
         {
            sendBeaconResponse = true;   // delay the response, give the other end a chance...
                        // BUT this means we could try to connect before they will recognise us.
                        // Should we queue the publish until then as well?

                        // But the UDP may fail to get there, so we need to be able to cope with new
                        // unadvertised connectors
         }
      }
   }
   return sendBeaconResponse;
}
//==============================================================================

UDPSocket::UDPSocket()
{
}
UDPSocket::~UDPSocket()
{

}

bool UDPSocket::setup(QNetworkInterface &iface, QNetworkAddressEntry &addr)
{
    ifaceName = iface.humanReadableName();
    qui = iface;
    qus = QSharedPointer<QUdpSocket>(new QUdpSocket);

    //connect(qus.data(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    connect(qus.data(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChange(QAbstractSocket::SocketState)));

    qui = iface;
    trace("Binding to " + addr.ip().toString());
    bool res = qus->bind(
                addr.ip(),//addr.ip(), QHostAddress::AnyIPv4,//TZConf::getZConf()->groupAddress,
                UPNP_PORT,
                QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint
                );
    trace(QString("UDP bind returns ") + (res?"true":"false") + " on " + addr.ip().toString() + " error " + qus->errorString());
    return res;
}
void UDPSocket::onSocketStateChange (QAbstractSocket::SocketState state)
{
    if ( state == QAbstractSocket::BoundState )
    {
        bool res = qus->joinMulticastGroup(TZConf::getZConf()->groupAddress, qui);
        if (res)
        {
            trace("Bound and set multicast interface " + TZConf::getZConf()->groupAddress.toString()
                  + " on " + qus->localAddress().toString() + " interface :" + ifaceName);
        }
        else
        {
            trace("Failed to set multicast interface " + TZConf::getZConf()->groupAddress.toString()
                  + " on " + qus->localAddress().toString() + " interface :" + ifaceName);
        }
    }
}
bool UDPSocket::sendMessage(const QString &mess )
{
    QByteArray packet = QByteArray(mess.toStdString().c_str());

    /*int ret =*/ qus->writeDatagram(packet.data(), packet.length(), TZConf::getZConf()->groupAddress, UPNP_PORT);

    trace("send datagram on " + ifaceName
          + " : " + mess);

    return true;
}
/*
void UDPSocket::onError(QAbstractSocket::SocketError socketError)
{
    trace("Error: " + qus->errorString());
}
*/
