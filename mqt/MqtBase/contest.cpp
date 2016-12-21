/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"
#include "Calendar.h"
#include "CalendarList.h"

bool LtLogSeq::operator() ( const BaseContact* s1, const BaseContact* s2 ) const
{
   return s1->getLogSequence() < s2->getLogSequence();
}

BaseContestLog::BaseContestLog( void ) :
      protectedContest( false ), suppressProtected(false),  unwriteable(false),
      nextBlock( 1 ),
      unfilledCount(0),
      stanzaCount( 0 ), cslotno( -1 ), locValid( false ),
      ode( 0.0 ), odn( 0.0 ), sinodn( 0.0 ), cosodn( 0.0 ),
      allowLoc8( false ), allowLoc4 ( false ),
      RSTField( true ), serialField( true ), locatorField( true ), QTHField( true ),
      otherExchange( false ), countryMult( false ), nonGCountryMult( false ),
      districtMult( false ), locMult( false ), GLocMult(false),
      M7Mults(false), NonUKloc_mult(false), NonUKloc_multiplier(0), UKloc_mult(false), UKloc_multiplier(0),
      UKACBonus(false),
      scoreMode( PPKM ),
      powerWatts( true ), maxSerial( 0 ),
      contestScore( 0 ), ndistrict( 0 ), nctry( 0 ), nlocs( 0 ),
      validationPoint( 0 ),
      nextScan( -2 ),
      countryWorked( 0 ), districtWorked( 0 ),
      mycall( "" ),
      QSO1( 0 ), QSO2( 0 ), QSO1p( 0 ), QSO2p( 0 ),
      kms1( 0 ), kms2( 0 ), kms1p( 0 ), kms2p( 0 ),
      mults1( 0 ), mults2( 0 ), mults1p( 0 ), mults2p( 0 ),
      bonus1( 0 ), bonus2( 0 ), bonus1p( 0 ), bonus2p( 0 ),
      bonus(0), nbonus(0),
      bonusYearLoaded(0)
{
   bearingOffset.setValue(0);
   mode.setValue( "J3E" );
   int i;

   int nc = MultLists::getMultLists() ->getCtryListSize();
   countryWorked = new int[ nc ];
   for ( i = 0; i < nc; i++ )
      countryWorked[ i ] = 0;

   nc = MultLists::getMultLists() ->getDistListSize();
   districtWorked = new int[ nc ];
   for ( i = 0; i < nc; i++ )
      districtWorked[ i ] = 0;

}
BaseContestLog::~BaseContestLog()
{
   delete [] districtWorked;
   delete [] countryWorked;
   districtWorked = 0;
   countryWorked = 0;

   freeAll();
   closeFile();
}
void BaseContestLog::freeAll()
{
   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
      delete ( *i );
   ctList.clear();
}
unsigned int BaseContestLog::indexOf( BaseContact * item )
{
   LogIterator f = std::lower_bound( ctList.begin(), ctList.end(), item, LtLogSeq() );
   if ( f == ctList.end() || ( *f ) != item )
   {
      return ( ctList.end() - ctList.begin() );
   }
   unsigned int diff = f - ctList.begin();
   return diff;
}
int BaseContestLog::getContactCount( void )
{
   return ctList.size();
}

BaseContact *BaseContestLog::pcontactAt( unsigned int i )
{
   if ( i < ctList.size() )
   {
      return ctList.at( i );
   }
   return 0;
}

