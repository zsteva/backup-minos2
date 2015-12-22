/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#include "base_pch.h"

#ifndef MatchThreadH
#define MatchThreadH 
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define SET_CHANGED 1
#define SET_NOT_GREATER 2

class MatchContact;
class matchElement
{
   public:
      bool match;
      bool empty;
      //      bool trimmed;
      QString mstr;
      QString rawstr;

      matchElement( void );
      unsigned char set
         ( const QString & );
      unsigned char checkGreater( const QString & );
      bool checkMatch( const QString & );
};
struct LtMatch
{
   bool operator() ( const MatchContact* s1, const MatchContact* s2 ) const
   {
      return * s1 < *s2;
   }
};
typedef codeproject::sorted_vector < MatchContact *, true, LtMatch > MatchList;
typedef MatchList::iterator MatchIterator;

class TMatchCollection
{
   public:
      MatchList matchList;
      void freeAll()
      {
         for ( MatchIterator i = matchList.begin(); i != matchList.end(); i++ )
         {
            delete ( *i );
            ( *i ) = 0;
         }
         matchList.clear();
      }
      TMatchCollection( void );
      ~TMatchCollection();
      int getContactCount( void );
      MatchContact *pcontactAt( int );
};
class Matcher
{
      bool matchRequired;  // use getter and setter below
   protected:
      enum MatchPhase {Exact, NoSuffix, NoLoc, Body, Country, District, Locator};
      enum ContestPhase {Starting, MainContest, Rest};

      MatchPhase mp;
      bool matchStarted;
      ContestPhase firstMatch;
      int tickct;

      int contestIndex;
      int contactIndex;

      matchElement matchcs;
      matchElement matchloc;
      matchElement matchqth;

      CountryEntry *ce;

      int thisContestMatched;
      virtual void matchDistrict( const QString &extraText ) = 0;
      virtual void matchCountry( const QString &cs ) = 0;
      virtual void replaceList( TMatchCollection *matchCollection ) = 0;
   public:

      Matcher( );
      virtual ~Matcher();
      TMatchCollection *matchCollection;

      void startMatch( CountryEntry *ce = 0 );
      void initMatch( void );
      void clearmatchall();
      virtual bool idleMatch( int limit ) = 0;

      void setMatchRequired( bool b )
      {
         matchRequired = b;
      }
      bool getMatchRequired()
      {
         return matchRequired;
      }

};
class ThisLogMatcher: public Matcher
{
      virtual void matchDistrict( const QString &extraText );
      virtual void matchCountry( const QString &cs );
      virtual void replaceList( TMatchCollection *matchCollection );
   public:
      ThisLogMatcher( );
      virtual ~ThisLogMatcher();

      virtual bool idleMatch( int limit );
      void addMatch( BaseContact *, BaseContestLog * );
};
class OtherLogMatcher: public Matcher
{
      virtual void matchDistrict( const QString &extraText );
      virtual void matchCountry( const QString &cs );
      virtual void replaceList( TMatchCollection *matchCollection );
   public:
      OtherLogMatcher( );
      virtual ~OtherLogMatcher();

      virtual bool idleMatch( int limit );
      void addMatch( BaseContact *, BaseContestLog * );
};
class ListMatcher: public Matcher
{
      virtual void matchDistrict( const QString &extraText );
      virtual void matchCountry( const QString &cs );
      virtual void replaceList( TMatchCollection *matchCollection );
   public:
      ListMatcher( );
      ~ListMatcher();

      virtual bool idleMatch( int limit );
      void addMatch( ListContact *, ContactList * );
};
//---------------------------------------------------------------------------
class TMatchThread : public QThread
{
    Q_OBJECT

private:

//      MinosEventListener  EL_CountrySelect;
      void CountrySelect_Event ( /*MinosEventBase & Event*/ );

//      MinosEventListener  EL_DistrictSelect;
      void DistrictSelect_Event ( /*MinosEventBase & Event*/ );

//      MinosEventListener  EL_LocatorSelect;
      void LocatorSelect_Event ( /*MinosEventBase & Event*/ );

      static void startMatch( CountryEntry *ce = 0 );

      static TMatchThread *matchThread;
      TMatchThread();

      ThisLogMatcher *thisLogMatch;
      OtherLogMatcher *otherLogMatch;
      ListMatcher *listMatch;

      TMatchCollection *myThisMatches;        // used to pass the match list out
      TMatchCollection *myOtherMatches;        // used to pass the match list out
      TMatchCollection *myListMatches;    // used to pass the match list out

      QString ctrymatch;
      QString distmatch;

      QString thisMatchStatus;
      QString otherMatchStatus;

      bool Terminated;

   protected:
      virtual void Execute();    // TThread method
   public:
      ScreenContact * mct;
      void Terminate()
      {
          Terminated = true;
      }

      void replaceThisContestList( TMatchCollection *matchCollection );
      void replaceOtherContestList( TMatchCollection *matchCollection );
      void replaceListList( TMatchCollection *matchCollection );
      void matchCountry( QString cs );
      void matchDistrict( QString dist );

      void ShowThisMatchStatus( QString mess );
      void ShowOtherMatchStatus( QString mess );
      static QString getThisMatchStatus( );
      static QString getOtherMatchStatus( );
      static void InitialiseMatchThread();
      static void FinishMatchThread();
      static TMatchThread *getMatchThread()
      {
         return matchThread;
      }

      // QThread interface
protected:
      virtual void run() override;
private slots:
      void on_ScreenContactChanged(ScreenContact *mct, BaseContestLog *context);
};
//---------------------------------------------------------------------------
#endif
