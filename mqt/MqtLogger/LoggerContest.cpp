/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "logger_pch.h"

#include "AdifImport.h"
#include "reg1test.h"
#include "printfile.h"
#include "enqdlg.h"
#include "MinosTestExport.h"
#include "LoggerContest.h"
//#include "TMinosBandChooser.h"
#include "BandList.h"

LoggerContestLog::LoggerContestLog( void ) : BaseContestLog(),
      GJVFile( false ), minosFile( false ),
      logFile( false ), adifFile( false ), ediFile( false ),
      needExport( false )
{
}
void LoggerContestLog::makeContact( bool timeNow, BaseContact *&lct )
{
   lct = new ContestContact( this, timeNow );
}
LoggerContestLog::~LoggerContestLog()
{
   closeFile();
}
void LoggerContestLog::initialiseINI()
{
   entryBundle.setProfile( BundleFile::bundleFiles[ epENTRYPROFILE ] );
   QTHBundle.setProfile( BundleFile::bundleFiles[ epQTHPROFILE ] );
   stationBundle.setProfile( BundleFile::bundleFiles[ epSTATIONPROFILE ] );
   entryBundleName.setValue( entryBundle.getSection() );
   QTHBundleName.setValue( QTHBundle.getSection() );
   stationBundleName.setValue( stationBundle.getSection() );
}
void LoggerContestLog::clearDirty()
{
   entryBundleName.clearDirty();
   QTHBundleName.clearDirty();
   stationBundleName.clearDirty();

   entrant.clearDirty();
   sqth1.clearDirty();
   sqth2.clearDirty();
   entTx.clearDirty();
   entRx.clearDirty();
   entAnt.clearDirty();
   entAGL.clearDirty();
   entASL.clearDirty();
   ops1.clearDirty();
   ops2.clearDirty();
   entCondx1.clearDirty();
   entCondx2.clearDirty();
   entCondx3.clearDirty();
   entCondx4.clearDirty();
   entName.clearDirty();
   entCall.clearDirty();
   entSect.clearDirty();
   entAddr1.clearDirty();
   entAddr2.clearDirty();
   entPostCode.clearDirty();
   entCity.clearDirty();
   entCountry.clearDirty();
   entPhone.clearDirty();
   entEMail.clearDirty();
   sectionList.clearDirty();
   bearingOffset.clearDirty();
   BaseContestLog::clearDirty();
}
void LoggerContestLog::setDirty()
{
   entryBundleName.setDirty();
   QTHBundleName.setDirty();
   stationBundleName.setDirty();

   entrant.setDirty();
   sqth1.setDirty();
   sqth2.setDirty();
   entTx.setDirty();
   entRx.setDirty();
   entAnt.setDirty();
   entAGL.setDirty();
   entASL.setDirty();
   sectionList.setDirty();
   ops1.setDirty();
   ops2.setDirty();
   entCondx1.setDirty();
   entCondx2.setDirty();
   entCondx3.setDirty();
   entCondx4.setDirty();
   entName.setDirty();
   entCall.setDirty();
   entSect.setDirty();
   entAddr1.setDirty();
   entAddr2.setDirty();
   entPostCode.setDirty();
   entCity.setDirty();
   entCountry.setDirty();
   entPhone.setDirty();
   entEMail.setDirty();
   bearingOffset.setDirty();

   BaseContestLog::setDirty();
}
bool LoggerContestLog::initialise( int sno )
{
   if ( !TContestApp::getContestApp() ->insertContest( this, sno ) )
   {
	  return false;
   }
   cslotno = sno;
   return true;
}

