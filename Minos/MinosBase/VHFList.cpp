/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2007 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"
#pragma hdrstop
#include "VHFList.h"
String calString[] =
{
   "vhfcontests",
   "hfcontests",
   "microcontests",
   "VHFContestsOther",
   "HFContestsOther"
};

std::string strupr( const std::string &s )
{
   std::string s2;
   for ( unsigned int i = 0; i < s.length(); i++ )
   {
      s2 += toupper( s[ i ] );
   }
   //s = s2;
   return s2;
}

int curYear = 0;

const char *monthTable[ 12 ] =
   {
      "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
   };
int monthLength[ 12 ] =
   {
      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
   };
const char *days[ 7 ] =
   {
      "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
   };
void setYear( int y )
{
   curYear = y;
   if ( y % 4 == 0 )
   {
      monthLength[ 1 ] = 29;
   }
   else
   {
      monthLength[ 1 ] = 28;
   }
}
int getMonth( const std::string &m )
{
   std::string m2 = strupr( m ).substr( 0, 3 );
   for ( int i = 0; i < 12; i++ )
   {
      if ( m2 == monthTable[ i ] )
      {
         return i + 1;
      }
   }
   return 0;
}
int getDayOfWeek( TDateTime dtg )
{
   int dw = dtg.DayOfWeek();    // Sunday is 1, Saturday is 7

   // We want Monday == 1, Sunday as 7

   dw--;
   if ( dw == 0 )
   {
      dw = 7;
   }
   return dw;
}
int getDate( int month, int day, int week )
{
   // Day 1 is Monday

   // day 6 week 1 means "first Saturday of the month"
   // and day 2 week 1 means "first Tuesday of the month".
   // week 2 day 2 would mean "2nd Tuesday of the month".
   // day 6+1 is Sunday after Saturday

   // day 1+3 week 3 is  Thursday after 3rd Monday

   // Sept 2010 day 4+13 week 2 is 13 days after 2nd Thursday (Sep 22 2010)

   // return the date of the month/day/week


   TDateTime startMonth( curYear, month, 1 );
   int dw = getDayOfWeek( startMonth ) - 1;    // make it 0 based

   // So now we need to go through
   int wcount = 1;   // start in first week
   for ( int i = 0; i < monthLength[ month - 1 ]; i++ )
   {
      if ( ( dw + i ) % 7 == day - 1 )
      {
         // we are on the correct day
         if ( wcount == week )
         {
            return i + 1;
         }
         wcount++;
      }
   }

   return 0;
}
int calendarYear = 0;
Calendar::Calendar(int year)
{
   calendarYear = year;
}
Calendar::~Calendar()
{
}

/*
The 9th EC Directive prescribes the start and end dates of summer time as the
last Sundays in March and October respectively. These dates are in line with
those already operating in the United Kingdom. The 9th Directive provides
that these start and end dates should apply indefinitely.
*/