BaseContact *BaseContestLog::pcontactAtSeq( unsigned long logSequence )
{
   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
   {
      if ( ( *i ) ->getLogSequence() == logSequence )
         return ( *i );
   }
   return 0;
}
void BaseContestLog::clearDirty()
{
   protectedContest.clearDirty();
   mycall.fullCall.clearDirty();

   name.clearDirty();
   location.clearDirty();
   myloc.loc.clearDirty();
   allowLoc4.clearDirty();
   allowLoc8.clearDirty();
   RSTField.clearDirty();
   serialField.clearDirty();
   locatorField.clearDirty();
   QTHField.clearDirty();
   power.clearDirty();
   mode.clearDirty();
   band.clearDirty();
   otherExchange.clearDirty();
   countryMult.clearDirty();
   nonGCountryMult.clearDirty();
   locMult.clearDirty();
   GLocMult.clearDirty();
   districtMult.clearDirty();

   M7Mults.clearDirty();
   UKACBonus.clearDirty();

   powerWatts.clearDirty();
   scoreMode.clearDirty();
   DTGStart.clearDirty();
   DTGEnd.clearDirty();
   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
   {
      ( *i ) ->clearDirty();
   }
}
void BaseContestLog::setDirty()
{
   protectedContest.setDirty();
   mycall.fullCall.setDirty();
   name.setDirty();
   location.setDirty();
   myloc.loc.setDirty();
   allowLoc4.setDirty();
   allowLoc8.setDirty();
   RSTField.setDirty();
   serialField.setDirty();
   locatorField.setDirty();
   QTHField.setDirty();
   power.setDirty();
   mode.setDirty();
   band.setDirty();
   otherExchange.setDirty();
   countryMult.setDirty();
   nonGCountryMult.setDirty();
   locMult.setDirty();
   GLocMult.setDirty();
   districtMult.setDirty();

   M7Mults.setDirty();
   UKACBonus.setDirty();

   powerWatts.setDirty();
   scoreMode.setDirty();
   DTGStart.setDirty();
   DTGEnd.setDirty();
   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
   {
      ( *i ) ->setDirty();
   }
}
void BaseContestLog::makeContact( bool timeNow, BaseContact *&lct )
{
   lct = new BaseContact( this, timeNow );
}
void BaseContestLog::validateLoc( void )
{
   if ( myloc.validate( ode, odn ) == LOC_OK )
   {
      cosodn = cos( odn );
      sinodn = sin( odn );
      locValid = true;
   }
   else
      locValid = false;

}
/*********************************************************************/
/* calculate distance and bearing of contact from our station        */
/*                                                                   */
/* uses global double variables odn, ode, sinodn, and cosodn, and pi */
/*                                                                   */
/* pi must be set to value of pi before calling                      */
/* our station longtitude and latitude must be set up in ode and odn */
/* before invoking this routine.   Also, cos(odn) must be set up in  */
/* cosodn and sin(odn) in sindodn.                                   */
/*                                                                   */
/* Latitude is NS, Longtitude is EW                                  */
/*                                                                   */
/*********************************************************************/
void BaseContestLog::disbear( double lon, double lat, double &dist, int &brg ) const
//double lon ;                    /* other stations longtitude */
//double lat ;                    /* other stations latitude */
//int *dist ;                     /* resulting distance */
//int *brg ;                      /* resulting bearing */

