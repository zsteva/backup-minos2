/*====================================================================================
    This file is part of AdjQt, the QT based version of the RSGB
    contest adjudication software.
    
    AdjQt and its predecessor AdjSQL are Copyright 1992 - 2016 Mike Goodey G0GJV 
 
    AdjQt is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AdjQt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AdjQt in file gpl.txt.  If not, see <http://www.gnu.org/licenses/>.
    
======================================================================================*/

#include "base_pch.h"
#include "tinyxml.h"
#include "BandList.h"
#include "CalendarList.h"

const char *TypeVHFContest = "<VHF from VHFContests.xml>";
const char *TypeMwaveContest = "<Microwave from MicroContestsxx.xml>";

int calendarYear = 2000;
std::string trimr ( const std::string &s )
{
    int i;
    for ( i = s.length() - 1; i >= 0; i-- )
    {
        if ( !iscntrl ( ( unsigned int ) ( s[ i ] ) ) && !isspace ( ( unsigned int ) ( s[ i ] ) ) )
            break;
    }
    std::string s2;
    if ( i >= 0 )
        s2 = s.substr ( 0, i + 1 );
    return s2;
}
std::string trim ( const std::string &s )
{
    std::string s2 = trimr ( s );
    unsigned int i;
    for ( i = 0; i < s2.length(); i++ )
    {
        if ( !iscntrl ( ( unsigned int ) ( s[ i ] ) ) && !isspace ( ( unsigned int ) ( s[ i ] ) ) )
            break;
    }
    std::string s3 = s2.substr ( i, s2.length() );
    return s3;
}
std::string sstrupr ( const std::string &s )
{
    unsigned int i;
    std::string s2;
    for ( i = 0; i < s.length(); i++ )
    {
        s2 += toupper ( ( unsigned int ) ( s[ i ] ) );
    }
    //   s = s2;   // don't modify the input
    return s2;
}

std::map<std::string, std::string> contestNameMap;

std::string getTypeName ( const std::string &xmlName, CalType calType )
{

    std::map<std::string, std::string>::iterator ctype = contestNameMap.find( xmlName );
    if ( ctype != contestNameMap.end() )
    {
        return ctype->second;
    }

    if ( calType == ectVHF || calType == ectVHFOther )
    {
        return TypeVHFContest;
    }
    if ( calType == ectMwave )
    {
        return TypeMwaveContest;
    }

    return "Undefined";
}

std::string strupr ( const std::string &s )
{
    std::string s2;
    for ( unsigned int i = 0; i < s.length(); i++ )
    {
        s2 += toupper ( s[ i ] );
    }
    //s = s2;
    return s2;
}

