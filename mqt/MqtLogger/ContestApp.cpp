/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "logger_pch.h"

#include "LoggerContest.h"
#include "MatchContact.h"
#include "MatchThread.h"
//#include "MultDisp.h"
//#include "StatsDisp.h"

//---------------------------------------------------------------------------

#define INITIAL_CONTEST_SLOTS 2
#define INITIAL_LIST_SLOTS 10

double bigClockCorr = 0;
TContestApp *TContestApp::contestApp = 0;
int inFontChange = 0;

TContestApp *TContestApp::getContestApp()
{
   static bool firstTime = true;
   if ( !contestApp && firstTime )
   {
      firstTime = false;
      contestApp = new TContestApp;
      contestApp->initialise();
   }
   return contestApp;
}
//---------------------------------------------------------------------------

bool isOpen(ContestSlot *cs, const QString &fn )
{
   BaseContestLog * con = cs->slot;
   if ( !bool( con ) )
      return false;
   if ( con->cfileName.compare(fn, Qt::CaseInsensitive ) == 0 )
      return true;
   return false;
}
bool isOpen(ListSlot *cs, const QString &fn )
{
   ContactList * con = cs->slot;
   if ( !bool( con ) )
      return false;
   if ( con->cfileName.compare(fn, Qt::CaseInsensitive ) == 0 )
      return true;
   return false;
}
ContestSlot::ContestSlot( void ) : slotno( -1 ), slot( 0 )
{}
ContestSlot::~ContestSlot()
{
   if ( slot )
   {
      delete slot;
      slot = 0;
   }
}
ListSlot::ListSlot( void ) : slotno( -1 ), slot( 0 )
{}
ListSlot::~ListSlot()
{
   if ( slot )
   {
      delete slot;
      slot = 0;
   }
}


