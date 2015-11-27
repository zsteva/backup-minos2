/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "base_pch.h"

//#include "..\MinosDisplay\TreeUtils.h"

std::string ListContact::getField( int ACol, const BaseContestLog *const curcon ) const
{
   // only used to get fields for main log display
   std::string res;
   if ( !curcon )
      return res;

   switch ( ACol )
   {
      case egCall:
         res = cs.fullCall.getValue();
         break;
      case egLoc:
         res = loc.loc.getValue();
         break;
      case egBrg:
         {
            QString brgbuff;
            double lon = 0.0;
            double lat = 0.0;

            if ( lonlat( loc.loc.getValue().c_str(), lon, lat ) == LOC_OK )
            {
               // we don't have it worked out already...
               double lon = 0.0;
               double lat = 0.0;
               int brg;
               double dist;



               int lres = lonlat( loc.loc.getValue().c_str(), lon, lat );
               if ( lres == LOC_OK )
               {
                  curcon->disbear( lon, lat, dist, brg );
               }
               else
                  if ( lres == LOC_SHORT )
                  {
//#warning  what here? (lres == LOC_SHORT)

                  }
			   int offset = curcon->bearingOffset.getValue();
               brgbuff = QString( "%1\xB0").arg( brg + offset, 3);
            }
            res = brgbuff.toStdString();
         }
         break;

      case egScore:
         {
            QString scorebuff;

            // we don't have it worked out already...
            double lon = 0.0;
            double lat = 0.0;
            int brg;
            double dist = 0.0;

            if ( lonlat( loc.loc.getValue().c_str(), lon, lat ) == LOC_OK )
            {
               curcon->disbear( lon, lat, dist, brg );
            }
            scorebuff = QString( "%1").arg(( int ) dist );
            res = scorebuff.toStdString();
         }
         break;

      case egExchange:
         {
            res = extraText;
         }
         break;

      case egComments:
         {
            res = comments;
         }
         break;
   }
   return res;
}