bool LoggerContestLog::initialise( const QString &fn, bool newFile, int slotno )
{
   if ( !initialise( slotno ) )
      return false;

   if ( TContestApp::getContestApp() ->isContestOpen( fn ) )
   {
      return false;
   }

   // open the settings bundle files
   initialiseINI();

   // open the LoggerContestLog file

   cfileName = fn;
   QString ext = ExtractFileExt( fn );
   publishedName = ExtractFileName( fn );
   if ( ext.compare(".gjv", Qt::CaseInsensitive ) == 0 )
   {
      GJVFile = true;
      setUnwriteable(true);
   }
   else
       if ( ext.compare(".Minos", Qt::CaseInsensitive ) == 0 )
      {
         minosFile = true;
      }
      else
         if ( ext.compare(".log", Qt::CaseInsensitive ) == 0 )
         {
            logFile = true;
            setUnwriteable(true);
         }
         else
            if ( ext.compare(".adi", Qt::CaseInsensitive ) == 0 )
            {
               adifFile = true;
               setUnwriteable(true);
            }
            else
               if ( ext.compare(".edi", Qt::CaseInsensitive ) == 0 )
               {
                  ediFile = true;
                  setUnwriteable(true);
               }
               else
               {
                  MinosParameters::getMinosParameters() ->mshowMessage( "Not a known file type! (" + ext + ")" );
                  return false;
               }
   if ( !newFile )
   {
      if (!FileAccessible(fn))
      {
          setUnwriteable(true);
      }

      QIODevice::OpenMode om = QIODevice::ReadWrite | QIODevice::Unbuffered;
      if (isUnwriteable())
      {
        om = QIODevice::ReadOnly;
      }
      QSharedPointer<QFile> contestFile(new QFile(fn));

      if (!contestFile->open(om))
      {
         QString lerr = contestFile->errorString();
         QString emess = "Failed to open LoggerContestLog file " + fn + " : " + lerr;
         MinosParameters::getMinosParameters() ->mshowMessage( emess );
         return false;
      }

      bool loadOK = false;
      needExport = false;

      if ( GJVFile )
      {
         GJVcontestFile = contestFile;
         if ( !GJVload() )     // load the header so that we can display it
            return false;
         loadOK = GJVloadContacts();
      }
      else
         if ( minosFile )
         {
            minosContestFile = contestFile;
            MinosTestImport mt( this );
            stanzaCount = mt.importTest( minosContestFile );
            if ( stanzaCount > 0 )
            {
               // set the bundles accordingly
               entryBundle.openSection( entryBundleName.getValue() );
               QTHBundle.openSection( QTHBundleName.getValue() );
               stationBundle.openSection( stationBundleName.getValue() );
               loadOK = true;
            }
         }
         else
            if ( logFile )
            {
               logContestFile = contestFile;
               loadOK = importLOG( logContestFile );
               needExport = true;
            }
            else
               if ( adifFile )
               {
                  adifContestFile = contestFile;
                  loadOK = importAdif( adifContestFile );
                  needExport = true;
               }
               else
                  if ( ediFile )
                  {
                     ediContestFile = contestFile;
                     loadOK = importReg1Test( ediContestFile );
                     needExport = true;
                  }
      scanContest();
      clearDirty();  // what we have just read CAN'T be dirty
      validateLoc();
      // run_contest_dialog has already loaded the LoggerContestLog and set log_count
      // here, we display a "loading" box
      if ( isUnwriteable() )     // Minos files can be unprotected if not realy RO
         closeFile();				// to preserve file handles

      if ( !loadOK )    // sets ct as well
      {
         return false;
      }
   }
   else
      if ( minosFile )
      {

          QIODevice::OpenMode om = QIODevice::ReadWrite | QIODevice::Unbuffered;
          if (isUnwriteable())
          {
            om = QIODevice::ReadOnly;
          }
          QSharedPointer<QFile> contestFile(new QFile(fn));

          if (!contestFile->open(om))
          {
             QString lerr = contestFile->errorString();
             QString emess = "Failed to create LoggerContestLog file " + fn + " : " + lerr;
             MinosParameters::getMinosParameters() ->mshowMessage( emess );
             return false;
          }

         if ( GJVFile )
         {
            GJVcontestFile = contestFile;
         }
         else
            if ( minosFile )
            {
               minosContestFile = contestFile;
            }
      }
      else
      {
         return false;
      }
   commonSave( newFile );

   return true;
}
void LoggerContestLog::setINIDetails()
{
   // extras that CAN come from INI file - implements bundle override
   if ( entryBundle.getSection() != noneBundle )
   {
      entryBundle.getStringProfile( eepCall, mycall.fullCall );
      entryBundle.getStringProfile( eepEntrant, entrant );
      entryBundle.getStringProfile( eepMyName, entName );
      entryBundle.getStringProfile( eepMyCall, entCall );
      //   entryBundle.getStringProfile(eepSection, entSect);

      entryBundle.getStringProfile( eepMyAddress1, entAddr1 );
      entryBundle.getStringProfile( eepMyAddress2, entAddr2 );
      entryBundle.getStringProfile( eepMyCity, entCity );
      entryBundle.getStringProfile( eepMyPostCode, entPostCode );
      entryBundle.getStringProfile( eepMyCountry, entCountry );
      entryBundle.getStringProfile( eepMyPhone, entPhone );
      entryBundle.getStringProfile( eepMyEmail, entEMail );
      //   entryBundle.getStringProfile(eepBand, band);
      //   entryBundle.getStringProfile(eepSection, entSect);
   }


   if ( QTHBundle.getSection() != noneBundle )
   {
      QTHBundle.getStringProfile( eqpLocator, myloc.loc );

      if ( districtMult.getValue() )
         QTHBundle.getStringProfile( eqpDistrict, location );
      else
         if ( otherExchange.getValue() && location.getValue().size() == 0 )
            QTHBundle.getStringProfile( eqpLocation, location );

      QTHBundle.getStringProfile( eqpStationQTH1, sqth1 );
      QTHBundle.getStringProfile( eqpStationQTH2, sqth2 );
	  QTHBundle.getStringProfile( eqpASL, entASL );
   }

   if ( stationBundle.getSection() != noneBundle )
   {
	  stationBundle.getStringProfile( espPower, power );
	  stationBundle.getStringProfile( espTransmitter, entTx );
	  stationBundle.getStringProfile( espReceiver, entRx );
	  stationBundle.getStringProfile( espAntenna, entAnt );
	  stationBundle.getStringProfile( espAGL, entAGL );
	  stationBundle.getIntProfile(espOffset, bearingOffset);
   }
}

