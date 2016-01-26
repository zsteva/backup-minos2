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
//std::string lastError( DWORD erno );
//std::string lastError( void );
//---------------------------------------------------------------------------
void trace( const char *mess );
void trace( const std::string & mess );
void trace( const QString & mess );
void enableTrace(const QString &where, const QString filePrefix );
void disableTrace( );
QString getTraceFileName();
#endif