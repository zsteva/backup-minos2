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
#include "fileutils.h"

QMutex CsGuard::m_mutex(QMutex::Recursive);
//---------------------------------------------------------------------------
static std::ofstream &getLogFile( QString name )
{
   static std::ofstream logFile( name.toStdString().c_str() );
   return logFile;
}

//---------------------------------------------------------------------------
std::ostream & MLogFile::createLogFile(const QString &path, const QString filePrefix, int keepDays )
{
    StaticDirectoryCreate ( path );

    QString TidyPrefix = path + "/" + filePrefix + "*";
    tidyFiles ( TidyPrefix, keepDays );
    QString fLogFileName = generateLogFileName ( path + "/" + filePrefix  );

    QString dtg = QDateTime::currentDateTime().toString( "yyyy MMM dd hh:m:ss.zzz" );

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
/*static */
void MLogFile::tidyFiles (const QString &Prefix, int KeepDays )
{
    QString s = Prefix;
    QString sDir = ExtractFileDir ( s );
    if ( sDir != "" )
    {
        if ( sDir[ sDir.size() - 1 ] != '/' )
            sDir += '/';
    }
    QDateTime kdt = QDateTime::currentDateTime().addDays( -KeepDays );

    QDirIterator files( sDir, QDir::Files | QDir::NoSymLinks , QDirIterator::NoIteratorFlags );
    while ( files.hasNext() )
    {
        files.next();
        QFileInfo fi(files.filePath());
        QDateTime fd(fi.created());

        if (fd < kdt)
        {
            if (!QFile::remove(files.filePath()))
            {
                trace("Failed to remove " + files.filePath());
            }
        }
    }
}
