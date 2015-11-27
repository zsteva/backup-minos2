/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef GJVThreadsH
#define GJVThreadsH

class CsGuard:public QMutexLocker
{
      static QMutex m_mutex;
   public:
      CsGuard():QMutexLocker(&m_mutex)
      {
      }

      ~CsGuard()
      {
      }
      static void ClearDown()
      {
      }
};
//---------------------------------------------------------------------------

class GJV_thread: public QThread
{
   private:
      QString threadName;

      void *arg;
      void ( *tex ) ( void * );

    public:
      GJV_thread( const QString &name );
      GJV_thread( const QString &name, void ( *tex ) ( void * ), void *arg, bool aboveNormalPriority = false );
      ~GJV_thread();

      void GJV_join();
      QString getThreadName() const
      {
         return threadName;
      }

      // QThread interface
protected:
      virtual void run() override;
      bool Terminated;
};

#endif