TDateTime localToUTC( TDateTime t )
{
   unsigned short year;
   unsigned short month;
   unsigned short day;
   unsigned short hour;
   unsigned short min;
   unsigned short sec;
   unsigned short msec;

   t.DecodeDate( &year, &month, &day );
   t.DecodeTime( &hour, &min, &sec, &msec );

   bool isDst = false;
   if ( month > 3 && month < 10 )
   {
      isDst = true;
   }
   else
      if ( month == 3)
      {
         int fSundayApril = getDate(4, 7, 1);
         int lsM = fSundayApril + 31 - 7 ;
         if (day >= lsM )
         {
            isDst = true;
         }
      }
      else
         if ( month == 10)
         {
            int fSundayNovember = getDate(11, 7, 1);
            int lsO = fSundayNovember + 31 - 7 ;
            if ( day < lsO )
            {
               isDst = true;
            }
         }
   if ( isDst )
   {
      t -= TDateTime( 1, 0, 0, 0 );
   }
   return t;
}
String CalendarYear::getPath()
{
   char appFName[ 255 ];
   int nLen = 0;
   nLen = ::GetModuleFileName( HInstance, appFName, 255 );
   appFName[ nLen ] = '\0';

   String fpath = ExtractFilePath( appFName );

   String path = fpath +  "configuration\\" + calString[type] + yearString() + ".xml";
   return path;
}
String CalendarYear::getURL()
{
   String url = getSite() + calString[type] + yearString() + ".xml";
   return url;

}
/*String CalendarYear::getProvisionalURL()
{
   String url = getSite() + calString[type] + yearString() + "-prov.xml";
   return url;

}
*/
//---------------------------------------------------------------------------
String VHFCalendarYear::getSite()
{
   return "http://www.rsgbcc.org/vhf/";
}
bool CalendarYear::downloadFile(TIdHTTP *IdHTTP1, bool showError)
{
   String calendarURL = getURL();
   try
   {
      String cal = IdHTTP1->Get( calendarURL );
      // and write it out

      TStringList *sl = new TStringList();
      sl->Text = cal;
      sl->SaveToFile( getPath() );
      trace(("HTPP Get of " + calendarURL + " OK").c_str());
      delete sl;
      return true;
   }
   catch ( Exception & e )
   {
      trace(("HTPP Get of " + calendarURL + " failed: " + e.Message).c_str());
      if (showError)
      {
         ShowMessage( "HTPP Get of " + calendarURL + " failed: " + e.Message );
      }
   }
   return false;
}
//---------------------------------------------------------------------------
String CTYCalendarYear::getSite()
{
   return "http://www.country-files.com/cty/cty.dat";
}
String CTYCalendarYear::getPath()
{
   char appFName[ 255 ];
   int nLen = 0;
   nLen = ::GetModuleFileName( HInstance, appFName, 255 );
   appFName[ nLen ] = '\0';

   String fpath = ExtractFilePath( appFName );
   String p = fpath + "\\Configuration\\" + "cty.dat";
   return p;
}
String CTYCalendarYear::getURL()
{
   return getSite();
}
//---------------------------------------------------------------------------
String LocSquaresCalendarYear::getSite()
{
   return "http://www.g0gjv.org.uk/LocSquares.ini";
}
String LocSquaresCalendarYear::getPath()
{
   char appFName[ 255 ];
   int nLen = 0;
   nLen = ::GetModuleFileName( HInstance, appFName, 255 );
   appFName[ nLen ] = '\0';

   String fpath = ExtractFilePath( appFName );
   String p = fpath + "\\Configuration\\" + "LocSquares.ini";
   return p;
}
String LocSquaresCalendarYear::getURL()
{
   return getSite();
}
bool Calendar::parseFile( const std::string &fname )
{
   HANDLE calFile = CreateFile( fname.c_str(),
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                0,                  // security
                                OPEN_EXISTING,
                                FILE_FLAG_WRITE_THROUGH,
                                0 );               // template handle
   if ( calFile == INVALID_HANDLE_VALUE )
   {
      return false;
   }
   bool fileComplete = false;

   std::string buffer;
   while ( !fileComplete )
   {
      char rdbuffer[ 4096 + 1 ];
      DWORD chRead = 0;
      DWORD chToRead = 4096;
      bool ReadOK = ReadFile ( calFile, rdbuffer, chToRead, &chRead, 0 );
      if ( ReadOK )
      {
         if ( chRead > 0 )
         {
            rdbuffer[ chRead ] = '\0';

            buffer += rdbuffer;
         }
         else
         {
            fileComplete = true;
         }

      }
      else
      {
         fileComplete = true;
      }
   }
   CloseHandle( calFile );
   std::string buffer2;

   size_t dtdPos = buffer.find( "<!DOCTYPE" );
   if ( dtdPos != std::string::npos )
   {
      buffer2 = buffer.substr( 0, dtdPos );

      size_t dtdEndPos = buffer.find( "]>" );
      if ( dtdEndPos == std::string::npos )
      {
         return false;
      }
      buffer2 += buffer.substr( dtdEndPos + 2, buffer.size() - dtdEndPos - 2 );
   }

   TiXmlBase::SetCondenseWhiteSpace( false );
   TiXmlDocument xdoc;
   const char *loaded = xdoc.Parse( buffer2.c_str() );
   if ( !loaded )
   {
      return false;
   }
   TiXmlElement *tix = xdoc.RootElement();
   if ( !tix || !checkElementName( tix, "calendar" ) )
   {
      return false;
   }
   for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      if ( checkElementName( e, "mult_type" ) )
      {
         if ( !parseMultType( e ) )
         {
            return false;
         }
      }
      else
         if ( checkElementName( e, "special_rule" ) )
         {
            if ( !parseSpecialRule( e ) )
            {
               return false;
            }
         }
         else
            if ( checkElementName( e, "section" ) )
            {
               if ( !parseSection( e ) )
               {
                  return false;
               }
            }
            else
               if ( checkElementName( e, "band" ) )
               {
                  if ( !parseBand( e ) )
                  {
                     return false;
                  }
               }
               else
                  if ( checkElementName( e, "contest_series" ) )
                  {
                     if ( !parseContestSeries( e ) )
                     {
                        return false;
                     }
                  }
                  else
                     if ( checkElementName( e, "contest" ) )
                     {
                        if ( !parseContest( e ) )
                        {
                           return false;
                        }
                     }
                     else
                        if ( checkElementName( e, "version" ) )
                        {
                           version = e->GetText();
                        }
                        else
                        {
                           std::string eval = e->Value();
                           continue;
                        }
   }
   // Now we have the raw info; we need to go through it and
   // generate all the individual contest details for logger/adjsql
   setYear( calendarYear );
    for ( std::map<std::string, CalendarContest>::iterator i = contests.begin(); i != contests.end(); i++ )
    {

        for ( std::vector<CalendarBandList>::iterator bl = ( *i ).second.bandList.begin(); bl != ( *i ).second.bandList.end(); bl++ )
        {
            // For each contest iterate the time list or the band list
            // the band list overrides the time list!

            std::vector<TimeList>::iterator tls;
            std::vector<TimeList>::iterator tle;

            CalendarBand &cb = bands[ (*bl).name ];

            if (cb.timeList.size() != 0)
            {
                tls = cb.timeList.begin();
                tle = cb.timeList.end();
            }
            else
            {
                tls = ( *i ).second.timeList.begin();
                tle = ( *i ).second.timeList.end();
            }

            for ( std::vector<TimeList>::iterator tl = tls; tl != tle; tl++ )
            {
                for ( std::vector<MonthList>::iterator ml = ( *tl ).monthList.begin(); ml != ( *tl ).monthList.end(); ml++ )
                {
                    // startDate needs to incorporate the week and day

                    // day 6 week 1 means "first Saturday of the month"
                    // and day 2 week 1 means "first Tuesday of the month".
                    // week 2 day 2 would mean "2nd Tuesday of the month".
                    // day 6+1 is Sunday after Saturday

                    // XMAS cumulatives also have <start_date_list>, but they appear to be the only ones


                    int sm = getMonth ( ( *ml ).month );
                    if ( sm == 0 )
                    {
                        continue;
                    }

                    // we need to copy the datelist before we add to it...

                    std::vector<StartDateList> startDateList = ( *tl ).startDateList;

                    if ( startDateList.size() == 0 )
                    {
                        // make the start info into a statt date (just one!)

                        std::string startWeek = ( *tl ).startWeek;
                        std::string startDay = ( *tl ).startDay;


                        std::string startday1 = startDay;
                        std::string startday2;
                        int istartday1, istartday2;

                        unsigned int ppos = startDay.find ( "+" );
                        if ( ppos != std::string::npos )
                        {
                            startday1 = startDay.substr ( 0, ppos );
                            startday2 = startDay.substr ( ppos + 1, startDay.size() );
                            istartday1 = atoi ( startday1.c_str() );
                            istartday2 = atoi ( startday2.c_str() );
                        }
                        else
                        {
                            istartday1 = atoi ( startday1.c_str() );
                            istartday2 = 0;
                        }
                        if ( istartday1 == 0 )
                        {
                            continue;   // next time list
                        }

                        int istartWeek = atoi ( startWeek.c_str() );
                        if ( istartWeek == 0 )
                        {
                            continue;   // next time list
                        }


                        //               std::string bstartWeek = (*bl).startWeek; // or iterate its timeList
                        int istartDate = getDate ( sm, istartday1, istartWeek );
                        if ( istartDate == 0 )
                        {
                            continue;   // next time list
                        }

                        istartDate += istartday2;

                        StartDateList sdl;
                        sdl.date = makeStr(istartDate);

                        startDateList.push_back(sdl);
                    }

                    if ( startDateList.size() )
                    {
                        // need to iterate the start dates
                        for ( unsigned int j = 0; j < startDateList.size(); j++ )
                        {
                            int istartDate = atoi ( startDateList[ j ].date.c_str() );
                            if ( istartDate == 0 )
                            {
                                continue;
                            }

                            IndividualContest ic;

                            std::string desc = trim ( ( *i ).second.description );
                            std::string sdesc = trim ( ( *i ).second.shortDescription );

                            // This needs changing once the contests are sorted
                            // as e.g. 70MHz cumulatives are defined in two groups
                            //if (desc[desc.size() - 1] == '#')
                            //{
                            //   desc += makeStr(instance++);
                            //}
                            ic.description = desc;
                            ic.shortDescription = sdesc;
                            ic.bands = ( *bl ).name;
                            ic.mode = ( *i ).second.mode;

                           try
                           {
                              ic.start = TDateTime( curYear, sm, istartDate );
                              int s = atoi(( *tl ).startTime.substr(0, 2).c_str()) * 60 + atoi(( *tl ).startTime.substr(2, 2).c_str());
                              ic.start +=  s / 1440.0;
                              ic.duration = ( *tl ).duration;
                              ic.finish = ic.start + atof( ic.duration.c_str() ) / 24;

                              std::string timeType = ( *tl ).timeType;
                              if ( timeType == "local" )
                              {
                                 ic.start = localToUTC( ic.start );
                                 ic.finish = localToUTC( ic.finish );
                              }
                           }
                           catch ( Exception & e )
                           {
                              ShowMessage( "Exception " + e.Message + " from " + ic.description.c_str() );
                           }

                            ic.ppKmScoring = ( ( *i ).second.scoring == CalendarContest::perkms );
                            for ( unsigned int j = 0; j < ( *i ).second.sectionList.size(); j++ )
                            {
                                std::string n = ( *i ).second.sectionList[ j ].name;

                                std::map<std::string, CalendarSection>::iterator s = sections.find ( n );

                                if ( s == sections.end() )
                                {
                                    continue;
                                }
                                int mls = ( *s ).second.monthList.size();
                                if ( mls )
                                {
                                    bool monthOK = false;
                                    for ( std::vector<MonthList>::iterator ml = ( *s ).second.monthList.begin(); ml != ( *s ).second.monthList.end(); ml++ )
                                    {
										if ( strupr ( ( *ml ).month ) == strupr ( monthTable[ sm - 1 ] ) )
                                        {
                                            monthOK = true;
                                            break;
                                        }
                                    }
                                    if ( !monthOK )
                                    {
                                        continue;
                                    }
                                }

                                if ( !sections[ n ].overall )
                                {
                                    ic.sections += n;
                                    ic.sections += ",";
                                }
                            }
                            ic.sections = ic.sections.substr ( 0, ic.sections.size() - 1 );  // lose any trailing comma
                            ic.mults = ( *i ).second.mult;
                            for ( unsigned int j = 0; j < ( *i ).second.specialRulesList.size(); j++ )
                            {
                                ic.specialRules += ( *i ).second.specialRulesList[ j ].name + " ";
                            }
                            //ic.power = ( *i ).second.power;

                            ic.reg1band = bands[ ic.bands ].reg1band.c_str();
                            if ( ic.reg1band == "1,2 GHz" )
                            {
                                ic.reg1band = "1,3 GHz";
                            }

                            calendar.push_back ( ic );
                        }
                    }
                }
            }
        }
    }
