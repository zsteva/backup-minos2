#ifndef MONITOREDLOG_H
#define MONITOREDLOG_H
#include "base_pch.h"

class MonitoringFrame;
class MonitoredLog
{
   private:
      void getLogStanza( int sno );
      bool monitorEnabled;

      qint64 inStanzaRequest;
      QSet <int> stanzasPulled;

      int lastScannedStanza;

      PublishState state;
      QString server;
      QString publishedName;
      int expectedStanzaCount;
      MinosTestImport *mt;
      BaseContestLog * contest;
      MonitoringFrame *frame;

   public:


      MonitoredLog();
      ~MonitoredLog();

      bool enabled() const
      {
         return monitorEnabled;
      }

      void initialise( const QString &server, const QString &name );
      BaseContestLog * getContest() const
      {
         return contest;
      }
      QString getPublishedName() const
      {
         return publishedName;
      }
      void setExpectedStanzaCount( int ec )
      {
         expectedStanzaCount = ec;
      }
      void setFrame( MonitoringFrame *f )
      {
         frame = f;
      }
      MonitoringFrame *getFrame()
      {
          return frame;
      }

      void setState(PublishState s)
      {
         state = s;
      }
      PublishState getState() const
      {
         return state;
      }
      void startMonitor();
      void stopMonitor();
      void checkMonitor();
      void processLogStanza( int stanza, const QString &stanzaData );
};

#endif // MONITOREDLOG_H
