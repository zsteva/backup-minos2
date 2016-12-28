/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "logger_pch.h"

//#include "gridhint.h"
#include "MatchContact.h"
#include "MatchThread.h"
#include "tsinglelogframe.h"
#include "tlogcontainer.h"

//---------------------------------------------------------------------------
TMatchThread *TMatchThread::matchThread = 0;

// wild card comparison, search string e for the wild card string in s
// At the moment we are using "space" as the wildcard.
// we always scan down s for the first char in e
bool wildComp( const QString &ss, const QString &ee )
{
   int s = 0;
   int sl = ss.length();
   int e = 0;
   int el = ee.length();

   while (s < sl && ss[s] == ' ' )
      s++;
   if ( s == sl )
      return false;
   while (e < el && ee[e] == ' ' )
      e++;
   if ( e == el )
      return false;


   int estart = e;

   // scan for first char of e in s

   int sstart = s;	// where to restart search

   while ( sstart < sl )
   {
      s = sstart;		// position moving pointer
      e = estart;		// go back to the start of the searching string

      while ( s < sl && e < el && ( ss[s] != ee[e] ) )
         s++;
      if ( s >= sl )
         return false;		// s has ended without a match on char 1 of e

      sstart = ++s;			// next time start one on from this match
      e++;						// first char has matched
      // now attempt to match
      while ( s < sl && e < el )
      {
         if (
            (ss[s] == ee[e] )
            || ( ( ee[e] == ' ' ) || ( ee[e] == '*' ) || ( ee[e] == '?' ) )
         )
         {
            s++;
            e++;
            continue;
         }
         break;		// match failed, break out
      }
      if ( e >= el )
         return true;		// we are at the end of the searching string, so matched

      // otherwise try again at next matching start char
   }
   return false;
}

TMatchThread::TMatchThread()
      : QThread(   ), myThisMatches( 0 ), myOtherMatches( 0 ),myListMatches( 0 ), mct(0), Terminated(false)
//      ,EL_CountrySelect ( EN_CountrySelect, & CountrySelect_Event )
//      ,EL_DistrictSelect ( EN_DistrictSelect, & DistrictSelect_Event )
//      ,EL_LocatorSelect ( EN_LocatorSelect, & LocatorSelect_Event )
{
   thisLogMatch = new ThisLogMatcher();
   otherLogMatch = new OtherLogMatcher();
   listMatch = new ListMatcher();
   connect(&MinosLoggerEvents::mle, SIGNAL(ScreenContactChanged(ScreenContact*,BaseContestLog*)), this, SLOT(on_ScreenContactChanged(ScreenContact*,BaseContestLog*)));

   connect(&MinosLoggerEvents::mle, SIGNAL(CountrySelect(QString,BaseContestLog*)), this, SLOT(on_CountrySelect(QString,BaseContestLog*)));
   connect(&MinosLoggerEvents::mle, SIGNAL(DistrictSelect(QString,BaseContestLog*)), this, SLOT(on_DistrictSelect(QString,BaseContestLog*)));
   connect(&MinosLoggerEvents::mle, SIGNAL(LocSelect(QString,BaseContestLog*)), this, SLOT(on_LocatorSelect(QString,BaseContestLog*)));
}
void TMatchThread::FinishMatchThread()
{
   if ( matchThread )
   {
      matchThread->Terminate();
      trace( "WaitFor TMatchThread" );
      matchThread->wait(ULONG_MAX);
   }
   trace( "TMatchThread close complete" );
}
void TMatchThread::InitialiseMatchThread()
{
   if ( !matchThread )
   {
      matchThread = new TMatchThread();
      matchThread->start(LowPriority);
   }
}
void TMatchThread::on_ScreenContactChanged(ScreenContact *sct, BaseContestLog *context)
{
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   if (context == ct)
   {
       mct = sct;
      if (mct)
      {
         // we want to initialise the search from the screen contact - break what couplings we can
         // we need to take care over thread safety as well!
         startMatch();
      }
   }
}
//---------------------------------------------------------------------------
void TMatchThread::on_CountrySelect(QString sel, BaseContestLog *c)
{
    BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   if (c == ct)
   {
      mct = 0;
      QSharedPointer<CountryEntry> ce = MultLists::getMultLists() ->getCtryForPrefix( sel );
      startMatch(ce);
   }
}

void TMatchThread::on_DistrictSelect(QString sel, BaseContestLog *c)
{
    BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   if (c == ct)
   {

   }
}

