/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
#ifndef CalcsH
#define CalcsH 
#include <QString>

//----------------------------------------------------------------------------
extern int normBrg( int mbear );
extern int varBrg( int mbear );
extern char lonlat(const QString &pinqra, double &lon, double &lat );
extern QString WtodbW( const QString &W );
extern QString dbWtoW( const QString &dbW );

#endif
