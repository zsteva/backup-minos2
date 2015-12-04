/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "logger_pch.h"
#include <sys\stat.h>

#include "MLogFile.h"
const QString noneBundle = "<None>";

// Here we define all the possible profile entries in a section
// we use them for new sections in a bundle, and also to get
// values from a bundle.

// What do we do about variable entries? Do we allow them?
// What about about entries for other Minos programs
// where we won't know what is is relevant?

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

      std::vector <IniEntryPtr> entries;

      bool isDirty();
      void setClean();

};
typedef INISection *IniSectionPtr;
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
class INIFile
{
      bool fileDirty;
   public:
      bool loadINIFile( void );
      bool writeINIFile( void );
      bool checkStat( void );
      bool checkKeyExists(const QString &Section,
                           const QString &Entry
                         );
      int getPrivateProfileString(const QString &Section,
                                   const QString &Entry,
                                   const QString &DefaultValue,
                                   QString &Buffer);
      int getPrivateProfileList(const QString &Section,
                                   const QString &Entry,
                                   QStringList &Buffer);

      int getPrivateProfileInt( const QString &Section,
                                const QString &Entry,
                                int DefaultValue );

      bool writePrivateProfileString( const QString &Section,
                                      const QString &Entry,
                                      const QString &Buffer );

      bool dupSection( const QString &oldname, const QString &newname );

      std::vector <IniSectionPtr> sections;
      bool fileLoaded;
      bool isDirty();
      void setClean();

      QString loadedFileName;

      struct stat statbuf;

