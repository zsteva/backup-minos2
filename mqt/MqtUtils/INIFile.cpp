/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "mqtUtils_pch.h"
#include <sys/stat.h>
#include "fileutils.h"
#include "MLogFile.h"
#include "INIFile.h"
#include "MShowMessageDlg.h"

class INIFile;
class INISection;
class INIEntry //: public NameChain
{
private:
    bool entryValid;
    QString entryValue;
    bool entryDirty;

public:
    QString name;

    INIEntry( INISection *cb,
              const QString &name, bool Valid );  // Constructor.

    ~INIEntry();        // Destructor.

    QString getValue( void );
    void setValue( const QString &Value );
    bool isValidEntry( void );
    bool isDirty()
    {
        return entryDirty;
    }
    void setClean()
    {
        entryDirty = false;
    }

};
typedef INIEntry *IniEntryPtr;
struct INIEntryCmp
{
    QString cmpstr;
    INIEntryCmp( const QString &s ) : cmpstr( s )
    {}

    bool operator() ( IniEntryPtr &s1 ) const
    {
        return s1->name.compare(cmpstr, Qt::CaseInsensitive ) == 0;
    }
};
class INISection
{
private:
    bool entryValid;
    bool sectDirty;
public:
    QString name;

    INISection( INIFile *cb,
                const QString &name, bool Valid );

    ~INISection();   // Destructor.

    bool isValidSection( void );

    QVector <IniEntryPtr> entries;

    bool isDirty();
    void setClean();

};
struct INISectionCmp
{
    QString cmpstr;
    INISectionCmp( const QString &s ) : cmpstr( s )
    {}

    bool operator() ( IniSectionPtr &s1 ) const
    {
        return s1->name.compare(cmpstr, Qt::CaseInsensitive ) == 0;
    }
};
//==============================================================================
INISection::INISection( INIFile *cb, const QString &name, bool valid )
    : name( name.trimmed() ), entryValid( valid ), sectDirty( false )
{
    cb->sections.push_back( this );
}

INISection::~INISection()
{
    // delete all entries
    for ( QVector <IniEntryPtr>::iterator this_ent = entries.begin();
          this_ent != entries.end(); this_ent++ )
    {
        delete ( *this_ent );
    }
    entries.clear();
}

bool INISection::isValidSection( void )
{
    return entryValid;
}
bool INISection::isDirty( void )
{
    if ( sectDirty )
        return true;

    for ( QVector <IniEntryPtr>::iterator thisEntry = entries.begin();
          thisEntry != entries.end(); thisEntry++ )
    {
        if ( ( *thisEntry ) ->isDirty() )
            return true;
    }
    return false;   // for now, just in case...
}
void INISection::setClean( void )
{
    //walk all sections and set each clean
    for ( QVector <IniEntryPtr>::iterator thisEntry = entries.begin();
          thisEntry != entries.end(); thisEntry++ )
    {
        ( *thisEntry ) ->setClean();
    }
}
//==============================================================================


INIEntry::INIEntry( INISection *cb, const QString &name, bool valid )
    : name( name.trimmed() ), entryValid( valid ), entryDirty( false )
{
    cb->entries.push_back( this );
}

INIEntry::~INIEntry()
{}

QString INIEntry::getValue( void )
{
    return entryValue;
}

void INIEntry::setValue( const QString &value )
{
    if ( value != entryValue )
    {
        entryValue = value.trimmed();
        entryDirty = true;
    }
}

bool INIEntry::isValidEntry( void )
{
    return entryValid;
}
//==============================================================================

INIFile::INIFile( const QString &name ) : fileLoaded( false ),   /*invalid( true ),*/ fileDirty( false )
{
    loadedFileName = name.trimmed() ;

    bool ret = true;
    if ( !FileExists( loadedFileName ) )
    {
        StaticForceDirectories( ExtractFilePath( loadedFileName ) );
        QFile fh(loadedFileName);
        ret = fh.open(QIODevice::ReadWrite);
        if ( ret )
            fh.close();
    }
    if (ret)
    {
        QFileInfo qf(name.trimmed());
        loadedFileName = qf.canonicalFilePath();
    }
}
INIFile::~INIFile()
{
    writePrivateProfileString( 0, 0, 0 );
    // delete all sections
    for ( QVector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
    {
        delete ( *thisSect );
    }
    sections.clear();
    fileLoaded = false;
}
bool INIFile::dupSection( const QString &oldname, const QString &newname )
{
    QVector<IniSectionPtr>::iterator newSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( newname ) );
    if ( newSect == sections.end() )
    {
        QVector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( oldname ) );
        if ( thisSect != sections.end() )
        {
            INISection * oldsect = ( *thisSect );
            INISection *newsect = new INISection( this, newname, true );
            for ( QVector <IniEntryPtr>::iterator this_ent = oldsect->entries.begin(); this_ent != oldsect->entries.end(); this_ent++ )
            {
                INIEntry *i = ( *this_ent );
                QString n = i->name;
                QString v = i->getValue();
                INIEntry *newent = new INIEntry( newsect, n, true );
                newent->setValue( v );
            }
            return true;
        }
        return false;
    }
    return false;  // section already exists
}

