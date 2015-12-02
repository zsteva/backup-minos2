/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef MinosTestExportH
#define MinosTestExportH 
//---------------------------------------------------------------------------

class RPCParam;
class RPCParamStruct;
class RPCRequest;
class MinosTestExport
{
   private:
      LoggerContestLog *ct;

      int stanzaCount;
      void makeHeader( RPCParamStruct * st, unsigned long ts );
      void sendRequest( boost::shared_ptr<QFile> expfd, const std::string &cmd, RPCParamStruct *st );
      void exportMode( boost::shared_ptr<QFile> expfd );
      void exportContest(boost::shared_ptr<QFile> expfd );
      void exportQTH( boost::shared_ptr<QFile> expfd );
      void exportEntry(boost::shared_ptr<QFile> expfd );
      void exportStation( boost::shared_ptr<QFile> expfd );
      void exportCurrent(boost::shared_ptr<QFile> expfd );
      void exportOperators( boost::shared_ptr<QFile> expfd );
      void exportBundles(boost::shared_ptr<QFile> expfd );

   public:
      MinosTestExport( LoggerContestLog * const ct );
      MinosTestExport( );
      ~MinosTestExport();

      int exportAllDetails( boost::shared_ptr<QFile> expfd, bool newfile );
      int exportTest(boost::shared_ptr<QFile> expfd, int mindump, int maxdump );

      int exportComment( boost::shared_ptr<QFile> expfd, const ContestContact *lct );
      int exportQSO( boost::shared_ptr<QFile> expfd, const ContestContact *lct );

};
#endif
