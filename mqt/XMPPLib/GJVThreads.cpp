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

QMutex CsGuard::m_mutex(QMutex::Recursive);
//---------------------------------------------------------------------------

GJV_thread::GJV_thread(const QString &name, void ( *tex ) ( void * ),
                        void *arg, bool aboveNormalPriority )
      : threadName( name ), arg( arg ) , tex( tex ), Terminated( false )
{
    start(aboveNormalPriority?QThread::HighPriority:QThread::InheritPriority);
}
GJV_thread::~GJV_thread()
{
    if (!isFinished())
        GJV_join();
}
void GJV_thread::run()
{
    ( *( tex ) ) ( arg );
}


void GJV_thread::GJV_join()
{
   // called from main thread
   Terminated = true;
   // Wait for request to close or for a read thread to terminate.
   /*DWORD dwWait = */
   wait( 10000 );

}


