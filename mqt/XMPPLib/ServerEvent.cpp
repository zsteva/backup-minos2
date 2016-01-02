/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "XMPP_pch.h"
#include <QUuid>

QSharedMemory ServerEvent;

QString makeUuid()
{
    QUuid uuid = QUuid::createUuid();
    return uuid.toString();
}
QString getServerId()
{
   const QString key = "ServerUUID";

   QSettings settings("G0GJV", "MinosQtServer");    // overrie the app settings

   QString uuid = settings.value(key, "").toString();
   if (uuid.size() == 0)
   {
       uuid = makeUuid();
       settings.setValue(key, uuid);
   }
   return uuid;

}

void makeServerEvent( bool create )
{
    CsGuard cs;
    ServerEvent.setKey( "MinosQtServer" );
    if (create)
    {
        ServerEvent.create( 1 );

    }
    else
    {
        ServerEvent.detach();
    }
}

bool checkServerReady()
{
    CsGuard cs;
    QSharedMemory mem( "MinosQtServer" );
    if ( mem.attach() )
    {
        mem.detach();
        return true;
    }
    return false;
}
void makeServerShowEvent( )
{
    /*
   static std::string serverShowEventName = getServerId() + "Show";
   if ( !serverShowEvent )
   {
      // we should set up th ACLs etc so we could run serevr as a service
      serverShowEvent = CreateEventA( 0, TRUE, FALSE, serverShowEventName.c_str() ); // Named Manual reset
   }
   return serverShowEvent;
   */
}
bool getShowServers()
{
    /*
   static std::string serverShowEventName = getServerId() + "Show";

   bool ret = false;
   HANDLE serverShowEvent = CreateEventA( 0, TRUE, FALSE, serverShowEventName.c_str() ); // Named Manual reset
   if ( serverShowEvent )
   {
      if (WaitForSingleObject(serverShowEvent, 0) == WAIT_OBJECT_0)
      {
         ret = true;
      }
      else
      {
         ret = false;
      }

      CloseHandle( serverShowEvent );
   }
   return ret;
   */
    return true;
}
void setShowServers(bool /*state*/)
{
    /*
   HANDLE sshEvent = makeServerShowEvent();
   if (state)
   {
      SetEvent(sshEvent);
   }
   else
   {
      ResetEvent(sshEvent);
   }
   */
}

