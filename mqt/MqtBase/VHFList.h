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
//---------------------------------------------------------------------------
class MultType
{
   public:
      QString name;
      QString description;
      QString longDescription;
      QString scoringDescription;
      QString exchange;
};
class SpecialRule
{
   public:
      QString name;
      QString description;
      QString longDescription;
};
class MonthList
{
   public:
      QString month;
};
class Section
{
   public:
      Section() : singleAntenna( false ), overall( false )
      {}
      std::vector<MonthList> monthList;

      QString name;
      QString description;
      QString longDescription;
      QString power;
      QString height;
      bool singleAntenna;
      bool overall;
};
class StartDateList
{
   public:
      QString date;

};
class TimeList
{
   public:
      std::vector<MonthList> monthList;
      std::vector<StartDateList> startDateList;
      QString startDay;
      QString startWeek;
      QString startTime;
      QString timeType;
      QString duration;
      QString mult;
};
class ContestBand
{
   public:
      QString name;
      QString reg1band;
      QString wavelength;
      std::vector<TimeList> timeList;
};
class ContestList
{
   public:
      QString name;

};
class ContestSeries
{
   public:
      QString name;
      QString description;
      std::vector<ContestList>contestList;
      std::vector<QString>awardList;
};
class SectionList
{
   public:
      QString name;

};
class CalendarBandList
{
   public:
      QString name;

};
class SpecialRuleList
{
   public:
      QString name;

};
class Contest
{
   public:
      Contest() : scoring( perkms ), iaru( false )
      {}

      QString name;
      QString description;
      QString shortDescription;
      enum ScoreType{oneppq, perkms} scoring;
      bool iaru;
      QString mult;
      QString power;
      QString mode;
      QString entryDate;

      std::vector<TimeList> timeList;
      std::vector<CalendarBandList> bandList;
      std::vector<SectionList> sectionList;
      std::vector<SpecialRuleList> specialRulesList;
      std::vector<QString> awardList;

};
class IndividualContest
{
   public:
      QDateTime start;
      QDateTime finish;
      QString duration;

      QString description;
      QString shortDescription;
      QString bands;
      QString reg1band;
      bool ppKmScoring;
      QString sections;
      QString mults;
      QString specialRules;
      QString power;
      QString antenna;

      bool operator<( const IndividualContest& rhs ) const;
};
class TiXmlElement;
class Calendar
{
   public:
      Calendar(int year);
      ~Calendar();
      bool parseFile(const QString &fname );
      bool parseMultType( TiXmlElement * e );
      bool parseSpecialRule( TiXmlElement * e );
      bool parseSection( TiXmlElement * e );
      bool parseBand( TiXmlElement * e );
      bool parseContestSeries( TiXmlElement * e );
      bool parseContest( TiXmlElement * e );

      bool parseTimeList( TiXmlElement * e, std::vector<TimeList> &timeList );

      QString version;
      std::map<QString, MultType> mults;
      std::map<QString, SpecialRule> specialRules;
      std::map<QString, Section> sections;
      std::map<QString, ContestBand> bands;
      std::map<QString, ContestSeries> contestSeries;
      std::map<QString, Contest> contests;

      std::vector<IndividualContest> calendar;
};
extern QDateTime localToUTC( QDateTime t );
extern void setYear( int y );
extern int getMonth( const QString &m );
extern int getDate( int month, int day, int week );
extern int getDayOfWeek( QDateTime dtg );

#endif