void TMatchThread::on_LocatorSelect(QString sel, BaseContestLog *c)
{
    BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   if (c == ct)
   {

   }
}
//---------------------------------------------------------------------------
// limit the total  number of hits
const int MATCH_LIM = 20;
void TMatchThread::Execute()
{
   //---- Place thread code here ----
   try
   {
      while ( !Terminated )
      {
         thisLogMatch->initMatch();  // does nothing unless matchRequired is true
         otherLogMatch->initMatch();  // does nothing unless matchRequired is true
         listMatch->initMatch();

         // so it only does a max of 20+20 contacts before switching
         // to "other" of log/list
         if ( !thisLogMatch->idleMatch( 20 ) && !otherLogMatch->idleMatch( 20 ) && !listMatch->idleMatch( 20 ) )
            msleep(100);
      }
      delete thisLogMatch;
      thisLogMatch = 0;
      delete otherLogMatch;
      otherLogMatch = 0;
      delete listMatch;
      listMatch = 0;
   }
   catch ( ... )
   {
      // exception caught - in wrong thread, so we can't do much
      trace( "Exception in match thread" );
   }
   trace( "Exiting match thread" );
}
void TMatchThread::run()
{
    Execute();
}

//---------------------------------------------------------------------------
/*static*/ void TMatchThread::startMatch(   QSharedPointer<CountryEntry> ce )
{
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   MinosLoggerEvents::SendMatchStarting(ct);

   matchThread->thisLogMatch->startMatch( ce );
   matchThread->otherLogMatch->startMatch( ce );
   matchThread->listMatch->startMatch( ce );

}
//---------------------------------------------------------------------------
void TMatchThread::replaceThisContestList( TMatchCollection *matchCollection )
{
   myThisMatches = matchCollection;
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   MinosLoggerEvents::SendReplaceThisLogList(myThisMatches, ct);
}
void TMatchThread::replaceOtherContestList( TMatchCollection *matchCollection )
{
   myOtherMatches = matchCollection;
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   MinosLoggerEvents::SendReplaceOtherLogList(myOtherMatches, ct);
}
//---------------------------------------------------------------------------
void TMatchThread::replaceListList( TMatchCollection *matchCollection )
{
   myListMatches = matchCollection;
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   MinosLoggerEvents::SendReplaceListList(myListMatches, ct);
}
//---------------------------------------------------------------------------
void TMatchThread::ShowThisMatchStatus( QString mess )
{
   thisMatchStatus = mess;
}
QString TMatchThread::getThisMatchStatus( )
{
   if ( matchThread )
   {
      return matchThread->thisMatchStatus;
   }
   return "";
}
void TMatchThread::ShowOtherMatchStatus( QString mess )
{
   otherMatchStatus = mess;
}
QString TMatchThread::getOtherMatchStatus( )
{
   if ( matchThread )
   {
      return matchThread->otherMatchStatus;
   }
   return "";
}

//=============================================================================
void TMatchThread::matchCountry( QString cs )
{
   ctrymatch = cs;
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   MinosLoggerEvents::SendScrollToCountry(ctrymatch, ct);
}
//=============================================================================
void  TMatchThread::matchDistrict( QString dist )
{
   distmatch = dist;
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   MinosLoggerEvents::SendScrollToDistrict(distmatch, ct);
}

//=============================================================================
TMatchCollection::TMatchCollection( void )
{}
TMatchCollection::~TMatchCollection( void )
{
}
int TMatchCollection::getContestCount( void )
{
   return contestMatchList.size();
}

QSharedPointer<BaseMatchContest> TMatchCollection::pcontestAt( int i )
{
    if (i > contestMatchList.size())
        return QSharedPointer<BaseMatchContest>();

    return std::next(contestMatchList.begin(), i)->wt;
}

int TMatchCollection::contactCount()
{
    int cc = 0;
    for (ContestMatchIterator i = contestMatchList.begin(); i != contestMatchList.end(); i++ )
    {
        cc += i->wt->contactMatchList.size();
    }
    return cc;
}

//=============================================================================
matchElement::matchElement( void ) : match( false ), empty( true )
{
   mstr.clear();
   rawstr.clear();
}
static QString match_temp;
unsigned char matchElement::checkGreater( const QString &s )
{
   unsigned char res = 0;
   match_temp.clear();
   if ( !s.length() )
   {
      return res;
   }

   strcpysp( match_temp, s, EXTRALENGTH );
   strupr( match_temp );

   int oldlen = rawstr.length();
   int newlen = match_temp.length();

   if ( newlen == oldlen )
   {
      if ( match_temp.compare( rawstr, Qt::CaseInsensitive ) == 0 )
         return res;		// no change
      res = SET_CHANGED | SET_NOT_GREATER;
   }
   else
   {
      res = SET_CHANGED | SET_NOT_GREATER;
      if ( ( oldlen == 0 ) || ( ( newlen > oldlen ) && ( strnicmp( rawstr, match_temp, oldlen ) == 0 ) ) )
         res &= ~SET_NOT_GREATER;
   }

   return res;
}
unsigned char matchElement::set
   ( const QString &s )
{
   unsigned char res = 0;
   match_temp.clear();
   match = false;
   empty = true;

   if ( !s.length() )
   {
      // Null argument used to clear it out
      mstr.clear();
      rawstr.clear();
      return res;
   }

   res = checkGreater( s );

   if ( !match_temp.isEmpty() )
   {
      empty = false;
      match = true;
   }

   mstr = match_temp;
   rawstr = match_temp;
   return res;
}
bool matchElement::checkMatch( const QString &s )
{
   bool ismatch = true;
   if ( match && !empty )
   {
      if ( !wildComp( s, mstr ) )
         ismatch = false;
   }

   return ismatch;
}