      INIFile(const QString &name );
      ~INIFile();
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
   for ( std::vector <IniEntryPtr>::iterator this_ent = entries.begin();
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

   for ( std::vector <IniEntryPtr>::iterator thisEntry = entries.begin();
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
   for ( std::vector <IniEntryPtr>::iterator thisEntry = entries.begin();
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
   for ( std::vector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
   {
      delete ( *thisSect );
   }
   sections.clear();
   fileLoaded = false;
}
bool INIFile::dupSection( const QString &oldname, const QString &newname )
{
   std::vector<IniSectionPtr>::iterator newSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( newname ) );
   if ( newSect == sections.end() )
   {
      std::vector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( oldname ) );
      if ( thisSect != sections.end() )
      {
         INISection * oldsect = ( *thisSect );
         INISection *newsect = new INISection( this, newname, true );
         for ( std::vector <IniEntryPtr>::iterator this_ent = oldsect->entries.begin(); this_ent != oldsect->entries.end(); this_ent++ )
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

   for ( std::vector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
   {
      if ( ( *thisSect ) ->isDirty() )
         return true;
   }
   return false;   // for now, just in case...
}
void INIFile::setClean( void )
{
   //walk all sections and set each clean
   for ( std::vector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
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

   if (!inf.open(QIODevice::ReadWrite|QIODevice::Text))
      return false;

   QTextStream out(&inf);

   for ( std::vector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
   {
      const QString sname = ( *thisSect ) ->name;

      if ( ( *thisSect ) ->isValidSection() )
      {
          QString s = QString("[%1]\n").arg(sname);
          out << s;
      }

      for ( std::vector <IniEntryPtr>::iterator this_entry = ( *thisSect ) ->entries.begin(); this_entry != ( *thisSect ) ->entries.end(); this_entry++ )
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
      for ( std::vector <IniSectionPtr>::iterator thisSect = sections.begin(); thisSect != sections.end(); thisSect++ )
      {
         delete ( *thisSect );
      }
      sections.clear();
      fileLoaded = false;
   }

   fileLoaded = true;

   QFile inf(loadedFileName);


   // here we need to stat the file to see if it has changed
   // - but what do we do if it has? We should have loaded
   // it VERY recently

   if (!inf.open(QIODevice::ReadOnly|QIODevice::Text))
   {
      MinosParameters::getMinosParameters() ->mshowMessage( QString( "Initialisation file \"" ) + loadedFileName + "\" not found." );
      return false;
   }

   thisSect = new INISection( this, "?Comments", false );
   // create dummy section for leading comments

   TEMPBUFF( buffer, 256 );
   while ( inf.read( buffer, 256 ) != 0 )
   {
      TEMPBUFF ( Parameter, 256 );
      if ( strchr( buffer, '[' ) && ( sscanf( buffer, " [ %255[^\n]", Parameter ) == 1 ) )
      {
         char * p = strchr( Parameter, ']' );
         if ( p != 0 )
         {
            *p = '\0';
            thisSect = new INISection( this, Parameter, true );
            realSections = true;
            continue;
         }
         // and if no trailing ']' should we be lenient?
      }

      if ( buffer[ strlen( buffer ) - 1 ] == '\n' )
         buffer[ strlen( buffer ) - 1 ] = 0;		// take off trailing new line

      char *a[ 3 ];
      bool sep2seen;
      int scnt = parseLine( buffer, '=', a, 2, 0, sep2seen );

      if ( scnt )
      {
         this_entry = new INIEntry( thisSect, a[ 0 ], true );
         // somewhere we need to cope with quoted parameters
         this_entry->setValue( QString(a[ 1 ]).trimmed() );
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
   inf.close();
   // now stat the file so we can check for changes
   checkStat();

   return realSections;
}

bool INIFile::checkKeyExists( const QString &Section,
                              const QString &Entry
                            )
{
   loadINIFile();

   std::vector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
   if ( thisSect != sections.end() )
   {
      if ( Entry == 0 )
         return true;
      std::vector<IniEntryPtr>::iterator this_entry = std::find_if( ( *thisSect ) ->entries.begin(), ( *thisSect ) ->entries.end(), INIEntryCmp( Entry ) );
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

   std::vector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
   if ( thisSect != sections.end() )
   {
      if ( Entry.isEmpty() )
      {
         /* build list of entry names in buffer */
         for ( std::vector <IniEntryPtr>::iterator this_entry = ( *thisSect ) ->entries.begin(); this_entry != ( *thisSect ) ->entries.end(); this_entry++ )
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

   std::vector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
   if ( thisSect == sections.end() )
   {
      Buffer = DefaultValue;
   }
   else
   {
      if ( !Entry.isEmpty() )
      {
         std::vector<IniEntryPtr>::iterator this_entry = std::find_if( ( *thisSect ) ->entries.begin(), ( *thisSect ) ->entries.end(), INIEntryCmp( Entry ) );
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

   std::vector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
   if ( thisSect == sections.end() )
   {
      return DefaultValue;
   }

   std::vector<IniEntryPtr>::iterator this_entry = std::find_if( ( *thisSect ) ->entries.begin(), ( *thisSect ) ->entries.end(), INIEntryCmp( Entry ) );
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

   std::vector<IniSectionPtr>::iterator thisSect = std::find_if( sections.begin(), sections.end(), INISectionCmp( Section ) );
   if ( Entry == 0 )
   {
      if ( thisSect != sections.end() )
      {
         fileDirty = true;
         for ( std::vector <IniEntryPtr>::iterator this_ent = ( *thisSect ) ->entries.begin(); this_ent != ( *thisSect ) ->entries.end(); this_ent++ )
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
      std::vector<IniEntryPtr>::iterator this_entry = std::find_if( ( *thisSect ) ->entries.begin(), ( *thisSect ) ->entries.end(), INIEntryCmp( Entry ) );
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


//=============================================================================
ProfileEntry::ProfileEntry(int id, const QString &n, const QString &d, const QString &dname, const QString &h, bool RO )
      : id( id ), name( n ), sdefaultval( d ), hint( h ), pt( petString ), dispname(dname), RO(RO)
{}
ProfileEntry::ProfileEntry( int id, const QString &n, int d, const QString & dname, const QString &h, bool RO )
      : id( id ), name( n ), idefaultval( d ), sdefaultval( QString::number( d ) ), hint( h ), pt( petInteger ), dispname(dname), RO(RO)
{}
ProfileEntry::ProfileEntry(int id, const QString &n, bool d, const QString &dname, const QString &h, bool RO )
      : id( id ), name( n ), bdefaultval( d ), sdefaultval( makeStr( d ) ), hint( h ), pt( petBool ), dispname(dname), RO(RO)
{}
void ProfileEntry::createEntry( SettingsBundle *s )
{
   switch ( pt )
   {
      case petString:
         s->setStringProfile( name, sdefaultval ) ;
         break;
      case petInteger:
         s->setIntProfile( name, idefaultval ) ;
         break;
      case petBool:
         s->setBoolProfile( name, bdefaultval ) ;
         break;
   }
}
//=============================================================================
BundleFile::BundleFile( PROFILES p )  //: iniFile( 0 )
{
   switch ( p )
   {
	  case epLOGGERPROFILE:

         entries.push_back( ProfileEntry( elpListDirectory, "List Directory", "./Lists", "", "Default archive list directory", false ) );
         entries.push_back( ProfileEntry( elpLogDirectory, "Log Directory", "./Logs", "", "Default logs directory", false ) );

         entries.push_back( ProfileEntry( elpEntryFile, "EntryFile", "./Configuration/Entry.ini", "Entry settings file", "File containing entry settings", false ) );
         entries.push_back( ProfileEntry( elpStationFile, "StationFile", "./Configuration/Station.ini", "Station settings file", "File containing station settings", false ) );
         entries.push_back( ProfileEntry( elpQTHFile, "QTHFile", "./Configuration/QTH.ini", "QTH settings file", "File containing QTH settings", false ) );
         entries.push_back( ProfileEntry( elpLocsFile, "LocsFile", "./Configuration/LocSquares.ini", "Country locators file", "File containing valid locators for countries", false ) );

         entries.push_back( ProfileEntry( elpDisplayFile, "DisplayFile", "./Configuration/Display.ini", "Display settings file", "File containing saved display settings", false ) );
		 entries.push_back( ProfileEntry( elpDisplaySection, "DisplaySection", "Default", "Display file section", "Section to use in display file", false ) );
         entries.push_back( ProfileEntry( elpOperatorFile, "OperatorFile", "./Configuration/Operator.ini", "Operators file", "File containing operators", false ) );
		 entries.push_back( ProfileEntry( elpOperatorSection, "OperatorSection", "Default", "Operators file section", "section to use in operators file", false ) );
         entries.push_back( ProfileEntry( elpPreloadFile, "PreloadFile", "./Configuration/Preload.ini", "Log and List preload file", "File containing log and list pre-loads", false ) );
		 entries.push_back( ProfileEntry( elpPreloadSection, "PreloadSection", "Default", "Preload file section", "Section to use in preload file", false ) );
       entries.push_back( ProfileEntry( elpAutoFill, "AutoFill", false, "Auto Fill signal report", "Auto Fill signal report on return", false ) );

		 break;
	  case epPRELOADPROFILE:
		 entries.push_back( ProfileEntry( eppCurrent, "Current", 0, "", "hint", false ) );
         entries.push_back( ProfileEntry( epp1, "1", "", "", "hint", false ) );
         entries.push_back( ProfileEntry( epp2, "2", "", "", "hint", false ) );
         entries.push_back( ProfileEntry( epp3, "3", "", "", "hint", false ) );
         entries.push_back( ProfileEntry( epp4, "4", "", "", "hint", false ) );
         entries.push_back( ProfileEntry( epp5, "5", "", "", "hint", false ) );
         entries.push_back( ProfileEntry( epp6, "6", "", "", "hint", false ) );
         entries.push_back( ProfileEntry( epp7, "7", "", "", "hint", false ) );
         entries.push_back( ProfileEntry( epp8, "8", "", "", "hint", false ) );
         entries.push_back( ProfileEntry( epp9, "9", "", "", "hint", false ) );
         entries.push_back( ProfileEntry( epp10, "10", "", "", "hint", false ) );
         break;
      case epDISPLAYPROFILE:
         entries.push_back( ProfileEntry( edpTop, "Top", 10, "", "Top of Logger window", false ) );
         entries.push_back( ProfileEntry( edpLeft, "Left", 25, "", "Left of Logger window", false ) );
         entries.push_back( ProfileEntry( edpWidth, "Width", 780, "", "Width of Logger window", false ) );
         entries.push_back( ProfileEntry( edpHeight, "Height", 590, "", "Height of Logger window", false ) );
         entries.push_back( ProfileEntry( edpShowContinentEU, "ShowContinentEU", true, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpShowContinentAS, "ShowContinentAS", false, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpShowContinentAF, "ShowContinentAF", false, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpShowContinentOC, "ShowContinentOC", false, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpShowContinentSA, "ShowContinentSA", false, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpShowContinentNA, "ShowContinentNA", false, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpShowWorked, "ShowWorked", true, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpShowUnworked, "ShowUnworked", true, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpNextContactDetailsOnLeft, "ShowNextContactDetailsOnLeft", false, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpScrollingContestTabs, "ScrollingContestTabs", false, "", "hint", false ) );
         entries.push_back( ProfileEntry( edpShowOperators, "ShowOperators", true, "", "hint", false ) );

         entries.push_back( ProfileEntry( edpEditor, "Editor", "Notepad.exe", "", "Default editor", false ) );
         entries.push_back( ProfileEntry( edpAutoBandMapTuneAmount, "AutoBandMapTuneAmount", 2000, "", "Send to band map on tune more than x Hz ", false ) );
         entries.push_back( ProfileEntry( edpAutoBandMapTune, "AutoBandMapTune", false, "", "Enable send to band map on tune", false ) );
         entries.push_back( ProfileEntry( edpAutoBandMapTimeLapse, "AutoBandMapTimeLapse", 20, "", "Send to band map on leaving more than x seconds ", false ) );
         entries.push_back( ProfileEntry( edpAutoBandMapTime, "AutoBandMapTime", false, "", "Enable send to band map on time lapse", false ) );
         entries.push_back( ProfileEntry( edpStatisticsPeriod1, "Statistics Period 1", 10, "", "Statistics Period 1", false ) );
         entries.push_back( ProfileEntry( edpStatisticsPeriod2, "Statistics Period 2", 60, "", "Statistics Period 2", false ) );
         break;
      case epENTRYPROFILE:
         entries.push_back( ProfileEntry( eepCall, "Call", "", "Call Used", "Call sign used", false ) );
         entries.push_back( ProfileEntry( eepEntrant, "Entrant", "", "On Behalf Of (Club)", "Name of club/group", false ) );
         entries.push_back( ProfileEntry( eepMyName, "MyName", "", "My Name", "Name of responsible operator", false ) );
         entries.push_back( ProfileEntry( eepMyCall, "MyCall", "", "My Call", "Callsign of responsible operator", false ) );

         entries.push_back( ProfileEntry( eepMyAddress1, "MyAddress1", "", "My Address", "Address line 1 of responsible operator", false ) );
         entries.push_back( ProfileEntry( eepMyAddress2, "MyAddress2", "", "My Address", "Address line 2 of responsible operator", false ) );
         entries.push_back( ProfileEntry( eepMyCity, "MyCity", "", "My City", "City of responsible operator", false ) );
         entries.push_back( ProfileEntry( eepMyCountry, "MyCountry", "", "My Country", "Country of responsible operator", false ) );
         entries.push_back( ProfileEntry( eepMyPostCode, "MyPostCode", "", "My Postcode", "Post Code of responsible operator", false ) );
         entries.push_back( ProfileEntry( eepMyPhone, "MyPhone", "", "My Phone", "Phone no. of responsible operator", false ) );
         entries.push_back( ProfileEntry( eepMyEmail, "MyEmail", "", "My Email", "eMail address of responsible operator", false ) );
         break;
      case epQTHPROFILE:
         entries.push_back( ProfileEntry( eqpLocator, "Locator", "", "", "Locator", false ) );
         entries.push_back( ProfileEntry( eqpDistrict, "District", "", "", "District Exchange", false ) );
         entries.push_back( ProfileEntry( eqpLocation, "Location", "", "", "Descriptive Location Exchange", false ) );
         entries.push_back( ProfileEntry( eqpStationQTH1, "StationQTH1", "", "Station QTH", "Address line 1/2 of station", false ) );
         entries.push_back( ProfileEntry( eqpStationQTH2, "StationQTH2", "", "Station QTH", "Address line 2/2 of station", false ) );
         entries.push_back( ProfileEntry( eqpASL, "ASL", 0, "QTH Height ASL (metres)", "QTH height ASL (metres)", false ) );
         break;
      case epSTATIONPROFILE:
         entries.push_back( ProfileEntry( espPower, "Power", 0, "Transmitter Power", "Transmit Power (Watts)", false ) );
         entries.push_back( ProfileEntry( espTransmitter, "Transmitter", "", "", "Transmit Equipment", false ) );
         entries.push_back( ProfileEntry( espReceiver, "Receiver", "", "", "Receive Equipment", false ) );
         entries.push_back( ProfileEntry( espAntenna, "Antenna", "", "", "Antenna details", false ) );
         entries.push_back( ProfileEntry( espAGL, "AGL", 0, "Antenna Height AGL (metres)", "Antenna Height AGL (metres)", false ) );
		 entries.push_back( ProfileEntry( espOffset, "Bearing Offset", 0, "Antenna Bearing Offset", "Amount to offset antenna bearings", false ) );
		 break;

      case epLOCSQUARESPROFILE:
      default:
        break;
   }
}
bool BundleFile::openProfile( const QString &fname, const QString &bname )
{
   bundleName = bname;
   iniFile = boost::shared_ptr<INIFile>( new INIFile( fname ) );

   iniFile->loadINIFile();
   return true;
}
BundleFile::~BundleFile()
{
}
/*static*/
boost::shared_ptr<BundleFile>BundleFile::bundleFiles[ epSTATIONPROFILE + 1 ]; //  =  {0};
/*static*/
boost::shared_ptr<BundleFile>BundleFile::getBundleFile( PROFILES p )
{
   if ( !bool( bundleFiles[ p ] ) )
   {
      bundleFiles[ p ] = boost::shared_ptr<BundleFile>( new BundleFile( p ) );
   }
   return bundleFiles[ p ];
}
ProfileEntry &BundleFile::GetKey( int p )
{
   static ProfileEntry NullEntry( -1, "", 0,"",  "", false );
   for ( unsigned int i = 0; i < entries.size(); i++ )
   {
      if ( entries[ i ].id == p )
         return entries[ i ];
   }
   return NullEntry;
}
//=============================================================================
SettingsBundle::SettingsBundle()
{}

SettingsBundle::~SettingsBundle()
{
   try
   {
      closeProfile();
   }
   catch ( ... )
   {}
}
QString SettingsBundle::getBundle()
{
   return bundleFile->getBundle();
}

void SettingsBundle::setProfile( boost::shared_ptr<BundleFile> b )
{
   bundleFile = b;
}
void SettingsBundle::openSection( const QString &psect )
{
   if ( psect.size() )
      currsection = psect;
   else
      currsection = noneBundle;
}

bool SettingsBundle::isSectionPresent()
{
   std::vector<IniSectionPtr>::iterator thisSect = std::find_if( bundleFile->iniFile->sections.begin(), bundleFile->iniFile->sections.end(), INISectionCmp( currsection ) );
   if ( thisSect == bundleFile->iniFile->sections.end() )
   {
      return false;
   }

   if ( ( *thisSect ) ->entries.size() == 0)
   {
      return false;
   }
   return true;
}

void SettingsBundle::closeProfile()
{
   flushProfile();
}
QString SettingsBundle::getSection()
{
   return currsection;
}
bool SettingsBundle::newSection( const QString &newname )
{
   // Create a new section with no match
   std::vector<IniSectionPtr>::iterator newSect = std::find_if( bundleFile->iniFile->sections.begin(), bundleFile->iniFile->sections.end(), INISectionCmp( newname ) );
   if ( newSect == bundleFile->iniFile->sections.end() )
   {
      currsection = newname;
      for ( std::vector<ProfileEntry>::iterator i = bundleFile->entries.begin(); i != bundleFile->entries.end(); i++ )
      {
         ( *i ).createEntry( this );
      }
      return true;
   }
   return false;
}


bool SettingsBundle::dupSection( const QString &newname )
{
   if ( currsection == noneBundle )
   {
      return false;
   }
   // Create a new section that matches the old
   return bundleFile->iniFile->dupSection( currsection, newname );
}

bool SettingsBundle::checkKeyExists(const QString &key )
{
   if ( !bundleFile || currsection == noneBundle )
   {
      return false;
   }
   return bundleFile->iniFile->checkKeyExists( currsection, key );
}
QString SettingsBundle::displayNameOf( int enumKey )
{
   if ( bool( bundleFile ) && currsection != noneBundle )
   {
      for ( std::vector<ProfileEntry>::iterator i = bundleFile->entries.begin(); i != bundleFile->entries.end(); i++ )
      {
         if ( ( *i ).id == enumKey )
         {
            if (( *i ).dispname.size())
            {
               return ( *i ).dispname;
            }
            return ( *i ).name;
         }
      }
   }
   return "";
}
bool SettingsBundle::isReadOnly( int enumKey )
{
   if ( bool( bundleFile ) && currsection != noneBundle )
   {
      for ( std::vector<ProfileEntry>::iterator i = bundleFile->entries.begin(); i != bundleFile->entries.end(); i++ )
      {
         if ( ( *i ).id == enumKey )
         {
            return ( *i ).RO;
         }
      }
   }
   return "";
}

void SettingsBundle::getBoolProfile(const QString &key, bool &value, bool def )
{
   if ( !bundleFile || currsection == noneBundle )
   {
      value = def;
      return ;
   }
   int intval = bundleFile->iniFile->getPrivateProfileInt( currsection, key, ( int ) def );
   if ( intval )
      value = true;
   else
      value = false;
}
void SettingsBundle::getBoolProfile( int enumkey, bool &value )
{
   ProfileEntry & p = bundleFile->GetKey( enumkey );
   getBoolProfile( p.name, value, p.bdefaultval );
}
void SettingsBundle::getBoolProfile( int enumkey, MinosItem<bool> &value )
{
   bool temp;
   getBoolProfile( enumkey, temp );
   value.setValue( temp );
}
void SettingsBundle::setBoolProfile( const QString &key, bool value )
{
   if ( !bundleFile || currsection == noneBundle )
   {
      return ;
   }
   bundleFile->iniFile->writePrivateProfileString( currsection, key, value ? "1" : "0" );
}
void SettingsBundle::setBoolProfile( int enumkey, bool value )
{
   ProfileEntry & p = bundleFile->GetKey( enumkey );
   setBoolProfile( p.name, value );
}
void SettingsBundle::getStringProfile(const QString &key, QString &value, const QString &def )
{
   if ( !bundleFile || currsection == noneBundle )
   {
      value = def;
      return ;
   }
   QString inbuff;
   bundleFile->iniFile->getPrivateProfileString( currsection, key, def, inbuff );
   value = inbuff;
}
void SettingsBundle::getStringProfile( int enumkey, QString &value )
{
   ProfileEntry & p = bundleFile->GetKey( enumkey );
   getStringProfile( p.name, value, p.sdefaultval );
}

void SettingsBundle::getStringProfile(int enumkey, MinosItem<QString> &value )
{
   QString temp;
   getStringProfile( enumkey, temp );
   value.setValue( temp );
}
void SettingsBundle::setStringProfile( const QString &key, const QString &value )
{
   if ( !bundleFile || currsection == noneBundle )
   {
      return ;
   }
   bundleFile->iniFile->writePrivateProfileString( currsection, key, value );
}
void SettingsBundle::setStringProfile(int enumkey, const QString &value )
{
   ProfileEntry & p = bundleFile->GetKey( enumkey );
   setStringProfile( p.name, value );
}

void SettingsBundle::getIntProfile( const QString &key, int &value, int def )
{
   if ( !bundleFile || currsection == noneBundle )
   {
      value = def;
      return ;
   }
   value = bundleFile->iniFile->getPrivateProfileInt( currsection, key, def );
}
void SettingsBundle::getIntProfile( int enumkey, int &value )
{
   ProfileEntry & p = bundleFile->GetKey( enumkey );
   getIntProfile( p.name, value, p.idefaultval );
}
void SettingsBundle::getIntProfile( int enumkey, MinosItem<int> &value )
{
   int temp;
   getIntProfile( enumkey, temp );
   value.setValue( temp );
}
void SettingsBundle::setIntProfile(const QString &key, int value )
{
   if ( !bundleFile || currsection == noneBundle )
   {
      return ;
   }
   QString sval = QString::number( value );
   bundleFile->iniFile->writePrivateProfileString( currsection, key, sval );
}
void SettingsBundle::setIntProfile( int enumkey, int value )
{
   ProfileEntry & p = bundleFile->GetKey( enumkey );
   setIntProfile( p.name, value );
}

QStringList SettingsBundle::getProfileEntries( )
{
   QStringList sectlist;
   if ( !bundleFile || currsection == noneBundle )
   {
      return sectlist;
   }

   /*int mlen =*/ bundleFile->iniFile->getPrivateProfileList( currsection, "", sectlist );

   return sectlist;
}
std::vector<int> SettingsBundle::getBundleEntries( )
{
   std::vector<int> e;
   if ( bundleFile )
   {
      for ( std::vector<ProfileEntry>::iterator i = bundleFile->entries.begin(); i != bundleFile->entries.end(); i++ )
      {
         e.push_back( ( *i ).id );
      }
   }
   return e;

}
QStringList SettingsBundle::getBundleHints( )
{
   QStringList e;
   if ( bundleFile )
   {
      for ( std::vector<ProfileEntry>::iterator i = bundleFile->entries.begin(); i != bundleFile->entries.end(); i++ )
      {
         e.append( ( *i ).hint );
      }
   }
   return e;
}
QStringList SettingsBundle::getSections( )
{
   QStringList slist;
   if ( !bundleFile )
   {
      return slist;
   }
   slist.append( noneBundle );
   for ( std::vector <IniSectionPtr>::iterator thisSect = bundleFile->iniFile->sections.begin(); thisSect != bundleFile->iniFile->sections.end(); thisSect++ )
   {

      if ( ( *thisSect ) ->isValidSection() )
      {
         slist.append( ( *thisSect ) ->name );
      }
   }
   if (currsection.size() && std::find(slist.begin(), slist.end(), currsection) == slist.end())
   {
      slist.push_back(currsection);
   }
   return slist;
}
void SettingsBundle::clearProfileSection( bool clearCurr)
{
   // clear the content AND the section header
   if ( bundleFile )
   {
      bundleFile->iniFile->writePrivateProfileString( currsection, 0, 0 );
      if (clearCurr)
      {
         currsection = noneBundle;
      }
   }
}
void SettingsBundle::flushProfile( void )
{
   if ( !bundleFile || !bundleFile->iniFile )
   {
      return ;
   }
   bundleFile->iniFile->writePrivateProfileString( 0, 0, 0 );
}

