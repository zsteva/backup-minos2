/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef ContestAppH
#define ContestAppH

//---------------------------------------------------------------------------
class TMatchThread;
class ContestSlot
{
   public:
      int slotno;
      BaseContestLog * slot;

      ContestSlot( void );
      ~ContestSlot();
};
class ListSlot
{
   public:
      int slotno;
      ContactList * slot;

      ListSlot( void );
      ~ListSlot();
};
typedef QMap < int, QSharedPointer<ContestSlot> > ContestSlotList;
typedef ContestSlotList::iterator SlotIterator;

typedef QMap < int, QSharedPointer<ListSlot> > ListSlotList;
typedef ListSlotList::iterator ListSlotIterator;

class TContestApp : public MinosParameters
{
   private:

      BaseContestLog * currentContest;
      static TContestApp *contestApp;
      int magneticVariation;
      bool preloadComplete;
   public:
      static TContestApp *getContestApp();
      void setPreloadComplete()
      {
          preloadComplete = true;
      }


      // general settings - choose at startup
      SettingsBundle loggerBundle;     // startup defaults - INI files, sections, etc
      SettingsBundle logsPreloadBundle;    // logs to be preloaded
      SettingsBundle listsPreloadBundle;      // lists to be preloaded
      SettingsBundle displayBundle;    // display settings - or is this an op preference
      SettingsBundle locsBundle;       // Country valid locators

      ContestSlotList contestSlotList;
      ListSlotList listSlotList;


      int period1, period2;

      bool reverseBearing;
      bool dispCountry;
      bool dispBearing;
      bool dispScore;

      virtual int getMagneticVariation() override
      {
         return magneticVariation;
      }
      virtual void getDisplayColumnWidth( const QString &key, int &val, int def ) override;
      virtual void setDisplayColumnWidth( const QString &key, int val ) override;
      virtual void getBoolDisplayProfile( int enumkey, bool &value ) override;
      virtual void setBoolDisplayProfile( int enumkey, bool value ) override;
      virtual void flushDisplayProfile( void ) override;
      virtual int getBigClockCorrection() override;
      virtual int getStatsPeriod1() override;
      virtual int getStatsPeriod2() override;
      virtual void setStatsPeriod1( int ) override;
      virtual void setStatsPeriod2( int ) override;
      virtual bool getAllowLoc4() override;
      virtual bool getAllowLoc8() override;
      virtual bool yesNoMessage( QWidget* Owner, const QString &mess ) override;
      virtual void mshowMessage( const QString &mess, QWidget* Owner = 0 ) override;
      virtual BaseContestLog * getCurrentContest() override;
      virtual bool insertList( ContactList *p, int sno ) override;
      virtual bool isContestOpen( const QString fn ) override;
      virtual bool isListOpen(const QString fn ) override;
      virtual QVector<BaseContestLog *> getContestList() override;

      TContestApp();
      bool initialise();
      ~TContestApp();
      void close();

      int getContestSlotCount()
      {
         return contestSlotList.size();
      }
      int getListSlotCount()
      {
         return listSlotList.size();
      }
      int getOccupiedListSlotCount();

      bool insertContest( BaseContestLog *p, int sno );
      BaseContestLog * findFirstContest();
      int findContest( BaseContestLog * p );
      int findList( ContactList * p );
      //      LoggerContestLog * findContest( LoggerContestLog *p );
      int removeContest(BaseContestLog * p , bool writePreload);
      int removeList( ContactList * p );
      void writeContestList();
      void writeListsList();
      LoggerContestLog * openFile(const QString &fn, bool newFile, int slotno );
      ContactList * openListFile( const QString &fn, int slotno );
      void closeFile(BaseContestLog *contest , bool writePreload);
      void closeListFile( ContactList *list );

      void setCurrentContest( BaseContestLog * );

};
extern int bigClockCorr;

extern bool isOpen(QSharedPointer<ContestSlot> cs, const QString &fn );
extern bool isOpen(QSharedPointer<ListSlot> cs, const QString &fn );
extern void closeContestApp( void );
extern bool initialiseContestApp( void );
extern bool contestAppLoadFiles( void );


#endif
