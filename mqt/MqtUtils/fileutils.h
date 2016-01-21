#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QDir>
#include <QFileInfo>
#include <QFile>

bool FileExists( const QString& Name );
bool DirectoryExists(const QString &path );
QString GetCurrentDir();
void SetCurrentDir( const QString &dir );

QString ExtractFileDir( const QString &fname );
QString ExtractFilePath(const QString &fname );
QString ExtractFileName(const QString &fname );
QString ExtractFileExt(const QString &fname );
bool FileAccessible(const QString &fname);
bool FileWriteable(const QString &fname);
bool StaticDirectoryExists ( const QString & Name );
bool StaticForceDirectories (const QString &Dir );
bool StaticDirectoryCreate (const QString &Path );

bool CreateDir( const QString &s );

#endif // FILEUTILS_H
