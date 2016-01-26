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
class LogEventQ
{
   public:
      static std::deque <LogEvent *> logQueue;
};
std::deque <LogEvent *> LogEventQ::logQueue;
//---------------------------------------------------------------------------
void queueLog( LogEvent *a )
{
   if ( a )
   {
      CsGuard lck;

      LogEventQ::logQueue.push_back( a );
//      if ( logEventEvent )
//         SetEvent( logEventEvent );
   }
}
//---------------------------------------------------------------------------

LogEvent * deQueueLog()
{
   CsGuard lck;

   if ( !LogEventQ::logQueue.empty() )
   {
      LogEvent * log = LogEventQ::logQueue[ 0 ];
      LogEventQ::logQueue.pop_front();
      return log;
   }
   return 0;
}
*/
//---------------------------------------------------------------------------
void logMessage( const QString &level, const QString &mess )
{
   // queue a log event on the event queue

    trace( "Log : " + level + " " + mess );

/*
   LogEvent * ev = new LogEvent;
   ev->eMessageType = level;
   ev->eMessage = mess;
   queueLog( ev );
   */
}
//---------------------------------------------------------------------------
void createCloseEvent()
{
    /*
   if ( !minosCloseEvent )
      minosCloseEvent = CreateEvent( NULL, TRUE, FALSE, _T("MinosClosedownEvent") ); // manual reset, initially false
      */
}
//---------------------------------------------------------------------------
void signalCloseEvent()
{
   createCloseEvent();
//   SetEvent( minosCloseEvent );
}
//---------------------------------------------------------------------------
bool checkCloseEvent()
{
   createCloseEvent();
//   return ( WaitForSingleObject( minosCloseEvent , 0 ) == WAIT_OBJECT_0 );
   return false;
}
//---------------------------------------------------------------------------
void resetCloseEvent()
{
   createCloseEvent();
   //ResetEvent( minosCloseEvent );
}
//---------------------------------------------------------------------------