//---------------------------------------------------------------------------
static void initClock( void )
{
   std::ifstream strm( "./Configuration/time.correction", std::ios::binary ); // should close when it goes out of scope
   if ( strm )
   {
      long big_corr;
      strm >> big_corr;
      bigClockCorr = big_corr / dtg::daySecs;
   }
}
bool TContestApp::initialise()
{
   // Force the short date format to be as we expect

   //ShortDateFormat = "dd/MM/yyyy";
   // Under Linux/Wine (in particular) the current working directory may not
   // be where expected. So reset it to where it should be

   // Eventually, we should have an installation system so that e.g. under Vista
   // we can put things like config under the user directory

   if (!DirectoryExists("./Configuration"))
   {
      // try for executable directory
      QString fpath = QCoreApplication::applicationDirPath();

      if (DirectoryExists(fpath + "/Configuration"))
      {
         QDir::setCurrent(fpath);
      }
      while (!DirectoryExists("./Configuration"))
      {
         QString destDir = QFileDialog::getExistingDirectory(
                       0,
                       "Set Minos Working Directory",
                       fpath,
                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                        );
         if ( !destDir.isEmpty() )
         {

            if (destDir.toUpper().indexOf("/CONFIGURATION") == destDir.size() - QString("/Configuration").size())
            {
               destDir = destDir.left(destDir.size() - QString("/Configuration").size());
            }
            QDir::setCurrent(destDir);
         }
      }
   }
   // delay opening the trace file until we know where to put it
   enableTrace( "./TraceLog", "MinosLogger_" );
   // we need to open our bundles...
   // and we need to discover the defaults from the initial splash screen

   // Where do we get the initial defaults from? I think we need an "initial"
   // config file that we can use to store the others...


   //----------------------------------

   BundleFile::bundleFiles[ epLOGGERPROFILE ] = QSharedPointer<BundleFile>( new BundleFile( epLOGGERPROFILE ) );
   BundleFile::bundleFiles[ epPRELOADPROFILE ] = QSharedPointer<BundleFile>( new BundleFile( epPRELOADPROFILE ) );
   BundleFile::bundleFiles[ epDISPLAYPROFILE ] = QSharedPointer<BundleFile>( new BundleFile( epDISPLAYPROFILE ) );
   BundleFile::bundleFiles[ epENTRYPROFILE ] = QSharedPointer<BundleFile>( new BundleFile( epENTRYPROFILE ) );
   BundleFile::bundleFiles[ epQTHPROFILE ] = QSharedPointer<BundleFile>( new BundleFile( epQTHPROFILE ) );
   BundleFile::bundleFiles[ epSTATIONPROFILE ] = QSharedPointer<BundleFile>( new BundleFile( epSTATIONPROFILE ) );
   BundleFile::bundleFiles[ epLOCSQUARESPROFILE ] = QSharedPointer<BundleFile>( new BundleFile( epLOCSQUARESPROFILE ) );

   //----------------------------------
   BundleFile::bundleFiles[ epLOGGERPROFILE ] ->openProfile( "./Configuration/MinosLogger.ini", "Logger Defaults" );
   loggerBundle.setProfile( BundleFile::bundleFiles[ epLOGGERPROFILE ] );
   loggerBundle.openSection( "Default" );

   //----------------------------------
   QString preloadfile;
   loggerBundle.getStringProfile( elpPreloadFile, preloadfile );
   BundleFile::bundleFiles[ epPRELOADPROFILE ] ->openProfile( preloadfile, "Preload options" );

   preloadBundle.setProfile( BundleFile::bundleFiles[ epPRELOADPROFILE ] );

   QString preloadsect;
   loggerBundle.getStringProfile( elpPreloadSection, preloadsect );
   preloadBundle.openSection( preloadsect );
   //----------------------------------

   QString dispfile;
   loggerBundle.getStringProfile( elpDisplayFile, dispfile );
   BundleFile::bundleFiles[ epDISPLAYPROFILE ] ->openProfile( dispfile, "Display defaults" );

   displayBundle.setProfile( BundleFile::bundleFiles[ epDISPLAYPROFILE ] );

   QString dispsect;
   loggerBundle.getStringProfile( elpDisplaySection, dispsect );
   displayBundle.openSection( dispsect );
   //----------------------------------

   QString entfile;
   loggerBundle.getStringProfile( elpEntryFile, entfile );
   BundleFile::bundleFiles[ epENTRYPROFILE ] ->openProfile( entfile, "Contest Entry details" );
   //----------------------------------

   QString qthfile;
   loggerBundle.getStringProfile( elpQTHFile, qthfile );
   BundleFile::bundleFiles[ epQTHPROFILE ] ->openProfile( qthfile, "QTH details" );
   //----------------------------------

   QString stationfile;
   loggerBundle.getStringProfile( elpStationFile, stationfile );
   BundleFile::bundleFiles[ epSTATIONPROFILE ] ->openProfile( stationfile, "Station details" );
   //----------------------------------

   QString locsfile;
   loggerBundle.getStringProfile( elpLocsFile, locsfile );
   BundleFile::bundleFiles[ epLOCSQUARESPROFILE ] ->openProfile( locsfile, "Valid locator squares" );

   locsBundle.setProfile( BundleFile::bundleFiles[ epLOCSQUARESPROFILE ] );
   //----------------------------------


   initClock();
   TMatchThread::InitialiseMatchThread();

   return true;
}
bool contestAppLoadFiles( void )
{
   return MultLists::getMultLists(); // ->loadMultFiles();
}
void closeContestApp( void )
{
   if ( TContestApp::getContestApp() )
   {
      TContestApp::getContestApp() ->close();
   }
   delete TContestApp::getContestApp();
}
void TContestApp::close()
{
   writeContestList();
   TMatchThread::FinishMatchThread();
}
TContestApp::TContestApp() : MinosParameters(), magneticVariation( 0 ), period1( 5 ), period2( 20 ),
      reverseBearing( false ), dispCountry( false ), dispBearing( true ), dispScore( true ),
      currentContest( 0 ), preloadComplete(false)

{

   for ( int i = 0; i < INITIAL_CONTEST_SLOTS; i++ )
   {
      ContestSlot *cs = new ContestSlot;
      cs->slotno = i + 1;
      contestSlotList.insert( cs );
   }
   for ( int i = 0; i < INITIAL_LIST_SLOTS; i++ )
   {
      ListSlot *cs = new ListSlot;
      cs->slotno = i + 1;
      listSlotList.insert( cs );
   }
}
TContestApp::~TContestApp()
{
   freeAll();           // closes slots - and maybe bundles

   loggerBundle.closeProfile();
   preloadBundle.closeProfile();
   displayBundle.closeProfile();
   contestApp = 0;

   delete MultLists::getMultLists();
   CsGuard::ClearDown();
}

