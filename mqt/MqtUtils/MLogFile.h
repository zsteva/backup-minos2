/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		VHF Contest Adjudication
//
// COPYRIGHT         (c) M. J. Goodey G0GJV, 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MLogFileH
#define MLogFileH 
#include <QString>
#include <QMutexLocker>
#include <QTextStream>
//---------------------------------------------------------------------------
bool CreateDir(const QString &s );
bool FileExists(const QString &path );
bool DirectoryExists( const QString &path );
QString GetCurrentDir();
QString ExtractFileDir(const QString &fname );
QString ExtractFilePath( const QString &fname );
QString ExtractFileName(const QString &fname );
QString ExtractFileExt( const QString &fname );

class CsGuard:public QMutexLocker
{
      static QMutex m_mutex;
   public:
      CsGuard():QMutexLocker(&m_mutex)
      {
      }

      ~CsGuard()
      {
      }
      static void ClearDown()
      {
      }
};


//---------------------------------------------------------------------------
class MLogFile
{
   private:
      QString fLogFileName;
   public:

      MLogFile()
      { }
      // CreateLogFile is called by the boot form at startup
      void createLogFile(const QString &path, const QString filePrefix, int keepDays );

      // Log and LogT are used to log data without and with a timestamp;
      QTextStream &log( void );
      QTextStream & logT( void );
      // Log logs a string with a time prefix hh:mm:ss
      QTextStream &log( const QString &s );
      void close( void );

      // Generates a suffix of the form yyyyddmmhhmmss
      static QString generateLogFileName( const QString &prefix );
      static void tidyFiles( const QString &prefix, int keepDays );
      QString getTraceFileName()
      {
         return fLogFileName;
      }
};
//---------------------------------------------------------------------------
#endif