bool INIFile::checkStat( void )
{
    struct stat tempstat;
    int statret = stat( loadedFileName.toStdString().c_str(), &tempstat );
    if ( statret )
        return true;	// failed => different(?)
    if ( statret == 0 && memcmp( &tempstat, &statbuf, sizeof( struct stat ) ) != 0 )
    {
        memcpy( &statbuf, &tempstat, sizeof( struct stat ) );
        return true;
    }
    return false;
}
bool INIFile::isDirty( void )
{
    if ( fileDirty )
        return true;

    for ( QVector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
    {
        if ( ( *thisSect ) ->isDirty() )
            return true;
    }
    return false;   // for now, just in case...
}
void INIFile::setClean( void )
{
    //walk all sections and set each clean
    for ( QVector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
    {
        ( *thisSect ) ->setClean();
    }
}

bool INIFile::writeINIFile( void )
{
    if ( !isDirty() )
        return true;

    QFile inf(loadedFileName);

    // here we need to stat the file to see if it has changed
    // - but what do we do if it has? We should have loaded
    // it VERY recently

    if (!inf.open(QIODevice::WriteOnly|QIODevice::Text))
        return false;

    QTextStream out(&inf);

    for ( QVector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
    {
        const QString sname = ( *thisSect ) ->name;

        if ( ( *thisSect ) ->isValidSection() )
        {
            QString s = QString("[%1]\n").arg(sname);
            out << s;
        }

        for ( QVector <IniEntryPtr>::iterator this_entry = ( *thisSect ) ->entries.begin(); this_entry != ( *thisSect ) ->entries.end(); this_entry++ )
        {
            const QString name = ( *this_entry ) ->name;
            const QString val = ( *this_entry ) ->getValue();
            if ( ( *this_entry ) ->isValidEntry() )
            {
                QString s = QString("%1=%2\n").arg(name).arg(val);
                out << s;
            }
            else
            {
                QString s = QString("%1\n").arg(val);
                out << s;
            }
        }
    }
    inf.close();

    setClean();
    // now stat the file so we can check for changes
    checkStat();
    return true;
}


bool INIFile::loadINIFile()
{
    INISection *thisSect;
    IniEntryPtr this_entry;
    bool realSections = false;

    if ( fileLoaded )
    {
        if ( !checkStat() )
            return false;			// no change, so don't re-read
        writePrivateProfileString( 0, 0, 0 );
        for ( QVector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
        {
            delete ( *thisSect );
        }
        sections.clear();
        fileLoaded = false;
    }

    fileLoaded = true;

    QFile lf(loadedFileName);

    // here we need to stat the file to see if it has changed
    // - but what do we do if it has? We should have loaded
    // it VERY recently

    if (!lf.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        mShowMessage( QString( "Initialisation file \"" ) + loadedFileName + "\" not found.", 0 );
        return false;
    }
    QTextStream inf(&lf);

    thisSect = new INISection( this, "?Comments", false );
    // create dummy section for leading comments

    while (!inf.atEnd())
    {
        QString buffer = inf.readLine( 256 );

        QStringList p1 = buffer.split('[');
        if (p1.length() > 1)
        {
            QStringList p2 = p1[1].split(']');
            if (p2.length())
            {
                QString Parameter = p2[0];
                thisSect = new INISection( this, Parameter, true );
                realSections = true;
                continue;
            }
        }

        QStringList a = buffer.split('=');
        int scnt = a.size();

        if ( scnt )
        {
            this_entry = new INIEntry( thisSect, a[ 0 ], true );
            // somewhere we need to cope with quoted parameters
            this_entry->setValue( a[ 1 ] );
            this_entry->setClean();
        }
        else
        {
            // create comment entry
            this_entry = new INIEntry( thisSect, "??", false );
            this_entry->setValue( a[ 0 ] );
            this_entry->setClean();
        }
    }
    // now stat the file so we can check for changes
    checkStat();

    return realSections;
}

bool INIFile::checkKeyExists( const QString &Section,
                              const QString &Entry
                              )
{
    loadINIFile();

    QVector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
    if ( thisSect != sections.end() )
    {
        if ( Entry == 0 )
            return true;
        QVector<IniEntryPtr>::iterator this_entry = std::find_if( ( *thisSect ) ->entries.begin(), ( *thisSect ) ->entries.end(), INIEntryCmp( Entry ) );
        if ( this_entry != ( *thisSect ) ->entries.end() )
        {
            return true;
        }
    }
    return false;
}

int INIFile::getPrivateProfileList( const QString &Section,
                                    const QString &Entry,
                                    QStringList &Buffer )
{
    loadINIFile();

    QVector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
    if ( thisSect != sections.end() )
    {
        if ( Entry.isEmpty() )
        {
            /* build list of entry names in buffer */
            for ( QVector <IniEntryPtr>::iterator this_entry = ( *thisSect ) ->entries.begin(); this_entry != ( *thisSect ) ->entries.end(); this_entry++ )
            {
                if ( ( *this_entry ) ->isValidEntry() )
                {
                    Buffer.append( (*this_entry ) ->name);
                }
            }
        }
    }
    return Buffer.count();
}
int INIFile::getPrivateProfileString( const QString &Section,
                                      const QString &Entry,
                                      const QString &DefaultValue,
                                      QString &Buffer )
{
    loadINIFile();

    QVector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
    if ( thisSect == sections.end() )
    {
        Buffer = DefaultValue;
    }
    else
    {
        if ( !Entry.isEmpty() )
        {
            QVector<IniEntryPtr>::iterator this_entry = std::find_if( ( *thisSect ) ->entries.begin(), ( *thisSect ) ->entries.end(), INIEntryCmp( Entry ) );
            if ( this_entry == ( *thisSect ) ->entries.end() )
            {
                Buffer = DefaultValue;
            }
            else
            {
                Buffer = ( *this_entry ) ->getValue();
            }
        }
    }
    return Buffer.size();
}


int INIFile::getPrivateProfileInt(const QString &Section,
                                  const QString &Entry,
                                  int DefaultValue )
{
    if ( ( Section == 0 ) || ( Entry == 0 ) )
        return 0;

    loadINIFile();

    QVector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
    if ( thisSect == sections.end() )
    {
        return DefaultValue;
    }

    QVector<IniEntryPtr>::iterator this_entry = std::find_if( ( *thisSect ) ->entries.begin(), ( *thisSect ) ->entries.end(), INIEntryCmp( Entry ) );
    if ( this_entry == ( *thisSect ) ->entries.end() )
    {
        return DefaultValue;
    }

    return ( *this_entry ) ->getValue().toInt();
}



bool INIFile::writePrivateProfileString(const QString &Section,
                                        const QString &Entry,
                                        const QString &Buffer )
{
    /*
   lpAppName

   Points to a null-terminated string containing the name of the section to which
   the string will be copied. If the section does not exist, it is created. The name
   of the section is case-independent; the string can be any combination of uppercase
    and lowercase letters.

   lpKeyName

   Points to the null-terminated string containing the name of the key to be associated
    with a string. If the key does not exist in the specified section, it is created.
    If this parameter is 0, the entire section, including all entries within the section, is deleted.

   lpString

   Points to a null-terminated string to be written to the file. If this parameter is 0,
   the key pointed to by the lpKeyName parameter is deleted.
   Windows 95: This platform does not support the use of the TAB (\t) character as part of this parameter.

   lpFileName

   Points to a null-terminated string that names the initialization file.


   */
    if ( ( Section == 0 ) && ( Entry == 0 ) && ( Buffer == 0 ) )
    {
        /*    Windows 95 keeps a cached version of WIN.INI to improve performance.
          If all three parameters are 0, the function flushes the cache. The
            function always returns FALSE after flushing the cache, regardless of
            whether the flush succeeds or fails.
      */
        writeINIFile();
        return false;
    }

    loadINIFile();

    QVector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
    if ( Entry == 0 )
    {
        if ( thisSect != sections.end() )
        {
            fileDirty = true;
            for ( QVector <IniEntryPtr>::iterator this_ent = ( *thisSect ) ->entries.begin(); this_ent != ( *thisSect ) ->entries.end(); this_ent++ )
            {
                delete ( *this_ent );
            }
            ( *thisSect ) ->entries.clear();

            delete ( *thisSect );
            sections.erase( thisSect );
        }
    }
    else
    {
        if ( thisSect == sections.end() )
        {
            /* create new section */
            new INISection( this, Section, true );
            thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
            fileDirty = true;
        }
        QVector<IniEntryPtr>::iterator this_entry = std::find_if( ( *thisSect ) ->entries.begin(), ( *thisSect ) ->entries.end(), INIEntryCmp( Entry ) );
        if ( Buffer == 0 )
        {
            if ( this_entry != ( *thisSect ) ->entries.end() )
            {
                delete ( *this_entry );
                ( *thisSect ) ->entries.erase( this_entry );
                fileDirty = true;
            }
        }
        else
        {
            if ( this_entry == ( *thisSect ) ->entries.end() )
            {
                /* create new entry */
                new INIEntry( ( *thisSect ), Entry, true );
                this_entry = std::find_if( ( *thisSect ) ->entries.begin(), ( *thisSect ) ->entries.end(), INIEntryCmp( Entry ) );
                fileDirty = true;
            }
            ( *this_entry ) ->setValue( Buffer );
        }
    }
    writeINIFile();

    return true;
}

bool INIFile::isSectionPresent(QString sname)
{
    QVector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( sname ) );
    if ( thisSect == sections.end() )
    {
        return false;
    }

    if ( ( *thisSect ) ->entries.size() == 0)
    {
        return false;
    }
    return true;
}
QStringList INIFile::getSections( )
{
    QStringList slist;
    for ( QVector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
    {
        if ( ( *thisSect ) ->isValidSection() )
        {
            slist.append( ( *thisSect ) ->name );
        }
    }

    return slist;
}
