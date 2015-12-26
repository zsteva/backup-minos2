#include "logger_pch.h"

#include "tqsoeditdlg.h"
#include "ui_tqsoeditdlg.h"

TQSOEditDlg *QSOEditDlg;

//---------------------------------------------------------------------------
class QSOHistoryNode
{
   public:
      BaseContact *root;
      int historyOffset;
      QSOHistoryNode() : root( 0 ), historyOffset( -1 )
      {}
};
//---------------------------------------------------------------------------
TQSOEditDlg::TQSOEditDlg(QWidget *parent, bool catchup, bool unfilled )
    :QDialog(parent),
    ui(new Ui::TQSOEditDlg),
    firstContact( 0 ), contest( 0 ),
    catchup(catchup), unfilled(unfilled)
{
    ui->setupUi(this);

    ui->GJVQSOEditFrame->setAsEdit();

    connect(ui->GJVQSOEditFrame, SIGNAL(QSOFrameCancelled()), this, SLOT(on_EditFrameCancelled()));
    connect(&MinosLoggerEvents::mle, SIGNAL(AfterSelectContact(BaseContact *, BaseContestLog *)), this, SLOT(on_AfterSelectContact(BaseContact *, BaseContestLog *)));
}
TQSOEditDlg::~TQSOEditDlg()
{
    delete ui;
}
int TQSOEditDlg::exec()
{
    ui->GJVQSOEditFrame->unfilled = unfilled;
    ui->GJVQSOEditFrame->catchup = catchup;

    // we had this so that we could close the form easily on startup
    // when the conatct was zero - not sure if still needed
    ui->GJVQSOEditFrame->initialise( contest, /*this,*/ catchup );

    // Supress the tabstops we weren't able to manage in the form designed
    // to discover where they went, uncomment the top block in
    // GJVQSOEditFrameROFieldTimerTimer

//    GJVQSOEditFrame->TabStop = false;

    ui->GJVQSOEditFrame->selectEntry( firstContact );   // first contact for the dialog to deal with
    if (catchup)
    {
       setWindowTitle("Catch-up (Post Entry)");
    }
    else if ( unfilled )
    {
       setWindowTitle("Completing unfilled contacts");
    }
    else
    {
       setWindowTitle("Editting QSO");
    }
    firstContact = 0;

    int ret = QDialog::exec();

    if (catchup)
    {
       LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );
       DisplayContestContact * lct = dynamic_cast<DisplayContestContact*>( ui->GJVQSOEditFrame->selectedContact );
       ct->removeContact(lct);
       ct->maxSerial--;
    }

    MinosLoggerEvents::SendAfterLogContact(contest);

    return ret;
}

//---------------------------------------------------------------------------
void TQSOEditDlg::keyPressEvent( QKeyEvent* event )
{
    ui->GJVQSOEditFrame->keyPressEvent(event);
}
void TQSOEditDlg::on_EditFrameCancelled()
{
    accept();
}

//---------------------------------------------------------------------------

void TQSOEditDlg::selectContact( BaseContestLog * ccontest, DisplayContestContact *lct )
{
   // this is the first call after construction
   contest = ccontest;
   firstContact = lct;
}
//---------------------------------------------------------------------------
void TQSOEditDlg::addTreeRoot(BaseContact *lct)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->QSOHistoryTree);

    QString line = lct->updtime.getDate( DTGACCURATE ) + " " + lct->updtime.getTime( DTGACCURATE ) + " UTC";
    treeItem->setText(0, line);

    lct->getText(line, contest);
    addTreeChild(treeItem, line);
    treeItem->setExpanded(true);
}

