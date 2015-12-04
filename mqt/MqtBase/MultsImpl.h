//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
#ifndef MultsImplH
#define MultsImplH 
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


template < class itemtype >
struct MultCmp
{
   bool operator() ( const itemtype s1, const itemtype s2 ) const
   {
      return * s1 < *s2;
   }
};

template < class itemtype >
class MultList : public codeproject::sorted_vector < itemtype, true, MultCmp <itemtype> >
{
   protected:
      virtual int getWorked( int /*item*/, BaseContestLog * const /*ct*/ )
      {
         return 0;
      }
   public:
      QString multfilename;
      virtual bool procLine( char ** ) = 0;
      MultList()
      {}
      virtual ~MultList()
      {
         freeAll();
      }
      void loadEntries( const QString &sfname, const QString &fmess )
      {
         TEMPBUFF( buff, 256 );
         QString fname = sfname;
         if ( multfilename.length() )
            fname = multfilename;

         // populate the contest object from the file
         std::ifstream istr( fname.toStdString().c_str() ); // should close when it goes out of scope
         if ( !checkFileOK( istr, fname, fmess ) )
            return ;

         // loop through file, parsing each line into a_exp entries
         // for each line, call proc_line
         // ignore comment lines. (any non alpha/num char)


         while ( istr.getline( buff, 255 ) )
         {
            char * a[ 255 ];
            if ( buff[ 0 ] != '/' && !isalnum( buff[ 0 ] ) )      // allow '/' for suffixes
               continue;   // skip comment lines
            bool sep2seen;
            parseLine( buff, ',', a, 255, 0, sep2seen );

            if ( !procLine( a ) )
               break;

         }
      }
      void freeAll()
      {
         for ( typename MultList::iterator i = MultList::begin(); i != MultList::end(); i++ )
            delete ( *i );
         MultList::clear();
      }
      virtual int slen( bool )
      {
         return -1;
      }
      unsigned int indexOf( itemtype item )
      {
         typename MultList::iterator f = std::lower_bound( MultList::begin(), MultList::end(), item, MultCmp <itemtype>() );
         if ( f == MultList::end() || ( *f ) != item )
         {
            return ( MultList::end() - MultList::begin() );
         }
         unsigned int diff = f - MultList::begin();
         return diff;
      }

      QString getText( int item, int Column, BaseContestLog *const ct )
      {
         QString dest;
         if ( item >= (int)MultList::size() )
            return dest;
         itemtype ce = MultList::at( item );
         switch ( Column )
         {
            case ectCall:
               {
                  dest = ce->str( true );
                  break;
               }

            case ectWorked:
               {
                  TEMPBUFF( wcount, 10 );
                  sprintf( wcount, "%d", getWorked( item, ct ) );
                  dest = wcount;
                  break;
               }

            case ectLocator:
               {
                  dest = ce->central.loc.getValue().left( 6 );
                  break;
               }

            case ectBearing:
               {
                  double longitude = 0.0;
                  double latitude = 0.0;
                  double useScore = 0.0;
                  bool brgscoreValid = true;
                  int useBearing = 0;

                  QString brgbuff;
                  int valRes = lonlat( ce->central.loc.getValue(), longitude, latitude );
                  if ( ( valRes == LOC_OK ) && ct ->locValid )
                     ct ->disbear( longitude, latitude, useScore, useBearing );
                  else
                     brgscoreValid = false;
                  if ( brgscoreValid && ( valRes == LOC_OK ) )
                  {
                     brgbuff = QString( "%1\xb0").arg(varBrg( useBearing + ct->bearingOffset.getValue()), 3 );
                  }

                  dest = brgbuff;
                  break;
               }


            case ectName:
               {
                  // want to list synonyms after the real name, for both countries and districts
                  dest = ce->realName;
                  break;
               }

            case ectOtherCalls:
               {
                  ce->addSynonyms( dest );
                  break;
               }
         }
         return dest;
      }