//=============================================================================

Matcher::Matcher() : contestIndex( -1 ), contactIndex( -1 ), mp( Exact ),
      matchStarted( false ), firstMatch( Starting ), tickct( 0 ), ce( 0 ),
      matchCollection( 0 ), matchRequired( false )
{
   matchCollection = new TMatchCollection;
}
Matcher::~Matcher()
{
   clearmatchall();
   delete matchCollection;
   matchCollection = 0;
}

void Matcher::clearmatchall( )
{
   contestIndex = -1;
   contactIndex = -1;
   matchStarted = false;
   matchRequired = false;

   matchCollection->contestMatchList.clear();

   // don't want to keep the old, so clear down the memory of selection terms
   matchcs.set( "" );
   matchloc.set( "" );
   matchqth.set( "" );
   ce.reset();
   thisContestMatched = 0;

   TMatchThread::getMatchThread() ->ShowThisMatchStatus( "" );
   TMatchThread::getMatchThread() ->ShowOtherMatchStatus( "" );
}
// mark things that we want to start a match
void Matcher::startMatch( QSharedPointer<CountryEntry> c )
{
   matchRequired = true;
   matchStarted = false;				// if it started we need to stop it...
   tickct = 0;
   ce = c;
   thisContestMatched = 0;
}

// on timer, start it off NB this NOT in the main thread
void Matcher::initMatch( void )
{
   try
   {
      if ( matchRequired )
      {
         tickct++;
         if ( tickct < 2 )
            return ;
      }
      else
      {
         return ;
      }

      if ( !TContestApp::getContestApp() ->getCurrentContest() )
      {
         clearmatchall();
         return ;
      }

      if ( !ce )
      {
         clearmatchall();
         // we need to have been sent this... in a start match action?
         ScreenContact * mct = TMatchThread::getMatchThread()->mct;
         if ( !mct )
            return ;

         unsigned char qth_changed = matchqth.set( mct->extraText );
         if ( qth_changed & SET_CHANGED )
         {
            matchDistrict( mct->extraText );
         }
         unsigned char changed = matchcs.set( mct->cs.fullCall.getValue() );	// we rely on set to set up the search terms
         if ( changed & SET_CHANGED )
         {
            matchCountry( mct->cs.fullCall.getValue() );   // scroll to
         }
         changed |= matchloc.set( mct->loc.loc.getValue() );			// so don't do this all on one line
         changed |= qth_changed;

         if ( matchcs.empty && matchloc.empty && matchqth.empty )
         {
            clearmatchall();
            replaceList( );
            return ;
         }

         if ( !changed & SET_CHANGED )
            return ;

         if ( changed & SET_NOT_GREATER )
         {
            replaceList( );
         }
         // we have something matchable, so go to it!

         mp = Exact;		// set phase
      }
      else
      {
         QSharedPointer<CountryEntry> c = ce;	// preserve over cleardown
         clearmatchall();
         mp = Country;
         ce = c;
      }

      contestIndex = 0;
      contactIndex = 0;
      matchStarted = true;
      firstMatch = Starting;
   }
   catch ( ... )
   {
      trace( "Exception in init match" );
   }
}
//==============================================================================
ThisLogMatcher::ThisLogMatcher()
{}
ThisLogMatcher::~ThisLogMatcher()
{}
void ThisLogMatcher::matchDistrict( const QString &extraText )
{
   TMatchThread::getMatchThread() ->matchDistrict( extraText );   // scroll to
}
void ThisLogMatcher::matchCountry( const QString &cs )
{
   TMatchThread::getMatchThread() ->matchCountry( cs );   // scroll to
}
void ThisLogMatcher::addMatch( QSharedPointer<BaseContact> cct, BaseContestLog *ccon )
{
   if ( !cct )
      return ;

   ContestMatchList &contestMatchList = matchCollection->contestMatchList;
   MapWrapper<BaseMatchContest> wmc = MapWrapper<BaseMatchContest>(new MatchContactLog);
   if (contestMatchList.size() == 0)
   {
       wmc.wt->matchedContest = ccon;
       contestMatchList.insert(wmc, wmc);
   }
   QSharedPointer<BaseMatchContest> found;
   foreach(MapWrapper<BaseMatchContest> test, contestMatchList)
   {
       if (test.wt->getContactLog() == ccon)
       {
            found = test.wt;
            MapWrapper<MatchContact> wmct(new MatchLogContact( ccon, cct ));
            found->contactMatchList.insert( wmct, wmct );
            break;
       }
   }

   thisContestMatched = matchCollection->contactCount();
}
bool ThisLogMatcher::idleMatch( int limit )
{
    try
    {
       // this is called repeatedly off the timer to execute the match
       if ( !matchStarted || ( contestIndex < 0 ) )
          return false;				// nothing to do (yet)

       int cnt = matchCollection->contactCount();
       if ( ( firstMatch == Starting ) && ( ( mp == Exact ) || ( mp == Country ) ) )
       {
          contestIndex = 0;
          contactIndex = 0;
          firstMatch = MainContest;
       }
       else
          if ( ( contestIndex >= 1 ) || ( cnt > MATCH_LIM ) )
          {
             bool EndScan = true;

             if ( cnt == 0 )
             {
                switch ( mp )
                {
                   case Exact:
                      {
                         if ( matchcs.match )
                         {
                            bool dropthrough = false;
                            std::string smstr = matchcs.mstr.toStdString();// allowed conversion through std::string
                            const char *c = smstr.c_str();
                            // need to trim out any leading and trailing
                            while ( *c && *c != '/' )
                               c++;

                            const char *c2 = c;
                            if ( *c2 )
                               c2++;	// skip the first /
                            while ( *c2 && *c2 != '/' )
                               c2++;

                            if ( *c && *c2 )
                            {
                               matchcs.mstr = QString( c ).left(c2 - c );	// copy back over ourselves
                            }
                            else
                               if ( *c && ( c - smstr.c_str() < 3 ) && ( strlen( c ) > 2 ) )
                               {
                                  // prefix less than 3 chars and suffix more than 1 character
                                  matchcs.mstr = QString( c );	// copy back over ourselves
                               }
                               else
                                  if ( *c == '/' )
                                  {
                                     matchcs.mstr = matchcs.mstr.left( c - smstr.c_str() );	// copy back over ourselves
                                     //                                 *c = 0;				// force a stop on the /
                                  }
                                  else
                                  {
                                     // want to drop through
                                     dropthrough = true;
                                  }

                            if ( !dropthrough )
                            {
                               TMatchThread::getMatchThread() ->ShowThisMatchStatus( " - No exact match" );
                               mp = NoSuffix;
                               contestIndex = 0;
                               contactIndex = 0;
                               firstMatch = MainContest;
                               EndScan = false;
                               break;
                            }
                         }
                      }
                      // or no /P anyway, so drop through

                   case NoSuffix:
                      {
                         if ( matchcs.match && ( matchloc.match || matchqth.match ) )
                         {
                            matchloc.set( "" );
                            matchqth.set( "" );
                            mp = NoLoc;
                            contestIndex = 0;
                            contactIndex = 0;
                            firstMatch = MainContest;
                            EndScan = false;
                            TMatchThread::getMatchThread() ->ShowThisMatchStatus( " - No match No Suffix" );
                            break;
                         }
                      }
                      // or no loc anyway, so drop through

                   case NoLoc:
                      {
                         if ( matchcs.match )
                         {
                            // We know that cs is not empty
                            // strip temp cs of its leading country and number
                             std::string smstrStart = matchcs.mstr.toStdString();// allowed conversion through std::string
                            const char * mstrStart = smstrStart.c_str();
                            const char * c = &mstrStart[ smstrStart.length() ];

                            while ( c > mstrStart )
                            {
                               if ( !isdigit( *( c - 1 ) ) )
                                  c--;
                               else
                                  break;
                            }

                            if ( c > mstrStart )   	// i.e. we havent got back to the start
                            {
                               if ( *c )   			// we would be left with something
                               {
                                  matchcs.mstr = c;	// copy back over
                                  mp = Body;
                                  contestIndex = 0;
                                  contactIndex = 0;
                                  firstMatch = MainContest;
                                  EndScan = false;
                                  TMatchThread::getMatchThread() ->ShowThisMatchStatus( " - No match No LOC" );
                                  break;
                               }
                            }
                         }
                      }
                      // else we have already done what we can, so drop through

                   case Country:
                   case District:
                   case Locator:
                   case Body:
                      {
                         // we have finished.
                         break;
                      }
                }
             }

             if ( EndScan )
             {
                matchStarted = false;
                setMatchRequired( false );

                BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
                if ( bool( ct ) && ( mp != Country )  && ( mp != District )  && ( mp != Locator ) )
                   addMatch( ct->DupSheet.getCurDup(), ct );	// in case it isn't already

                QString buff;
                // now make it display
                replaceList( );

                if ( thisContestMatched )
                {
                   // conteste focused the top line of matches here
                   // want to manage plurals, and local/other contests
                   QString matchSuffix = "Possible";
                   if (mp == Country)
                   {
                      matchSuffix = "Country";
                   }
                   else if (mp == District)
                   {
                      matchSuffix = "District";
                   }
                   else if (mp == Locator)
                   {
                      matchSuffix = "Locator";
                   }
                   buff = QString( " - %1%2 %3 matches" )
                           .arg ( ( cnt > MATCH_LIM ) ? ">" : "" )
                           .arg(thisContestMatched)
                           .arg( ( mp == Exact ) ? "" : matchSuffix );
                }
                else
                {
                   buff = " - No match";
                }
                TMatchThread::getMatchThread() ->ShowThisMatchStatus( buff );
                contestIndex = -1;
                return false;
             }
          }

       BaseContestLog *ccon = TContestApp::getContestApp() ->getCurrentContest();		// we always go through current FIRST! to make sure we see it

       while (  matchStarted && limit > 0 )
       {
          if ( !ccon ||
               ( contactIndex >= ccon->getContactCount() )
             )
          {
             // we need to move on

             contactIndex = 0;
             contestIndex++;
             return true;
          }
          QSharedPointer<BaseContact> cct = ccon->pcontactAt( contactIndex++ );
          if ( !cct )
             return true;

          // now do the match

          if ( mp != Country && mp != District && mp != Locator )
          {
             bool csmatch = false;
             bool locmatch = false;
             bool qthmatch = false;

             csmatch = matchcs.checkMatch( ( ( mp == Body ) ? ( cct->cs.body ) : ( cct->cs.fullCall.getValue() ) ) );

             if ( csmatch )
             {
                locmatch = matchloc.checkMatch( cct->loc.loc.getValue() );
             }

             if ( csmatch && locmatch )
             {
                if ( !matchqth.match || matchqth.empty )
                {
                   qthmatch = true;
                }
                else
                {
                   QString uprqth;
                   strcpysp( uprqth, cct->extraText.getValue(), EXTRALENGTH );
                   uprqth = uprqth.toUpper();
                   qthmatch = matchqth.checkMatch( uprqth );

                   if ( bool( ccon ) && !qthmatch )
                   {
                      if ( ccon->districtMult.getValue() )
                      {
                         // attempt match against district code
                         if ( cct->districtMult && wildComp( cct->districtMult->districtCode, matchqth.mstr ) )
                            qthmatch = true;
                      }
                   }
                }
             }

             if ( csmatch && locmatch && qthmatch )
             {
                addMatch( cct, ccon );
                if ( matchCollection->contactCount() > MATCH_LIM )
                   break;
             }
          }
          else if (mp == Country)
          {
             if ( cct->ctryMult && ( cct->ctryMult == ce ) )
             {
                addMatch( cct, ccon );
                if ( matchCollection->contactCount() > MATCH_LIM )
                   break;
             }

          }
          else if (mp == District)
          {
 /*            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
             {
                addMatch( cct, ccon );
                if ( matchCollection->contactCount() > MATCH_LIM )
                   break;
             }
 */
          }
          else if (mp == Locator)
          {
 /*            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
             {
                addMatch( cct, ccon );
                if ( matchCollection->contactCount() > MATCH_LIM )
                   break;
             }
 */
          }
       }
    }
    catch ( ... )
    {
       trace( "Exception in log idle match" );
    }
    return true;
}
void ThisLogMatcher::replaceList(  )
{
   TMatchThread::getMatchThread() ->replaceThisContestList( matchCollection );
   matchCollection = new TMatchCollection;  // pass ownership
}
//==============================================================================
OtherLogMatcher::OtherLogMatcher()
{}
OtherLogMatcher::~OtherLogMatcher()
{}
void OtherLogMatcher::matchDistrict( const QString &extraText )
{
   TMatchThread::getMatchThread() ->matchDistrict( extraText );   // scroll to
}
void OtherLogMatcher::matchCountry( const QString &cs )
{
   TMatchThread::getMatchThread() ->matchCountry( cs );   // scroll to
}
void OtherLogMatcher::addMatch( QSharedPointer<BaseContact> cct, BaseContestLog * ccon )
{

   if ( !cct )
      return ;

   ContestMatchList &contestMatchList = matchCollection->contestMatchList;
   MapWrapper<BaseMatchContest> wmc(new MatchContactLog);


   QSharedPointer<BaseMatchContest> found;
   foreach(MapWrapper<BaseMatchContest> test, contestMatchList)
   {
       if (test.wt->getContactLog() == ccon)
       {
            found = test.wt;
            break;
       }
   }
   if (!found)
   {
       wmc.wt->matchedContest = ccon;
       contestMatchList.insert(wmc, wmc);
   }
   found.reset();
   foreach(MapWrapper<BaseMatchContest> test, contestMatchList)
   {
       if (test.wt->getContactLog() == ccon)
       {
            found = test.wt;
            MapWrapper<MatchContact> mct(new MatchLogContact( ccon, cct ));
            found->contactMatchList.insert( mct, mct );
            break;
       }
   }

   thisContestMatched = matchCollection->contactCount();
}
bool OtherLogMatcher::idleMatch( int limit )
{
   try
   {
      // this is called repeatedly off the timer to execute the match
      if ( !matchStarted || ( contestIndex < 0 ) )
         return false;				// nothing to do (yet)

      int cnt = matchCollection->contactCount();
      if ( ( firstMatch == Starting ) && ( ( mp == Exact ) || ( mp == Country ) ) )
      {
         contestIndex = 0;
         contactIndex = 0;
         firstMatch = Rest;
      }
      else
         if ( ( contestIndex >= TContestApp::getContestApp() ->getContestSlotCount() ) || ( cnt > MATCH_LIM ) )
         {
            bool EndScan = true;

            if ( cnt == 0 )
            {
               switch ( mp )
               {
                  case Exact:
                     {
                        if ( matchcs.match )
                        {
                           bool dropthrough = false;
                           std::string smstr = matchcs.mstr.toStdString();// allowed conversion through std::string
                           const char *c = smstr.c_str();
                           // need to trim out any leading and trailing
                           while ( *c && *c != '/' )
                              c++;

                           const char *c2 = c;
                           if ( *c2 )
                              c2++;	// skip the first /
                           while ( *c2 && *c2 != '/' )
                              c2++;

                           if ( *c && *c2 )
                           {
                              matchcs.mstr = QString( c ).left(c2 - c );	// copy back over ourselves
                           }
                           else
                              if ( *c && ( c - smstr.c_str() < 3 ) && ( strlen( c ) > 2 ) )
                              {
                                 // prefix less than 3 chars and suffix more than 1 character
                                 matchcs.mstr = QString( c );	// copy back over ourselves
                              }
                              else
                                 if ( *c == '/' )
                                 {
                                    matchcs.mstr = matchcs.mstr.left( c - smstr.c_str() );	// copy back over ourselves
                                    //                                 *c = 0;				// force a stop on the /
                                 }
                                 else
                                 {
                                    // want to drop through
                                    dropthrough = true;
                                 }

                           if ( !dropthrough )
                           {
                              TMatchThread::getMatchThread() ->ShowOtherMatchStatus( " - No exact match" );
                              mp = NoSuffix;
                              contestIndex = 0;
                              contactIndex = 0;
                              firstMatch = Rest;
                              EndScan = false;
                              break;
                           }
                        }
                     }
                     // or no /P anyway, so drop through

                  case NoSuffix:
                     {
                        if ( matchcs.match && ( matchloc.match || matchqth.match ) )
                        {
                           matchloc.set( "" );
                           matchqth.set( "" );
                           mp = NoLoc;
                           contestIndex = 0;
                           contactIndex = 0;
                           firstMatch = Rest;
                           EndScan = false;
                           TMatchThread::getMatchThread() ->ShowOtherMatchStatus( " - No match No Suffix" );
                           break;
                        }
                     }
                     // or no loc anyway, so drop through

                  case NoLoc:
                     {
                        if ( matchcs.match )
                        {
                           // We know that cs is not empty
                           // strip temp cs of its leading country and number
                            std::string smstrStart = matchcs.mstr.toStdString();// allowed conversion through std::string
                           const char * mstrStart = smstrStart.c_str();
                           const char * c = &mstrStart[ smstrStart.length() ];

                           while ( c > mstrStart )
                           {
                              if ( !isdigit( *( c - 1 ) ) )
                                 c--;
                              else
                                 break;
                           }

                           if ( c > mstrStart )   	// i.e. we havent got back to the start
                           {
                              if ( *c )   			// we would be left with something
                              {
                                 matchcs.mstr = c;	// copy back over
                                 mp = Body;
                                 contestIndex = 0;
                                 contactIndex = 0;
                                 firstMatch = Rest;
                                 EndScan = false;
                                 TMatchThread::getMatchThread() ->ShowOtherMatchStatus( " - No match No LOC" );
                                 break;
                              }
                           }
                        }
                     }
                     // else we have already done what we can, so drop through

                  case Country:
                  case District:
                  case Locator:
                  case Body:
                     {
                        // we have finished.
                        EndScan = true;
                        break;
                     }
            }
            }

            if ( EndScan )
            {
               matchStarted = false;
               setMatchRequired( false );

               // now make it display
               replaceList( );

               QString buff = " - No match";

               TMatchThread::getMatchThread() ->ShowOtherMatchStatus( buff );
               contestIndex = -1;
               return false;
            }
         }

      QSharedPointer<ContestSlot> cs = TContestApp::getContestApp() ->contestSlotList[ contestIndex ];
      BaseContestLog * ccon = cs->slot;
      if ( ccon == TContestApp::getContestApp() ->getCurrentContest() )
      {
         contactIndex = ccon->getContactCount();	// force to go on
      }

      while (  matchStarted && limit > 0 )
      {
         if ( !ccon ||
              ( contactIndex >= ccon->getContactCount() )
            )
         {
            // we need to move on

            contactIndex = 0;
            contestIndex++;
            return true;
         }
         QSharedPointer<BaseContact> cct = ccon->pcontactAt( contactIndex++ );
         if ( !cct )
            return true;

         // now do the match

         if ( mp != Country && mp != District && mp != Locator )
         {
            bool csmatch = false;
            bool locmatch = false;
            bool qthmatch = false;

            QString matchPart;
            if (mp == Body)
            {
                matchPart = cct->cs.body;
            }
            else
            {
                matchPart = cct->cs.fullCall.getValue();
            }
            csmatch = matchcs.checkMatch( matchPart );

            if ( csmatch )
            {
               locmatch = matchloc.checkMatch( cct->loc.loc.getValue() );
            }

            if ( csmatch && locmatch )
            {
               if ( !matchqth.match || matchqth.empty )
               {
                  qthmatch = true;
               }
               else
               {
                  QString uprqth;
                  strcpysp( uprqth, cct->extraText.getValue(), EXTRALENGTH );
                  uprqth = uprqth.toUpper();
                  qthmatch = matchqth.checkMatch( uprqth );

                  if ( ccon != nullptr && !qthmatch )
                  {
                     if ( ccon->districtMult.getValue() )
                     {
                        // attempt match against district code
                        if ( cct->districtMult && wildComp( cct->districtMult->districtCode, matchqth.mstr ) )
                           qthmatch = true;
                     }
                  }
               }
            }

            if ( csmatch && locmatch && qthmatch )
            {
               addMatch( cct, ccon );
               if ( matchCollection->contactCount() > MATCH_LIM )
                  break;
            }
         }
         else if (mp == Country)
         {
            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
            {
               addMatch( cct, ccon );
               if ( matchCollection->contactCount() > MATCH_LIM )
                  break;
            }

         }
         else if (mp == District)
         {
/*            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
            {
               addMatch( cct, ccon );
               if ( matchCollection->contactCount() > MATCH_LIM )
                  break;
            }
*/
         }
         else if (mp == Locator)
         {
/*            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
            {
               addMatch( cct, ccon );
               if ( matchCollection->contactCount() > MATCH_LIM )
                  break;
            }
*/
         }
      }
   }
   catch ( ... )
   {
      trace( "Exception in log idle match" );
   }
   return true;
}
void OtherLogMatcher::replaceList( )
{
   TMatchThread::getMatchThread() ->replaceOtherContestList( matchCollection );
   matchCollection = new TMatchCollection;  // pass ownership

}

