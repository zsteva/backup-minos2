#ifndef MONITOREDCONTESTLOG_H
#define MONITOREDCONTESTLOG_H
#include "base_pch.h"


class MonitoredContestLog:public BaseContestLog
{
public:
   MonitoredContestLog();
   virtual ~MonitoredContestLog();

   virtual void makeContact( bool time_now, QSharedPointer<BaseContact>& ) override;
};
#endif // MONITOREDCONTESTLOG_H