void TQSOEditDlg::addTreeChild(QTreeWidgetItem *parent,
                  QString text)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();

    treeItem->setText(0, text);

    parent->addChild(treeItem);
}
void TQSOEditDlg::on_AfterSelectContact( BaseContact *lct, BaseContestLog *contest)
{
  ui->QSOHistoryTree->clear();
  if (lct)
  {
      for (unsigned int i = 0; i < lct->getHistory().size(); ++i)
      {
          addTreeRoot(&lct->getHistory()[i]);
      }
  }
  refreshOps(ui->GJVQSOEditFrame->screenContact);
}
//---------------------------------------------------------------------------
void TQSOEditDlg::refreshOps( ScreenContact &screenContact )
{
    /*
   GJVQSOEditFrame->MainOpComboBox->Clear();
   GJVQSOEditFrame->SecondOpComboBox->Clear();
   // refill the op combo boxes from the curent contest, and select the correct op
   BaseContestLog * contest = TContestApp::getContestApp() ->getCurrentContest();

   for ( OperatorIterator i = contest->oplist.begin(); i != contest->oplist.end(); i++ )
   {
      if ( ( *i ).size() )
      {
         GJVQSOEditFrame->MainOpComboBox->Items->Add( ( *i ).c_str() );
         GJVQSOEditFrame->SecondOpComboBox->Items->Add( ( *i ).c_str() );
      }
   }
   GJVQSOEditFrame->SecondOpComboBox->Text = screenContact.op2.c_str();
   GJVQSOEditFrame->MainOpComboBox->Text = screenContact.op1.c_str();

   // and if this is the last contact, the ops should also propogate into the contest
   // for the NEXT contact
   */
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*

void __fastcall TQSOEditDlg::GJVQSOEditFrame1GJVOKButtonClick( TObject *Sender )
{
   if ( GJVQSOEditFrame->doGJVOKButtonClick( Sender ) )
   {
      if (catchup)
      {
         LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );
         int ctmax = ct->maxSerial + 1;
         DisplayContestContact *lct = ct->addContact( ctmax, 0, false, catchup );
         GJVQSOEditFrame->selectEntry( lct );
      }
      else
      {
         ModalResult = mrOk;
      }
   }

}
*/
//---------------------------------------------------------------------------
/*
void __fastcall TQSOEditDlg::GJVQSOEditFrame1GJVForceButtonClick(
   TObject *Sender )
{
   if ( GJVQSOEditFrame->doGJVForceButtonClick( Sender ) )
   {
      GJVQSOEditFrame->DateEdit->ReadOnly = !catchup;
      GJVQSOEditFrame->TimeEdit->ReadOnly = !catchup;
      if (catchup)
      {
         LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );
         int ctmax = ct->maxSerial + 1;
         DisplayContestContact *lct = ct->addContact( ctmax, 0, false, catchup );
         GJVQSOEditFrame->selectEntry( lct );
      }
      else
      {
         ModalResult = mrOk;
      }
   }
}
*/
//---------------------------------------------------------------------------
/*
void __fastcall TQSOEditDlg::GJVQSOEditFrame1GJVCancelButtonClick(
   TObject *Sender )
{
   GJVQSOEditFrame->doGJVCancelButtonClick( Sender );
   ModalResult = mrCancel;
}
*/
//---------------------------------------------------------------------------
/*
void __fastcall TQSOEditDlg::FormKeyUp( TObject *Sender, WORD &Key,
                                        TShiftState Shift )
{
   if ( ( Shift.Contains( ssCtrl ) || Shift.Contains( ssShift ) ) && Key >= VK_F1 && Key <= VK_F12 )
   {
      if ( Shift.Contains( ssCtrl ) && Shift.Contains( ssShift ) )
      {
         // keyer record keys
         TSendDM::sendKeyerRecord( Key - VK_F1 + 1 );
      }
      else
      {
         // Keyer play keys
         TSendDM::sendKeyerPlay( Key - VK_F1 + 1 );
      }
   }
   else
      if ( Shift.Empty() )
      {
         if ( ( Key == VK_F1 || Key == VK_F2 || Key == VK_F3 || Key == VK_F4 || Key == VK_F5 || Key == VK_F6) )
         {
            GJVQSOEditFrame->setActiveControl( &Key );
         }
      }

}
*/
//---------------------------------------------------------------------------
/*
void __fastcall TQSOEditDlg::QSOHistoryTreeBeforeItemErase(
   TBaseVirtualTree *Sender, TCanvas *TargetCanvas, PVirtualNode Node,
   TRect &ItemRect, TColor &ItemColor, TItemEraseAction &EraseAction )
{
   QSOHistoryNode * qn = ( QSOHistoryNode * ) Sender->GetNodeData( Node );

   std::string line;
   if ( qn && qn->root )
   {
      if ( qn->root->contactFlags.getValue() & FORCE_LOG )
      {
         ItemColor = ( TColor ) 0x00C080FF;
         EraseAction = eaColor;
      }
   }

}
*/
//---------------------------------------------------------------------------
void TQSOEditDlg::selectCatchup( BaseContestLog * c )
{
   // Kick off Post Entry/catchup
   // We need to create a new contact, and set the "post entry" flag
   // and then trigger the qso edit dialog on it

   // Also note that we don't get a dups display doing it this way
   // Not sure if that matters...

   // OR can we build a match window into the edit dialog?

   catchup = true;
   LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( c );

   int ctmax = ct->maxSerial + 1;

   DisplayContestContact *lct = ct->addContact( ctmax, 0, false, catchup );
   selectContact(c, lct);
//   GJVQSOEditFrame->FirstUnfilledButton->Enabled = false;
}
/*
void __fastcall TQSOEditDlg::GJVQSOEditFrameGJVCancelButtonClick(
      TObject *Sender)
{
  GJVQSOEditFrame->doGJVCancelButtonClick(Sender);

}
*/
//---------------------------------------------------------------------------
/*
void __fastcall TQSOEditDlg::GJVQSOEditFrameROFieldTimerTimer(TObject *Sender)
{
  GJVQSOEditFrame->ROFieldTimerTimer(Sender);

}
*/
//---------------------------------------------------------------------------