int LoggerContestLog::readBlock( int bno )
{
    bool sres = GJVcontestFile->seek(bno * bsize);
   if ( !sres)
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "(read) seek failed!" );
   }

   int rsize = GJVcontestFile->read(diskBuffer, bsize);

   diskBuffer[ rsize ] = 0;
   if ( rsize < bsize )
      MinosParameters::getMinosParameters() ->mshowMessage( "Short read" );

   return rsize;
}
int LoggerContestLog::writeBlock(QSharedPointer<QFile> fd, int bno )
{
   // fd will not be contest_file if we are exporting a GJV file

   if ( !fd.data() )
      return bsize;		//tell lies

   int n = atoi( diskBuffer );
   if ( n != bno )
      MinosParameters::getMinosParameters() ->mshowMessage( "Invalid block number for write!!" );

   bool sres = GJVcontestFile->seek(bno * bsize);
   if ( !sres )
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "(write) seek failed!" );
   }
   int ret = fd->write(diskBuffer, bsize);
   if ( ret != bsize )
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "bad reply from write!" );
   }
   return ret;
}
void LoggerContestLog::closeFile( void )
{
   if ( GJVcontestFile.data() )
   {
      GJVcontestFile->close();
   }
   if ( minosContestFile.data() )
   {
      minosContestFile->close();
   }
   if ( logContestFile.data() )
   {
      logContestFile->close();
   }
   if ( adifContestFile.data() )
   {
      adifContestFile->close();
   }
   if ( ediContestFile.data() )
   {
      ediContestFile->close();
   }

   GJVcontestFile.reset();
   minosContestFile.reset();
   logContestFile.reset();
   adifContestFile.reset();
   ediContestFile.reset();
}
DisplayContestContact *LoggerContestLog::addContact( int newctno, int extraFlags, bool saveNew, bool catchup )
{
   // add the contact number as an new empty contact, with disk block and log_seq

   bool timenow = true;
   if ( ( extraFlags & TO_BE_ENTERED ) || catchup )
      timenow = false;

   BaseContact *bct = 0;
   makeContact( timenow, bct );
   DisplayContestContact *lct = dynamic_cast<DisplayContestContact *> (bct);

   QString temp = QString( "%1" ).arg(newctno, 3 );
   lct->serials.setValue( temp );
   lct->setLogSequence( nextBlock << 16 );
   nextBlock++;
   if ( newctno > maxSerial )
   {
      maxSerial = newctno;
   }
   lct->contactFlags.setValue( lct->contactFlags.getValue() | extraFlags );

   if (catchup)
   {
      lct->op1 = currentOp1;
      lct->op2 = currentOp2;
   }
   if ( saveNew )
   {
      lct->commonSave();		// make sure contact is correct
   }
   ctList.insert( lct );
   if ( saveNew )
   {
      commonSave( false );
   }

   return lct;
}
DisplayContestContact *LoggerContestLog::addContactBetween( BaseContact *prior, BaseContact *next )
{
   // add the contact number as an new empty contact, with disk block and log_seq

   if (!next)
   {
      MinosParameters::getMinosParameters() ->mshowMessage("Attempt to insert after last contact - not allowed. Pease report a bug!");
      return 0;
   }
   bool timenow = false;

   BaseContact *bct = 0;
   makeContact( timenow, bct );
   DisplayContestContact *lct = dynamic_cast<DisplayContestContact *> (bct);

   lct->serials.setValue( "" );

   unsigned long pls =  prior?prior->getLogSequence():0;
   unsigned long nls =  next->getLogSequence();

   unsigned long seq = (pls + nls)/2;

   lct->setLogSequence( seq );

   lct->commonSave();		// make sure contact is correct
   ctList.insert( lct );
   commonSave( false );

   return lct;
}
//==========================================================================
void LoggerContestLog::removeContact( DisplayContestContact *lct )
{
   ctList.erase(lct);
   delete lct;
}
//==========================================================================
bool LoggerContestLog::commonSave( bool newfile )
{
   if ( !isReadOnly() )
   {
      if ( GJVFile )
      {
         MinosParameters::getMinosParameters() ->mshowMessage( "GJV should have been opened read only!" );
         return false; // it better be read only!
      }
      else
         if ( minosFile )
         {
            return minosSaveFile( newfile );
         }
   }
   return false;
}
//==========================================================================
bool LoggerContestLog::minosSaveFile( bool newfile )
{
   MinosTestExport mt( this );
   stanzaCount += mt.exportAllDetails( minosContestFile, newfile );
   clearDirty();
   return true;
}
bool LoggerContestLog::minosSaveContestContact( const ContestContact *lct )
{
   MinosTestExport mt( this );
   stanzaCount += mt.exportQSO( minosContestFile, lct );
   return true;
}
//==========================================================================
bool LoggerContestLog::GJVsave( GJVParams &gp )
{
   const QString nulc;

   // save the LoggerContestLog details to file_desc
   clearBuffer();
   buffpt = 0;

   strtobuf( "0" );    // block number for LoggerContestLog block
   strtobuf( GJVVERSION );
   strtobuf( band );
   strtobuf( name );
   strtobuf( mycall.fullCall );
   strtobuf( myloc.loc );
   strtobuf( location );

   opyn( otherExchange );
   opyn( false /*Country_mult && County_mult*/ );	//CC_mult
   opyn( locMult );

   QString temp = QString::number( gp.count ); // leave off the dummy entry!
   strtobuf( temp );

   strtobuf( power );

   strtobuf( nulc );
   //   strtobuf( mainOpNow );
   strtobuf( nulc );
   //   strtobuf( secondOpNow );
   strtobuf( mode );
   opyn( true );						// RADIAL rings are DEAD
   opyn( false );                // was post entry
   opyn( scoreMode.getValue() == PPQSO );
   opyn( countryMult );
   opyn( false /*County_mult*/ );

   opyn( districtMult );

   opyn( powerWatts );
   opyn( false /*scoreMode == GSPECIAL*/ );
   opyn( allowLoc8 );

   opyn( !RSTField.getValue() );
   opyn( !serialField.getValue() );
   opyn( !locatorField.getValue() );
   opyn( !QTHField.getValue() );

   strtobuf(); // clear tail

   QSharedPointer<QFile> fd = gp.fd;

   writeBlock( fd, 0 );

   // note that each contact will be saved as entered
   return true;
}
bool LoggerContestLog::GJVload( void )
{
   QString temp;
   logCount = 0;

   // load the LoggerContestLog details from file_desc
   clearBuffer();
   readBlock( 0 );
   buffpt = 0;
   buftostr( temp );
   if ( temp.toInt() != 0 )
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "Invalid block 0 in LoggerContestLog file" );
      return false;
   }

   buftostr( temp );
   if ( strnicmp( temp, GJVVERSION, VERLENGTH ) != 0 )
   {
      MinosParameters::getMinosParameters() ->mshowMessage( QString( "Invalid LoggerContestLog file format (" ) + temp + ", " + GJVVERSION + " expected)" );
      return false;
   }

   buftostr( band );
   buftostr( name );
   buftostr( temp );
   mycall = callsign( strupr( temp ) );
   buftostr( myloc.loc );
   buftostr( location );

   otherExchange.setValue( inyn() );
   inyn();
   locMult.setValue( inyn() );

   buftostr( temp );
   logCount = temp.toInt();

   buftostr( power );

   buftostr( temp );
   //   buftostr( mainOpNow );

   buftostr( temp );
   //   buftostr( secondOpNow );

   buftostr( mode );
   inyn();
   scoreMode.setValue( PPKM );			// don't take any notice of radial flag!

   inyn();        // was post entry
   if ( inyn() )
   {
      scoreMode.setValue( PPQSO );
   }
   countryMult.setValue( inyn() );
   inyn();
   districtMult.setValue( inyn() );

   powerWatts.setValue( inyn() );

   if ( inyn() )
   {
      //      scoreMode = GSPECIAL;
   }

   allowLoc8.setValue( inyn() );
   RSTField.setValue( !inyn() );
   serialField.setValue( !inyn() );
   locatorField.setValue( !inyn() );

   QTHField.setValue( !inyn() );

   return true;

}
bool LoggerContestLog::GJVloadContacts( void )
{
   ContestContact * rct = 0;

   nextBlock = 1;

   for ( int i = 0; i < logCount; i++ )
   {
       BaseContact *bct = rct;
      makeContact( false, bct );
      rct = dynamic_cast<ContestContact *>(bct);
      if ( rct->GJVload( ( int ) nextBlock ) )
      {
         nextBlock++;
         ctList.insert( rct );
         int maxct = rct->serials.getValue().toInt();
         if ( maxct > maxSerial )
            maxSerial = maxct;

      }
   }
   return true;
}
bool LoggerContestLog::export_contest(QSharedPointer<QFile> expfd, ExportType exptype )
{
   bool ret = false;
   commonSave( false );
   setDirty();                // We always need to export everything
   switch ( exptype )
   {
         //enum ExportType {EREG1TEST, EADIF, EG0GJV, EMINOS };

      case EG0GJV:
         ret = exportGJV( expfd );
         break;

      case EMINOS:
         ret = exportMinos( expfd );
         break;

      case EADIF:
         ret = exportADIF( expfd );
         break;

      case EKML:
         ret = exportKML( expfd );
         break;

      case EREG1TEST:
         ret = exportREG1TEST( expfd );
         break;

      case EPRINTFILE:
         ret = exportPrintFile(expfd);

      default:
         return false;
   }
   clearDirty();    // BUT don't leave it dirty!!
   return ret;
}
static bool uhNeeded = false;
static bool utNeeded = false;
void LoggerContestLog::procUnknown( BaseContact *cct, writer &wr )
{
   QString lbuff;

   if ( cct->QSOValid
        && !( ( cct->cs.valRes == ERR_DUPCS )
              || ( cct->contactFlags.getValue() & NON_SCORING )
              || ( cct->contactScore.getValue() <= 0 )
            )
      )
   {

      // no district when required
      if ( countryMult.getValue() && cct->ctryMult == 0 )   	// invalid country
         lbuff = "Unknown Country  ";

      else
         if ( districtMult.getValue() && cct->ctryMult && cct->ctryMult->hasDistricts()     // continentals dont have counties
              && cct->districtMult == 0 && !( cct->contactFlags.getValue() & VALID_DISTRICT ) )   	// invalid country
         {
            lbuff = "Unknown District   ";
         }
         else
            return ;

      QString sl;
      cct->getText( sl, this );

      lbuff += sl;

      if ( uhNeeded )
      {
         uhNeeded = false;
         utNeeded = true;
         wr.lwrite( "\r\n\r\n    Contacts with Unknown Country/District\r\n" );
      }
      wr.lwrite( lbuff.toStdString().c_str() );
   }
}

