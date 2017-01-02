/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#include "Base_pch.h"

#include "tinyxml.h"
#include "portconf.h"
#include "controlport.h"

#include "CommonMonitor.h"
#include "MinosLines.h"

//==============================================================================
timerTicker::timerTicker()
{
    connect(&b, SIGNAL(timeout()), this, SLOT(tick()));
    b.start(55);
}
timerTicker::~timerTicker()
{
}

//==============================================================================
commonController::commonController(QWidget *p ):parent(p)
{
}
void commonController::closeDown()
{
   // close down each port
   for ( QVector <commonPort *>::iterator icp = portChain.begin(); icp != portChain.end(); icp++ )
   {
      if ( *icp )
         ( *icp ) ->closePort();
      delete ( *icp );
   }
   portChain.clear();
}
commonController::~commonController()
{
}
/*
void commonController::ptt(int)
{
 
}
void commonController::key(int)
{
 
}
*/
bool commonController::initialise( )
{
   return true;
}


void commonController::tickEvent()        // this will often be an interrupt routine
{
   checkControls();
}
void commonController::checkControls( void )
{
   // loop through ports, checkControls on each
   for ( QVector < commonPort *>::iterator i = portChain.begin(); i != portChain.end(); i++ )
   {
      ( *i ) ->checkControls();
   }
}
commonPort *commonController::createPort( const PortConfig &port )
{
    commonPort * cp = 0;
    foreach(commonPort *ni, portChain)
    {
        if (ni->portName == port.name)
        {
            cp = ni;
            break;
        }
    }

   if ( cp )
      return cp;
   else
   {
      // create the port

      switch ( port.portType )
      {
         case PortConfig::eptSerial:
            //cp = new serialPort( port );
            break;

        case PortConfig::eptWindows:
            cp = new WindowsMonitorPort( parent, port );
            break;

         case PortConfig::eptK8055:
            //cp = new K8055Port( port );
            break;

         case PortConfig::eptUBW:
            //cp = new UBWPort( port );
            break;
      }

   }
   return cp;
}
commonLineControl *commonController::findLine( const QString &name, bool lineIn )
{
   commonLineControl * clc = 0;
   for ( QVector < commonPort *>::iterator i = portChain.begin(); i != portChain.end(); i++ )
   {
      clc = ( *i ) ->findLine( name, lineIn );
      if ( clc )
      {
         break;
      }
   }
   return clc;
}
void commonController::lineChange( commonLineControl *line )
{
   commonLineControl * pttout = findLine( "PTTOut", false );
   commonLineControl *pttin = findLine( "PTTIn", true );
   commonLineControl *l1 = findLine( "L1", true );
   commonLineControl *l2 = findLine( "L2", true );
   if ( pttout && pttin && l1 && l2 )
   {
      setLines( pttout->getState(), pttin->getState(), l1->getState(), l2->getState() );
   }
   LineSet *ls = LineSet::GetLineSet();
   ls->publish( line->lineName, line->getState() );
}
//==============================================================================