{
   if ( myloc.valRes != LOC_OK )
   {
      dist = 1;
      brg = 0;
      return ;
   }
   if ( lon == ode && lat == odn )        /* same square ! */ /* testing doubles for equality! */
   {
      // same square - commenced Kilometre, so always 1
      dist = 1;
      brg = 0 ;                       /* or bearing */
      return ;                         /* just exit */
   }

   double coslat = cos( lat );                    /* pre-calculate values */
   double sinlat = sin( lat );
   double coscos = cosodn * coslat ;

   // first distance into dx

   double co = cos( ode - lon ) * coscos + sinodn * sinlat ;
   double ca = atan( fabs( sqrt( 1.0 - co * co ) / co ) );
   if ( co < 0.0 )
      ca = pi - ca ;
   double dx = 6371.291 * ca ;       /* 6371.291 is approved radius of earth */

   // and then the bearing

   double si = sin( lon - ode ) * coscos ;
   co = sinlat - sinodn * cos( ca );
   double az = atan( fabs( si / co ) );
   if ( co < 0.0 )
      az = pi - az ;
   if ( si <  0.0 )
      az = -az ;
   if ( az < 0.0 )
      az = az + 2.0 * pi ;

   az = az / dr ;                      /* convert to degrees */
   az += 0.5 ;                /* correct angle */
   dx = ceil( dx );                      // adjust for commenced kilometer
   dx += 0.5 ;              // make sure double truncates properly back to int
   dist = dx ;			                  /* return result */
   if ( static_cast<int>(az) == 0 )                  /* due north */
      az = 360.00 ;                 	/* so show valid */
   brg = static_cast< int > (az) ;                   /* and give it back as integer */
}
//---------------------------------------------------------------------------
bool BaseContestLog::getsdist( const char *loc, char *minloc, double &mindist )
{
   int brg;
   double dist = 0.0;
   double lon = 0.0;
   double lat = 0.0;

   int lres = lonlat( loc, lon, lat );
   if ( lres == LOC_OK || lres == LOC_SHORT )
   {
      disbear( lon, lat, dist, brg );
      if ( dist < mindist )
      {
         mindist = dist;
         strcpy( minloc, loc );
      }
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------
int BaseContestLog::CalcNearest( const QString &qscalcloc )
{
    std::string scalcloc = qscalcloc.toStdString(); // allowed conversion through std::string
   const char * calcloc = scalcloc.c_str();

   if ( scalcloc.size() != 4 )
      return 0;	// only valid 4 fig locs

   // calculate the nearest point of loc2 from loc1

   double mindist = 1000000.0;

   char minloc[ LOCLENGTH + 1 ] = {0};
   char temploc[ LOCLENGTH + 1 ];

   strncpy( temploc, calcloc, LOCLENGTH );
   temploc[ LOCLENGTH] = 0;

   for ( char i = 'A'; i <= 'X'; i++ )
   {
      temploc[ 4 ] = 'A';
      temploc[ 5 ] = i;
      temploc[ 6 ] = 0;
      if ( !getsdist( temploc, minloc, mindist ) )
      {
         return -1;
      }

      temploc[ 4 ] = 'X';

      if ( !getsdist( temploc, minloc, mindist ) )
      {
         return -1;
      }

      temploc[ 4 ] = i;
      temploc[ 5 ] = 'X';
      if ( !getsdist( temploc, minloc, mindist ) )
      {
         return -1;
      }

      temploc[ 5 ] = 'A';
      if ( !getsdist( temploc, minloc, mindist ) )
      {
         return -1;
      }
   }
   return mindist;
}
void BaseContestLog::getMatchText( BaseContact *pct, QString &disp, const BaseContestLog *const ct ) const
{
   if ( DupSheet.isCurDup( pct ) )
   {
      disp = "DUPLICATE OF ";
   }
   QString temp;
   pct->getText( temp, ct );
   disp += temp;

   disp = disp.trimmed();
}
bool BaseContestLog::isCurDup( BaseContact *pct) const
{
   return pct && DupSheet.isCurDup( pct );
}
void BaseContestLog::getMatchField(BaseContact *pct, int col, QString &disp, const BaseContestLog *const ct ) const
{
   if ( col ==0 && isCurDup( pct ) )
   {
      disp = "DUP OF";
      return ;
   }
   QString temp;
   if ( pct )
      temp = pct->getField( col, ct );

   disp = temp.trimmed();
}
bool BaseContestLog::updateStat( BaseContact *cct )
{
   // need to check if a valid DTG
   bool acted = false;

   QDateTime cttime;

   if ( ( cct->contactScore.getValue() <= 0 ) || !cct->time.getDtg( cttime ) )
      return true;

   QDateTime t = QDateTime::currentDateTimeUtc().addSecs( MinosParameters::getMinosParameters() ->getBigClockCorrection());

   int tdiff = cttime.secsTo(t);
   if ( tdiff < 0 )
      return true;

   long thisscore = cct->contactScore.getValue();
   switch ( scoreMode.getValue() )
   {
      case PPKM:
         break;

      case PPQSO:
         if ( thisscore > 0 )
            thisscore = 1;
         else
            thisscore = 0;
         break;

   }
   int sp1 = MinosParameters::getMinosParameters() ->getStatsPeriod1();
   int sp2 = MinosParameters::getMinosParameters() ->getStatsPeriod2();

   // find the time since the beginning of the contest

   QDateTime  contestStart = CanonicalToTDT(DTGStart.getValue());
   int fromContestStart = contestStart.secsTo(QDateTime::currentDateTime());
   if (sp1 > fromContestStart/120)
   {
      sp1 = fromContestStart/120;
   }
   if (sp2 > fromContestStart/120)
   {
      sp2 = fromContestStart/120;
   }

   if ( tdiff < sp1 * 60L )
   {
      // need a common routine
      // This period
      QSO1++;
      kms1 += thisscore;
      mults1 += cct->multCount;
      bonus1 += cct->bonus;
      acted = true;
   }
   else
      if ( tdiff < sp1 * 60L * 2 )
      {
         // need a common routine
         // previous period
         QSO1p++;
         kms1p += thisscore;
         mults1p += cct->multCount;
         bonus1p += cct->bonus;
         acted = true;
      }

   if ( tdiff < sp2 * 60L )
   {
      // need a common routine
      // this period
      QSO2++;
      kms2 += thisscore;
      mults2 += cct->multCount;
      bonus2 += cct->bonus;
      acted = true;
   }
   else
      if ( tdiff < sp2 * 60L * 2 )
      {
         // need a common routine
         // previous period
         QSO2p++;
         kms2p += thisscore;
         mults2p += cct->multCount;
         bonus2p += cct->bonus;
         acted = true;
      }

   return acted;
}
void BaseContestLog::updateStats( void )
{
   QSO1 = 0;
   kms1 = 0;
   mults1 = 0;
   QSO2 = 0;
   kms2 = 0;
   mults2 = 0;
   QSO1p = 0;
   kms1p = 0;
   mults1p = 0;
   QSO2p = 0;
   kms2p = 0;
   mults2p = 0;
   bonus1 = 0;
   bonus1p = 0;
   bonus2 = 0;
   bonus2p = 0;
   for ( int i = getContactCount() - 1; i >= 0; i-- )
   {
      if ( !updateStat( ctList[ i ] ) )
         break;
   }
}
int BaseContestLog::getValidQSOs()
{
   int nvalid = 0;
   for ( unsigned int i = 0; i < ctList.size(); i++ )
   {
      BaseContact *dct = ctList[ i ];

      if ( dct->contactFlags.getValue() & ( LOCAL_COMMENT | COMMENT_ONLY | DONT_PRINT ) )
         continue;

      if ( dct->contactScore.getValue() > 0 )
         nvalid++;
   }
   return nvalid;
}

static void isBestDX( BaseContact *cct, BaseContact **bestDX )
{

   if ( cct->contactFlags.getValue() & ( NON_SCORING | COMMENT_ONLY | LOCAL_COMMENT | DONT_PRINT ) )
      return ;

   if ( cct->cs.valRes != CS_OK )
      return ;

   if ( ( !*bestDX ) || ( ( cct->contactScore.getValue() > ( *bestDX ) ->contactScore.getValue() ) ) )
      * bestDX = cct;
}
BaseContact *BaseContestLog::getBestDX( void )
{
   BaseContact * bestDX = 0;
   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
      isBestDX( ( *i ), &bestDX );
   return bestDX;
}
QString BaseContestLog::dateRange( DTG dstyle )
{
   QString date1 = "999999";
   QString date2 = "000000";
   LogIterator low = ctList.end();
   LogIterator high = ctList.end();
   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
   {
      if ( ( *i ) ->contactScore.getValue() > 0 )
      {
         QString qsodate = ( *i ) ->time.getDate( DTGLOG );
         if ( qsodate < date1 )
         {
            low = i;
            date1 = ( *i ) ->time.getDate( DTGLOG );
         }
         if ( qsodate > date2 )
         {
            high = i;
            date2 = ( *i ) ->time.getDate( DTGLOG );
         }
      }
   }
   if ( low == ctList.end() || high == ctList.end() )
   {
      return "";
   }
   return ( *low ) ->time.getDate( dstyle ) + ";" + ( *high ) ->time.getDate( dstyle );
}

void BaseContestLog::setScore( QString &buff )
{
   ContestScore cs(this, QDateTime::currentDateTime());
   getScoresTo(cs, QDateTime::currentDateTime());
   buff = cs.disp();
}
// and we need to do this a bit more often to pick up unfilled properly
void BaseContestLog::scanContest( void )
{
   DupSheet.clear();

   locs.llist.clear();

   delete [] districtWorked;
   delete [] countryWorked;
   districtWorked = 0;
   countryWorked = 0;

   int nc = MultLists::getMultLists() ->getDistListSize();
   districtWorked = new int[ nc ];
   memset( districtWorked, 0, nc * sizeof( int ) );

   nc = MultLists::getMultLists() ->getCtryListSize();
   countryWorked = new int[ nc ];
   memset( countryWorked, 0, nc * sizeof( int ) );

   // set up for the idle loop scan
   // NB we may need to clear e.g. the accumulated score

   // we will need to clear the multiplier work counts


   contestScore = 0;
   ndistrict = 0;
   nctry = 0;
   nlocs = 0;

   nextScan = -1;
   unfilledCount = 0;

//   oplist.clear();
   QString curop1 = currentOp1.getValue();
   oplist.insert( curop1 );
   QString curop2 = currentOp2.getValue();
   oplist.insert( curop2 );
   while ( nextScan >= -1 )
   {
      // get the next contact in sequence and do any required scan checks
      nextScan++;
      if ( nextScan >= getContactCount() )
      {
         // end of scan

         nextScan = -2;

         if ( isReadOnly() )
         {
            DupSheet.clear();
         }

         break;
      }
      BaseContact *nct = ctList[ nextScan ];
      if ( !nct )
         break ;

      if (nct->contactFlags.getValue() & TO_BE_ENTERED)
      {
         unfilledCount++;
      }
      QString temp = nct->op1.getValue();
      if (temp.size())
      {
         curop1 = temp;
         oplist.insert( curop1 );
      }
      curop2 = nct->op2.getValue();
      oplist.insert( curop2 );

      if ( nct->contactFlags.getValue() & ( NON_SCORING | DONT_PRINT | LOCAL_COMMENT | COMMENT_ONLY ) )
      {
         nct->contactScore.setValue( -1 );		// force it!
         continue;
      }

      validationPoint = nct;

      // check for duplicates; accumulate the current points score

      nct->cs.valRes = CS_NOT_VALIDATED;
      nct->cs.validate( );

      nct->bearing = -1;		// force a recalc
      nct->loc.validate();

      //int index;

      if ( DupSheet.checkCurDup( nct, 0, true ) )    // check for dup, insert it if required
         nct->cs.valRes = ERR_DUPCS;


      nct->multCount = 0;
      nct->newDistrict = false;
      nct->newCtry = false;
      nct->locCount = 0;
      nct->newGLoc = false;
      nct->newNonGLoc = false;
      nct->bonus = 0;
      nct->newBonus = false;
      nct->checkContact( );   // in scanContest

//      nct->baddtg = false;
      if (nct->time.notEntered() == 0 && !(nct->contactFlags.getValue() & TO_BE_ENTERED))
      {
         nct->time = nct->getHistory()[0]->updtime;
         nct->time.clearDirty();
         nct->time.setBadDtg();
      }
   }
   if (currentOp1.getValue().size() == 0)
   {
      currentOp1.setValue( curop1 );
   }
}
void BaseContestLog::getScoresTo(ContestScore &cs, QDateTime limit)
{
// #warning doesn't pick up invalid QSOs - e.g. needs district and doesn't have one
   cs.nctry = 0;
   cs.ndistrict = 0;
   cs.nlocs = 0;
   cs.nGlocs = 0;
   cs.nonGlocs = 0;
   cs.nqsos = 0;
   cs.contestScore = 0;
   cs.bonus = 0;
   cs.nbonus = 0;

   int nextScan = -1;

   while ( nextScan >= -1 )
   {
      // get the next contact in sequence and do any required scan checks
      nextScan++;
      if ( nextScan >= getContactCount() )
      {
         // end of scan

         nextScan = -2;

         break;
      }
      BaseContact *nct = ctList[ nextScan ];
      if ( !nct )
         break ;

// NB this doesn't cope with crazy times from test contests and QSOs

      QDateTime start = CanonicalToTDT( DTGStart.getValue() );

      QString dtgstr = nct->time.getDate(DTGFULL) + nct->time.getTime(DTGLOG);
      QDateTime ncheck = CanonicalToTDT( dtgstr );

//      int elapsed = start.secsTo(ncheck);
      if (ncheck > limit)
      {
         nextScan = -2; // continue; we want to include this one
         break;
      }


      if ( nct->contactFlags.getValue() & ( NON_SCORING | DONT_PRINT | LOCAL_COMMENT | COMMENT_ONLY | TO_BE_ENTERED ) )
      {
         //trace(QString("flags ") + nct->cs.fullCall.getValue() + " " + nct->serials.getValue());
         continue;
      }
      if (nct->cs.valRes != CS_OK)
      {
         //trace(QString("bad callsign ") + nct->cs.fullCall.getValue() + " " + nct->serials.getValue());
         continue;
      }

     if ( locatorField.getValue() || nct->contactScore.getValue() >= 0 )   		// don't add -1 scores in, but DO add zero km
         // as it is 1 point.
      {
         int cscore = nct->contactScore.getValue();
         switch ( scoreMode.getValue() )
         {
            case PPKM:
               {
                  if ( nct->contactFlags.getValue() & XBAND )
                  {
                     cscore = ( cscore + 1 ) / 2;
                  }
                  cs.contestScore += cscore;
               }
               break;

            case PPQSO:
               if ( cscore > 0 )
                  cs.contestScore++;
               break;

         }
         cs.nctry += nct->newCtry?1:0;
         cs.ndistrict += nct->newDistrict?1:0;
         cs.nlocs += nct->locCount;
         cs.nqsos++;

         cs.bonus += nct->bonus;
         cs.nbonus += nct->newBonus?1:0;

         if (nct->newGLoc)
         {
            cs.nGlocs++;
         }
         else if ((nct->newNonGLoc))
         {
            cs.nonGlocs++;
         }
      }
      else
      {
         trace(QString("neg score ") + nct->cs.fullCall.getValue() + " " + nct->serials.getValue());
      }
   }
   cs.nmults = 0;
   if ( countryMult.getValue() )
   {
      cs.brcc1 = cs.brcc2 = ' ';
      cs.nmults += cs.nctry;
   }
   if ( districtMult.getValue() )
   {
      cs.brcc3 = cs.brcc4 = ' ';
      cs.nmults += cs.ndistrict;
   }
   if ( locMult.getValue() )
   {
      cs.brloc1 = cs.brloc2 = ' ';
      cs.nmults += cs.nlocs;
   }
   if (UKACBonus.getValue())
   {
      cs.brbonus1 = cs.brbonus2 = ' ';
   }
   cs.nmults = std::max(cs.nmults, 1);

   cs.totalScore = (cs.contestScore + cs.bonus)*cs.nmults;

}
//============================================================
DupContact::DupContact( BaseContact *c ) : dct( c ), sct( 0 )
{}
DupContact::DupContact( ScreenContact *c ) : dct( 0 ), sct( c )
{}
DupContact::DupContact() : dct( 0 ), sct( 0 )
{}
DupContact::~DupContact()
{}
bool DupContact::operator<( const DupContact& rhs ) const
{
   callsign * c1 = 0;    // search item
   callsign *c2 = 0;    // collection item

   if ( dct )
   {
      c1 = &dct->cs;
   }
   else
      if ( sct )
      {
         c1 = &sct->cs;
      }
   if ( rhs.dct )
   {
      c2 = &rhs.dct->cs;
   }
   else
      if ( rhs.sct )
      {
         c2 = &rhs.sct->cs;
      }

   int ret = c1->prefix2.compare(c2->prefix2, Qt::CaseInsensitive );
   if ( ret == 0 )
   {
      ret = c1->number.compare(c2->number, Qt::CaseInsensitive );
      if ( ret == 0 )
      {
         ret = c1->body.compare( c2->body, Qt::CaseInsensitive );
         if ( ret == 0 )
            return false;
      }
   }
   return ret < 0;
}
bool DupContact::operator==( const DupContact& rhs ) const
{
   callsign * c1 = 0;    // search item
   callsign *c2 = 0;    // collection item

   if ( dct )
   {
      c1 = &dct->cs;
   }
   else
      if ( sct )
      {
         c1 = &sct->cs;
      }
   if ( rhs.dct )
   {
      c2 = &rhs.dct->cs;
   }
   else
      if ( rhs.sct )
      {
         c2 = &rhs.sct->cs;
      }

   int ret = c1->prefix2.compare(c2->prefix2, Qt::CaseInsensitive );
   if ( ret != 0 )
      return false;

   ret = c1->number.compare(c2->number, Qt::CaseInsensitive );
   if ( ret != 0 )
      return false;

   ret = c1->body.compare( c2->body, Qt::CaseInsensitive );
   if ( ret != 0 )
      return false;

   return true;
}
bool DupContact::operator!=( const DupContact& rhs ) const
{
   return !( *this == rhs );
}

bool LtDup::operator() ( const DupContact* s1, const DupContact* s2 ) const
{
   bool res = ( *s1 < *s2 );
   return res;
}

dupsheet::dupsheet()
{}
dupsheet::~dupsheet()
{
   clear();
}
bool dupsheet::checkCurDup( ScreenContact *nct, BaseContact *valp, bool insert )
{
   curdup = 0;
   if ( nct->cs.valRes == CS_OK )
   {
      DupContact test( nct );
      bool exists = std::binary_search( ctList.begin(), ctList.end(), &test, LtDup() );
      if ( exists )
      {
         if ( !( nct->contactFlags & VALID_DUPLICATE ) )
         {
            DupIterator c = std::lower_bound( ctList.begin(), ctList.end(), &test, LtDup() );

            if ( valp && valp->getLogSequence() <= ( *c ) ->dct->getLogSequence() )
            {
               return false; // as val point earlier than current list item
            }

            if ( c != ctList.end() )
               curdup = *c;

            return true;
         }
      }
      else
         if ( insert )
         {
            DupContact * ins = new DupContact( nct );
            ctList.insert( ins );
            return false;
         }
   }
   return false;
}
bool dupsheet::checkCurDup( BaseContact *nct, BaseContact *valp, bool insert )
{
   curdup = 0;
   if ( nct->cs.valRes == CS_OK )
   {
      DupContact test( nct );
      bool exists = std::binary_search( ctList.begin(), ctList.end(), &test, LtDup() );
      if ( exists )
      {
         if ( !( nct->contactFlags.getValue() & VALID_DUPLICATE ) )
         {
            DupIterator c = std::lower_bound( ctList.begin(), ctList.end(), &test, LtDup() );

            if ( valp && valp->getLogSequence() <= ( *c ) ->dct->getLogSequence() )
            {
               return false; // as val point earlier than current list item
            }

            if ( c != ctList.end() )
               curdup = *c;

            return true;
         }
      }
      else
         if ( insert )
         {
            DupContact * ins = new DupContact( nct );
            ctList.insert( ins );
            return false;
         }
   }
   return false;
}

bool dupsheet::isCurDup( ScreenContact *nct ) const
{
   const DupContact test( nct );
   bool cd = ( curdup && ( *curdup == test ) ) ;

   if ( cd && !nct->isNextContact() )
   {
      if ( nct->getLogSequence() > curdup->dct->getLogSequence() )
      {
         return false;
      }
   }
   return cd;
}
bool dupsheet::isCurDup(BaseContact *nct ) const
{
   const DupContact test( nct );
   bool cd = curdup && ( *curdup == test ) ;

   if ( cd )
   {
      if ( nct->getLogSequence() > curdup->dct->getLogSequence() )
      {
         return false;
      }
   }
   return cd;
}
void dupsheet::clearCurDup()
{
   curdup = 0;
}
BaseContact *dupsheet::getCurDup()
{
   if ( curdup )
      return curdup->dct;
   return 0;
}
void dupsheet::clear()
{
   curdup = 0;
   for ( DupIterator i = ctList.begin(); i != ctList.end(); i++ )
      delete ( *i );
   ctList.clear();
}
//============================================================
void BaseContestLog::processMinosStanza( const QString &methodName, MinosTestImport * const mt )
{
   unsigned long logSequence = static_cast< unsigned long > (- 1);

   int itemp;
   if ( mt->getStructArgMemberValue( "lseq", itemp ) )
      logSequence = static_cast< unsigned long > (itemp);

   if ( methodName == "MinosLogContest" )
   {
      mt->getStructArgMemberValue( "name", name );
      mt->getStructArgMemberValue( "band", band );

      bool btemp;
      if ( mt->getStructArgMemberValue( "scoreKms", btemp ) )
         scoreMode.setInitialValue( btemp ? PPKM : PPQSO );

      mt->getStructArgMemberValue( "startTime", DTGStart );
      mt->getStructArgMemberValue( "endTime", DTGEnd );

      mt->getStructArgMemberValue( "districtMult", districtMult );
      mt->getStructArgMemberValue( "DXCCMult", countryMult );
      mt->getStructArgMemberValue( "NonGCtryMult", nonGCountryMult );
      mt->getStructArgMemberValue( "locMult", locMult );
      mt->getStructArgMemberValue( "GLocMult", GLocMult );
      mt->getStructArgMemberValue( "QTHReq", otherExchange );
      mt->getStructArgMemberValue( "AllowLoc4", allowLoc4 );
      mt->getStructArgMemberValue( "AllowLoc8", allowLoc8 );

      mt->getStructArgMemberValue( "UKACBonus", UKACBonus );
      if (UKACBonus.getValue())
      {
          loadBonusList();
      }
      mt->getStructArgMemberValue( "M7Mults", M7Mults);

      if ( M7Mults.getValue())
      {
         NonUKloc_mult = true;
         NonUKloc_multiplier = 1;
         UKloc_mult = true;
         UKloc_multiplier = 2;
      }
      else
      {
         if (locMult.getValue())
         {
            UKloc_multiplier = 1;
            UKloc_mult = true;
            if (GLocMult.getValue())
            {
               NonUKloc_multiplier = 0;
               NonUKloc_mult = false;
            }
            else
            {
               NonUKloc_multiplier = 1;
               NonUKloc_mult = true;
            }
         }
         else
         {
            UKloc_multiplier = 0;
            UKloc_mult = false;
            NonUKloc_multiplier = 0;
            NonUKloc_mult = false;
         }
      }

   }
   else
      if ( methodName == "MinosLogMode" )
      {
         mt->getStructArgMemberValue( "protectedContest", protectedContest );
      }
      else
         if ( methodName == "MinosLogQTH" )
         {
            if ( mt->getStructArgMemberValue( "locator", myloc.loc ) )
               validateLoc();
            mt->getStructArgMemberValue( "district", location );
            mt->getStructArgMemberValue( "location", location ); // doubled up...
         }
         else
            if ( methodName == "MinosLogEntry" )
            {
               mt->getStructArgMemberValue( "call", mycall.fullCall );
            }
            else
               if ( methodName == "MinosLogStation" )
               {
                  mt->getStructArgMemberValue( "power", power );
               }
               else
                  if ( methodName == "MinosLogOperators" )
                  {}
                  else
                     if ( methodName == "MinosLogCurrent" )
                     {}
                     else
                        if ( methodName == "MinosLogBundles" )
                        {}
                        else
                           if ( methodName == "MinosLogComment" )
                           {
                              BaseContact * rct = pcontactAtSeq( logSequence );
                              if ( !rct )
                              {
                                 makeContact( false, rct );
                                 rct->setLogSequence( logSequence );
                                 ctList.insert( rct );
                                 if (logSequence >> 16 >= nextBlock)
                                 {
                                    nextBlock = (logSequence >> 16) + 1;
                                 }
                              }
                              rct->processMinosStanza( methodName, mt );

                           }
                           else
                              if ( methodName == "MinosLogQSO" )
                              {
                                 BaseContact * rct = pcontactAtSeq( logSequence );
                                 if ( !rct )
                                 {
                                    makeContact( false, rct );
                                    rct->setLogSequence( logSequence );
                                    ctList.insert( rct );
                                    // Was just nextBlock++ - no test
                                    if (logSequence >> 16 >= nextBlock)
                                    {
                                       nextBlock = (logSequence >> 16) + 1;
                                    }
                                 }
                                 rct->processMinosStanza( methodName, mt );

                              }
}

//====================================================================
void BaseContestLog::setStanza( unsigned int /*stanza*/, int /*stanzaStart*/ )
{
   // not used in base log
   // Used in Logger to give file position of stanza
}
//====================================================================
bool BaseContestLog::getStanza( unsigned int /*stanza*/, QString & /*stanzaData*/ )
{
   // not used in base log
   // used in Logger to get stanza direct from a file
   return false;
}
//====================================================================
BaseContact * BaseContestLog::findNextUnfilledContact()
{
   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
   {
      if ( ( *i ) ->contactFlags.getValue() & TO_BE_ENTERED )
      {
         return ( *i );
      }
   }
   return 0;
}
//====================================================================
bool BaseContestLog::checkTime(const dtg &t) const
{
//   try
//   {
      QString dtgstr = t.getDate(DTGFULL) + t.getTime(DTGLOG);

      QDateTime check = CanonicalToTDT( dtgstr );
      QString t1 = DTGStart.getValue();
      QDateTime start = CanonicalToTDT( t1 );
      t1 = DTGEnd.getValue();
      QDateTime end = CanonicalToTDT( t1 );

      if (check < start)
      {
         return false;
      }
      if (check >= end)
      {
         return false;
      }
      return true;
//   }
//   catch (EConvertError & /*e*/)
//   {
//      return false;
//   }
}
static bool loadCalYear ( Calendar &cal, int year )
{
    bool loaded = false;
    QVector<QSharedPointer<CalendarYear> > yearList;
    for ( int i = LOWYEAR; i <= HIGHYEAR; i++ )
    {
        yearList.push_back ( QSharedPointer<CalendarYear> ( new VHFCalendarYear ( i ) ) );
    }

    for ( int i = yearList.size() - 1; i >= 0; i-- )
    {
        if ( !loaded && FileExists ( yearList[ i ] ->getPath() ) && year >= calendarFormYear + yearList[ i ] ->yearOffset )
        {
            loaded = cal.parseFile ( yearList[ i ] ->getPath() );
        }
    }
    return loaded;
}
void BaseContestLog::loadBonusList()
{

    QDateTime  contestStart = CanonicalToTDT(DTGStart.getValue());
    int year = contestStart.date().year();
    if (year != bonusYearLoaded)
    {

        Calendar vhf(year, ectVHF);
        bool loaded = loadCalYear ( vhf, year );

        if (loaded)
        {
            bonusYearLoaded = year;
        }

        MultType B2 = vhf.mults["B2"];

        if (B2.bonuses.size() == 0)
        {
            // load from ./Configuration/B2Mults.xml
            vhf = Calendar(year, ectVHF);
            loaded = vhf.parseFile ( "./Configuration/B2Mults.xml" );
            B2 = vhf.mults["B2"];
        }

        locBonuses.clear();

        for (QMap<QString, int>::iterator i = B2.bonuses.begin(); i != B2.bonuses.end(); i++)
        {
            QString name = i.key();
            int value = i.value();

            locBonuses[name] = value;
        }
    }
}
int BaseContestLog::getSquareBonus(QString sloc)
{
    int bonus = 0;
    QMap<QString, int>::iterator l = locBonuses.find(sloc);

    if ( l != locBonuses.end())
    {
       // specific bonus for square allocated
       bonus = l.value();
    }
    else
    {
        QMap<QString, int>::iterator l = locBonuses.find("DEFAULT");
        if ( l != locBonuses.end())
        {
           // specific bonus for square allocated
           bonus = l.value();
        }
    }
    return bonus;
}

//====================================================================
ContestScore::ContestScore(BaseContestLog *ct, QDateTime limit)
{
   brcc1 = '(';
   brcc2 = ')';
   brcc3 = '(';
   brcc4 = ')';
   brloc1 = '(';
   brloc2 = ')';
   brbonus1 = '(';
   brbonus2 = ')';

   ct->getScoresTo(*this, limit);
   name = ct->publishedName;
   UKACBonus = ct->UKACBonus.getValue();
}
QString ContestScore::disp()
{
    QString buff;
    if (UKACBonus == true)
    {
        buff = QString( "Score: Qsos: %1; %2 pts :%3%4 countries%5: bonuses %6(%7) = %8" )
            .arg(nqsos).arg(contestScore).arg(brcc1).arg(nctry).arg(brcc2)
            .arg(bonus) .arg(nbonus)
            .arg(totalScore );
    }
    else
    {
        buff = QString( "Score: Qsos: %1; %2 pts :%3%4 countries%5:%6%7 districts%8:%9%10(%11/%12) locators %13 = %14" )
            .arg(nqsos).arg(contestScore).arg(brcc1).arg(nctry).arg(brcc2).arg(brcc3).arg(ndistrict)
            .arg(brcc4).arg(brloc1).arg(nlocs).arg(nGlocs).arg(nonGlocs).arg(brloc2)
//            % brbonus1 % bonus % nbonus % brbonus2
            .arg(totalScore );    }
   return buff;
}
//====================================================================