static int curYear = 0;

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
void setYear ( int y )
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
int getMonth ( const std::string &m )
{
    std::string m2 = strupr ( m ).substr ( 0, 3 );
    for ( int i = 0; i < 12; i++ )
    {
        if ( m2 == monthTable[ i ] )
        {
            return i + 1;
        }
    }
    return 0;
}
int getDayOfWeek ( QDateTime dtg )
{
    int dw = dtg.date().dayOfWeek();    // Monday is 1, Sunday is 7
    return dw;
}
int getDate ( int month, int day, int week )
{
    // Day 1 is Monday

    // day 6 week 1 means "first Saturday of the month"
    // and day 2 week 1 means "first Tuesday of the month".
    // week 2 day 2 would mean "2nd Tuesday of the month".
    // day 6+1 is Sunday after Saturday

    // day 1+3 week 3 is  Thursday after 3rd Monday

    // Sept 2010 day 4+13 week 2 is 13 days after 2nd Thursday (Sep 22 2010)

    // return the date of the month/day/week


    QDateTime startMonth ( QDate( curYear, month, 1 ) );
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
Calendar::Calendar ( int year, CalType calType ) : calType ( calType )
{
    calendarYear = year;
}

Calendar::~Calendar()
{}

/*
The 9th EC Directive prescribes the start and end dates of summer time as the
last Sundays in March and October respectively. These dates are in line with
those already operating in the United Kingdom. The 9th Directive provides
that these start and end dates should apply indefinitely.
*/


QDateTime localToUTC( QDateTime t )
{
    unsigned short month = t.date().month();
    unsigned short day = t.date().day();

    bool isDst = false;

    if ( month > 3 && month < 10 )
    {
        isDst = true;
    }
    else
        if ( month == 3 )
        {
            int fSundayApril = getDate ( 4, 7, 1 );
            int ldM = fSundayApril + 31 - 7 ;
            if ( day >= ldM )
            {
                isDst = true;
            }
        }
        else
            if ( month == 10 )
            {
                int fSundayNovember = getDate ( 11, 7, 1 );
                int lsO = fSundayNovember + 31 - 7 ;
                if ( day < lsO )
                {
                    isDst = true;
                }
            }
    if ( isDst )
    {
        t = t.addSecs( -3600 );       // subtract 1 hour
    }
    return t;
}

bool Calendar::parseFile ( const QString &fname )
{
    QFile file( fname );

    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        return false;
    }

    QByteArray total = file.readAll();

    std::string buffer = QString( total ).toStdString();

    std::string buffer2;

    size_t dtdPos = buffer.find ( "<!DOCTYPE" );
    if ( dtdPos != std::string::npos )
    {
        buffer2 = buffer.substr ( 0, dtdPos );

        size_t dtdEndPos = buffer.find ( "]>" );
        if ( dtdEndPos == std::string::npos )
        {
            return false;
        }
        buffer2 += buffer.substr ( dtdEndPos + 2, buffer.size() - dtdEndPos - 2 );
    }

    TiXmlBase::SetCondenseWhiteSpace ( false );
    TiXmlDocument xdoc;
    const char *loaded = xdoc.Parse ( buffer2.c_str() );
    if ( !loaded )
    {
        return false;
    }
    TiXmlElement *tix = xdoc.RootElement();
    if ( !tix || !checkElementName ( tix, "calendar" ) )
    {
        return false;
    }
    for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
    {
        if ( checkElementName ( e, "version" ) )
        {
            if ( !parseVersion ( e ) )
            {
                return false;
            }
        }
        if ( checkElementName ( e, "mult_type" ) )
        {
            if ( !parseMultType ( e ) )
            {
                return false;
            }
        }
        else
            if ( checkElementName ( e, "special_rule" ) )
            {
                if ( !parseSpecialRule ( e ) )
                {
                    return false;
                }
            }
            else
                if ( checkElementName ( e, "section" ) )
                {
                    if ( !parseSection ( e ) )
                    {
                        return false;
                    }
                }
                else
                    if ( checkElementName ( e, "band" ) )
                    {
                        if ( !parseBand ( e ) )
                        {
                            return false;
                        }
                    }
                    else
                        if ( checkElementName ( e, "contest_series" ) )
                        {
                            if ( !parseContestSeries ( e ) )
                            {
                                return false;
                            }
                        }
                        else
                            if ( checkElementName ( e, "contest" ) )
                            {
                                if ( !parseContest ( e ) )
                                {
                                    return false;
                                }
                            }
                            else
                            {
                                std::string eval = e->Value();
                                //<!ELEMENT calendar (version, icalurl?, googleurl?, notes_list*, mult_type+, special_rule+, category*, section+, band+, contest_series*, contest+)>
                                continue;
                            }
    }
    // Now we have the raw info; we need to go through it and
    // generate all the individual contest details for logger/adjsql
    setYear ( calendarYear );
    for ( std::map<std::string, CalendarContest>::iterator i = contests.begin(); i != contests.end(); i++ )
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

                    CalendarContest cc = ( *i ).second;
                    TimeList tli = ( *tl );
                    MonthList mli = ( *ml );
                    CalendarBandList cbli = ( *bl );

                    int sm = getMonth ( ( *ml ).month );
                    if ( sm == 0 )
                    {
                        continue;
                    }



                    if ( ( *tl ).startDateList.size() )
                    {
                        // need to iterate the start dates
                        for ( unsigned int j = 0; j < ( *tl ).startDateList.size(); j++ )
                        {
                            int istartDate = atoi ( ( *tl ).startDateList[ j ].date.c_str() );
                            if ( istartDate == 0 )
                            {
                                continue;
                            }

                            IndividualContest ic;

                            std::string desc = trim ( ( *i ).second.description );
                            std::string sdesc = trim ( ( *i ).second.shortDescription );
                            std::string name = trim ( ( *i ).second.name );
                            std::string typeName = getTypeName ( name, calType );

                            // This needs changing once the contests are sorted
                            // as e.g. 70MHz cumulatives are defined in two groups
                            //if (desc[desc.size() - 1] == '#')
                            //{
                            //   desc += makeStr(instance++);
                            //}
                            ic.name = name;
                            ic.typeName = typeName;
                            ic.description = desc;
                            ic.shortDescription = sdesc;
                            ic.bands = ( *bl ).name;

                            ic.start = QDateTime ( QDate( curYear, sm, istartDate ) );
                            int h = atoi( ( *tl ).startTime.substr( 0, 2 ).c_str() );
                            int m = atoi( ( *tl ).startTime.substr( 2, 2 ).c_str() );

                            ic.start = ic.start.addSecs( h * 3600 + m * 60 );

                            ic.duration = ( *tl ).duration;
                            double dur = atof ( ic.duration.c_str() );
                            double durh = ( int ) ( dur / 24 );
                            double durm = ( dur - durh ) * 60;
                            ic.finish = ic.start.addSecs( ( int ) durh * 3600 + ( int ) durm * 60 );

                            std::string timeType = ( *tl ).timeType;
                            if ( timeType == "local" )
                            {
                                ic.start = localToUTC ( ic.start );
                                ic.finish = localToUTC ( ic.finish );
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
                                        if ( sstrupr ( ( *ml ).month ) == sstrupr ( monthTable[ sm - 1 ] ) )
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
                            ic.power = ( *i ).second.power;

                            ic.reg1band = bands[ ic.bands ].reg1band.c_str();
                            if ( ic.reg1band == "1,2 GHz" )
                            {
                                ic.reg1band = "1,3 GHz";
                            }

                            calendar.push_back ( ic );
                        }
                    }
                    else
                    {

                        // NB - band list overrides time list!

                        std::string startWeek;
                        std::string startDay;
                        std::string startTime;
                        std::string timeType;
                        std::string duration;

                        CalendarBand &blst = bands[ ( *bl ).name ];
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


                        std::string startday1 = startDay;
                        std::string startday2;
                        int istartday1, istartday2;

                        size_t ppos = startDay.find ( "+" );
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
                            continue;
                        }

                        int istartWeek = atoi ( startWeek.c_str() );
                        if ( istartWeek == 0 )
                        {
                            continue;
                        }


                        //               std::string bstartWeek = (*bl).startWeek; // or iterate its timeList
                        int istartDate = getDate ( sm, istartday1, istartWeek );
                        if ( istartDate == 0 )
                        {
                            continue;
                        }

                        istartDate += istartday2;

                        IndividualContest ic;

                        std::string desc = trim ( ( *i ).second.description );
                        std::string name = trim ( ( *i ).second.name );
                        std::string typeName = getTypeName ( name, calType );

                        // This needs changing once the contests are sorted
                        // as e.g. 70MHz cumulatives are defined in two groups
                        //if (desc[desc.size() - 1] == '#')
                        //{
                        //   desc += makeStr(instance++);
                        //}

                        ic.name = name;
                        ic.typeName = typeName;
                        ic.description = desc;
                        ic.bands = ( *bl ).name;

                        ic.start = QDateTime ( QDate( curYear, sm, istartDate ) );
                        int h = atoi( startTime.substr( 0, 2 ).c_str() );
                        int m = atoi( startTime.substr( 2, 2 ).c_str() );

                        ic.start = ic.start.addSecs( h * 3600 + m * 60 );

                        ic.duration = duration;
                        double dur = atof ( ic.duration.c_str() );
                        double durh = ( int ) ( dur / 24 );
                        double durm = ( dur - durh ) * 60;
                        ic.finish = ic.start.addSecs( ( int ) durh * 3600 + ( int ) durm * 60 );

                        std::string timeType1 = timeType;
                        if ( timeType1 == "local" )
                        {
                            ic.start = localToUTC ( ic.start );
                            ic.finish = localToUTC ( ic.finish );
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
                                    if ( sstrupr ( ( *ml ).month ) == sstrupr ( monthTable[ sm - 1 ] ) )
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
                        ic.power = ( *i ).second.power;

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
    std::sort ( calendar.begin(), calendar.end() );
    return true;

}
bool IndividualContest::operator< ( const IndividualContest& rhs ) const
{
    // p1 is from list; p2 is the one being searched for

    if ( start != rhs.start )
    {
        return start < rhs.start;
    }
    if ( description != rhs.description )
    {
        return description < rhs.description;
    }
    return bands < rhs.bands;
}
bool Calendar::parseVersion ( TiXmlElement * tix )
{
    version = tix->GetText();
    return true;
}
bool Calendar::parseMultType ( TiXmlElement * tix )
{
    MultType mt;
    for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
    {
        if ( checkElementName ( e, "name" ) )
        {
            mt.name = e->GetText();
        }
        else
            if ( checkElementName ( e, "description" ) )
            {
                mt.description = e->GetText();
            }
            else
                if ( checkElementName ( e, "long_description" ) )
                {
                    mt.longDescription = e->GetText();
                }
                else
                    if ( checkElementName ( e, "exchange" ) )
                    {
                        mt.exchange = e->GetText();
                    }
                    else
                        if ( checkElementName ( e, "scoring_description" ) )
                        {
                            mt.scoringDescription = e->GetText();
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
bool Calendar::parseSpecialRule ( TiXmlElement * tix )
{
    SpecialRule sr;
    for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
    {
        if ( checkElementName ( e, "name" ) )
        {
            sr.name = e->GetText();
        }
        else
            if ( checkElementName ( e, "description" ) )
            {
                sr.description = e->GetText();
            }
            else
                if ( checkElementName ( e, "long_description" ) )
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
bool Calendar::parseSection ( TiXmlElement * tix )
{
    CalendarSection s;
    for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
    {
        if ( checkElementName ( e, "name" ) )
        {
            s.name = e->GetText();
        }
        else
            if ( checkElementName ( e, "description" ) )
            {
                s.description = e->GetText();
            }
            else
                if ( checkElementName ( e, "long_description" ) )
                {
                    s.longDescription = e->GetText();
                }
                else
                    if ( checkElementName ( e, "power" ) )
                    {
                        s.power = e->GetText();
                    }
                    else
                        if ( checkElementName ( e, "height" ) )
                        {
                            s.height = e->GetText();
                        }
                        else
                            if ( checkElementName ( e, "singleantenna" ) )
                            {
                                s.singleAntenna = ( std::string(e->GetText()) == "1" );
                            }
                            else
                                if ( checkElementName ( e, "overall_section" ) )
                                {
                                    s.overall = true;
                                }
                                else
                                    if ( checkElementName ( e, "month_list" ) )
                                    {
                                        MonthList m;
                                        m.month = e->GetText();
                                        s.monthList.push_back ( m );
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
bool Calendar::parseBand ( TiXmlElement * tix )
{
    CalendarBand b;
    for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
    {
        if ( checkElementName ( e, "name" ) )
        {
            b.name = e->GetText();
        }
        else
            if ( checkElementName ( e, "reg1band" ) )
            {
                b.reg1band = e->GetText();
                if ( b.reg1band == "1,2 GHz" )
                {
                    b.reg1band = "1,3 GHz";
                }
            }
            else
                if ( checkElementName ( e, "wavelength" ) )
                {
                    b.wavelength = e->GetText();
                }
                else
                    if ( checkElementName ( e, "time_list" ) )
                    {
                        if ( ! ( parseTimeList ( e, b.timeList ) ) )
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
bool Calendar::parseContestSeries ( TiXmlElement * tix )
{
    ContestSeries cs;
    for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
    {
        if ( checkElementName ( e, "name" ) )
        {
            cs.name = e->GetText();
        }
        else
            if ( checkElementName ( e, "description" ) )
            {
                cs.description = e->GetText();
            }
            else
                if ( checkElementName ( e, "contest_list" ) )
                {
                    CalendarContestList cl;
                    cl.name = e->GetText();
                    cs.contestList.push_back ( cl );
                }
                else
                    if ( checkElementName ( e, "award_list" ) )
                    {
                        std::string al;
                        al = e->GetText();
                        cs.awardList.push_back ( al );
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
bool Calendar::parseContest ( TiXmlElement * tix )
{
    CalendarContest c;
    for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
    {
        if ( checkElementName ( e, "name" ) )
        {
            c.name = e->GetText();
        }
        else
            if ( checkElementName ( e, "description" ) )
            {
                c.description = e->GetText();
            }
            else
                if ( checkElementName ( e, "short_description" ) )
                {
                    c.shortDescription = e->GetText();
                }
                else
                    if ( checkElementName ( e, "time_list" ) )
                    {
                        if ( ! ( parseTimeList ( e, c.timeList ) ) )
                            return false;
                    }
                    else
                        if ( checkElementName ( e, "section_list" ) )
                        {
                            CalendarSectionList sl;
                            sl.name = e->GetText();
                            c.sectionList.push_back ( sl );
                        }
                        else
                            if ( checkElementName ( e, "category_list" ) )
                            {
                            }
                            else
                                if ( checkElementName ( e, "cabrillo_name" ) )
                                {
                                }
                                else
                                    if ( checkElementName ( e, "rst_type" ) )
                                    {
                                    }
                                    else
                                        if ( checkElementName ( e, "entry_email" ) )
                                        {
                                        }
                                        else
                                            if ( checkElementName ( e, "sprint" ) )
                                            {
                                            }
                                            else
                                                if ( checkElementName ( e, "special_rules_list" ) )
                                                {
                                                    SpecialRuleList srl;
                                                    srl.name = e->GetText();
                                                    c.specialRulesList.push_back ( srl );
                                                }
                                                else
                                                    if ( checkElementName ( e, "band_list" ) )
                                                    {
                                                        CalendarBandList bl;
                                                        bl.name = e->GetText();
                                                        c.bandList.push_back ( bl );
                                                    }
                                                    else
                                                        if ( checkElementName ( e, "award_list" ) )
                                                        {
                                                            std::string al;
                                                            al = e->GetText();
                                                            c.awardList.push_back ( al );
                                                        }
                                                        else
                                                            if ( checkElementName ( e, "iaru" ) )
                                                            {
                                                                c.iaru = ( std::string(e->GetText()) == "1" );
                                                            }
                                                            else
                                                                if ( checkElementName ( e, "scoring" ) )
                                                                {
                                                                    for ( TiXmlElement * f = e->FirstChildElement(); f; f = f->NextSiblingElement() )
                                                                    {
                                                                        if ( checkElementName ( f, "perkms" ) )
                                                                        {
                                                                            c.scoring = CalendarContest::perkms;
                                                                        }
                                                                        else
                                                                            if ( checkElementName ( f, "oneppq" ) )
                                                                            {
                                                                                c.scoring = CalendarContest::oneppq;
                                                                            }
                                                                            else
                                                                                if ( checkElementName ( f, "desc" ) )
                                                                                {
                                                                                    c.scoring = CalendarContest::desc;
                                                                                }
                                                                                else
                                                                                    return false;
                                                                        break;
                                                                    }
                                                                }
                                                                else
                                                                    if ( checkElementName ( e, "mult" ) )
                                                                    {
                                                                        c.mult = e->GetText();
                                                                    }
                                                                    else
                                                                        if ( checkElementName ( e, "power" ) )
                                                                        {
                                                                            c.power = e->GetText();
                                                                        }
                                                                        else
                                                                            if ( checkElementName ( e, "mode" ) )
                                                                            {
                                                                                c.mode = e->GetText();
                                                                            }
                                                                            else
                                                                                if ( checkElementName ( e, "entry_date" ) )
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
bool Calendar::parseTimeList ( TiXmlElement * tix, std::vector<TimeList> &timeList )
{
    TimeList t;
    for ( TiXmlElement * e = tix->FirstChildElement(); e; e = e->NextSiblingElement() )
    {
        if ( checkElementName ( e, "month_list" ) )
        {
            MonthList m;
            m.month = e->GetText();
            t.monthList.push_back ( m );
        }
        else
            if ( checkElementName ( e, "start_date_list" ) )
            {
                StartDateList sdl;
                sdl.date = e->GetText();
                t.startDateList.push_back ( sdl );
            }
            else
                if ( checkElementName ( e, "start_day" ) )
                {
                    t.startDay = e->GetText();
                }
                else
                    if ( checkElementName ( e, "start_week" ) )
                    {
                        t.startWeek = e->GetText();
                    }
                    else
                        if ( checkElementName ( e, "start_time" ) )
                        {
                            t.startTime = e->GetText();
                        }
                        else
                            if ( checkElementName ( e, "time_type" ) )
                            {
                                t.timeType = e->GetText();
                            }
                            else
                                if ( checkElementName ( e, "duration" ) )
                                {
                                    t.duration = e->GetText();
                                }
                                else
                                    if ( checkElementName ( e, "mult" ) )
                                    {
                                        t.mult = e->GetText();
                                    }
                                    else
                                    {
                                        std::string eval = e->Value();
                                        continue;
                                    }
    }
    timeList.push_back ( t );
    return true;
}
