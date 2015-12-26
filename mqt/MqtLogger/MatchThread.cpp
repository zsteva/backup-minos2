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
void TMatchThread::CountrySelect_Event ( /*MinosEventBase & Event*/ )
{
    /*
   ActionEvent2<QString,  BaseContestLog *, EN_CountrySelect> & S = dynamic_cast<ActionEvent2<QString, BaseContestLog *, EN_CountrySelect> &> ( Event );
   QString sel = S.getData();
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   if (S.getContext() == ct)
   {
      mct = 0;
      CountryEntry *ce = MultLists::getMultLists() ->getCtryForPrefix( sel );
      startMatch(ce);
   }
   */
}

//---------------------------------------------------------------------------
void TMatchThread::DistrictSelect_Event ( /*MinosEventBase & Event*/ )
{
    /*
   ActionEvent2<QString,  BaseContestLog *, EN_DistrictSelect> & S = dynamic_cast<ActionEvent2<QString, BaseContestLog *, EN_DistrictSelect> &> ( Event );
   QString sel = S.getData();
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   if (S.getContext() == ct)
   {

   }
   */
}

//---------------------------------------------------------------------------
void TMatchThread::LocatorSelect_Event (/*MinosEventBase & Event*/)
{
    /*
   ActionEvent2<QString,  BaseContestLog *, EN_LocatorSelect> & S = dynamic_cast<ActionEvent2<QString, BaseContestLog *, EN_LocatorSelect> &> ( Event );
   QString sel = S.getData();
   BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
   if (S.getContext() == ct)
   {

   }
   */
}