      QString getText( int item, BaseContestLog *const ct )
      {
         // Only called for fullMultDisp
         QString dest;
         if ( item >= (int)MultList::size() )
            return dest;
         itemtype ce = MultList::at( item );

         double longitude = 0.0;
         double latitude = 0.0;
         double useScore = 0.0;
         bool brgscoreValid = true;
         int useBearing = 0;

         QString brgbuff;
         QString buff;

         int valRes = lonlat( ce->central.loc.getValue(), longitude, latitude );
         if ( ( valRes == LOC_OK ) && ct ->locValid )
            ct ->disbear( longitude, latitude, useScore, useBearing );
         else
            brgscoreValid = false;

         if ( brgscoreValid && ( valRes == LOC_OK ) )
         {
            brgbuff = QString("%1\xb0").arg( varBrg( useBearing), 3 ) ;
         }

         int next = 0;
         next = placestr( buff, ce->str( true ), next, slen( true ) );

         QString wcount;
         wcount = QString("(%1)").arg( getWorked( item, ct ) );
         next = placestr( buff, wcount, next + 1, 5 );

         next = placestr( buff, ce->central.loc.getValue(), next + 1, 6 );
         next = placestr( buff, brgbuff, next + 1, 4 );

         // want to list synonyms after the real name, for both countries and districts
         next = placestr( buff, ce->realName, next + 1, ce->realName.length() );

         QString namebuff;
         ce->addSynonyms( namebuff );

         placestr( buff, namebuff, next + 1, 254 - next );

         dest = buff;
         return dest;
      }

};
class GlistList : public MultList < GlistEntry * >
{
      // list of DistrictEntry
   public:
      GlistList( void );
      virtual ~GlistList();
      void load( void );
      virtual bool procLine( char ** );

};
class DistrictList : public MultList < DistrictEntry * >
{
      // list of DistrictEntry
   protected:
      virtual int getWorked( int item, BaseContestLog *const ct );
   public:
      DistrictList( void );
      virtual ~DistrictList();
      void load( void );
      virtual bool procLine( char ** );
      virtual int slen( bool );
};

class DistrictSynonymList : public MultList < DistrictSynonym * >
{
      // list of DistrictSynonym
   public:
      DistrictSynonymList( void );
      virtual ~DistrictSynonymList();
      void load( void );
      virtual bool procLine( char ** );
};


class CountryList : public MultList < CountryEntry * >
{
      // list of CountryEntry
   protected:
      virtual int getWorked( int item, BaseContestLog *const ct );
   public:
      CountryList( void );
      virtual ~CountryList();
      void load( void );
      virtual bool procLine( char ** );
      virtual int slen( bool );
      void loadEntries( const QString &fname, const QString &fmess );
};

class CountrySynonymList : public MultList < CountrySynonym * >
{
      // list of CountrySynonym
   public:
      CountrySynonymList( void );
      virtual ~CountrySynonymList();
      void load( void );
      virtual bool procLine( char ** );
};


class MultListsImpl: public MultLists
{
      static MultListsImpl *multLists;
      bool loadMultFiles ( void );
      MultListsImpl();
   public:
      CountryList ctryList;
      CountrySynonymList ctrySynList;
      DistrictList distList;
      DistrictSynonymList distSynList;
      GlistList glist;

      static MultListsImpl *getMultLists();
      ~MultListsImpl();
      //      void addCountry( bool addsyn );
      virtual CountrySynonym *searchCountrySynonym( const QString &syn );
      virtual DistrictEntry *searchDistrict( const QString &syn );
      virtual int getCtryListSize();
      virtual int getDistListSize();
      virtual CountryEntry *getCtryForPrefix( const QString &forcedMult );
      virtual QString getCtryListText( int item, int Column, BaseContestLog *const ct );
      virtual QString getDistListText( int item, int Column, BaseContestLog *const ct );
      virtual CountryEntry * getCtryListAt( int index );
      virtual int getCtryListIndexOf( CountryEntry * );
      virtual int getDistListIndexOf( DistrictEntry * );
      virtual bool isUKprefix(const callsign &cs);

};
#endif
