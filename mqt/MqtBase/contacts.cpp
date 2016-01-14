/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

//============================================================
namespace ContactBuffs
{
    QString scorebuff;
    QString brgbuff;
    QString buff2;
    QString qthbuff;
    QString srbuff;
    QString ssbuff;
    QString buff;
}
//==========================================================================
//==========================================================================
BaseContact::BaseContact( BaseContestLog * contest, bool time_now ) :
      contest( contest ), contactScore( -1 ), time( time_now ), updtime( time_now ),
      contactFlags( 0 ), multCount( 0 ),
      bearing( -1 ),
      districtMult( 0 ), ctryMult( 0 ),
      QSOValid( false ),
      locCount( 0 ), newGLoc(false), newNonGLoc(false), newDistrict( false ), newCtry( false ),
      bonus(0), newBonus(false)
{
}
BaseContact::BaseContact( const BaseContact &ct )
      : time( false ), updtime( false )
{
   *this = ct;
}
BaseContact& BaseContact::operator =( const BaseContact &ct )
{
   contest = ct.contest;
   updtime = ct.updtime;      //CONTAIN MinosItem
   cs = ct.cs;   //CONTAIN MinosItem
   loc = ct.loc;   //CONTAIN MinosItem
   time = ct.time;      //CONTAIN MinosItem

   extraText = ct.extraText;
   mode = ct.mode;
   reps = ct.reps;
   serials = ct.serials;
   repr = ct.repr;
   serialr = ct.serialr;
   comments = ct.comments;
   contactFlags = ct.contactFlags;
   forcedMult = ct.forcedMult;

   op1 = ct.op1;         // current main op - derived from contacts
   op2 = ct.op2;         // current second op - derived from contacts

   contactScore = ct.contactScore;

   return *this;
}
//==========================================================================
//==========================================================================
void BaseContact::clearDirty()
{
   cs.fullCall.clearDirty();
   loc.loc.clearDirty();
   time.clearDirty();

   extraText.clearDirty();
   mode.clearDirty();
   reps.clearDirty();
   serials.clearDirty();
   repr.clearDirty();
   serialr.clearDirty();
   comments.clearDirty();
   contactFlags.clearDirty();
   forcedMult.clearDirty();
   op1.clearDirty();
   op2.clearDirty();
   contactScore.clearDirty();
}
void BaseContact::setDirty()
{
   cs.fullCall.setDirty();
   loc.loc.setDirty();
   time.setDirty();

   extraText.setDirty();
   mode.setDirty();
   reps.setDirty();
   serials.setDirty();
   repr.setDirty();
   serialr.setDirty();
   comments.setDirty();
   contactFlags.setDirty();
   forcedMult.setDirty();
   op1.setDirty();
   op2.setDirty();
   contactScore.setDirty();
}
//==========================================================================
CountryEntry *findCtryPrefix( const callsign &cs )
{
   CountryEntry * ctryMult;

   QString testpart = "/";	// look for e.g. /RVI as a country suffix
   testpart += cs.suffix;	// look for e.g. /RVI as a country suffix

   CountrySynonym *csyn = 0;
   if ( cs.suffix.length() )
      csyn = MultLists::getMultLists() ->searchCountrySynonym( testpart );

   if ( !csyn )   	// look with number
   {
      /*
      		// eg for <pe/f0ctt/mm> (g0gjv/p) [F6CTT/RVI/P] ?F6CTT/RVI?
      		char prefix[BITLENGTH + 1]; // <pe> (g) [RVI] ???? country of location
      		char prefix2[BITLENGTH + 1];  // <f> (g) [F] ?F?country of issue
      		char number[NUMBITLENGTH + 1]; // <0> (0) [6] ?6?numeric part
      		char body[BITLENGTH + 1];  // <ctt> (gjv) [CTT] ?CTT?main body
      		char suffix[TRAILBITLENGTH + 1]; // <mm> (p) [P] ?RVI?trailer
      */
      if ( cs.prefix != cs.prefix2 )
      {
         // we have a leading / for a pre-pended prefix, so callsign itself is
         // not relevant
         csyn = MultLists::getMultLists() ->searchCountrySynonym( cs.prefix );
      }

      if ( !csyn )
      {
         // take the whole callsign, extra prefix, suffix, the lot and look for the
         // longest matching synonym. If the list is incomplete then this may
         // misidentify the country - e.g. if GW were missed out then this algorithm
         // would (wrongly?) allow a match on G. This might be a benefit for e.g.
         // PA or DL, as we don't need to put all the synonyms in, just the base
         // letter

         // There are really foul callsigns, such as
         // Glorioso Island:      FR-G  e.g.  FR7GL
         // Callbook implies that FR#*/G (see below) is also valid
         // Short of entering ALL these, not sure what to do

         // How about a '#' in the synonym meaning any number?
         // Also need an "any letter" - what about '?', to match
         // DOS wild cards? As well as * to mean a sequence of letters

         // This coding cannot be fully expanded into the synonym list; maybe
         // some kind of synonym decision tree needs to be built
         // This is getting VERY nasty; maybe we just say to enter the actual call
         // as a synonym (but of what... we need a placeholder for the main country)!

// replacement algorithm - HF inspired
// just keep stripping it back until we get a match

         testpart = cs.fullCall.getValue();
         testpart = trimr( testpart );

         int clen = testpart.length();
         while ( ( clen >= 1 ) && ( !csyn ) )
         {
            // we need to stop when we get to the basic prefix...
            // otherwise RVI6ABC ends up matching R, which is UA
            testpart = testpart.left(clen);
            clen--;
            csyn = MultLists::getMultLists() ->searchCountrySynonym(testpart );
         }
      }
   }
   ctryMult = ( csyn ) ? ( csyn->country ) : 0;
   return ctryMult;
}
void BaseContact::getText( QString &dest, const BaseContestLog * const curcon ) const
{
   ContactBuffs::scorebuff.clear();
   ContactBuffs::scorebuff.clear();
   ContactBuffs::brgbuff.clear();
   ContactBuffs::buff2.clear();
   ContactBuffs::qthbuff.clear();
   ContactBuffs::srbuff.clear();
   ContactBuffs::ssbuff.clear();
   ContactBuffs::buff.clear();

   if ( contactFlags.getValue() & ( LOCAL_COMMENT | COMMENT_ONLY | DONT_PRINT ) )
   {
      ContactBuffs::buff = QString("%1 %2 %3")
               .arg(time.getTime( DTGDISP ), 5)
               .arg(( contactFlags.getValue() & DONT_PRINT ) ? "DELETED" : ( contactFlags.getValue() & LOCAL_COMMENT ) ? "LOCAL COMMENT" : "COMMENT FOR ADJUDICATOR")
               .arg(comments.getValue(), 60);
   }
   else
   {
      // if contest requires a serial
      makestrings( curcon ->serialField.getValue() );

      int offset = curcon->bearingOffset.getValue();
      const QChar degreeChar(0260); // octal value
      ContactBuffs::brgbuff = QString("%1%2").arg( varBrg( bearing + offset ), 3 ).arg(degreeChar);

      int thisscore = contactScore.getValue();

      if ( contactScore.getValue() < 0 )
         thisscore = 0;
      ContactBuffs::scorebuff = QString::number(thisscore );

      ContactBuffs::qthbuff = extraText.getValue().left( 100 );

      if ( contactFlags.getValue() & MANUAL_SCORE )
         ContactBuffs::brgbuff = "MAN";

      if ( contactFlags.getValue() & DONT_PRINT )
         ContactBuffs::scorebuff = "DEL";
      else
         if ( contactFlags.getValue() & NON_SCORING )
            ContactBuffs::scorebuff = "N/S";
         else
         {
            // look at the contest dup
            if ( ( cs.valRes == ERR_DUPCS ) && ( curcon == contest ) )
               ContactBuffs::scorebuff = "DUP";
         }
   }

   if ( contactFlags.getValue() & VALID_DUPLICATE )
      ContactBuffs::buff2 = "BP ";
   else
      if ( contactFlags.getValue() & XBAND )
         ContactBuffs::buff2 = "XB ";

   strcpysp( ContactBuffs::buff, comments.getValue(), 42 );
   if ( !ContactBuffs::buff.isEmpty() )
   {
      strcpysp( ContactBuffs::buff2, ContactBuffs::qthbuff, 20 );
      ContactBuffs::buff2 += " | ";
      ContactBuffs::buff2 += ContactBuffs::buff;
   }
   else
      strcpysp( ContactBuffs::buff2, ContactBuffs::qthbuff, 42 );

   ContactBuffs::buff.clear();
   int next = 0;
   next = placestr( ContactBuffs::buff, time.getDate( DTGDISP ), next, 8 );
   next += 2;
   next = placestr( ContactBuffs::buff, time.getTime( DTGDISP ), next, 5 );

   next += 1;
   next = placestr( ContactBuffs::buff, cs.fullCall.getValue(), next, 11 );

   if ( curcon ->RSTField.getValue() )
      next = placestr( ContactBuffs::buff, reps.getValue(), next, 3 );
   next = placestr( ContactBuffs::buff, ContactBuffs::ssbuff, next, -4 );
   if ( curcon ->RSTField.getValue() )
      next = placestr( ContactBuffs::buff, repr.getValue(), next + 1, 3 );
   next = placestr( ContactBuffs::buff, ContactBuffs::srbuff, next, -4 );

   next = placestr( ContactBuffs::buff, loc.loc.getValue(), next + 1, ( curcon ->allowLoc8.getValue() ) ? 8 : 6 );

   next = placestr( ContactBuffs::buff, ContactBuffs::brgbuff, next + 1, 4 );
   next = placestr( ContactBuffs::buff, ContactBuffs::scorebuff, next, -5 );

   next = placestr( ContactBuffs::buff, op1.getValue(), next, -8 );
   next += 1;
   next = placestr( ContactBuffs::buff, op2.getValue(), next, -8 );

   next = placestr( ContactBuffs::buff, ContactBuffs::buff2, next + 1, 90 );

   dest = QString( ContactBuffs::buff ).trimmed();
}

void BaseContact::makestrings( bool sf ) const
{
   int sr = serialr.getValue().toInt();
   int ss = serials.getValue().toInt();
   if ( ss )
      ContactBuffs::ssbuff = QString("%1 ").arg(ss, 3);
   else
      ContactBuffs::ssbuff.clear();

   if ( sr && sf )
       ContactBuffs::srbuff = QString("%1 ").arg(sr, 3);
   else
      ContactBuffs::srbuff.clear();
}