bool TContestApp::insertContest( BaseContestLog * p, unsigned int sno )
{

   // called from LoggerContestLog::initialise
   if ( ( int ) sno != -1 )
   {
      if ( sno >= contestSlotList.size() )
      {
         for ( unsigned int i = contestSlotList.size(); i <= sno; i++ )
         {
            ContestSlot *cs = new ContestSlot;
            cs->slotno = i + 1;
            contestSlotList.insert( cs );
         }
      }
      ContestSlot *cs = contestSlotList[ sno ];
      if ( bool( cs->slot ) )
         return false;
      else
      {
         cs->slot = p;
         return true;
      }
   }
   for ( unsigned int i = 0; i < contestSlotList.size(); i++ )
   {
      ContestSlot *cs = contestSlotList[ i ];
      if ( !bool( cs->slot ) )
      {
         cs->slot = p;
         return true;
      }
   }
   ContestSlot *cs = new ContestSlot;
   cs->slot = p;
   cs->slotno = contestSlotList.size() + 1;
   contestSlotList.insert( cs );
   return true;
}
bool TContestApp::insertList( ContactList * p, unsigned int sno )
{

   // called from LoggerContestLog::initialise
   if ( ( int ) sno != -1 )
   {
      if ( sno >= listSlotList.size() )
      {
         for ( unsigned int i = listSlotList.size(); i <= sno; i++ )
         {
            ListSlot *cs = new ListSlot;
            cs->slotno = i + 1;
            listSlotList.insert( cs );
         }
      }
      ListSlot *cs = listSlotList[ sno ];
      if ( cs->slot )
         return false;
      else
      {
         cs->slot = p;
         return true;
      }
   }
   for ( unsigned int i = 0; i < listSlotList.size(); i++ )
   {
      ListSlot *cs = listSlotList[ i ];
      if ( !cs->slot )
      {
         cs->slot = p;
         return true;
      }
   }
   ListSlot *cs = new ListSlot;
   cs->slot = p;
   cs->slotno = listSlotList.size() + 1;
   //   contestSlotList.push_back( cs );
   listSlotList.insert( cs );
   return true;
}
int TContestApp::getOccupiedListSlotCount()
{
   int cnt = 0;
   for ( ListSlotIterator i = listSlotList.begin(); i != listSlotList.end(); i++ )
   {
      if ( ( *i ) && ( *i ) ->slot )
      {
         cnt++;
      }
   }
   return cnt;
}

bool TContestApp::isListOpen( const QString fn )
{
   for ( ListSlotIterator i = listSlotList.begin(); i != listSlotList.end(); i++ )
   {
      if ( isOpen( ( *i ), fn ) )
      {
         mshowMessage( "File " + fn + " is already open!" );
         return true;
      }
   }
   return false;
}
bool TContestApp::isContestOpen(const QString fn )
{
   for ( SlotIterator i = TContestApp::getContestApp() ->contestSlotList.begin(); i != TContestApp::getContestApp() ->contestSlotList.end(); i++ )
   {
      if ( isOpen( ( *i ), fn ) )
      {
         MinosParameters::getMinosParameters() ->mshowMessage( "File " + fn + " is already open!" );
         return true;
      }
   }
   return false;
}


