/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#ifndef MinosParametersH
#define MinosParametersH 
//---------------------------------------------------------------------------
class BaseContestLog;
class ContactList;
class MinosParameters
{
   private:
      static MinosParameters *mp;
   protected:
      static void setMinosParameters( MinosParameters *m )
      {
         mp = m;
      }
   public:
      MinosParameters()
      {
         mp = this;
      }
      virtual ~MinosParameters()
      {
         mp = 0;
      }

      static MinosParameters *getMinosParameters();
      virtual bool insertContest( BaseContestLog *p, unsigned int sno ) = 0;
      virtual int getMagneticVariation() = 0;
      virtual void getDisplayColumnWidth( const std::string &key, int &val, int def ) = 0;
      virtual void setDisplayColumnWidth( const std::string &key, int val ) = 0;
      virtual void getBoolDisplayProfile( int enumkey, bool &value ) = 0;
      virtual void setBoolDisplayProfile( int enumkey, bool value ) = 0;
      virtual void flushDisplayProfile( void ) = 0;
      virtual int getBigClockCorrection() = 0;
      virtual int getStatsPeriod1() = 0;
      virtual int getStatsPeriod2() = 0;
      virtual void setStatsPeriod1( int ) = 0;
      virtual void setStatsPeriod2( int ) = 0;
      virtual bool getAllowLoc4() = 0;
      virtual bool getAllowLoc8() = 0;
      virtual bool yesNoMessage( QWidget* Owner, QString mess ) = 0;
      virtual void mshowMessage( QString mess, QWidget* Owner = 0 ) = 0;
      virtual BaseContestLog *getCurrentContest() = 0;
      virtual bool insertList( ContactList *p, unsigned int sno ) = 0;
      virtual bool isContestOpen( const QString fn ) = 0;
      virtual bool isListOpen( const QString fn ) = 0;
      virtual std::vector<BaseContestLog *> getContestList() = 0;
};
class MinosParametersAdapter : public MinosParameters
{
   public:
      MinosParametersAdapter()
      {}
      ~MinosParametersAdapter() override
      {}
      virtual bool insertContest( BaseContestLog *p, unsigned int sno ) override;
      virtual int getMagneticVariation() override;
      virtual void getDisplayColumnWidth( const std::string &key, int &val, int def ) override;
      virtual void setDisplayColumnWidth( const std::string &key, int val ) override;
      virtual void getBoolDisplayProfile( int enumkey, bool &value ) override;
      virtual void setBoolDisplayProfile( int enumkey, bool value ) override;
      virtual void flushDisplayProfile( ) override;
      virtual int getBigClockCorrection() override;
      virtual int getStatsPeriod1() override;
      virtual int getStatsPeriod2() override;
      virtual void setStatsPeriod1( int ) override;
      virtual void setStatsPeriod2( int ) override;
      virtual bool getAllowLoc4() override;
      virtual bool getAllowLoc8() override;
      virtual bool yesNoMessage( QWidget* Owner, QString mess ) override;
      virtual void mshowMessage( QString mess, QWidget* Owner = 0 ) override;
      virtual BaseContestLog *getCurrentContest() override;
      virtual bool insertList( ContactList *p, unsigned int sno ) override;
      virtual bool isContestOpen( const QString fn ) override;
      virtual bool isListOpen(const QString fn ) override;
      virtual std::vector<BaseContestLog *> getContestList() override;
};
#endif
