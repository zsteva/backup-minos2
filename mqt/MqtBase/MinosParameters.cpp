/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

MinosParameters *MinosParameters::mp = 0;
MinosParameters *MinosParameters::getMinosParameters()
{
   if ( !mp )
   {
      mShowMessage( "MinosParameters not initialised", 0 );
   }
   return mp;
}

bool MinosParametersAdapter::insertContest( BaseContestLog */*p*/, unsigned int /*sno*/ )
{
   return true;
}
int MinosParametersAdapter::getMagneticVariation()
{
   return 0;
}
void MinosParametersAdapter::getDisplayColumnWidth( const std::string &/*key*/, int &val, int def )
{
   val = def;
}
void MinosParametersAdapter::setDisplayColumnWidth( const std::string &/*key*/, int /*val*/ )
{
}
void MinosParametersAdapter::getBoolDisplayProfile( int /*enumkey*/, bool &value )
{
   value = true;
}
void MinosParametersAdapter::setBoolDisplayProfile( int /*enumkey*/, bool /*value*/ )
{
}
void MinosParametersAdapter::flushDisplayProfile()
{
}
int MinosParametersAdapter::getBigClockCorrection()
{
   return 0;
}
int MinosParametersAdapter::getStatsPeriod1()
{
   return 0;
}
int MinosParametersAdapter::getStatsPeriod2()
{
   return 0;
}
void MinosParametersAdapter::setStatsPeriod1( int /*p*/ )
{
}
void MinosParametersAdapter::setStatsPeriod2( int /*p*/ )
{
}
bool MinosParametersAdapter::getAllowLoc4()
{
   return false;
}
bool MinosParametersAdapter::getAllowLoc8()
{
   return false;
}
bool MinosParametersAdapter::yesNoMessage(QWidget * /*Owner*/, QString /*mess*/ )
{
   return false;
}
void MinosParametersAdapter::mshowMessage( QString /*mess*/, QWidget* /*Owner*/ )
{
}
BaseContestLog * MinosParametersAdapter::getCurrentContest()
{
   return 0;
}
bool MinosParametersAdapter::insertList( ContactList */*p*/, unsigned int /*sno*/ )
{
   return false;
}
bool MinosParametersAdapter::isContestOpen(const QString /*fn*/ )
{
   return false;
}
bool MinosParametersAdapter::isListOpen(const QString /*fn*/ )
{
   return false;
}
std::vector<BaseContestLog *> MinosParametersAdapter::getContestList()
{
   std::vector<BaseContestLog *> logList;
   return logList;
}


