/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#include "XMPP_pch.h"


//---------------------------------------------------------------------------
static std::ofstream &getLogFile( QString name )
{
   static std::ofstream logFile( name.toStdString().c_str() );
   return logFile;
}

//---------------------------------------------------------------------------
std::ostream & MLogFile::createLogFile(const QString &prefix, int keepDays )
{
   //Close();
   QDateTime dt = QDateTime::currentDateTime();
   QString tidyPrefix = prefix + "*";
   tidyFiles( tidyPrefix, keepDays );
   fLogFileName = generateLogFileName( prefix );

   QString sDir = ExtractFileDir( fLogFileName);
   if ( !DirectoryExists( sDir ) )
      CreateDir( sDir );

   QString sdt;
    QString dtg = dt.toString( "yyyy MMM dd hh:m:ss.zzz" );

	//   getLogFile().open( fLogFileName.c_str() );
	getLogFile( fLogFileName );
    return log() << dtg.toStdString().c_str() << std::endl;
}
//---------------------------------------------------------------------------
std::ostream & MLogFile::log( void )
{
	return getLogFile( "" );
}
//---------------------------------------------------------------------------
std::ostream & MLogFile::logT( void )
{
    CsGuard lock;
    QDateTime dt = QDateTime::currentDateTime();
    int id = (int)QThread::currentThreadId();
    QString time = dt.toString( "hh:mm:ss.zzz" ) + " <" + QString::number(id) + ">";
    return log() << time.toStdString().c_str();
}
//---------------------------------------------------------------------------
void MLogFile::close( void )
{
   getLogFile( "" ).close();
}
//---------------------------------------------------------------------------
std::ostream & MLogFile::log(const QString &s )
{
   CsGuard scoped_lock;
   return logT() << s.toStdString().c_str() << std::endl;
}

//---------------------------------------------------------------------------
/*static */QString MLogFile::generateLogFileName(const QString &prefix )
{
   QDateTime dt = QDateTime::currentDateTime();
   QString s = prefix;
   s += dt.toString( "yyyyMMdd_hhmmss" ) + ".log";
   return s;
}
//---------------------------------------------------------------------------
/*static */void MLogFile::tidyFiles(const QString &prefix,   int keepDays )
{
   QString s = prefix;

   QString sDir = ExtractFileDir( s );
   if ( sDir != "" )
   {
      if ( sDir[ sDir.size() ] != '\\' && sDir[ sDir.size() ] != '/' )
         sDir += '/';
   }
   QDateTime kdt = QDateTime::currentDateTime().addDays(keepDays);
/*
   TSearchRec sr;
   int iAttributes = faDirectory | faSysFile | faHidden;

   if ( FindFirst( s, faAnyFile	, sr ) == 0 )
   {
      do
      {
         if ( sr.Name != "" && sr.Attr & ~iAttributes )
         {
            TDateTime dt;
            dt = dt.FileDateToDateTime( sr.Time );
            if ( dt < kdt )
            {
               String sName = sDir + sr.Name;
               DeleteFile( sName.c_str() );
            }
         }
      }
      while ( FindNext( sr ) == 0 );
      FindClose( sr );
   }
   */
}

