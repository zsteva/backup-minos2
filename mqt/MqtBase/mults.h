/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
#ifndef MultsH
#define MultsH 
//----------------------------------------------------------------------------
class DistrictEntry;
class DistrictSynonym;
class DistrictList;
class CountrySynonymList;
class CountryEntry;
class CountrySynonym;					 //
class CountryList;
class MultEntry;
class BaseContestLog;


const int CONTINENTS = 6;
struct ContList
{
   char continent[ 3 ];
   bool allow;
};

const int UKREGIONS = 10;

enum eMultGridCols {ectCall, ectWorked, ectLocator, ectBearing, ectName, ectOtherCalls,
                    ectMultMaxCol
                   };

#define GLIST_PREFIX_LEN 5
class GlistEntry
{
   public:
      GlistEntry( const QString &cd, const QString &syn );
      virtual ~GlistEntry();

      bool operator<( const GlistEntry& rhs ) const;
      bool operator==( const GlistEntry& rhs ) const;
      bool operator!=( const GlistEntry& rhs ) const;

      QString synPrefix;
      QString dupPrefix;
};
class MultEntry
{
   public:
      int listOffset;
      locator central;	// central point to take bearings to
      QString realName;

      MultEntry( const QString &name, const QString &cloc );
      virtual ~MultEntry();

      virtual QString str( bool ) = 0;

      virtual void addSynonyms( QString & );
};
#define DISTRICT_CODE_LENGTH 2
class DistrictEntry : public MultEntry
{
   public:

      QString districtCode; // RSGB code
      CountryEntry * country1; // country containing district
      CountryEntry * country2; // country containing district

      DistrictEntry( const QString &cd, const QString &name, const QString &prefix, const QString &prefix2, const QString &cloc );
      virtual ~DistrictEntry();
      bool operator<( const DistrictEntry& rhs ) const;
      bool operator==( const DistrictEntry& rhs ) const;
      bool operator!=( const DistrictEntry& rhs ) const;

      virtual QString str( bool );
      virtual void addSynonyms( QString & );
};

class DistrictSynonym
{
   public:
      DistrictSynonym( const QString &cd, const QString &syn );
      virtual ~DistrictSynonym();

      QString synonym;
      DistrictEntry *district;
      bool operator<( const DistrictSynonym& rhs ) const;
      bool operator==( const DistrictSynonym& rhs ) const;
      bool operator!=( const DistrictSynonym& rhs ) const;
};

class CountryEntry : public MultEntry
{
      char distLimit;
   public:
      QString basePrefix;
      QString continent;

      int districtLimit( void );
      bool hasDistricts( void );

      CountryEntry( const QString &continent, const QString &prefix, const QString &name, const QString &cloc );
      virtual ~CountryEntry();
      virtual QString str( bool );
      virtual void addSynonyms( QString & );
      bool operator<( const CountryEntry& rhs ) const;
      bool operator==( const CountryEntry& rhs ) const;
      bool operator!=( const CountryEntry& rhs ) const;
};

class CountrySynonym
{
   public:
      QString synPrefix;
      CountryEntry * country;

      CountrySynonym( const QString &syn, const QString &prefix );
      virtual ~CountrySynonym();

      void getDupPrefix( QString & );
      virtual void synCat( QString &add_buff );

      virtual int compare( const CountrySynonym & ) const;

      bool operator<( const CountrySynonym& rhs ) const;
      bool operator==( const CountrySynonym& rhs ) const;
      bool operator!=( const CountrySynonym& rhs ) const;
};

class LocCount
{
   public:
      unsigned short UKLocCount;
      unsigned short nonUKLocCount;
      bool UKMultGiven;
      LocCount():UKLocCount(0), nonUKLocCount(0), UKMultGiven(false){}
};
class LocSquare
{
   public:
      LocSquare( const QString &loc );
      LocCount *map( int num ); // give count char for loc num
      LocCount *map(const QString &num ); // give count char for loc num
      void clear( void );

      QString loc; // two letter main square

      LocCount numbers[ 10 ][ 10 ]; // map of parts worked
      bool operator<( const LocSquare& rhs ) const;
      bool operator==( const LocSquare& rhs ) const;
      bool operator!=( const LocSquare& rhs ) const;

};

struct LocSqCmp
{
   bool operator() ( const LocSquare *s1, const LocSquare *s2 ) const
   {
      return * s1 < *s2;
   }
};

typedef codeproject::sorted_vector < LocSquare *, true, LocSqCmp > LocSquareList;
typedef LocSquareList::iterator LocSquareIterator;
class LocList
{
   public:
      LocSquareList llist;
      LocList( void );
      virtual ~LocList();
      void freeAll();
};

class MultLists
{
   public:
      static MultLists *getMultLists();
      MultLists();
      virtual ~MultLists();

      //      void addCountry( bool addsyn ) = 0;
      virtual CountrySynonym *searchCountrySynonym( const QString &syn ) = 0;
      virtual DistrictEntry *searchDistrict( const QString &syn ) = 0;

      virtual int getCtryListSize() = 0;
      virtual int getDistListSize() = 0;
      virtual CountryEntry *getCtryForPrefix( const QString &forcedMult ) = 0;
      virtual QString getCtryListText( int item, int Column, BaseContestLog *const ct ) = 0;
      virtual QString getDistListText( int item, int Column, BaseContestLog *const ct ) = 0;
      virtual CountryEntry * getCtryListAt( int index ) = 0;
      virtual int getCtryListIndexOf( CountryEntry * ) = 0;
      virtual int getDistListIndexOf( DistrictEntry * ) = 0;
      virtual bool isUKprefix(const callsign &cs) = 0;
//      virtual DistrictEntry *getDistrictEntry(int item) = 0;
//      virtual CountryEntry *getCountryEntry(int item) = 0;
      virtual int getDistWorked(int item, BaseContestLog *const ct ) = 0;
      virtual int getCountryWorked(int item, BaseContestLog *const ct ) = 0;
};
#endif
