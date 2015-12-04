/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef MinosTestImportH
#define MinosTestImportH 
//---------------------------------------------------------------------------
class MinosTestImport: public RPCDispatcher
{
   protected:
      typedef void ( MinosTestImport::* DispatchCallback ) ( XStanza *stanza ) ;
      DispatchCallback dispatchCallback;

      virtual void dispatchResponse( XStanza *a );
      void analyseImportLog( XStanza * );
      void analyseImportTest( XStanza * );
   private:
      BaseContestLog *ct;
      int curfpos;
      int iqOffset;

      boost::shared_ptr<RPCParam> body;

      int stanzaCount;

      bool getStructArgMemberValueDTG( boost::shared_ptr<RPCParam> rpm, const QString &name, QString &val );
      bool getStructArgMemberValueDTG( boost::shared_ptr<RPCParam>rpm, const QString &name, MinosItem<QString> &val );
      bool getStructArgMemberValue( boost::shared_ptr<RPCParam>rpm, const QString &name, QString &val );
      bool getStructArgMemberValue( boost::shared_ptr<RPCParam>rpm, const QString &name, MinosItem<QString> &val );
      bool getStructArgMemberValue(boost::shared_ptr<RPCParam>rpm, const QString &name, int &val );
      bool getStructArgMemberValue( boost::shared_ptr<RPCParam>rpm, const QString &name, MinosItem<int> &val );
      bool getStructArgMemberValue( boost::shared_ptr<RPCParam>rpm, const QString &name, bool &val );
      bool getStructArgMemberValue( boost::shared_ptr<RPCParam>rpm, const QString &name, MinosItem<bool> &val );
   public:
      void processMinosStanza( RPCRequest *xs );
      void processLogMinosStanza( RPCRequest *xs );
      MinosTestImport( BaseContestLog * const ct );
      MinosTestImport( );
      ~MinosTestImport();

      void startImportTest();
      int importTestBuffer( const QString &buffer );
      void endImportTest();

      int importTest( boost::shared_ptr<QFile> ctfile );
      bool analyseTest( boost::shared_ptr<QFile> ctfile );
      int readTestFile( boost::shared_ptr<QFile> ctfile );

      bool getStructArgMemberValueDTG( const QString &name, QString &val );
      bool getStructArgMemberValueDTG( const QString &name, MinosItem<QString> &val );
      bool getStructArgMemberValue( const QString &name, QString &val );
      bool getStructArgMemberValue( const QString &name, MinosItem<QString> &val );
      bool getStructArgMemberValue( const QString &name, int &val );
      bool getStructArgMemberValue( const QString &name, MinosItem<int> &val );
      bool getStructArgMemberValue( const QString &name, bool &val );
      bool getStructArgMemberValue( const QString &name, MinosItem<bool> &val );

      void setBit( unsigned short &cf, unsigned short bit, bool set
                    );
};
#endif
