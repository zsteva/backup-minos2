/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
#ifndef LoggerContestH
#define LoggerContestH 
//----------------------------------------------------------------------------
class DisplayContestContact;
struct StanzaPos
{
   unsigned int stanza;
   long stanzaStart;
};
class LoggerContestLog : public BaseContestLog
{
      friend class MinosTestImport;
      // a LoggerContestLog is a sorted collection of ContestContact records, sorted by
      // the logSequence field
   private:

      // These are Logger specific, as they relate to storage
      bool minosFile;
      bool GJVFile;
      int logCount;  // Only used loading GJV files
      bool logFile;
      bool adifFile;
      bool ediFile;
      bool needExport;
      boost::shared_ptr<QFile> minosContestFile;
      boost::shared_ptr<QFile> GJVcontestFile;
      boost::shared_ptr<QFile> logContestFile;
      boost::shared_ptr<QFile> adifContestFile;
      boost::shared_ptr<QFile> ediContestFile;
      bool importLOG( boost::shared_ptr<QFile>  );
      bool importAdif( boost::shared_ptr<QFile>  );
      bool importReg1Test( boost::shared_ptr<QFile> adifContestFile );
      std::vector<StanzaPos> stanzaLocations;

   protected:
   public:
      bool isMinosFile()
      {
         return minosFile;
      }
      bool isGJVFile()
      {
         return GJVFile;
      }
      bool isLogFile()
      {
         return logFile;
      }
      bool needsExport()
      {
         return needExport;
      }

      virtual void processMinosStanza( const QString &methodName, MinosTestImport *mt );
      void setStanza( unsigned int stanza, int stanzaStart );
      virtual bool getStanza( unsigned int Stanza, QString &stanzaData );

      // The contest details

      // Bundles are specific to the off disk logs, not the monitored logs

      SettingsBundle entryBundle;      // individual LoggerContestLog
      MinosItem<QString> entryBundleName;
      SettingsBundle QTHBundle;        // individual QTH
      MinosItem<QString> QTHBundleName;
      SettingsBundle stationBundle;    // individual station
      MinosItem<QString> stationBundleName;

      MinosItem<QString> VHFContestName;

      // Most of this info is bundle related, and not relevant when monitored (?)

      MinosItem<QString> entrant;
      MinosItem<QString> entSect;
      MinosItem<QString> sqth1;
      MinosItem<QString> sqth2;
      MinosItem<QString> entTx;
      MinosItem<QString> entRx;
      MinosItem<QString> entAnt;
      MinosItem<QString> entAGL;
      MinosItem<QString> entASL;
      MinosItem<QString> ops1;        // list of ops for contest
      MinosItem<QString> ops2;        // list of ops for contest
      MinosItem<QString> entCondx1;
      MinosItem<QString> entCondx2;
      MinosItem<QString> entCondx3;
      MinosItem<QString> entCondx4;
      MinosItem<QString> entName;
      MinosItem<QString> entCall;
      MinosItem<QString> entAddr1;
      MinosItem<QString> entAddr2;
      MinosItem<QString> entPostCode;
      MinosItem<QString> entCity;
      MinosItem<QString> entCountry;
      MinosItem<QString> entPhone;
      MinosItem<QString> entEMail;
      MinosItem<QString> sectionList; // needed to be carried forward

      // dirty info is only relevant when it is being editted
      // but needs to stay with the data

      virtual void clearDirty();
      virtual void setDirty();
      // end of contest details


      QStringList bands;
      QStringList sections;

      void setINIDetails();

      // startup/closedown

      LoggerContestLog( void );
      void initialiseINI();
      bool initialise( int slotno );
      bool initialise(const QString &, bool, int slotno );
      ~LoggerContestLog();


      // common file stuff
      bool commonSave( bool newfile );
      void closeFile( void );

      // GJV file manipulation
      int readBlock( int bno );
      int writeBlock(boost::shared_ptr<QFile> fd, int bno );
      bool GJVsave( GJVParams & );
      virtual bool GJVload( void );
      bool GJVloadContacts( void );

      // minos file
      virtual bool minosSaveFile( bool newfile );
      virtual bool minosSaveContestContact( const ContestContact *lct );
      // Import/export etc

      bool export_contest( boost::shared_ptr<QFile>, ExportType );
      bool exportGJV(boost::shared_ptr<QFile> );
      void procUnknown( BaseContact *cct, writer &wr );
      bool exportADIF( boost::shared_ptr<QFile> expfd );
      bool exportKML( boost::shared_ptr<QFile> expfd );
      bool exportREG1TEST(boost::shared_ptr<QFile> expfd );
      bool exportPrintFile( boost::shared_ptr<QFile> expfd );
      bool exportMinos(boost::shared_ptr<QFile> expfd );

      virtual void makeContact( bool time_now, BaseContact *& );
      DisplayContestContact *addContact( int newctno, int extra_flags, bool save_new, bool catchup );
      DisplayContestContact *addContactBetween( BaseContact *prior, BaseContact *next );
      void removeContact( DisplayContestContact * );

};

#endif