//   for ( std::map<std::string, Contest>::iterator i = contests.begin(); i != contests.end(); i++ )
//   {
//      int instance = 1;  // should be used to add contest sequence when name ends with a #
//      // but we need to have them sorted in dtg order for that to work.
//      instance;          // take out the warning that it isn't used
//      // For each contest iterate the time list and the band list
//      for ( std::vector<TimeList>::iterator tl = ( *i ).second.timeList.begin(); tl != ( *i ).second.timeList.end(); tl++ )
//      {
//         for ( std::vector<MonthList>::iterator ml = ( *tl ).monthList.begin(); ml != ( *tl ).monthList.end(); ml++ )
//         {
//            for ( std::vector<CalendarBandList>::iterator bl = ( *i ).second.bandList.begin(); bl != ( *i ).second.bandList.end(); bl++ )
//            {
//               // startDate needs to incorporate the week and day
//
//               // day 6 week 1 means "first Saturday of the month"
//               // and day 2 week 1 means "first Tuesday of the month".
//               // week 2 day 2 would mean "2nd Tuesday of the month".
//               // day 6+1 is Sunday after Saturday
//
//               // XMAS cumulatives also have <start_date_list>, but they appear to be the only ones
//
//               // Is it best to generate a table of 365(6) days, and assign each one month/week/day
//
//               // And we may need month pointers, so we cna then count days from each
//
//               // Somewhere, we need to evaluate the year... for now, we preset it
//
//               int sm = getMonth( ( *ml ).month );
//               if ( sm == 0 )
//               {
//                  continue;
//               }
//
//
//
//               if ( ( *tl ).startDateList.size() )
//               {
//                  // need to iterate the start dates
//                  for ( unsigned int j = 0; j < ( *tl ).startDateList.size(); j++ )
//                  {
//                     int istartDate = atoi( ( *tl ).startDateList[ j ].date.c_str() );
//                     if ( istartDate == 0 )
//                     {
//                        continue;
//                     }
//
//                     IndividualContest ic;
//
//                     std::string desc = trim( ( *i ).second.description );
//                     std::string sdesc = trim( ( *i ).second.shortDescription );
//                     /*
//                     // This needs changing once the contests are sorted
//                     // as e.g. 70MHz cumulatives are defined in two groups
//                     if (desc[desc.size() - 1] == '#')
//                     {
//                        desc += makeStr(instance++);
//                     }
//                     */
//                     ic.description = desc;
//                     ic.shortDescription = sdesc;
//                     ic.bands = ( *bl ).name;
//
//                     try
//                     {
//                        ic.start = TDateTime( curYear, sm, istartDate );
//                        int s = atoi(( *tl ).startTime.substr(0, 2).c_str()) * 60 + atoi(( *tl ).startTime.substr(2, 2).c_str());
//                        ic.start +=  s / 1440.0;
//                        ic.duration = ( *tl ).duration;
//                        ic.finish = ic.start + atof( ic.duration.c_str() ) / 24;
//
//                        std::string timeType = ( *tl ).timeType;
//                        if ( timeType == "local" )
//                        {
//                           ic.start = localToUTC( ic.start );
//                           ic.finish = localToUTC( ic.finish );
//                        }
//                     }
//                     catch ( Exception & e )
//                     {
//                        ShowMessage( "Exception " + e.Message + " from " + ic.description.c_str() );
//                     }
//                     ic.ppKmScoring = ( ( *i ).second.scoring == Contest::perkms );
//                     for ( unsigned int j = 0; j < ( *i ).second.sectionList.size(); j++ )
//                     {
//                        std::string n = ( *i ).second.sectionList[ j ].name;
//                        std::map<std::string, Section>::iterator s = sections.find(n);
//
//                        if (s == sections.end())
//                        {
//                           continue;
//                        }
//                        int mls = (*s).second.monthList.size();
//                        if (mls)
//                        {
//                           bool monthOK = false;
//                           for (std::vector<MonthList>::iterator ml = (*s).second.monthList.begin(); ml != (*s).second.monthList.end(); ml++)
//                           {
//                              if (strupr((*ml).month) == strupr(monthTable[sm - 1]) )
//                              {
//                                 monthOK = true;
//                                 break;
//                              }
//                           }
//                           if (!monthOK)
//                           {
//                              continue;
//                           }
//                        }
//                        if ( !sections[ n ].overall )
//                        {
//                           ic.sections += n;
//                           ic.sections += ",";
//                        }
//                     }
//                     // check if the section has a month_list
//
//                     ic.sections = ic.sections.substr( 0, ic.sections.size() - 1 );   // lose any trailing comma
//                     ic.mults = ( *i ).second.mult;
//                     for ( unsigned int j = 0; j < ( *i ).second.specialRulesList.size(); j++ )
//                     {
//                        ic.specialRules += ( *i ).second.specialRulesList[ j ].name + " ";
//                     }
//                     ic.power = ( *i ).second.power;
//
//                     ic.reg1band = bands[ ic.bands ].reg1band.c_str();
//                     if (ic.reg1band == "1,2 GHz")
//                     {
//                        ic.reg1band = "1,3 GHz";
//                     }
//
//                     calendar.push_back( ic );
//                  }
//               }
//               else
//               {
//
//                  // NB - band list overrides time list!
//                  std::string startWeek;
//                  std::string startDay;
//                  std::string startTime;
//                  std::string timeType;
//                  std::string duration;
//
//                  ContestBand &blst = bands[ ( *bl ).name ];
//                  if ( blst.timeList.size() )
//                  {
//                     TimeList & b = blst.timeList[ 0 ];
//
//                     startWeek = b.startWeek;
//                     startDay = b.startDay;
//                     startTime = b.startTime;
//                     duration = b.duration;
//                     timeType = b.timeType;
//                  }
//                  else
//                  {
//                     startWeek = ( *tl ).startWeek;
//                     startDay = ( *tl ).startDay;
//                     startTime = ( *tl ).startTime;
//                     duration = ( *tl ).duration;
//                     timeType = ( *tl ).timeType;
//                  }
//
//                  std::string startday1 = startDay;
//                  std::string startday2;
//                  int istartday1, istartday2;
//
//                  unsigned int ppos = startDay.find("+");
//                  if (ppos != std::string::npos)
//                  {
//                     startday1 = startDay.substr(0, ppos);
//                     startday2 = startDay.substr(ppos + 1, startDay.size());
//                     istartday1 = atoi(startday1.c_str());
//                     istartday2 = atoi(startday2.c_str());
//                  }
//                  else
//                  {
//                     istartday1 = atoi(startday1.c_str());
//                     istartday2 = 0;
//                  }
//                  if (istartday1 == 0)
//                  {
//                     continue;
//                  }
//
//                  int istartWeek = atoi( startWeek.c_str() );
//                  if ( istartWeek == 0 )
//                  {
//                     continue;
//                  }
//
//
//                  //               std::string bstartWeek = (*bl).startWeek; // or iterate its timeList
//                  int istartDate = getDate( sm, istartday1, istartWeek );
//                  if ( istartDate == 0 )
//                  {
//                     continue;
//                  }
//
//                  istartDate += istartday2;
//
//                  IndividualContest ic;
//
//                  std::string desc = trim( ( *i ).second.description );
//                  /*
//                  // This needs changing once the contests are sorted
//                  // as e.g. 70MHz cumulatives are defined in two groups
//                  if (desc[desc.size() - 1] == '#')
//                  {
//                     desc += makeStr(instance++);
//                  }
//                  */
//                  ic.description = desc;
//                  ic.bands = ( *bl ).name;
//
//                  try
//                  {
//                     ic.start = TDateTime( curYear, sm, istartDate );
//
//                     if (startTime == "2130")
//                     {
//                        String temp = ic.start.FormatString( "dd/mm/yyyy hh:mm" );
//                     }
//
//                     int s = atoi(startTime.substr(0, 2).c_str()) * 60 + atoi(startTime.substr(2, 2).c_str());
//                     ic.start +=  s / 1440.0;
//
//                     String temp2 = ic.start.FormatString( "dd/mm/yyyy hh:mm" );
//
//                     ic.duration = duration;
//                     ic.finish = ic.start + atof( ic.duration.c_str() ) / 24;
//                     if ( timeType == "local" )
//                     {
//                        ic.start = localToUTC( ic.start );
//                        ic.finish = localToUTC( ic.finish );
//                     }
//
//                     String temp3 = ic.start.FormatString( "dd/mm/yyyy hh:mm" );
//
//                  }
//                  catch ( Exception & e )
//                  {
//                     ShowMessage( "Exception " + e.Message + " from " + ic.description.c_str() );
//                  }
//
//                  ic.ppKmScoring = ( ( *i ).second.scoring == Contest::perkms );
//                  for ( unsigned int j = 0; j < ( *i ).second.sectionList.size(); j++ )
//                  {
//                     std::string n = ( *i ).second.sectionList[ j ].name;
//                     std::map<std::string, Section>::iterator s = sections.find(n);
//
//                     if (s == sections.end())
//                     {
//                        continue;
//                     }
//                     int mls = (*s).second.monthList.size();
//                     if (mls)
//                     {
//                        bool monthOK = false;
//                        for (std::vector<MonthList>::iterator ml = (*s).second.monthList.begin(); ml != (*s).second.monthList.end(); ml++)
//                        {
//                           if (strupr((*ml).month) == strupr(monthTable[sm - 1]))
//                           {
//                              monthOK = true;
//                              break;
//                           }
//                        }
//                        if (!monthOK)
//                        {
//                           continue;
//                        }
//                     }
//                     if ( !sections[ n ].overall )
//                     {
//                        ic.sections += n;
//                        ic.sections += ",";
//                     }
//                  }
//                  ic.sections = ic.sections.substr( 0, ic.sections.size() - 1 );   // lose any trailing comma
//                  ic.mults = ( *i ).second.mult;
//                  for ( unsigned int j = 0; j < ( *i ).second.specialRulesList.size(); j++ )
//                  {
//                     ic.specialRules += ( *i ).second.specialRulesList[ j ].name + " ";
//                  }
//                  ic.power = ( *i ).second.power;
//
//                  ic.reg1band = bands[ ic.bands ].reg1band.c_str();
//                  if (ic.reg1band == "1,2 GHz")
//                  {
//                     ic.reg1band = "1,3 GHz";
//                  }
//
//                  calendar.push_back( ic );
//               }
//            }
//         }
//      }
//   }
   std::sort( calendar.begin(), calendar.end() );
   return true;
}
bool IndividualContest::operator<( const IndividualContest& rhs ) const
{
   // p1 is from list; p2 is the one being searched for

   if (start != rhs.start)
   {
      return start < rhs.start;
   }
   if (description != rhs.description)
   {
      return description < rhs.description;
   }
   return bands < rhs.bands;
}
bool Calendar::parseMultType( TiXmlElement * tix )
{
   MultType mt;
   for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      if ( checkElementName( e, "name" ) )
      {
         mt.name = e->GetText();
      }
      else
         if ( checkElementName( e, "description" ) )
         {
            mt.description = e->GetText();
         }
         else
            if ( checkElementName( e, "long_description" ) )
            {
               mt.longDescription = e->GetText();
            }
            else
               if ( checkElementName( e, "exchange" ) )
               {
                  mt.exchange = e->GetText();
               }
               else
                  if ( checkElementName( e, "scoring_description" ) )
                  {
                     mt.scoringDescription = e->GetText();
                  }
                  else
                      if ( checkElementName ( e, "bonus" ) )
                      {
                          //<bonus square="IO83" points="500"/>
                          std::string square = trim(e->Attribute("square"));
                          std::string points = e->Attribute("points");

                          mt.bonuses[square] = atoi(points.c_str());
                      }
                      else
                        {
                           std::string eval = e->Value();
                           continue;
                        }
   }
   mults[ mt.name ] = mt;
   return true;
}
bool Calendar::parseSpecialRule( TiXmlElement * tix )
{
   SpecialRule sr;
   for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      if ( checkElementName( e, "name" ) )
      {
         sr.name = e->GetText();
      }
      else
         if ( checkElementName( e, "description" ) )
         {
            sr.description = e->GetText();
         }
         else
            if ( checkElementName( e, "long_description" ) )
            {
               sr.longDescription = e->GetText();
            }
            else
            {
               std::string eval = e->Value();
               continue;
            }
   }
   specialRules[ sr.name ] = sr;
   return true;
}
bool Calendar::parseSection( TiXmlElement * tix )
{
   CalendarSection s;
   for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      if ( checkElementName( e, "name" ) )
      {
         s.name = e->GetText();
      }
      else
         if ( checkElementName( e, "description" ) )
         {
            s.description = e->GetText();
         }
         else
            if ( checkElementName( e, "long_description" ) )
            {
               s.longDescription = e->GetText();
            }
            else
               if ( checkElementName( e, "power" ) )
               {
                  //s.power = e->GetText();
               }
               else
                  if ( checkElementName( e, "height" ) )
                  {
                     s.height = e->GetText();
                  }
                  else
                     if ( checkElementName( e, "singleantenna" ) )
                     {
                        s.singleAntenna = ( e->GetText() == "1" );
                     }
                     else
                        if ( checkElementName( e, "overall_section" ) )
                        {
                           s.overall = true;
                        }
                        else
                           if ( checkElementName( e, "month_list" ) )
                           {
                              MonthList m;
                              m.month = e->GetText();
                              s.monthList.push_back( m );
                           }
                           else
                              {
                                 std::string eval = e->Value();
                                 continue;
                              }
   }
   sections[ s.name ] = s;
   return true;
}
bool Calendar::parseBand( TiXmlElement * tix )
{
   CalendarBand b;
   for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      if ( checkElementName( e, "name" ) )
      {
         b.name = e->GetText();
      }
      else
         if ( checkElementName( e, "reg1band" ) )
         {
            b.reg1band = e->GetText();
            if (b.reg1band == "1,2 GHz")
            {
               b.reg1band = "1,3 GHz";
            }
         }
         else
            if ( checkElementName( e, "wavelength" ) )
			{
			   const char *w = e->GetText();
			   if (w)
			   {
				   b.wavelength = std::string(w);
			   }
			}
            else
               if ( checkElementName( e, "time_list" ) )
               {
                  if ( !( parseTimeList( e, b.timeList ) ) )
                     return false;
               }
               else
               {
                  std::string eval = e->Value();
                  continue;
               }
   }
   bands[ b.name ] = b;
   return true;
}
bool Calendar::parseContestSeries( TiXmlElement * tix )
{
   ContestSeries cs;
   for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      if ( checkElementName( e, "name" ) )
      {
         cs.name = e->GetText();
      }
      else
         if ( checkElementName( e, "description" ) )
         {
            cs.description = e->GetText();
         }
         else
            if ( checkElementName( e, "contest_list" ) )
            {
               ContestList cl;
               cl.name = e->GetText();
               cs.contestList.push_back( cl );
            }
            else
               if ( checkElementName( e, "award_list" ) )
               {
                  std::string al;
                  al = e->GetText();
                  cs.awardList.push_back( al );
               }
               else
               {
                  std::string eval = e->Value();
                  continue;
               }
   }
   contestSeries[ cs.name ] = cs;
   return true;
}
bool Calendar::parseContest( TiXmlElement * tix )
{
   CalendarContest c;
   for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      if ( checkElementName( e, "name" ) )
      {
         c.name = e->GetText();
      }
      else
         if ( checkElementName( e, "description" ) )
         {
            c.description = e->GetText();
         }
         else
            if ( checkElementName( e, "short_description" ) )
            {
               c.shortDescription = e->GetText();
            }
            else
               if ( checkElementName( e, "time_list" ) )
               {
                  if ( !( parseTimeList( e, c.timeList ) ) )
                     return false;
               }
               else
                  if ( checkElementName( e, "section_list" ) )
                  {
                     SectionList sl;
                     sl.name = e->GetText();
                     c.sectionList.push_back( sl );
                  }
                  else
                     if ( checkElementName( e, "special_rules_list" ) )
                     {
                        SpecialRuleList srl;
                        srl.name = e->GetText();
                        c.specialRulesList.push_back( srl );
                     }
                     else
                        if ( checkElementName( e, "band_list" ) )
                        {
                           CalendarBandList bl;
                           bl.name = e->GetText();
                           c.bandList.push_back( bl );
                        }
                        else
                           if ( checkElementName( e, "iaru" ) )
                           {
                              c.iaru = ( e->GetText() == "1" );
                           }
                           else
                              if ( checkElementName( e, "scoring" ) )
                              {
                                 for ( TiXmlElement * f = e->FirstChildElement(); f; f = f->NextSiblingElement() )
                                 {
                                    if ( checkElementName( f, "perkms" ) )
                                    {
									   c.scoring = CalendarContest::perkms;
                                    }
                                    else
                                       if ( checkElementName( f, "oneppq" ) )
                                       {
										  c.scoring = CalendarContest::oneppq;
                                       }
                                       else
                                          return false;
                                    break;
                                 }
                              }
                              else
                                 if ( checkElementName( e, "mult" ) )
                                 {
                                    c.mult = e->GetText();
                                 }
                                 else
                                    if ( checkElementName( e, "power" ) )
                                    {
                                       //c.power = e->GetText();
                                    }
                                    else
                                       if ( checkElementName( e, "mode" ) )
                                       {
                                          std::string mode;
                                          mode = e->GetText();
                                          if (mode == "SSB")
                                          {
                                             mode = "J3E";
                                          }
                                          if (mode == "CW")
                                          {
                                             mode = "A1A";
                                          }
                                          if (mode == "FM")
                                          {
                                             mode = "F3E";
                                          }
                                          c.mode = mode;
                                       }
                                       else
                                          if ( checkElementName( e, "award_list" ) )
                                          {
                                             std::string al = e->GetText();
                                          }
                                          else
                                             if ( checkElementName( e, "entry_date" ) )
                                             {
                                                c.entryDate = e->GetText();
                                             }
                                             else
                                             {
                                                std::string eval = e->Value();
                                                continue;
                                             }
   }
   contests[ c.name ] = c;
   return true;
}
bool Calendar::parseTimeList( TiXmlElement * tix, std::vector<TimeList> &timeList )
{
   TimeList t;
   for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
   {
      if ( checkElementName( e, "month_list" ) )
      {
         MonthList m;
         m.month = e->GetText();
         t.monthList.push_back( m );
      }
      else
         if ( checkElementName( e, "start_date_list" ) )
         {
            StartDateList sdl;
            sdl.date = e->GetText();
            t.startDateList.push_back( sdl );
         }
         else
            if ( checkElementName( e, "start_day" ) )
            {
               t.startDay = e->GetText();
            }
            else
               if ( checkElementName( e, "start_week" ) )
               {
                  t.startWeek = e->GetText();
               }
               else
                  if ( checkElementName( e, "start_time" ) )
                  {
                     t.startTime = e->GetText();
                  }
                  else
                     if ( checkElementName( e, "time_type" ) )
                     {
                        t.timeType = e->GetText();
                     }
                     else
                        if ( checkElementName( e, "duration" ) )
                        {
                           t.duration = e->GetText();
                        }
                        else
                           if ( checkElementName( e, "mult" ) )
                           {
                              t.mult = e->GetText();
                           }
                           else
                           {
                              std::string eval = e->Value();
                              continue;
                           }
   }
   timeList.push_back( t );
   return true;
}

