/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef LogEventsH
#define LogEventsH 
#include <QString>
//---------------------------------------------------------------------------
extern bool checkCloseEvent();
extern void createCloseEvent();
//---------------------------------------------------------------------------
extern void logMessage( const QString &level, const QString &mess );
//---------------------------------------------------------------------------
#endif