//==============================================================================
ListMatcher::ListMatcher()
{}
ListMatcher::~ListMatcher()
{}
void ListMatcher::matchDistrict( const QString &/*extraText*/ )
{}
void ListMatcher::matchCountry( const QString &/*cs*/ )
{}
void ListMatcher::addMatch( ListContact *cct, ContactList * ccon )
{
       if ( !cct )
          return ;

       ContestMatchList &contestMatchList = matchCollection->contestMatchList;
       MapWrapper<BaseMatchContest> wmc(new MatchContactList);


       QSharedPointer<BaseMatchContest> found;
       foreach(MapWrapper<BaseMatchContest> test, contestMatchList)
       {
           if (test.wt->getContactList() == ccon)
           {
                found = test.wt;
                break;
           }
       }
       if (!found)
       {
           wmc.wt->matchedContest = ccon;
           contestMatchList.insert(wmc, wmc);
       }
       found.reset();
       foreach(MapWrapper<BaseMatchContest> test, contestMatchList)
       {
           if (test.wt->getContactList() == ccon)
           {
                found = test.wt;
                MapWrapper<MatchContact> mct(new MatchListContact( ccon, cct ));
                found->contactMatchList.insert( mct, mct );
                break;
           }
       }

       thisContestMatched = matchCollection->contactCount();
}

