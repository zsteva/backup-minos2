#ifndef CalendarListH
#define CalendarListH 
//---------------------------------------------------------------------------
enum CalType {ectVHF, ectHF, ectMwave, ectVHFOther, ectHFOther, ectHFBARTG};
int getDayOfWeek ( QDateTime dtg );
extern int curYear;

#define LOWYEAR -1
#define LOWURLYEAR -1
#define HIGHYEAR 1

extern QString calendarNameString[];

class CalendarYear
{
        virtual QString getSite() = 0;
        QString yearString()
        {
            QString y = QString::number( curYear + yearOffset );
            y = y.midRef( 2, 2 ).toString();
            return y;
        }
    public:
        CalendarYear ( CalType t, int y ) : type ( t ), yearOffset ( y )
        {
            if ( curYear == 0 )
            {
                curYear = QDate::currentDate().year();
            }

        }
        virtual ~CalendarYear(){}

        virtual bool downloadFile ( bool showError, QWidget *parent );

        bool loaded;
        int yearOffset;
        CalType type;
        virtual QString getPath();
        virtual QString getURL();
};
class HFCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        HFCalendarYear ( int year ) : CalendarYear ( ectHF, year )
        {
        }
};
class HFBARTGCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        HFBARTGCalendarYear ( int year ) : CalendarYear ( ectHFBARTG, year )
        {
        }
};
class VHFCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        VHFCalendarYear ( int year ) : CalendarYear ( ectVHF, year )
        {
        }
};
class MicroCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        MicroCalendarYear ( int year ) : CalendarYear ( ectMwave, year )
        {
        }
};
class HFOtherCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        HFOtherCalendarYear ( int year ) : CalendarYear ( ectHFOther, year )
        {
        }
        virtual QString getPath();
        virtual QString getURL();
};
class VHFOtherCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        VHFOtherCalendarYear ( int year ) : CalendarYear ( ectVHFOther, year )
        {
        }
        virtual QString getPath();
        virtual QString getURL();
};
class CTYCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        CTYCalendarYear ( int year ) : CalendarYear ( ectVHFOther, year )
        {
        }
        virtual QString getPath();
        virtual QString getURL();
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
        CalendarSection() : singleAntenna ( false ), overall ( false )
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
//        std::string power;
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
//        std::string power;
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
