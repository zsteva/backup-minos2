#include "fileutils.h"

bool FileExists(const QString &path )
{
    QFileInfo checkFile( path );
    // check if file exists and if yes: Is it really a file and no directory?
    if ( checkFile.exists() && checkFile.isFile() )
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool DirectoryExists( const QString &path )
{
    QFileInfo checkFile( path );
    // check if file exists and if yes: Is it really a file and no directory?
    if ( checkFile.exists() && checkFile.isDir() )
    {
        return true;
    }
    else
    {
        return false;
    }
}
QString GetCurrentDir()
{
    QDir dir( "." );
    return dir.absolutePath();
}
QString ExtractFileDir(const QString &fname )
{
    QFileInfo finf( fname );
    QString p = finf.dir().canonicalPath();
    return p;
}
QString ExtractFilePath( const QString &fname )
{
    return ExtractFileDir( fname );
}
QString ExtractFileName(const QString &fname )
{
    QFileInfo finf( fname );
    return finf.fileName();
}
QString ExtractFileExt( const QString &fname )
{
    QFileInfo finf( fname );
    return ( "." + finf.completeSuffix() );
}
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
bool FileAccessible(const QString &fname)
{
    qt_ntfs_permission_lookup++;
    bool exists = FileExists(fname);
    bool readable = QFileInfo(fname).isReadable();

    qt_ntfs_permission_lookup--;
    return exists && readable;
}
bool FileWriteable(const QString &fname)
{
    qt_ntfs_permission_lookup++;
    bool exists = FileExists(fname);
    bool writeable = QFileInfo(fname).isWritable();

    qt_ntfs_permission_lookup--;
    return exists && writeable;
}
const QString ExcludeTrailingBackslash( const QString &s )
{
    if ( s.length() && ( s[ s.length() - 1 ] == '\\' || s[ s.length() - 1 ] == '/' ) )
    {
        QStringRef s1(&s, 0, s.length() - 1);
//        QString s1 = s.substr( 0, s.length() - 1 );
        return s1.toString();
    }
    return s;
}

bool StaticDirectoryExists (const QString &Name )
{
    QFileInfo qinf( Name );
    bool dir = qinf.isDir();
    return dir;
}
bool StaticForceDirectories ( const QString & pDir )
{
    // translated from Pascal so we can static link

    if ( pDir.size() == 0 )
        return false;

    QString Dir = pDir;
    Dir = ExcludeTrailingBackslash ( Dir );
    if ( ( Dir.size() < 3 ) || StaticDirectoryExists ( Dir )
         || ( ExtractFilePath ( Dir ) == Dir ) )
        return true; // avoid 'xyz:\' problem.
    bool Result = StaticForceDirectories ( ExtractFilePath ( Dir ) );
    if ( Result )
    {
        Result = CreateDir ( Dir );
    }

    return Result;
}

//---------------------------------------------------------------------------
bool StaticDirectoryCreate (const QString &Path )
{
    if ( !StaticDirectoryExists ( Path ) )
    {
        bool res = StaticForceDirectories ( Path );
        if ( !res )
        {
            return false;
        }
    }
    return true;
}
bool CreateDir(const QString &s )
{
    QDir dir( s );
    if ( !dir.exists() )
    {
        dir.mkpath( "." );
    }
    return dir.exists();
}
void SetCurrentDir( const QString &dir )
{
    QDir::setCurrent( dir );
}
