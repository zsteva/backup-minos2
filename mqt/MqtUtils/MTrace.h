/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		VHF Contest Adjudication
//
// COPYRIGHT         M. J. Goodey, 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#ifndef MTraceH
#define MTraceH 
//---------------------------------------------------------------------------
void trace( const QString & mess );
void enableTrace(const QString &where, const QString filePrefix );
void disableTrace( );
QString getTraceFileName();
#endif
