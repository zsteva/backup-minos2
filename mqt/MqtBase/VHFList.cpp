/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2007 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

#include "VHFList.h"
/*
QString strupr( const QString &s )
{
   QString s2;
   for ( unsigned int i = 0; i < s.length(); i++ )
   {
      s2 += toupper( s[ i ] );
   }
   //s = s2;
   return s2;
}
*/
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
int getMonth( const QString &m )
{
   QString m2 = strupr( m ).left( 3 );
   for ( int i = 0; i < 12; i++ )
   {
      if ( m2 == monthTable[ i ] )
      {
         return i + 1;
      }
   }
   return 0;
}
int getDayOfWeek(QDateTime dtg )
{
   int dw = dtg.date().dayOfWeek();    // Monday is 1, Sunday is 7
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


   QDateTime startMonth( QDate(curYear, month, 1 ));
   int dw = getDayOfWeek ( startMonth ) - 1;   // make it 0 based

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

QDateTime localToUTC(QDateTime t )
{
    return t.toUTC();
}
bool Calendar::parseFile( const QString &fname )
{
    QFile file( fname );

    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        return false;
    }

    QByteArray total = file.readAll();

    QString buffer = QString( total );

    QString buffer2;

    int dtdPos = buffer.indexOf ( "<!DOCTYPE" );
    if ( dtdPos != -1 )
    {
        buffer2 = buffer.left(dtdPos );

        int dtdEndPos = buffer.indexOf ( "]>" );
        if ( dtdEndPos == -1 )
        {
            return false;
        }
        buffer2 += buffer.mid ( dtdEndPos + 2, buffer.size() - dtdEndPos - 2 );
    }

   TiXmlBase::SetCondenseWhiteSpace( false );
   TiXmlDocument xdoc;
   const char *loaded = xdoc.Parse( buffer2.toStdString().c_str() );
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
                           QString eval = e->Value();
                           continue;
                        }
   }
   // Now we have the raw info; we need to go through it and
   // generate all the individual contest details for logger/adjsql
   setYear( calendarYear );
   for ( std::map<QString, Contest>::iterator i = contests.begin(); i != contests.end(); i++ )
   {
      // For each contest iterate the time list and the band list
      for ( std::vector<TimeList>::iterator tl = ( *i ).second.timeList.begin(); tl != ( *i ).second.timeList.end(); tl++ )
      {
         for ( std::vector<MonthList>::iterator ml = ( *tl ).monthList.begin(); ml != ( *tl ).monthList.end(); ml++ )
         {
            for ( std::vector<CalendarBandList>::iterator bl = ( *i ).second.bandList.begin(); bl != ( *i ).second.bandList.end(); bl++ )
            {
               // startDate needs to incorporate the week and day

               // day 6 week 1 means "first Saturday of the month"
               // and day 2 week 1 means "first Tuesday of the month".
               // week 2 day 2 would mean "2nd Tuesday of the month".
               // day 6+1 is Sunday after Saturday

               // XMAS cumulatives also have <start_date_list>, but they appear to be the only ones

               // Is it best to generate a table of 365(6) days, and assign each one month/week/day

               // And we may need month pointers, so we cna then count days from each

               // Somewhere, we need to evaluate the year... for now, we preset it

               int sm = getMonth( ( *ml ).month );
               if ( sm == 0 )
               {
                  continue;
               }



               if ( ( *tl ).startDateList.size() )
               {
                  // need to iterate the start dates
                  for ( unsigned int j = 0; j < ( *tl ).startDateList.size(); j++ )
                  {
                     int istartDate = ( *tl ).startDateList[ j ].date.toInt();
                     if ( istartDate == 0 )
                     {
                        continue;
                     }

                     IndividualContest ic;

                     QString desc = ( *i ).second.description.trimmed();
                     QString sdesc = ( *i ).second.shortDescription.trimmed();
                     /*
                     // This needs changing once the contests are sorted
                     // as e.g. 70MHz cumulatives are defined in two groups
                     if (desc[desc.size() - 1] == '#')
                     {
                        desc += makeStr(instance++);
                     }
                     */
                     ic.description = desc;
                     ic.shortDescription = sdesc;
                     ic.bands = ( *bl ).name;

                    ic.start = QDateTime ( QDate( curYear, sm, istartDate ) );
                    int s = ( *tl ).startTime.left(2).toInt() * 60 + ( *tl ).startTime.mid(2, 2).toInt();
                    ic.start = ic.start.addSecs( s );

                    ic.duration = ( *tl ).duration;
                    double dur = ic.duration.toDouble();
                    double durh = ( int ) ( dur / 24 );
                    double durm = ( dur - durh ) * 60;
                    ic.finish = ic.start.addSecs( ( int ) durh * 3600 + ( int ) durm * 60 );

                    QString timeType = ( *tl ).timeType;
                    if ( timeType == "local" )
                    {
                       ic.start = localToUTC( ic.start );
                       ic.finish = localToUTC( ic.finish );
                    }

                    ic.ppKmScoring = ( ( *i ).second.scoring == Contest::perkms );
                     for ( unsigned int j = 0; j < ( *i ).second.sectionList.size(); j++ )
                     {
                        QString n = ( *i ).second.sectionList[ j ].name;
                        std::map<QString, Section>::iterator s = sections.find(n);

                        if (s == sections.end())
                        {
                           continue;
                        }
                        int mls = (*s).second.monthList.size();
                        if (mls)
                        {
                           bool monthOK = false;
                           for (std::vector<MonthList>::iterator ml = (*s).second.monthList.begin(); ml != (*s).second.monthList.end(); ml++)
                           {
                              if (strupr((*ml).month) == strupr(monthTable[sm - 1]) )
                              {
                                 monthOK = true;
                                 break;
                              }
                           }
                           if (!monthOK)
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
                     // check if the section has a month_list

                     ic.sections = ic.sections.left( ic.sections.size() - 1 );   // lose any trailing comma
                     ic.mults = ( *i ).second.mult;
                     for ( unsigned int j = 0; j < ( *i ).second.specialRulesList.size(); j++ )
                     {
                        ic.specialRules += ( *i ).second.specialRulesList[ j ].name + " ";
                     }
                     ic.power = ( *i ).second.power;

                     ic.reg1band = bands[ ic.bands ].reg1band;
                     if (ic.reg1band == "1,2 GHz")
                     {
                        ic.reg1band = "1,3 GHz";
                     }

                     calendar.push_back( ic );
                  }
               }
               else
               {

                  // NB - band list overrides time list!
                  QString startWeek;
                  QString startDay;
                  QString startTime;
                  QString timeType;
                  QString duration;

                  ContestBand &blst = bands[ ( *bl ).name ];
                  if ( blst.timeList.size() )
                  {
                     TimeList & b = blst.timeList[ 0 ];

                     startWeek = b.startWeek;
                     startDay = b.startDay;
                     startTime = b.startTime;
                     duration = b.duration;
                     timeType = b.timeType;
                  }
                  else
                  {
                     startWeek = ( *tl ).startWeek;
                     startDay = ( *tl ).startDay;
                     startTime = ( *tl ).startTime;
                     duration = ( *tl ).duration;
                     timeType = ( *tl ).timeType;
                  }

                  QString startday1 = startDay;
                  QString startday2;
                  int istartday1, istartday2;

                  int ppos = startDay.indexOf("+");
                  if (ppos != -1)
                  {
                     startday1 = startDay.left(ppos);
                     startday2 = startDay.mid(ppos + 1, startDay.size());
                     istartday1 = startday1.toInt();
                     istartday2 = startday2.toInt();
                  }
                  else
                  {
                     istartday1 = startday1.toInt();
                     istartday2 = 0;
                  }
                  if (istartday1 == 0)
                  {
                     continue;
                  }

                  int istartWeek = startWeek.toInt();
                  if ( istartWeek == 0 )
                  {
                     continue;
                  }


                  //               QString bstartWeek = (*bl).startWeek; // or iterate its timeList
                  int istartDate = getDate( sm, istartday1, istartWeek );
                  if ( istartDate == 0 )
                  {
                     continue;
                  }

                  istartDate += istartday2;

                  IndividualContest ic;

                  QString desc = ( *i ).second.description.trimmed();
                  /*
                  // This needs changing once the contests are sorted
                  // as e.g. 70MHz cumulatives are defined in two groups
                  if (desc[desc.size() - 1] == '#')
                  {
                     desc += makeStr(instance++);
                  }
                  */
                  ic.description = desc;
                  ic.bands = ( *bl ).name;

                 ic.start = QDateTime( QDate(curYear, sm, istartDate ));

                 int s = startTime.left(2).toInt() * 60 + startTime.mid(2, 2).toInt();
                 ic.start.addSecs(s * 60);

                 QString temp2 = ic.start.toString( "dd/MM/yyyy hh:mm" );

                 ic.duration = ( *tl ).duration;
                 double dur =  ic.duration.toDouble();
                 double durh = ( int ) ( dur / 24 );
                 double durm = ( dur - durh ) * 60;
                 ic.finish = ic.start.addSecs( ( int ) durh * 3600 + ( int ) durm * 60 );

                 if ( timeType == "local" )
                 {
                    ic.start = localToUTC( ic.start );
                    ic.finish = localToUTC( ic.finish );
                 }

                  ic.ppKmScoring = ( ( *i ).second.scoring == Contest::perkms );
                  for ( unsigned int j = 0; j < ( *i ).second.sectionList.size(); j++ )
                  {
                     QString n = ( *i ).second.sectionList[ j ].name;
                     std::map<QString, Section>::iterator s = sections.find(n);

                     if (s == sections.end())
                     {
                        continue;
                     }
                     int mls = (*s).second.monthList.size();
                     if (mls)
                     {
                        bool monthOK = false;
                        for (std::vector<MonthList>::iterator ml = (*s).second.monthList.begin(); ml != (*s).second.monthList.end(); ml++)
                        {
                           if (strupr((*ml).month) == strupr(monthTable[sm - 1]))
                           {
                              monthOK = true;
                              break;
                           }
                        }
                        if (!monthOK)
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
                  ic.sections = ic.sections.left( ic.sections.size() - 1 );   // lose any trailing comma
                  ic.mults = ( *i ).second.mult;
                  for ( unsigned int j = 0; j < ( *i ).second.specialRulesList.size(); j++ )
                  {
                     ic.specialRules += ( *i ).second.specialRulesList[ j ].name + " ";
                  }
                  ic.power = ( *i ).second.power;

                  ic.reg1band = bands[ ic.bands ].reg1band;
                  if (ic.reg1band == "1,2 GHz")
                  {
                     ic.reg1band = "1,3 GHz";
                  }

                  calendar.push_back( ic );
               }
            }
         }
      }
   }
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
                  {
                     QString eval = e->Value();
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
               QString eval = e->Value();
               continue;
            }
   }
   specialRules[ sr.name ] = sr;
   return true;
}
bool Calendar::parseSection( TiXmlElement * tix )
{
   Section s;
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
                  s.power = e->GetText();
               }
               else
                  if ( checkElementName( e, "height" ) )
                  {
                     s.height = e->GetText();
                  }
                  else
                     if ( checkElementName( e, "singleantenna" ) )
                     {
                        s.singleAntenna = ( QString(e->GetText()) == "1" );
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
                                 QString eval = e->Value();
                                 continue;
                              }
   }
   sections[ s.name ] = s;
   return true;
}
bool Calendar::parseBand( TiXmlElement * tix )
{
   ContestBand b;
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
               b.wavelength = e->GetText();
            }
            else
               if ( checkElementName( e, "time_list" ) )
               {
                  if ( !( parseTimeList( e, b.timeList ) ) )
                     return false;
               }
               else
               {
                  QString eval = e->Value();
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
                  QString al;
                  al = e->GetText();
                  cs.awardList.push_back( al );
               }
               else
               {
                  QString eval = e->Value();
                  continue;
               }
   }
   contestSeries[ cs.name ] = cs;
   return true;
}
bool Calendar::parseContest( TiXmlElement * tix )
{
   Contest c;
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
                              c.iaru = ( QString(e->GetText()) == "1" );
                           }
                           else
                              if ( checkElementName( e, "scoring" ) )
                              {
                                 for ( TiXmlElement * f = e->FirstChildElement(); f; f = f->NextSiblingElement() )
                                 {
                                    if ( checkElementName( f, "perkms" ) )
                                    {
                                       c.scoring = Contest::perkms;
                                    }
                                    else
                                       if ( checkElementName( f, "oneppq" ) )
                                       {
                                          c.scoring = Contest::oneppq;
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
                                       c.power = e->GetText();
                                    }
                                    else
                                       if ( checkElementName( e, "mode" ) )
                                       {
                                          c.mode = e->GetText();
                                       }
                                       else
                                          if ( checkElementName( e, "award_list" ) )
                                          {
                                             QString al = e->GetText();
                                          }
                                          else
                                             if ( checkElementName( e, "entry_date" ) )
                                             {
                                                c.entryDate = e->GetText();
                                             }
                                             else
                                             {
                                                QString eval = e->Value();
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
                              QString eval = e->Value();
                              continue;
                           }
   }
   timeList.push_back( t );
   return true;
}

