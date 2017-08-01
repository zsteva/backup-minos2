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
      QSharedPointer<QFile> minosContestFile;
      QSharedPointer<QFile> GJVcontestFile;
      QSharedPointer<QFile> logContestFile;
      QSharedPointer<QFile> adifContestFile;
      QSharedPointer<QFile> ediContestFile;
      bool importLOG( QSharedPointer<QFile>  );
      bool importAdif( QSharedPointer<QFile>  );
      bool importReg1Test( QSharedPointer<QFile> adifContestFile );
      QVector<StanzaPos> stanzaLocations;

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

      virtual void processMinosStanza( const QString &methodName, MinosTestImport * const mt ) override;
      virtual void setStanza( unsigned int stanza, int stanzaStart ) override;
      virtual bool getStanza(unsigned int Stanza, QString &stanzaData ) override;

      // The contest details

      // Bundles are specific to the off disk logs, not the monitored logs

      SettingsBundle entryBundle;      // individual LoggerContestLog
      MinosItem<QString> entryBundleName;
      SettingsBundle QTHBundle;        // individual QTH
      MinosItem<QString> QTHBundleName;
      SettingsBundle stationBundle;    // individual station
      MinosItem<QString> stationBundleName;

      SettingsBundle appBundle;    // individual apps
      MinosItem<QString> appBundleName;

      MinosItem<QString> appRigControl;
      MinosItem<QString> appBandMap;
      MinosItem<QString> appRotator;
      MinosItem<QString> appVoiceKeyer;


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
      int writeBlock(QSharedPointer<QFile> fd, int bno );
      bool GJVsave( GJVParams & );
      virtual bool GJVload( void );
      bool GJVloadContacts( void );

      // minos file
      virtual bool minosSaveFile( bool newfile );
      virtual bool minosSaveContestContact(const QSharedPointer<BaseContact> lct );
      // Import/export etc

      bool export_contest( QSharedPointer<QFile>, ExportType, bool noSerials );
      bool exportGJV(QSharedPointer<QFile> );
      void procUnknown( QSharedPointer<BaseContact> cct, writer &wr );
      bool exportADIF( QSharedPointer<QFile> expfd );
      bool exportKML( QSharedPointer<QFile> expfd );
      bool exportREG1TEST(QSharedPointer<QFile> expfd, bool noSerials );
      bool exportPrintFile( QSharedPointer<QFile> expfd );
      bool exportMinos(QSharedPointer<QFile> expfd );

      virtual void makeContact( bool time_now, QSharedPointer<BaseContact>& ) override;
      QSharedPointer<BaseContact> addContact(int newctno, int extra_flags, bool save_new, bool catchup , QString mode);
      QSharedPointer<BaseContact> addContactBetween( QSharedPointer<BaseContact> prior, QSharedPointer<BaseContact> next );
      void removeContact(QSharedPointer<BaseContact> );

};

#endif
