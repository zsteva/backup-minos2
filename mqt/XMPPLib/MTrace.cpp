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
static MLogFile mLogFile;
static bool logEnabled = false;

void trace( const QString & mess )
{
   mLogFile.log( mess );
}
void trace( const char *mess )
{
   mLogFile.log( mess );
}
void trace( const std::string & mess )
{
   mLogFile.log( mess.c_str() );
}
//---------------------------------------------------------------------------
void enableTrace(const QString &where , const QString filePrefix)
{
   if ( !logEnabled )
   {
      QString path = GetCurrentDir();
      if ( where[ 0 ] == '.' )
      {
         path += where.right( where.size() - 1 );
      }
      else
      {
         path += where;
      }
      mLogFile.createLogFile( path, filePrefix, 10 );
      logEnabled = true;
   }
}
void disableTrace( )
{
   if ( logEnabled )
   {
      mLogFile.close();
      logEnabled = false;
   }
}
//---------------------------------------------------------------------------
/* Windows only
std::string lastError( DWORD erno )
{
   LPVOID lpMsgBuf;

   FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      erno,
      MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),    // Default language
      ( LPTSTR ) & lpMsgBuf,
      0,
      NULL
   );
   std::string s;
   s = reinterpret_cast<const char*>( lpMsgBuf );

   // Free the buffer.
   LocalFree( lpMsgBuf );
   return s;
}
std::string lastError( void )
{
   return lastError( GetLastError() );
}
*/
QString getTraceFileName()
{
   return mLogFile.getTraceFileName();
}
//---------------------------------------------------------------------------

