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

#include <QSettings>

//---------------------------------------------------------------------------

#include "MServerZConf.h"
#include "MServerPubSub.h"
TZConf *ZConf = 0;
std::vector<Server *> serverList;
GJV_thread *zcThread = 0;

#define UPNP_PORT 9999
#define UPNP_GROUP "239.255.0.1"


bool TZConf::sendMessage(const QString &mess )
{
    QByteArray datagram = QByteArray(mess.toStdString().c_str());
    qint64 ret = qus->writeDatagram(datagram.data(), datagram.size(),
                             groupAddress, iPort);

   return ret > 0;
}
bool TZConf::initialise()
{
   groupAddress = QHostAddress(UPNP_GROUP);
   return true;
}
void runZcThread( void *t )
{
   TZConf * tzc = ( TZConf * ) t;
   tzc->runThread();
}
void TZConf::runThread()
{
    qus = QSharedPointer<QUdpSocket>(new QUdpSocket(0));
   initialise( );


   QString beaconreq = getZConfString(true);
   QString nobeaconreq = getZConfString(false);

   bool sendBeaconResponse = false;
   unsigned int beaconInterval = 60000;   // once a minute
   QDateTime lastTick = QDateTime::currentDateTimeUtc();

   // send our first beacon message out... with a "report" request attached
   sendMessage( beaconreq );

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

   while ( !closeApp )
   {
       if (qus->hasPendingDatagrams())
       {
           /*
           char message[ 4096 ];
           int cnt = recvfrom( RetVal->NOTIFY_RECEIVE_sock, message, 4095, 0,
                               ( struct sockaddr * ) & ( RetVal->addr ), &( RetVal->addrlen ) );
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
                 std::string recvHost = std::string( inet_ntoa( RetVal->addr.sin_addr ) );
                 trace( "UDP received from " + recvHost + " : " + message );

                 sendBeaconResponse = setZConfString(message, recvHost);
              }
           */
            if (sendBeaconResponse || lastTick.msecsTo(QDateTime::currentDateTime()) > beaconInterval )
            {
               CsGuard guard;
               trace(QString("Sending beacon, sendBeaconResponse = ") + (sendBeaconResponse?"true":"false"));
               lastTick = QDateTime::currentDateTime();
               sendMessage( sendBeaconResponse?nobeaconreq:beaconreq );   // timer requests beaconing, beacon just responds
               sendBeaconResponse = false;
            }
            else
            {
                QByteArray datagram;
                datagram.resize(qus->pendingDatagramSize());
                QHostAddress sender;
                quint16 senderPort;

                qus->readDatagram(datagram.data(), datagram.size(),
                                        &sender, &senderPort);

                sendBeaconResponse = setZConfString(QString(datagram), sender);
            }
       }

   }
   qus->close();
}
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
   CsGuard guard;
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
   TIniFile * servers = new TIniFile( ".\\Configuration\\Servers.ini" );
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
         port == String(ServerPort);
      }

      publishServer( uuid.c_str(), station.c_str(), host.c_str(), port.ToIntDef( ServerPort ), true );

   }
   delete params;
   delete servers;
   ServerScan();
   trace("Finished reading Server List File");
   */
}

//---------------------------------------------------------------------------
TZConf::TZConf( )
      : waitNameReply( false )
{}
TZConf::~TZConf( )
{
   zcThread->GJV_join();
   delete zcThread;
   
   for ( std::vector<Server *>::iterator i = serverList.begin(); i != serverList.end(); i++ )
   {
      delete ( *i );
   }
   serverList.clear();
}
//---------------------------------------------------------------------------
void TZConf::setName( const QString &name )
{
   localName = name.trimmed();
   zcThread = new GJV_thread( "ZConf", &runZcThread, ( void * ) this );
   readServerList();
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
            QHostAddress hosttarget, int PortAsNumber, bool autoReconnect )
{
   CsGuard g;
   trace( "publishServer Host " + hosttarget.toString() + " Station " + name +
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
         s->host = hosttarget.toString();
//      station should already be there
         s->port = PortAsNumber;

//         s->available = state;
         s->zconf = true;;
         if ( name == ZConf->getName() )
         {
            s->local = true;
         }
      }
   }
   else
   {
      trace("Station " + name + " not found");
      s = new Server( uuid, hosttarget.toString(), name, PortAsNumber );
//      s->available = state;
      s->autoReconnect = autoReconnect;
      if ( name == ZConf->getName() )
      {
         s->local = true;
      }
      serverList.push_back( s );
   }
   if ( s->local )
   {
      PubSubMain->publish( "", "LocalStation", name, hosttarget.toString(), psPublished );
   }
   else
   {
      MinosServerListener::getListener() ->checkServerConnected(s, true);
   }
   PubSubMain->publish( "", "Station", name, hosttarget.toString(), psPublished );
//   PubSubMain->publish( "", uuid, "Name", name );
//   PubSubMain->publish( "", uuid, "State", state?"true":"false" );
   trace("publishServer finished");
}
void TZConf::publishDisconnect(const QString &name)
{
   Server *s = findStation( name );
   if ( s )
   {
      PubSubMain->publish( "", "Station", name, s->host, psNotConnected );
   }
}
//==============================================================================
QString TZConf::getZConfString(bool beacon)
{
   QString Uuid = getServerId();
   return  QString("<minosServer ")
               + "UUID='" + Uuid
               + "' name='" + getName()
               + "' port='" + QString::number(ServerPort) + "'"
               + (beacon?" request='true'":"")
               + " />";
}
//==============================================================================
bool TZConf::setZConfString(const QString &message, QHostAddress recvHost)
{
   CsGuard g;
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
             iPort = ServerPort;
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

