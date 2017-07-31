#include "MonitoredContestLog.h"

//=============================================================================================

MonitoredContestLog::MonitoredContestLog() : BaseContestLog()
{
}
MonitoredContestLog::~MonitoredContestLog()
{
}

void MonitoredContestLog::makeContact( bool timeNow, BaseContact *&lct )
{
   lct = new DisplayContestContact( this, timeNow );
}
//=============================================================================================