bool ListMatcher::idleMatch( int limit )
{
   try
   {
      // this is called repeatedly off the timer to execute the match
      if ( !matchStarted || ( contestIndex < 0 ) )
         return false;				// nothing to do (yet)
      if ( TContestApp::getContestApp() ->getListSlotCount() <= 0 )
         return false;

      int cnt = matchCollection->contactCount();
      if ( ( firstMatch == Starting ) && ( ( mp == Exact ) || ( mp == Country ) || ( mp == District ) || ( mp == Locator ) ) )
      {
         contestIndex = 0;
         contactIndex = 0;
         firstMatch = MainContest;
      }
      else
         if ( ( contestIndex >= TContestApp::getContestApp() ->getListSlotCount() ) || ( cnt > MATCH_LIM ) )
         {
            bool EndScan = true;

            if ( cnt == 0 )
            {
               switch ( mp )
               {
                  case Exact:
                     {
                        if ( matchcs.match )
                        {
                           bool dropthrough = false;
                           std::string smatchcs = matchcs.mstr.toStdString();// allowed conversion through std::string
                           const char *c = smatchcs.c_str();
                           // need to trim out any leading and trailing
                           while ( *c && *c != '/' )
                              c++;

                           const char *c2 = c;
                           if ( *c2 )
                              c2++;	// skip the first /
                           while ( *c2 && *c2 != '/' )
                              c2++;

                           if ( *c && *c2 )
                           {
                              matchcs.mstr = QString( c ).left( c2 - c );	// copy back over ourselves
                           }
                           else
                              if ( *c && ( c - smatchcs.c_str() < 3 ) && ( strlen( c ) > 2 ) )
                              {
                                 // prefix less than 3 chars and suffix more than 1 character
                                 matchcs.mstr = QString( c );	// copy back over ourselves
                              }
                              else
                                 if ( *c == '/' )
                                 {
                                    matchcs.mstr = matchcs.mstr.left( c - smatchcs.c_str() );	// copy back over ourselves
                                    //                                 *c = 0;				// force a stop on the /
                                 }
                                 else
                                 {
                                    // want to drop through
                                    dropthrough = true;
                                 }

                           if ( !dropthrough )
                           {
                              //TMatchThread::getMatchThread()->ShowMatchStatus( " - No exact match" );
                              mp = NoSuffix;
                              contestIndex = 0;
                              contactIndex = 0;
                              firstMatch = MainContest;
                              EndScan = false;
                              break;
                           }
                        }
                     }
                     // or no /P anyway, so drop through

                  case NoSuffix:
                     {
                        if ( matchcs.match && ( matchloc.match || matchqth.match ) )
                        {
                           matchloc.set( "" );
                           matchqth.set( "" );
                           mp = NoLoc;
                           contestIndex = 0;
                           contactIndex = 0;
                           firstMatch = MainContest;
                           EndScan = false;
                           //TMatchThread::getMatchThread()->ShowMatchStatus( " - No match No Suffix" );
                           break;
                        }
                     }
                     // or no loc anyway, so drop through

                  case NoLoc:
                     {
                        if ( matchcs.match )
                        {
                           // We know that cs is not empty
                           // strip temp cs of its leading country and number
                            std::string smstrStart = matchcs.mstr.toStdString();// allowed conversion through std::string
                           const char * mstrStart = smstrStart.c_str();
                           const char * c = &smstrStart[ smstrStart.length() ];

                           while ( c > mstrStart )
                           {
                              if ( !isdigit( *( c - 1 ) ) )
                                 c--;
                              else
                                 break;
                           }

                           if ( c > mstrStart )   	// i.e. we havent got back to the start
                           {
                              if ( *c )   			// we would be left with something
                              {
                                 matchcs.mstr = c;	// copy back over
                                 mp = Body;
                                 contestIndex = 0;
                                 contactIndex = 0;
                                 firstMatch = MainContest;
                                 EndScan = false;
                                 //TMatchThread::getMatchThread()->ShowMatchStatus( " - No match No LOC" );
                                 break;
                              }
                           }
                        }
                     }
                     // else we have already done what we can, so drop through

                  case Country:
                  case District:
                  case Locator:
                  case Body:
                     {
                        // we have finished.
                        break;
                     }
               }
            }

            if ( EndScan )
            {
               matchStarted = false;
               setMatchRequired( false );

               QString buff;
               // now make it display
               replaceList( );

               contestIndex = -1;
               return false;
            }
         }

      ContactList * ccon;

      QSharedPointer<ListSlot> cs = TContestApp::getContestApp() ->listSlotList[ contestIndex ];
      ccon = cs->slot;

      while ( limit > 0  && !ce)
      {
//         limit -= 1;
         if ( !ccon ||
              ( contactIndex >= ccon->getContactCount() )
            )
         {
            // we need to move on

            bool next_needed = true;
            contactIndex = 0;

            if ( firstMatch == MainContest )
            {
               // we have done the current ContestLog, go to the rest
               firstMatch = Rest;
               contestIndex = -1;	// as it gets ++ed lower down
            }

            if ( next_needed )
            {
               // go to the next ContestLog
               contestIndex++;
               if ( contestIndex < TContestApp::getContestApp() ->getListSlotCount() )
               {
                  // ContestLog is valid, if it is a list set first part
                  QSharedPointer<ListSlot> cs = TContestApp::getContestApp() ->listSlotList[ contestIndex ];
                  ccon = cs->slot;
               }
            }
            return true;
         }
         ListContact *cct = ccon->pcontactAt( contactIndex++ );
         if ( !cct )
            return true;

         // now do the match
// And why aren't we doing a QTH match mere?
         bool csmatch = false;
         bool locmatch = false;

         csmatch = matchcs.checkMatch( ( ( mp == Body ) ? ( cct->cs.body ) : ( cct->cs.fullCall.getValue() ) ) );

         if ( csmatch )
         {
            locmatch = matchloc.checkMatch( cct->loc.loc.getValue() );
         }


         if ( csmatch && locmatch )
         {
            addMatch( cct, ccon );
            if ( matchCollection->contactCount() > MATCH_LIM )
               break;
         }
      }
   }
   catch ( ... )
   {
      trace( "Exception in list idle match" );
   }
   return true;
}
void ListMatcher::replaceList( )
{
   TMatchThread::getMatchThread() ->replaceListList( matchCollection );
   matchCollection = new TMatchCollection;  // pass ownership

}