BaseContestLog * TContestApp::findFirstContest()
{
   for ( unsigned int i = 0; i < contestSlotList.size(); i++ )
   {
      ContestSlot *cs = contestSlotList[ i ];
      if ( bool( cs->slot ) )
      {
         return cs->slot;
      }
   }
   return 0;
}
int TContestApp::findContest( BaseContestLog * p )
{
   for ( unsigned int i = 0; i < contestSlotList.size(); i++ )
   {
      ContestSlot *cs = contestSlotList[ i ];
      if ( cs->slot && p == cs->slot )
      {
         return i;
      }
   }
   return -1;
}
int TContestApp::findList( ContactList * p )
{
   for ( unsigned int i = 0; i < listSlotList.size(); i++ )
   {
      ListSlot *cs = listSlotList[ i ];
      if ( cs && cs->slot && p == cs->slot )
      {
         return i;
      }
   }
   return -1;
}
void TContestApp::removeContest( BaseContestLog * p )
{
   int i = findContest( p );
   if ( i >= 0 )
   {
      if ( getCurrentContest() == p )
         setCurrentContest( 0 );
      ContestSlot * cs = contestSlotList[ i ];
      cs->slot = 0;
   }
}
void TContestApp::removeList( ContactList * p )
{
   int i = findList( p );
   if ( i >= 0 )
   {
      ListSlot * cs = listSlotList[ i ];
      cs->slot = 0;
   }
}
void TContestApp::writeContestList()
{
    if (preloadComplete)
    {
        preloadBundle.clearProfileSection( false );
        for ( unsigned int i = 0; i < contestSlotList.size(); i++ )
        {
            ContestSlot *cs = contestSlotList[ i ];
            BaseContestLog * ct = cs->slot;
            if ( !ct )
                continue;

            QString ent = QString::number(cs->slotno );
            preloadBundle.setStringProfile( ent, ct->cfileName );
            if ( currentContest == ct )
            {
                preloadBundle.setIntProfile( eppCurrent, cs->slotno );
            }
        }
        for ( unsigned int i = 0; i < listSlotList.size(); i++ )
        {
            ListSlot *cs = listSlotList[ i ];
            ContactList * ct = cs->slot;
            if ( !ct )
                continue;

            QString ent = "List" + QString::number(cs->slotno );
            preloadBundle.setStringProfile( ent, ct->cfileName );
            // no need for any concept of a "current" list
        }
        preloadBundle.flushProfile();
    }
}
std::vector<BaseContestLog *> TContestApp::getContestList()
{
   std::vector<BaseContestLog *> logList;
   for ( unsigned int i = 0; i < contestSlotList.size(); i++ )
   {
      ContestSlot *cs = contestSlotList[ i ];
      BaseContestLog * ct = cs->slot;
      if ( !ct )
         continue;

      logList.push_back(ct);
   }
   return logList;
}
LoggerContestLog * TContestApp::openFile( const QString &fn, bool newFile, int slotno )
{
   LoggerContestLog * contest = new LoggerContestLog();

   if ( !contest->initialise( fn, newFile, slotno ) )    // this adds it to the slot
   {
      closeFile( contest );
      return 0;
   }

   return contest;
}
ContactList * TContestApp::openListFile(const QString &fn, int slotno )
{
   ContactList * list = new ContactList();
   if ( !list->initialise( fn, slotno ) )    // this adds it to the slot
   {
      removeList( list ); 		// must remove LoggerContestLog from its slot
      delete list;
      return 0;
   }

   return list;
}
void TContestApp::closeFile( BaseContestLog * contest )
{
   if ( contest )
   {
      removeContest( contest ); 		// must remove LoggerContestLog from its slot
      delete contest;
   }

   writeContestList();
}
// Predicate function for remove_if
bool noslot( ListSlot *ip )
{
   if ( ip == 0 )
      return true;
   else
      return false;
}
void TContestApp::closeListFile( ContactList *list )
{
   if ( list )
   {
      removeList( list ); 		// must remove LoggerContestLog from its slot
      delete list;
   }
   listSlotList.erase( std::remove_if( listSlotList.begin(), listSlotList.end(), noslot ), listSlotList.end() );
   writeContestList();
}
BaseContestLog * TContestApp::getCurrentContest()
{
   return currentContest;
}
void TContestApp::setCurrentContest( BaseContestLog * c )
{
   currentContest = c;
   int cc = findContest( c );
   preloadBundle.setIntProfile( eppCurrent, cc );
   preloadBundle.flushProfile();
}

void TContestApp::getDisplayColumnWidth( const QString &key, int &val, int def )
{
   displayBundle.getIntProfile( key, val, def );
}
void TContestApp::setDisplayColumnWidth( const QString &key, int val )
{
   if (inFontChange <= 0)
   {
      if ( val < 0 )
      {
         displayBundle.setStringProfile( key, 0 );
      }
      else
      {
         displayBundle.setIntProfile( key, val );
      }
   }
}
void TContestApp::getBoolDisplayProfile( int enumkey, bool &value )
{
   displayBundle.getBoolProfile( enumkey, value );
}
void TContestApp::setBoolDisplayProfile( int enumkey, bool value )
{
   displayBundle.setBoolProfile( enumkey, value );
}
void TContestApp::flushDisplayProfile( void )
{
   displayBundle.flushProfile();
}
int TContestApp::getBigClockCorrection()
{
   return bigClockCorr;
}

int TContestApp::getStatsPeriod1()
{
   displayBundle.getIntProfile( edpStatisticsPeriod1, period1 );
   return period1;
}
int TContestApp::getStatsPeriod2()
{
   displayBundle.getIntProfile( edpStatisticsPeriod2, period2 );
   return period2;
}
void TContestApp::setStatsPeriod1( int s1 )
{
   period1 = s1;
   displayBundle.setIntProfile( edpStatisticsPeriod1, period1 );
   displayBundle.flushProfile();
}
void TContestApp::setStatsPeriod2( int s2 )
{
   period2 = s2;
   displayBundle.setIntProfile( edpStatisticsPeriod2, period2 );
   displayBundle.flushProfile();
}
bool TContestApp::getAllowLoc4()
{
   BaseContestLog * ct = TContestApp::getCurrentContest();
   if ( !ct )
   {
      return false;
   }
   return ct->allowLoc4.getValue();
}
bool TContestApp::getAllowLoc8()
{
   BaseContestLog * ct = TContestApp::getCurrentContest();
   if ( !ct )
   {
      return false;
   }
   return ct->allowLoc8.getValue();
}
bool TContestApp::yesNoMessage( QWidget* Owner, const QString &mess )
{
   return mShowYesNoMessage( Owner, mess );
}
void TContestApp::mshowMessage( const QString &mess, QWidget* Owner )
{
   mShowMessage( mess, Owner );
}
