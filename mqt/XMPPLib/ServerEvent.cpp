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

//---------------------------------------------------------------------------
/*
static HANDLE serverEvent = 0;
static HANDLE serverShowEvent = 0;
*/
QString makeUuid()
{
    /*
   // if there isn't one, then make one
    QUuid uuid = QUuid::createUuid();
   GUID uuid;
   CoCreateGuid( &uuid );

   unsigned char *pid = 0;

   UuidToStringA( &uuid, &pid );
   std::string myUuid = ( char * ) pid;
   RpcStringFreeA( &pid );

   return myUuid;
   */
    return "Not a GUID";
}
QString getServerId()
{
   const QString key = "ServerUUID";
   /*
   // First, look in the registry for a GUID for us
   static QString myUuid;
   if ( myUuid.size( ) )
   {
      return myUuid;
   }
   TRegistry *Reg = new TRegistry;
   Reg->RootKey = HKEY_CURRENT_USER;
   if ( Reg->OpenKeyReadOnly( "SOFTWARE\\G0GJV\\Minos\\Server" ) )
   {
      myUuid = AnsiString(Reg->ReadString( key )).c_str();
   }
   if ( myUuid.size() == 0 )
   {
      // if there isn't one, then make one
      myUuid = makeUuid();
      Reg->CloseKey();
      if ( Reg->OpenKey( "SOFTWARE\\G0GJV\\Minos\\Server", true ) )
      {
         Reg->WriteString( key, myUuid.c_str() );
      }
      else
      {
         myUuid = "MinosServer"; // as we cannot write it, use something constant
      }
   }
   Reg->CloseKey();
   delete Reg;
   */
   return key ;
}

void makeServerEvent( bool /*create*/ )
{
    /*
   static std::string serverEventName = getServerId();
   if ( create )
   {
      if ( !serverEvent )
      {
         // we should set up th ACLs etc so we could run serevr as a service
         serverEvent = CreateEventA( 0, TRUE, FALSE, serverEventName.c_str() ); // Named Manual reset
      }
   }
   else
   {
      if ( serverEvent )
      {
         CloseHandle( serverEvent );
         serverEvent = 0;
      }
   }
   return serverEvent;
   */
}

bool checkServerReady()
{
    /*
   static std::string serverEventName = getServerId();

   bool ret = false;
   HANDLE serverEvent = CreateEventA( 0, TRUE, FALSE, serverEventName.c_str() ); // Named Manual reset
   if ( serverEvent )
   {
      if ( GetLastError() == ERROR_ALREADY_EXISTS )
      {
         ret = true;
      }
      CloseHandle( serverEvent );
   }
   return ret;
   */
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

