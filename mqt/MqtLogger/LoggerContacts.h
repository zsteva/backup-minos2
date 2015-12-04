/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
#ifndef LoggerContactsH
#define LoggerContactsH 
//----------------------------------------------------------------------------
// This header covers the structure of contests, logs, etc


class LoggerContestLog;

class ContestContact: public DisplayContestContact
{
   private:
      ContestContact( const ContestContact & );
      ContestContact& operator =( const ContestContact & );
   public:
      virtual void clearDirty();
      virtual void setDirty();

      virtual bool setField( int ACol, const QString Value );

      ContestContact( LoggerContestLog *contest, bool time_now );
      ~ContestContact();
      void getPrintFileText(QString &, short );
      void addReg1TestComment(QStringList &remarks );
      void getReg1TestText( QString & );
      QString getADIFLine();
      bool commonSave( );
      bool minosSave( );
      bool GJVsave( GJVParams & );
      virtual bool GJVload( int diskBlock );
      virtual void processMinosStanza( const QString &methodName, MinosTestImport * const mt );
};


#endif
