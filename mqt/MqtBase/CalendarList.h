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

#ifndef CalendarListH
#define CalendarListH 
//---------------------------------------------------------------------------
enum CalType {ectVHF, ectHF, ectMwave, ectVHFOther, ectHFOther, ectHFBARTG};
//extern std::map<std::string, std::string> contestNameMap;
int getDayOfWeek ( QDateTime dtg );

class MultType
{
    public:
        std::string name;
        std::string description;
        std::string longDescription;
        std::string scoringDescription;
        std::string exchange;
};
class SpecialRule
{
    public:
        std::string name;
        std::string description;
        std::string longDescription;
};
class MonthList
{
    public:
        std::string month;
};
class CalendarSection
{
    public:
        CalendarSection() : singleAntenna ( false ), overall ( false )
        {}
        std::vector<MonthList> monthList;

        std::string name;
        std::string description;
        std::string longDescription;
        std::string power;
        std::string height;
        bool singleAntenna;
        bool overall;
};
class StartDateList
{
    public:
        std::string date;

};
class TimeList
{
    public:
        std::vector<MonthList> monthList;
        std::vector<StartDateList> startDateList;
        std::string startDay;
        std::string startWeek;
        std::string startTime;
        std::string timeType;
        std::string duration;
        std::string mult;
};
class CalendarBand
{
    public:
        std::string name;
        std::string reg1band;
        std::string wavelength;
        std::vector<TimeList> timeList;
};
class CalendarContestList
{
    public:
        std::string name;

};
class ContestSeries
{
    public:
        std::string name;
        std::string description;
        std::vector<CalendarContestList>contestList;
        std::vector<std::string>awardList;
};
class CalendarSectionList
{
    public:
        std::string name;

};
class CalendarBandList
{
    public:
        std::string name;

};
class SpecialRuleList
{
    public:
        std::string name;

};
class CalendarContest
{
    public:
        CalendarContest() : scoring ( perkms ), iaru ( false )
        {}

        std::string name;
        std::string description;
        std::string shortDescription;
        enum ScoreType{oneppq, perkms, desc} scoring;
        bool iaru;
        std::string mult;
        std::string power;
        std::string mode;
        std::string entryDate;

        std::vector<TimeList> timeList;
        std::vector<CalendarBandList> bandList;
        std::vector<CalendarSectionList> sectionList;
        std::vector<SpecialRuleList> specialRulesList;
        std::vector<std::string> awardList;

};
class IndividualContest
{
    public:
        QDateTime start;
        QDateTime finish;
        std::string duration;

        std::string typeName;
        std::string name;
        std::string description;
        std::string shortDescription;
        std::string bands;
        std::string reg1band;
        bool ppKmScoring;
        std::string sections;
        std::string mults;
        std::string specialRules;
        std::string power;
        std::string antenna;

        bool operator< ( const IndividualContest& rhs ) const;
};
class TiXmlElement;
class Calendar
{
    public:
        Calendar ( int year, CalType calType );
        ~Calendar();
        CalType calType;
        bool parseFile (const QString &fname );
        bool parseVersion ( TiXmlElement * e );
        bool parseMultType ( TiXmlElement * e );
        bool parseSpecialRule ( TiXmlElement * e );
        bool parseSection ( TiXmlElement * e );
        bool parseBand ( TiXmlElement * e );
        bool parseContestSeries ( TiXmlElement * e );
        bool parseContest ( TiXmlElement * e );

        bool parseTimeList ( TiXmlElement * e, std::vector<TimeList> &timeList );

        std::string version;
        std::map<std::string, MultType> mults;
        std::map<std::string, SpecialRule> specialRules;
        std::map<std::string, CalendarSection> sections;
        std::map<std::string, CalendarBand> bands;
        std::map<std::string, ContestSeries> contestSeries;
        std::map<std::string, CalendarContest> contests;

        std::vector<IndividualContest> calendar;
};
extern QDateTime localToUTC ( QDateTime t );
extern void setYear ( int y );
extern int getMonth ( const std::string &m );
extern int getDate ( int month, int day, int week );

#endif
