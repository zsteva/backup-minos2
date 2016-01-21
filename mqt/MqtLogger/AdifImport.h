/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		VHF Contest Adjudication
//
// COPYRIGHT         M. J. Goodey, 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#ifndef AdifImportH
 #define AdifImportH

class ADIFImport
{
      int offset;
      int limit;
      bool getNextChar( char &ic );

      ADIFImport( LoggerContestLog * c, boost::shared_ptr<QFile> hFile );
      ~ADIFImport();

      bool importAdifFile();
      bool executeImport();

      void ADIFImportFieldDecode(QString Fieldname, int FieldLength, QString FieldType, QString FieldContent );
      void ADIFImportEndOfRecord( );

      boost::shared_ptr<QFile> adifContestFile;
      QString fileContent;

      LoggerContestLog * acontest;
      DisplayContestContact *aqso;
      int next_block;
   public:
      static bool doImportADIFLog( LoggerContestLog * c, boost::shared_ptr<QFile> hFile );

};

#endif