bool LoggerContestLog::exportGJV(QSharedPointer<QFile>fd )
{
   //	straight copy to disk

   int mind = 1;
   int maxd = maxSerial;
   if ( !enquireDialog(   /*Owner*/0, "Please give first serial to be dumped", mind ) )
      return false;
   if ( !enquireDialog(   /*Owner*/0, "Please give last serial to be dumped", maxd ) )
      return false;

   int mindump = std::min( mind, maxd );
   int maxdump = std::max( mind, maxd );

   // ????   if ( MessageBox( 0, "Do you wish to edit the file?", "Contest", MB_OKCANCEL ) != ID_CANCEL )
   //   if (cmOK != messageBox(mfOKCancel|mfConfirmation, "Dumping all contacts between serials %d and %d inclusive", mindump, maxdump))
   QString temp = QString( "Dumping all contacts between serials %1 and %2 inclusive" ).arg(mindump).arg(maxdump );
   if ( !MinosParameters::getMinosParameters() ->yesNoMessage( 0, temp ) )
      return false;

   GJVParams gp( fd );

   GJVsave( gp );

   bool inDump = false;

   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
   {
      ContestContact *lct = dynamic_cast<ContestContact *>( *i );
      // we need to test for "in dump"

      int serials = lct->serials.getValue().toInt();
      // dump the contact, until serial seen

      if ( ( serials == mindump ) || ( mindump == 0 ) )
         inDump = true;

      if ( inDump && ( serials <= maxdump ) )
      {
         if ( !lct->GJVsave( gp ) )
            break;
         gp.count++;
      }

      if ( serials >= maxdump )
      {
         break;	// as all dumped
      }
   }

   GJVsave( gp );

   return true;
}
bool LoggerContestLog::exportADIF(QSharedPointer<QFile> expfd )
{
   // ADIF format file entry
   // OP header
   // and EOH
   QString header = "Exported by G0GJV VHF LoggerContestLog logging system\r\n\r\n";

   header += QString( "From file " ) + cfileName + "\r\n\r\n";

   header += "<EOH>\r\n";

   int ret = expfd->write(header.toStdString().c_str());
   if (  ret != header.size() )
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "bad reply from write!" );
   }

   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
   {
      ContestContact *lct = dynamic_cast<ContestContact *>( *i );
      QString l = lct ->getADIFLine();
      if ( l.size() )
      {
         int ret = expfd->write(l.toStdString().c_str());
         if (  ret != l.size() )
         {
            MinosParameters::getMinosParameters() ->mshowMessage( "bad reply from write!" );
         }
         const char *EOR = "<EOR>\r\n";
         ret = expfd->write(EOR, strlen( EOR ));
         if ( ret != (int)strlen( EOR ) )
         {
            MinosParameters::getMinosParameters() ->mshowMessage( "bad reply from write!" );
         }
      }
   }

   return true;
}
bool LoggerContestLog::exportREG1TEST(QSharedPointer<QFile>expfd )
{
   // First test validity. Reg1test dictates in particular

   // band

   QString cb = band.getValue().trimmed();
   BandList &blist = BandList::getBandList();
   BandInfo bi;
   bool bandOK = blist.findBand(cb, bi);
   if (bandOK)
   {
      cb = bi.reg1test;
   }

   if ( !bandOK )
   {
       /*
      // put up a band chooser dialog
      std::auto_ptr<TMinosBandChooser> mshowMessage( new TMinosBandChooser( LogContainer ) );

      BandList &blist = BandList::getBandList();
      for (unsigned int i = 0; i < blist.bandList.size(); i++)
      {
         if (blist.bandList[ i ].reg1test.size())  // only put up real reg1test options
         {
            mshowMessage->BandCombo->Items->Add( blist.bandList[ i ].reg1test.c_str() );
         }
      }

      mshowMessage->BandCombo->ItemIndex = 0;

      QString capt = ( boost::format( "The band description chosen (%s) is invalid for Reg1Test"
                                          " (.EDI) entry. Please choose a valid band description." )
                           % band.getValue() ).str();
      mshowMessage->ScreedLabel->Caption = capt.c_str();

      mshowMessage->ShowModal();

      band.setValue( mshowMessage->BandCombo->Text.c_str() );
      */
   }


   reg1test * rtest = new reg1test( this );
   int rep = rtest->exportTest( expfd );
   delete rtest;
   return rep;
}
bool LoggerContestLog::exportPrintFile(QSharedPointer<QFile> expfd )
{
   PrintFile * rtest = new PrintFile( this );
   int rep = rtest->exportTest( expfd );
   delete rtest;
   return rep;
}
bool LoggerContestLog::exportMinos( QSharedPointer<QFile> expfd )
{
   int mind = 1;
   int maxd = maxSerial;
   if ( !enquireDialog(   /*Owner*/0, "Please give first serial to be dumped", mind ) )
      return false;
   if ( !enquireDialog(   /*Owner*/0, "Please give last serial to be dumped", maxd ) )
      return false;

   int mindump = std::min( mind, maxd );
   int maxdump = std::max( mind, maxd );

   // ????   if ( MessageBox( 0, "Do you wish to edit the file?", "Contest", MB_OKCANCEL ) != ID_CANCEL )
   //   if (cmOK != messageBox(mfOKCancel|mfConfirmation, "Dumping all contacts between serials %d and %d inclusive", mindump, maxdump))
   QString temp = QString( "Dumping all contacts between serials %1 and %2 inclusive" ).arg(mindump).arg(maxdump );
   if ( !MinosParameters::getMinosParameters() ->yesNoMessage( 0, temp ) )
      return false;

   MinosTestExport * mtest = new MinosTestExport( this );
   int rep = mtest->exportTest( expfd, mindump, maxdump );
   delete mtest;
   return ( rep > 0 );
}
static void ccor( double &y )
{
   while ( ( y < -180.0 ) || ( y > 180.0 ) )
      y += ( y < 0.0 ) ? 360.0 : -360.0;
}
static double raddeg( double x )
{
   double y = x * 180.0 / pi;

   ccor( y );
   return ( y );

}
static QString kmloutput ( Location *outgrid )
{
   QString qkml = QString("%1,%2").arg(raddeg ( outgrid->easting),3,'f',8 ).arg(raddeg ( outgrid->northing),3,'f',8 );
   return qkml;

}

