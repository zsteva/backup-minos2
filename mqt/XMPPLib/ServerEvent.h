/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef ServerEventH
#define ServerEventH 
//---------------------------------------------------------------------------

extern QString makeUuid();
extern QString getServerId();
extern void makeServerEvent( bool create );
extern bool checkServerReady();
extern void makeServerShowEvent( );
extern bool getShowServers();
extern void setShowServers(bool state);
#endif

