#include "logger_pch.h"

#include "tentryoptionsform.h"
#include "tsinglelogframe.h"
#include "ui_tsinglelogframe.h"

#include "focuswatcher.h"

TSingleLogFrame::TSingleLogFrame(QWidget *parent, BaseContestLog * contest) :
    QFrame(parent),
    ui(new Ui::TSingleLogFrame),
    contest( contest ),
    logColumnsChanged( false ),
    splittersChanged(false),
    currFreq( 0 ), oldFreq( 0 ),
    lastStanzaCount( 0 )
    // matchTreeClickNode( 0 ),
    //otherTreeClickNode( 0 ),
    //archiveTreeClickNode( 0 ),
    //xferTree( 0 ),

{
    ui->setupUi(this);

    ui->LogAreaSplitter->setClosingWidget(ui->ArchiveSplitter);
    ui->CribSplitter->setClosingWidget(ui->CribSheet);
    ui->FilterSplitter->setClosingWidget(ui->MultFilters);
    ui->MultSplitter->setClosingWidget(ui->FilterSplitter);
}

TSingleLogFrame::~TSingleLogFrame()
{
    delete ui;
}

QString TSingleLogFrame::makeEntry( bool saveMinos )
{
   LoggerContestLog * ct = dynamic_cast<LoggerContestLog *>( contest );
   if ( !ct )
   {
      return "";
   }
   TEntryOptionsForm EntryDlg( this, ct, saveMinos  );
   if ( saveMinos )
   {
      EntryDlg.setWindowTitle("Save imported log as a .Minos file");
   }
   if ( EntryDlg.exec() == QDialog::Accepted )
   {
      ct->commonSave( false );
      QString expName = EntryDlg.doFileSave( );
      return expName;
   }
   return "";
}
BaseContestLog * TSingleLogFrame::getContest()
{
   return contest;
}

void TSingleLogFrame::closeContest()
{
    if ( TContestApp::getContestApp() )
    {
//       RPCPubSub::publish( "ContestLog", contest->publishedName, makeStr( 0 ), psRevoked );
//       GJVQSOLogFrame->closeContest();
       TContestApp::getContestApp() ->closeFile( contest );
       contest = 0;
    }
}



