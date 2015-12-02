/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#ifndef Reg1testH
#define Reg1testH

class reg1testLine
{
   public:
      QString prefix;
      QString data;

      reg1testLine( const QString &, const QString &dat );
      reg1testLine( const QString &, const std::string &dat );
      reg1testLine( void );
      ~reg1testLine();

      reg1testLine & operator = ( const reg1testLine & );
};

class reg1test
{
      LoggerContestLog * ct;
      boost::shared_ptr<QFile> regContestFile;
      int nextBlock;
      std::vector <std::string> remarks;
      bool parseHeader( QString line );
      bool parseRemark( QString line );
      bool parseQSO( QString line );
      bool importReg1test();
   public:

      reg1test( LoggerContestLog * c, boost::shared_ptr<QFile> hFile );
      reg1test( LoggerContestLog * const ct );

      ~reg1test();
      bool exportTest( boost::shared_ptr<QFile> expfd );
      static bool doImportReg1test( LoggerContestLog * c, boost::shared_ptr<QFile> hFile );

};
#endif