//---------------------------------------------------------------------------
// limit the total  number of hits
const int MATCH_LIM = 20;
void TMatchThread::Execute()
{
   //---- Place thread code here ----
   //GJV_thread::setThreadName( "QSO Match" );
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
/*static*/ void TMatchThread::startMatch(   CountryEntry *ce )
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
   freeAll();
}
int TMatchCollection::getContactCount( void )
{
   return matchList.size();
}
MatchContact *TMatchCollection::pcontactAt( int i )
{
    if (i > matchList.size())
        return 0;
    return matchList.at( i );
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

   matchCollection->freeAll();

   // don't want to keep the old, so clear down the memory of selection terms
   matchcs.set( "" );
   matchloc.set( "" );
   matchqth.set( "" );
   ce = 0;
   thisContestMatched = 0;

   TMatchThread::getMatchThread() ->ShowThisMatchStatus( "" );
   TMatchThread::getMatchThread() ->ShowOtherMatchStatus( "" );
}
// mark things that we want to start a match
void Matcher::startMatch( CountryEntry *c )
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
            replaceList( matchCollection );
            return ;
         }

         if ( !changed & SET_CHANGED )
            return ;

         if ( changed & SET_NOT_GREATER )
         {
            replaceList( matchCollection );
         }
         // we have something matchable, so go to it!

         mp = Exact;		// set phase
      }
      else
      {
         CountryEntry *c = ce;	// preserve over cleardown
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
void ThisLogMatcher::addMatch( BaseContact *cct, BaseContestLog * ccon )
{
   if ( !cct )
      return ;

   MatchContact *mct = new MatchLogContact( ccon, cct );

   bool exists = std::binary_search( matchCollection->matchList.begin(), matchCollection->matchList.end(), mct );
   if ( !exists )
   {
      matchCollection->matchList.insert( mct );
   }
   else
   {
      delete mct;
   }
   thisContestMatched = matchCollection->matchList.size();
}
bool ThisLogMatcher::idleMatch( int limit )
{
    try
    {
       // this is called repeatedly off the timer to execute the match
       if ( !matchStarted || ( contestIndex < 0 ) )
          return false;				// nothing to do (yet)

       int cnt = matchCollection->matchList.size();
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
                            std::string smstr = matchcs.mstr.toStdString();
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
                             std::string smstrStart = matchcs.mstr.toStdString();
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
                replaceList( matchCollection );

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
          BaseContact *cct = ccon->pcontactAt( contactIndex++ );
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
                   TEMPBUFF( uprqth, EXTRALENGTH + 1 );
                   strcpysp( uprqth, cct->extraText.getValue(), EXTRALENGTH );
                   strupr( uprqth );
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
                if ( matchCollection->matchList.size() > MATCH_LIM )
                   break;
             }
          }
          else if (mp == Country)
          {
             if ( cct->ctryMult && ( cct->ctryMult == ce ) )
             {
                addMatch( cct, ccon );
                if ( matchCollection->matchList.size() > MATCH_LIM )
                   break;
             }

          }
          else if (mp == District)
          {
 /*            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
             {
                addMatch( cct, ccon );
                if ( matchCollection->matchList.size() > MATCH_LIM )
                   break;
             }
 */
          }
          else if (mp == Locator)
          {
 /*            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
             {
                addMatch( cct, ccon );
                if ( matchCollection->matchList.size() > MATCH_LIM )
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
void ThisLogMatcher::replaceList( TMatchCollection *matchCollection )
{
   TMatchThread::getMatchThread() ->replaceThisContestList( matchCollection );
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
void OtherLogMatcher::addMatch( BaseContact *cct, BaseContestLog * ccon )
{
   if ( !cct )
      return ;

   MatchContact *mct = new MatchLogContact( ccon, cct );

   bool exists = std::binary_search( matchCollection->matchList.begin(), matchCollection->matchList.end(), mct );
   if ( !exists )
   {
      matchCollection->matchList.insert( mct );
   }
   else
   {
      delete mct;
   }
}
bool OtherLogMatcher::idleMatch( int limit )
{
   try
   {
      // this is called repeatedly off the timer to execute the match
      if ( !matchStarted || ( contestIndex < 0 ) )
         return false;				// nothing to do (yet)

      int cnt = matchCollection->matchList.size();
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
                           std::string smstr = matchcs.mstr.toStdString();
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
                            std::string smstrStart = matchcs.mstr.toStdString();
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
               replaceList( matchCollection );

               QString buff = " - No match";

               TMatchThread::getMatchThread() ->ShowOtherMatchStatus( buff );
               contestIndex = -1;
               return false;
            }
         }

      ContestSlot *cs = TContestApp::getContestApp() ->contestSlotList.at( contestIndex );
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
         BaseContact *cct = ccon->pcontactAt( contactIndex++ );
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
                  TEMPBUFF( uprqth, EXTRALENGTH + 1 );
                  strcpysp( uprqth, cct->extraText.getValue(), EXTRALENGTH );
                  strupr( uprqth );
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
               if ( matchCollection->matchList.size() > MATCH_LIM )
                  break;
            }
         }
         else if (mp == Country)
         {
            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
            {
               addMatch( cct, ccon );
               if ( matchCollection->matchList.size() > MATCH_LIM )
                  break;
            }

         }
         else if (mp == District)
         {
/*            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
            {
               addMatch( cct, ccon );
               if ( matchCollection->matchList.size() > MATCH_LIM )
                  break;
            }
*/
         }
         else if (mp == Locator)
         {
/*            if ( cct->ctryMult && ( cct->ctryMult == ce ) )
            {
               addMatch( cct, ccon );
               if ( matchCollection->matchList.size() > MATCH_LIM )
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
void OtherLogMatcher::replaceList( TMatchCollection *matchCollection )
{
   TMatchThread::getMatchThread() ->replaceOtherContestList( matchCollection );
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

   MatchListContact *mct = new MatchListContact( ccon, cct );

   bool exists = std::binary_search( matchCollection->matchList.begin(), matchCollection->matchList.end(), mct );
   if ( !exists )
   {
      matchCollection->matchList.insert( mct );
   }
   else
   {
      delete mct;
   }
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

      int cnt = matchCollection->matchList.size();
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
                           std::string smatchcs = matchcs.mstr.toStdString();
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
                            std::string smstrStart = matchcs.mstr.toStdString();
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
               replaceList( matchCollection );

               contestIndex = -1;
               return false;
            }
         }

      ContactList * ccon;

      ListSlot *cs = TContestApp::getContestApp() ->listSlotList.at( contestIndex );
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
                  ListSlot * cs = TContestApp::getContestApp() ->listSlotList.at( contestIndex );
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
            if ( matchCollection->matchList.size() > MATCH_LIM )
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
void ListMatcher::replaceList( TMatchCollection *matchCollection )
{
   TMatchThread::getMatchThread() ->replaceListList( matchCollection );
}

