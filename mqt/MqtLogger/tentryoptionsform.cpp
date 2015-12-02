#include "logger_pch.h"
#include "tentryoptionsform.h"
#include "ui_tentryoptionsform.h"


TEntryOptionsForm::TEntryOptionsForm( QWidget* Owner, LoggerContestLog * cnt, bool saveMinos ):
    QDialog(Owner),
    ui(new Ui::TEntryOptionsForm),
    ct( cnt ), minosSave( saveMinos ), opsQSOLine1(-1), opsQSOLine2(-1),
    opsEntryLine1(-1), opsEntryLine2(-1)
{
    ui->setupUi(this);
}

TEntryOptionsForm::~TEntryOptionsForm()
{
    delete ui;
}
QString TEntryOptionsForm::doFileSave( )
{
#ifdef RUBBISH
    SaveDialog->InitialDir = ExtractFilePath( ct->cfileName.c_str() );

    String expName;
    String fname = ExtractFileName( ct->cfileName.c_str() );
    String ext = ExtractFileExt( ct->cfileName.c_str() );
    fname = fname.SubString( 1, fname.Length() - ext.Length() );

    //enum ExportType {EREG1TEST, EADIF, EG0GJV, EMINOS, EKML, EPRINTFILE };
    String defext;
    String filter;
    switch ( expformat )
    {
       case EG0GJV:
          defext = "GJV";
          filter = "GJV ContestLog files (*.gjv)|*.GJV|All Files (*.*)|*.*" ;
          break;
       case EMINOS:
          defext = "Minos";
          filter = "Minos ContestLog files (*.minos)|*.MINOS|All Files (*.*)|*.*" ;
          break;
       case EADIF:
          defext = "ADI";
          filter = "ADIF files (*.adi)|*.ADI|All Files (*.*)|*.*" ;
          break;
       case EKML:
          defext = "KML";
          filter = "KML(GoogleEarth) files (*.kml)|*.KML|All Files (*.*)|*.*" ;
          break;
       case EREG1TEST:
          {
             defext = "EDI";
             ct->mycall.valRes = CS_NOT_VALIDATED;
             ct->mycall.validate( );
             filter = "Region 1 EDI files (*.edi)|*.EDI|All Files (*.*)|*.*" ;
          }
          break;
       case EPRINTFILE:
       {
          defext = "TXT";
          filter = "Text output (*.txt)|*.TXT|All Files (*.*)|*.*" ;
       }
       break;
    }
    SaveDialog->FileName = fname;
    SaveDialog->DefaultExt = defext;
    SaveDialog->Filter = filter;
    while ( SaveDialog->Execute() )
    {
       TWaitCursor fred;
       expName = SaveDialog->FileName;

       // open the export file
       struct stat sbuf;
       if ( stat( expName.c_str(), &sbuf ) == 0 )
       {
          if ( !( sbuf.st_mode & S_IWRITE ) )
          {
             MinosParameters::getMinosParameters() ->mshowMessage( "File is Read Only" );
             return "";
          }
       }
       if ( MinosParameters::getMinosParameters() ->isContestOpen( expName.c_str() ) )
       {
          // then try again...
          continue;
       }

       HANDLE expfd = CreateFile( expName.c_str(),
                                  GENERIC_READ | GENERIC_WRITE,
                                  FILE_SHARE_READ,
                                  0,                  // security
                                  CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  0 );               // template handle
       if ( expfd == INVALID_HANDLE_VALUE )
       {
          char * emess = _strerror( "Failed to create export file" );
          MinosParameters::getMinosParameters() ->mshowMessage( emess );
          return "";
       }

       int ret = ct->export_contest( expfd, expformat );
       CloseHandle( expfd );

       if ( ret == -1 )
       {
          unlink( expName.c_str() );		// failure response, so delete file again
          expName = "";
       }
       break;
    }
    return expName;
#endif
    return "afile.txt";
}