bool LoggerContestLog::exportKML(QSharedPointer<QFile> expfd )
{
   typedef std::map <QString, ContestContact *> cmap; // map by call
   typedef std::map <QString, cmap> smap;       // map by prefix
   smap countries;

   for ( LogIterator i = ctList.begin(); i != ctList.end(); i++ )
   {
      ContestContact *ct = dynamic_cast<ContestContact *>( *i );
      if ( ct->ctryMult )
      {
         ( countries[ ct->ctryMult->basePrefix ] ) [ ct->cs.fullCall.getValue() ] = ct;
      }
      else
         if ( ct->QSOValid )
         {
            ( countries[ "unknown" ] ) [ ct->cs.fullCall.getValue() ] = ct;
         }
         else
         {
            // contact is invalid - don't display it
         }
   }

   QStringList kml;

   kml.append( "<kml xmlns=\"http://earth.google.com/kml/2.0\">" );
   kml.append( "<Document><visibility>0</visibility><open>1</open>" );
   kml.append( "<Folder><name><![CDATA[" + name.getValue() + " " + mycall.fullCall.getValue() + "]]></name><visibility>0</visibility><open>1</open>" );

   for ( smap::iterator s = countries.begin(); s != countries.end(); s++ )
   {
      kml.append( "<Folder><name><![CDATA[" + ( *s ).first + "]]></name><open>0</open><visibility>0</visibility>"  );
      for ( cmap::iterator e = ( ( *s ).second ).begin(); e != ( ( *s ).second ).end(); e++ )
      {
         Location l1;
         Location l2;
         ContestContact *ct = ( *e ).second;


         char inputbuff[ 100 ];
         strcpy( inputbuff, ct->loc.loc.getValue().toStdString().c_str() );
         l1.gridstyle = LOC;
         l1.datastring = inputbuff;
         l1.datalength = ct->loc.loc.getValue().length() + 1;
         l1.dataoffset = 0;

         char outputbuff[ 100 ];
         l2.datastring = outputbuff;
         l2.gridstyle = GEO;
         l2.datalength = 40;
         l2.dataoffset = 0;

         if ( transform( &l1, &l2 ) == GRIDOK )
         {
            kml.append( "<Placemark><visibility>0</visibility>" );
            kml.append( "<description><![CDATA[" + ct->cs.fullCall.getValue() + " " + ct->loc.loc.getValue() + "]]></description>"  );
            kml.append( "<name><![CDATA[" + ct->cs.fullCall.getValue() + "]]></name>"  );
            kml.append( "<Point><coordinates>" + kmloutput( &l2 ) + ",0</coordinates></Point>"  );
            kml.append( "</Placemark>" );
         }
      }
      kml.append( "</Folder>" );
   }

   kml.append( "</Folder>" );
   kml.append( "</Document>" );
   kml.append( "</kml>" );


   QTextStream ts( expfd.data() );
   ts << kml.join( "\r\n" );
   ts << "\r\n";

   return true;

}
//============================================================
bool LoggerContestLog::importLOG(QSharedPointer<QFile> hLogFile )
{
    QStringList ls;
    QTextStream in( hLogFile.data() );

    while ( !in.atEnd() )
    {
        QString line = in.readLine(255);
        ls.append(line);
    }

   TEMPBUFF( temp, 100 );
   // Import from LOG format
   // Needs modification for "new" log format

   locValid = false;
   //   save();
   // read remainder of lines; write each line as we go

   long next_block = 1;

   bool started = false;

   DisplayContestContact *ct = 0;
   int lineNo = -1;
   while ( ++lineNo < ls.count() )
   {
      // skip new format header information, until we can cope
      // properly with it
      QString stemp = trimr( ls[ lineNo ] );
      if ( stemp.length() == 0 || ( !started && !stemp[0].isDigit() ) )
      {
         // GJV.LOG

         /*
         Contest Title                        : VHFNFD
         Band in MHz                          : 432
         Section Entered                      : Multi Operator
         Name of Entrant/Club/Contest Group   : Bracknell Amateur Radio Club
         Band in MHz                          : %band%
         Section Entered                      : %section%
         Callsign Used                        : %call%
         QRH Locator Sent                     : %locator%
         Location as sent(District or QTH)    : %QTH%
         Brief Details of Transmitter Used    : %Transmitter%
         Brief Details of Receiver Used       : %Receiver%
         Power Output (Watts)                 : %power%
         Antenna Type                         : %Antenna%
         Antenna Height AGL(metres)           : %AGL%
         QTH Height ASL(metres)               : %ASL%

         Difficult ones:
         Name and Callsign of all Operators   :
         G0GJV  G4AUC  G0FCT  G0HVS
         Name, Callsign, Telephone Number, Address for Correspondence :
                                     : M. J. Goodey
                                     : G0GJV
                                     : 62, Rose Hill
                                     : Binfield
                                     : Berkshire
                                     : RG42 5LG
                               Phone : 01344 428614
                               Email : mjgoodey@compuserve.com

         */
         int spos = stemp.indexOf( ":" );
         if ( spos != -1 )
         {
            QString text = stemp.mid( spos + 1, stemp.size() ).trimmed();

            if ( stemp.toUpper().indexOf( "CONTEST TITLE" ) == 0 )
            {
               name.setValue( text );
            }
            else
               if (  stemp.toUpper().indexOf( "BAND IN MHZ" ) == 0 )
               {
                  band.setValue( text );
               }
               else
                  if ( stemp.toUpper().indexOf( "SECTION ENTERED" ) == 0 )
                  {
                     entSect.setValue( text );
                  }
                  else
                     if ( stemp.toUpper().indexOf( "NAME OF ENTRANT/CLUB/CONTEST GROUP" ) == 0 )
                     {
                        entrant.setValue( text );
                     }
                     else
                        if ( stemp.toUpper().indexOf( "CALLSIGN USED" ) == 0 )
                        {
                           // we need to tail clip the callsign - or we end up with things like
                           // M1DUD/QRP        CLASS : SO  /   50MHZ FIXED
                           int spos = text.indexOf( " " );
                           if ( spos != -1 )
                           {
                              text = text.left(spos ).trimmed();
                           }
                           mycall.fullCall.setValue( strupr( text ) );
                           mycall.valRes = CS_NOT_VALIDATED;
                           mycall.validate();

                        }
                        else
                           if ( stemp.toUpper().indexOf( "QRH LOCATOR SENT" ) == 0 ||
                                stemp.toUpper().indexOf( "QTH LOCATOR SENT" ) == 0 )
                           {
                              // yes, contestx DOES say QRH!
                              myloc.loc.setValue( strupr( text ) );
                              validateLoc();
                           }
                           else
                              if ( stemp.toUpper().indexOf( "POWER OUTPUT" ) == 0 )
                              {
                                 power.setValue( text );
                              }
                              else
                                 if ( stemp.toUpper().indexOf( "LOCATION AS SENT" ) == 0 )
                                 {
                                    location.setValue( text );
                                 }
                                 else
                                    if ( stemp.toUpper().indexOf( "ANTENNA TYPE" ) == 0 )
                                    {
                                       entAnt.setValue( text );
                                    }
                                    else
                                       if ( stemp.toUpper().indexOf( "ANTENNA HEIGHT" ) == 0 )
                                       {
                                          entAGL.setValue( text );
                                       }
                                       else
                                          if ( stemp.toUpper().indexOf( "QTH HEIGHT" ) == 0 )
                                          {
											 entASL.setValue( text );
                                          }
                                          else
                                             if ( stemp.toUpper().indexOf( "BRIEF DETAILS OF TRANSMITTER USED" ) == 0 )
                                             {
                                                entTx.setValue( text );
                                             }
                                             else
                                                if ( stemp.toUpper().indexOf( "BRIEF DETAILS OF RECEIVER USED" ) == 0 )
                                                {
                                                   entRx.setValue( text );
                                                }
         }
         continue;
      }
      started = true;

      BaseContact *bct = ct;
      makeContact( false, bct );
      ct = dynamic_cast<DisplayContestContact *>(bct);
      ct->setLogSequence( 0 );

      stemp += QString( 200, ' ' );   // make sure there is plenty more...

      std::string sstemp = stemp.toStdString();
      const char *lbuff = sstemp.c_str();
      // parse contact line in

      strcpysp( temp, &lbuff[ 0 ], 6 );
      ct->time.setDate( temp, DTGLOG );
      strcpysp( temp, &lbuff[ 7 ], 4 );
      ct->time.setTime( temp, DTGLOG );
      strcpysp( temp, &lbuff[ 21 ], 15 );
      ct->cs = callsign( strupr( temp ) );
      ct->cs.valRes = CS_NOT_VALIDATED;
      strcpysp( temp, &lbuff[ 37 ], 3 );
      ct->reps.setValue( temp );
      strcpysp( temp, &lbuff[ 41 ], 4 );
      ct->serials.setValue( temp );

      int maxct = ct->serials.getValue().toInt();
      if ( maxct > maxSerial )
         maxSerial = maxct;

      strcpysp( temp, &lbuff[ 46 ], 3 );
      ct->repr.setValue( temp );
      strcpysp( temp, &lbuff[ 51 ], 4 );
      ct->serialr.setValue( temp );

      // here is the score field
      strcpysp( temp, &lbuff[ 59 ], 5 );
      if ( atoi( temp ) == 0 )
         ct->contactFlags.setValue( NON_SCORING );

      strcpysp( temp, &lbuff[ 65 ], 6 );
      ct->op1.setValue( temp );

      strcpysp( temp, &lbuff[ 72 ], 6 );
      ct->loc.loc.setValue( temp );
      ct->loc.valRes = LOC_NOT_VALIDATED;

      //ct->comments = "";

      TEMPBUFF( extra, EXTRALENGTH + 1 );
      TEMPBUFF( comments, COMMENTLENGTH + 1 );

      strcpysp( extra, &lbuff[ 81 ], 2 );          // 81 is district code
      if ( extra[ 0 ] && extra[ 1 ] && ( extra[ 0 ] != ' ' && extra[ 1 ] != ' ' ) )
      {
         // we always attempt to import the district mult field
         strcpysp( comments, &lbuff[ 93 ], COMMENTLENGTH );
         ct->comments.setValue( comments );

      }
      else
      {
         strcpysp( extra, &lbuff[ 93 ], EXTRALENGTH );
      }
      ct->extraText.setValue( extra );

      // save contact

      // we have to have logSequence set before we insert - or it will cause
      // duplicates

      next_block++ ;
      ct->setLogSequence( next_block << 16 );

      ctList.insert( ct );
   }
   return true;
}
//====================================================================
bool LoggerContestLog::importAdif(QSharedPointer<QFile> adifContestFile )
{
   return ADIFImport::doImportADIFLog( this, adifContestFile );
}
//====================================================================
bool LoggerContestLog::importReg1Test(QSharedPointer<QFile> r1ContestFile )
{
   // in AdjSQL we do this as a semi-colon separated format, not as something special
   return reg1test::doImportReg1test( this, r1ContestFile );
}
//====================================================================
void LoggerContestLog::processMinosStanza( const QString &methodName, MinosTestImport * const mt )
{
   BaseContestLog::processMinosStanza( methodName, mt );

   if ( methodName == "MinosLogContest" )
   {
      mt->getStructArgMemberValue( "section", entSect );
      mt->getStructArgMemberValue( "sectionList", sectionList );
   }
   else
      if ( methodName == "MinosLogMode" )
      {}
      else
         if ( methodName == "MinosLogQTH" )
         {
            mt->getStructArgMemberValue( "stationQTH1", sqth1 );
            mt->getStructArgMemberValue( "stationQTH2", sqth2 );
			mt->getStructArgMemberValue( "ASL", entASL );
		 }
		 else
			if ( methodName == "MinosLogEntry" )
			{
			   mt->getStructArgMemberValue( "entrant", entrant );
			   mt->getStructArgMemberValue( "myName", entName );
			   mt->getStructArgMemberValue( "myCall", entCall );
			   mt->getStructArgMemberValue( "myAddress1", entAddr1 );
			   mt->getStructArgMemberValue( "myAddress2", entAddr2 );
			   mt->getStructArgMemberValue( "myCity", entCity );
			   mt->getStructArgMemberValue( "myCountry", entCountry );
			   mt->getStructArgMemberValue( "myPostCode", entPostCode );
			   mt->getStructArgMemberValue( "myPhone", entPhone );
			   mt->getStructArgMemberValue( "myEmail", entEMail );
			}
			else
			   if ( methodName == "MinosLogStation" )
			   {
				  mt->getStructArgMemberValue( "transmitter", entTx );
				  mt->getStructArgMemberValue( "receiver", entRx );
				  mt->getStructArgMemberValue( "antenna", entAnt );
				  mt->getStructArgMemberValue( "AGL", entAGL );
				  mt->getStructArgMemberValue( "offset", bearingOffset);
			   }
			   else
				  if ( methodName == "MinosLogOperators" )
				  {
					 mt->getStructArgMemberValue( "ops1", ops1 );
					 mt->getStructArgMemberValue( "ops2", ops2 );
					 mt->getStructArgMemberValue( "currentOp1", currentOp1 );
					 oplist.insert(currentOp1.getValue());
                     mt->getStructArgMemberValue( "currentOp2", currentOp2 );
                     oplist.insert(currentOp2.getValue());
                  }
                  else
                     if ( methodName == "MinosLogCurrent" )
                     {
                        mt->getStructArgMemberValue( "condx1", entCondx1 );
                        mt->getStructArgMemberValue( "condx2", entCondx2 );
                        mt->getStructArgMemberValue( "condx3", entCondx3 );
                        mt->getStructArgMemberValue( "condx4", entCondx4 );
                     }
                     else
                        if ( methodName == "MinosLogBundles" )
                        {
                           mt->getStructArgMemberValue( "VHFContestName", VHFContestName );
                           mt->getStructArgMemberValue( "entryBundle", entryBundleName );
                           mt->getStructArgMemberValue( "QTHBundle", QTHBundleName );
                           mt->getStructArgMemberValue( "stationBundle", stationBundleName );

                        }
                        else
                           if ( methodName == "MinosLogComment" )
                           {
                              // should have been dealt with in BaseContest
                           }
                           else
                              if ( methodName == "MinosLogQSO" )
                              {
                                 // should have been dealt with in BaseContest
                              }
}
//====================================================================
void LoggerContestLog::setStanza( unsigned int stanza, int stanzaStart )
{
   StanzaPos s;
   s.stanza = stanza;
   s.stanzaStart = stanzaStart;
   stanzaLocations.push_back( s );
}
//====================================================================
bool LoggerContestLog::getStanza( unsigned int stanza, QString &stanzaData )
{
   if ( stanza - 1 >= stanzaLocations.size() )
   {
      return false;
   }
   StanzaPos *s = &stanzaLocations[ stanza - 1 ];
   if ( s->stanza != stanza )
   {
      return false;
   }
   // read the stanza from the log file - which we will have to open for read

   QFile contestFile(cfileName);
   bool ret = contestFile.open(QIODevice::ReadOnly);
   if ( !ret )
   {
      QString emess = "Failed to open LoggerContestLog file for monitoring" + cfileName + " : " + contestFile.errorString();
      MinosParameters::getMinosParameters() ->mshowMessage( emess );
      return false;
   }
   ret = contestFile.seek( s->stanzaStart );
   if ( !ret )
   {
      MinosParameters::getMinosParameters() ->mshowMessage( "(write) seek failed!" );
   }
   char *buffer = new char [ 4096 + 1 ];      // it sounds a lot, but without parsing it...
   int chRead = contestFile.read(buffer, 4096 );

   bool readOK = true;
   if ( chRead > 0 )
    {
        buffer[ chRead ] = 0;
        stanzaData = buffer;
        int epos = stanzaData.indexOf( "</iq>" );     // trim the excess - if there is any
        if ( epos != -1 )
        {
            stanzaData = stanzaData.left(epos + 5 );
        }
    }
    else
        readOK = false;

   delete [] buffer;
   contestFile.close();
   return readOK;
}
//====================================================================

