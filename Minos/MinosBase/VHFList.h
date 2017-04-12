/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2007 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#ifndef VHFListH
#define VHFListH 
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
//---------------------------------------------------------------------------
enum CalType {ectVHF, ectHF, ectMwave, ectVHFOther, ectHFOther};
extern String calString[];
#define LOWYEAR -2
#define LOWURLYEAR -1
#define HIGHYEAR 1

extern int curYear;

class CalendarYear
{
      virtual String getSite() = 0;
      String yearString()
      {
         String y = String(curYear + yearOffset);
         y = y.SubString(3, 2);
         return y;
      }
   public:
      CalendarYear(CalType t, int y): type(t), yearOffset(y)
      {
         if (curYear == 0)
         {
            SYSTEMTIME st;
            GetLocalTime( &st );
            curYear = st.wYear;
         }

      }
      virtual bool downloadFile(TIdHTTP *IdHTTP1, bool showError);

      bool loaded;
      int yearOffset;
      CalType type;
      virtual String getPath();
      virtual String getURL();
//      virtual String getProvisionalURL();
};
class VHFCalendarYear : public CalendarYear
{
      virtual String getSite();
   public:
      VHFCalendarYear(int year): CalendarYear(ectVHF, year)
      {

      }
};
class CTYCalendarYear : public CalendarYear
{
      virtual String getSite();
   public:
      CTYCalendarYear ( ) : CalendarYear ( ectVHFOther, 0 )
      {

      }
      virtual String getPath();
      virtual String getURL();
};
class LocSquaresCalendarYear : public CalendarYear
{
      virtual String getSite();
   public:
      LocSquaresCalendarYear ( ) : CalendarYear ( ectVHFOther, 0 )
      {

      }
      virtual String getPath();
      virtual String getURL();
};

class MultType
{
   public:
      std::string name;
      std::string description;
      std::string longDescription;
      std::string scoringDescription;
      std::string exchange;

      std::map<std::string, int> bonuses;
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
      CalendarSection() : singleAntenna( false ), overall( false )
      {}
      std::vector<MonthList> monthList;

      std::string name;
      std::string description;
      std::string longDescription;
      //std::string power;
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
class ContestList
{
   public:
      std::string name;

};
class ContestSeries
{
   public:
      std::string name;
      std::string description;
      std::vector<ContestList>contestList;
      std::vector<std::string>awardList;
};
class SectionList
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
      CalendarContest() : scoring( perkms ), iaru( false )
      {}

      std::string name;
      std::string description;
      std::string shortDescription;
      enum ScoreType{oneppq, perkms} scoring;
      bool iaru;
      std::string mult;
      //std::string power;
      std::string mode;
      std::string entryDate;

      std::vector<TimeList> timeList;
      std::vector<CalendarBandList> bandList;
      std::vector<SectionList> sectionList;
      std::vector<SpecialRuleList> specialRulesList;
      std::vector<std::string> awardList;

};
class IndividualContest
{
   public:
      TDateTime start;
      TDateTime finish;
      std::string duration;

      std::string description;
      std::string shortDescription;
      std::string bands;
      std::string reg1band;
      bool ppKmScoring;
      std::string sections;
      std::string mults;
      std::string specialRules;
      //std::string power;
      std::string mode;
      std::string antenna;

      bool operator<( const IndividualContest& rhs ) const;
};
class TiXmlElement;
class Calendar
{
   public:
      Calendar(int year);
      ~Calendar();
      bool parseFile( const std::string &fname );
      bool parseMultType( TiXmlElement * e );
      bool parseSpecialRule( TiXmlElement * e );
      bool parseSection( TiXmlElement * e );
      bool parseBand( TiXmlElement * e );
      bool parseContestSeries( TiXmlElement * e );
      bool parseContest( TiXmlElement * e );

      bool parseTimeList( TiXmlElement * e, std::vector<TimeList> &timeList );

      std::string version;
      std::map<std::string, MultType> mults;
      std::map<std::string, SpecialRule> specialRules;
	  std::map<std::string, CalendarSection> sections;
      std::map<std::string, CalendarBand> bands;
      std::map<std::string, ContestSeries> contestSeries;
      std::map<std::string, CalendarContest> contests;

      std::vector<IndividualContest> calendar;
};
extern TDateTime localToUTC( TDateTime t );
extern void setYear( int y );
extern int getMonth( const std::string &m );
extern int getDate( int month, int day, int week );
extern int getDayOfWeek( TDateTime dtg );

#endif
