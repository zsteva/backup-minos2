/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
#include "logger_pch.h"
#pragma hdrstop
#include "LoggerContest.h"

#include "SendRPCDM.h"
#include "ServerEvent.h"
#include "MatchContact.h"
#include "MatchThread.h"
#include "QSOLogFrame.h"
#include "QSOEdit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "QSOFrame"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TGJVQSOLogFrame::TGJVQSOLogFrame( TComponent* Owner )
      : TGJVEditFrame( Owner ), partialContact( 0 )
{}
__fastcall TGJVQSOLogFrame::~TGJVQSOLogFrame()
{
   delete partialContact;
}
//---------------------------------------------------------------------------
void TGJVQSOLogFrame::initialise( BaseContestLog * contest,  bool /*catchup*/ )
{
   TGJVEditFrame::initialise( contest, false );
   BandMapPanel->Visible = checkServerReady();
   MinosLoggerEvents::SendReportOverstrike(overstrike, contest);
   refreshOps();
}
//---------------------------------------------------------------------------
void TGJVQSOLogFrame::updateQSODisplay()
{
   TGJVEditFrame::updateQSODisplay();
}
//---------------------------------------------------------------------------
void TGJVQSOLogFrame::logScreenEntry( )
{
   if (!contest || contest->isReadOnly() )
      return ;

   int ctmax = atoi( screenContact.serials.c_str() );

   if ( ctmax > contest->maxSerial )
      contest->maxSerial = ctmax;

   LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );
   if ( !ct )
   {
      return ;
   }
   DisplayContestContact *lct = ct->addContact( ctmax, 0, false, false, screenContact.mode );	// "current" doesn't get flag, don't save ContestLog yet

   bool contactmodeCW = ( screenContact.reps.size() == 3 && screenContact.repr.size() == 3 );
   bool curmodeCW = ( stricmp( screenContact.mode, "A1A" ) == 0 );

   if ( contactmodeCW != curmodeCW )
   {
      // ask if change...
      if ( !curmodeCW )
      {
         if ( MinosParameters::getMinosParameters() ->yesNoMessage( this, "Change mode to A1A?" ) )
         {
            screenContact.mode = "A1A";
         }
      }
      else
      {
         if ( MinosParameters::getMinosParameters() ->yesNoMessage( this, "Change mode to J3E?" ) )
         {
            screenContact.mode = "J3E";
         }
      }
   }
   ct->currentMode.setValue( screenContact.mode );
   screenContact.op1 = ct->currentOp1.getValue() ;
   screenContact.op2 = ct->currentOp2.getValue();

   lct->copyFromArg( screenContact );
   lct->time.setDirty(); // As we may have created the contact with the same time as the screen contact
                         // This then becomes "not dirty", so we end up not saving the dtg.
                         // But this only happens when seconds are :00, as the main log
                         // is only to a minute resolution 

   lct->commonSave();
   ct->commonSave(false);

   killPartial();

   MinosLoggerEvents::SendAfterLogContact(ct);
   startNextEntry( );	// select the "next"
}
//---------------------------------------------------------------------------
void TGJVQSOLogFrame::getScreenContactTime()
{
   updateQSOTime();
   screenContact.time.setDate( DateLabel->Caption.c_str(), DTGDISP );
   screenContact.time.setTime( TimeLabel->Caption.c_str(), DTGDISP );
}
//---------------------------------------------------------------------------
void TGJVQSOLogFrame::showScreenContactTime( ScreenContact &temp )
{
   // display the contents of the contest->screenContact

   DateLabel->Caption = temp.time.getDate( DTGDISP ).c_str();
   TimeLabel->Caption = temp.time.getTime( DTGDISP ).c_str();
}
//==============================================================================
void TGJVQSOLogFrame::checkTimeEditEnter(TLabeledEdit */*tle*/, bool &/*ovr*/)
{

}
//==============================================================================
void TGJVQSOLogFrame::checkTimeEditExit()
{

}
//==============================================================================
bool TGJVQSOLogFrame::isTimeEdit(TLabeledEdit */*tle*/)
{
   return false;
}
//==============================================================================
void TGJVQSOLogFrame::logCurrentContact( )
{
   if (!contest || contest->isReadOnly() )
      return ;

   // copy the display into the correct log entry, etc

   getScreenEntry();

   // first check for extra lines to be added
   int ctno = atoi( screenContact.serials.c_str() );
   if ( ctno > contest->maxSerial + 1 )
   {

      if ( mShowYesNoMessage( this, "Serial sent is too high - should I create the missing contacts?") )
      {
         int orflag = 0;

         if ( mShowYesNoMessage( this, "Do you want to enter these contacts later?" ) )
         {
            orflag = TO_BE_ENTERED;
         }

         int nct_no = contest->maxSerial + 1;
         do
         {
            // last child is "current contact", and we need to add TO IT
            LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );
            ct->addContact( nct_no, orflag, true, false, screenContact.mode ); // last contact
            nct_no++;
         }
         while ( nct_no < ctno ) ;
      }
      else
      {
         // reset focus to serial sent
         //????????????? But its disabled...
         selectField( SerTXEdit );
         return ;
      }
   }

   logScreenEntry( );  // true => move on to next contact
}
//==============================================================================
void TGJVQSOLogFrame::startNextEntry( )
{
   if (contest->unfilledCount <= 0 || contest->isReadOnly())
   {
      FirstUnfilledButton->Visible = false;
   }
   else
   {
      FirstUnfilledButton->Visible = true;
   }

   if ( !restorePartial() )
   {
      updateTimeAllowed = true;
      screenContact.initialise( contest );
   }

   updateQSOTime();
   showScreenEntry();

   MinosLoggerEvents::SendAfterSelectContact(0, contest);
}
void TGJVQSOLogFrame::doGJVCancelButtonClick( TObject */*Sender*/ )
{
   SerTXEdit->ReadOnly = true;

   ScreenContact *temp = 0;
   if ( !partialContact )
   {
      savePartial();
      temp = partialContact;
      partialContact = 0;
   }
   updateTimeAllowed = true;
   startNextEntry();
   CallsignEdit->SetFocus();

   if ( temp )
      partialContact = temp;

   MinosLoggerEvents::SendShowErrorList();
}
//==============================================================================
bool TGJVQSOLogFrame::savePartial()
{
   if ( !partialContact )
   {
      getScreenEntry();
      partialContact = new ScreenContact( );
      partialContact->copyFromArg( screenContact );
      return true;
   }
   return false;
}
bool TGJVQSOLogFrame::restorePartial( void )
{
   if ( partialContact )
   {
      screenContact.copyFromArg( *( partialContact ) );
      showScreenEntry();

      killPartial();
      return true;
   }
   return false;
}
void TGJVQSOLogFrame::killPartial( void )
{
   if ( partialContact )
   {
      delete partialContact;
      partialContact = 0;
   }
}
//==============================================================================
void TGJVQSOLogFrame::doGJVEditChange( TObject *Sender )
{
   // sensitive field changed - trigger match scan
   if ( contest )
   {
      getScreenEntry();

      if ( current == CallsignEdit || Sender == CallsignEdit )
      {
         // clear the error list
         contest->DupSheet.clearCurDup();	// as edited, no longer a dup(?)

         if ( ( current == CallsignEdit ))
         {
            valid( cmCheckValid ); // make sure all single and cross field

            if (screenContact.cs.valRes == ERR_DUPCS)
            {
               CallsignEdit->Color = clRed;
            }
            else
            {
               CallsignEdit->Color = clWindow;
            }
            CallsignEdit->Repaint();
         }
      }
      if ( current == LocEdit || Sender == LocEdit )
      {
         // force bearing calc
         calcLoc();
      }
      MinosLoggerEvents::SendScreenContactChanged(&screenContact, contest);
   }
}
//==============================================================================
void TGJVQSOLogFrame::transferDetails( const BaseContact * lct, const BaseContestLog *matct )
{
   CallsignEdit->Text = lct->cs.fullCall.getValue().c_str();
   LocEdit->Text = lct->loc.loc.getValue().c_str();  // also forces update of score etc

   // only transfer qth info if required for this ContestLog
   // and it might be valid...
   if ( contest->districtMult.getValue() || contest->otherExchange.getValue() )
   {
      if ( ( contest->districtMult.getValue() && matct->districtMult.getValue() ) ||
           ( contest->otherExchange.getValue() && matct->otherExchange.getValue() )
         )
      {
         if ( contest->QTHField.getValue() )
         {
            std::string exch = lct->extraText.getValue();
            if (exch.size())
            {
               QTHEdit->Text = exch.c_str();
            }
         }
      }
   }
   valid( cmCheckValid ); // make sure all single and cross field
   // validation has been done

   // make sure dtg gets entered if needed
   CallsignEdit->SetFocus();
   doAutofill();
   CallsignEdit->SetFocus();

   doGJVEditChange(CallsignEdit);
   doGJVEditChange(LocEdit);
   doGJVEditChange(QTHEdit);
}
void TGJVQSOLogFrame::transferDetails( const ListContact * lct, const ContactList */*matct*/ )
{
   CallsignEdit->Text = lct->cs.fullCall.getValue().c_str();
   LocEdit->Text = lct->loc.loc.getValue().c_str();

   // only transfer qth info if required for this ContestLog
   // and it might be valid...
   if ( contest->districtMult.getValue() || contest->otherExchange.getValue() )
   {
      if ( contest->districtMult.getValue() || contest->otherExchange.getValue() )
      {
         if ( contest->QTHField.getValue() )
         {
            std::string exch = lct->extraText;
            if (exch.size())
            {
               QTHEdit->Text = exch.c_str();
            }
         }
      }
   }
   valid( cmCheckValid ); // make sure all single and cross field
   // validation has been done

   // make sure dtg gets entered if needed
   CallsignEdit->SetFocus();
   doAutofill();
   CallsignEdit->SetFocus();

   doGJVEditChange(CallsignEdit);
   doGJVEditChange(LocEdit);
   doGJVEditChange(QTHEdit);
}
//==============================================================================
void TGJVQSOLogFrame::setFreq( String f )
{
   sCurrFreq = f;
   bool temp;
   TContestApp::getContestApp() ->displayBundle.getBoolProfile( edpAutoBandMapTune, temp );
   if ( temp )
      BandMapButtonClick( this );
}

void __fastcall TGJVQSOLogFrame::BandMapButtonClick( TObject *Sender )
{
   getScreenEntry();

   std::string call = screenContact.cs.fullCall.getValue();
   std::string loc = screenContact.loc.loc.getValue();
   std::string utc = dtg( true ).getIsoDTG();
   std::string qth = screenContact.extraText;
   std::string freq = sCurrFreq.c_str();

   if ( call.size() || loc.size() || qth.size() )
   {
      // Transfer to band map app
      TSendDM::sendBandMap( freq, call, utc, loc, qth );
      doGJVCancelButtonClick( Sender ); // equivalent of hitting ESC
   }

}
//---------------------------------------------------------------------------

void TGJVQSOLogFrame::updateQSOTime(bool fromTimer)
{
   // Check if dtg is within the contest time
   // If not we wish to show as red
   // We need to do this in log displays as well

   TDateTime tdt = dtg::getCorrectedUTC();
   String d = tdt.FormatString("dd/mm/yy");

   if (d[3] != '/')
   {
      // letting the computer sleep seems to lose the short date format (at least)
      DateSeparator = '/';
      ShortDateFormat = "dd/MM/yyyy";
      TimeSeparator = ':';
      ShortTimeFormat = "hh:mm";
      d = dtg::getCorrectedUTC( ).FormatString( "dd/mm/yyyy hh:nn:ss" ) + " UTC       ";
   }
   DateLabel->Caption = d;
   String t = tdt.FormatString("hh:nn");
   TimeLabel->Caption = t;

   dtg time(false);
   time.setDate( DateLabel->Caption.c_str(), DTGDISP );
   time.setTime( TimeLabel->Caption.SubString(1, 5).c_str(), DTGDISP );

   bool timeOK = false;

   if (contest)
   {
      timeOK = contest->checkTime(time);
   }

   if (fromTimer)
   {
      DateLabel->Font->Assign(MinosParameters::getMinosParameters() ->getSysFont());
      TimeLabel->Font->Assign(MinosParameters::getMinosParameters() ->getSysFont());
      if (timeOK)
      {
         DateLabel->Font->Color = clWindowText;
         TimeLabel->Font->Color = clWindowText;
      }
      else
      {
         DateLabel->Font->Color = clRed;
         TimeLabel->Font->Color = clRed;
      }
   }
}
//---------------------------------------------------------------------------

void __fastcall TGJVQSOLogFrame::CatchupButtonClick(TObject */*Sender*/)
{
   std::string mode = ModeComboBoxGJV->Text.c_str();
   std::auto_ptr <TQSOEditDlg> qdlg( new TQSOEditDlg( this, true, false ) );
   qdlg->selectCatchup( contest, mode );

   qdlg->ShowModal();

   contest->scanContest();

   screenContact.initialise(contest);
   screenContact.mode = mode;
   showScreenEntry();

   refreshOps();
   selectField( 0 );
}
//---------------------------------------------------------------------------

void __fastcall TGJVQSOLogFrame::FirstUnfilledButtonClick(TObject */*Sender*/)
{
// Go to the first unfilled QSO
// If there aren't any then it needs to be made invisible
// ScanContest can work out how many there are - and we can display that on the button
   MinosLoggerEvents::SendNextUnfilled(contest);
}
//---------------------------------------------------------------------------

void __fastcall TGJVQSOLogFrame::MainOpComboBoxExit(TObject */*Sender*/)
{
   if (contest)
   {
      std::string op1 = MainOpComboBox->Text.c_str();
      contest->currentOp1.setValue( op1 );
      if ( op1.size() )
      {
         contest->oplist.insert( op1 );
      }
      contest->commonSave(false);
      refreshOps();
   }
}
//---------------------------------------------------------------------------

void __fastcall TGJVQSOLogFrame::SecondOpComboBoxExit(TObject */*Sender*/)
{
   if (contest)
   {
      std::string op2 = SecondOpComboBox->Text.c_str();
      contest->currentOp2.setValue( op2 );
      if ( op2.size() )
      {
         contest->oplist.insert( op2 );
      }
      contest->commonSave(false);
      refreshOps();
   }
}
//---------------------------------------------------------------------------
void TGJVQSOLogFrame::refreshOps()
{
   if (contest)
   {
      TGJVEditFrame::refreshOps();
      MainOpComboBox->Text = contest->currentOp1.getValue().c_str();
      SecondOpComboBox->Text = contest->currentOp2.getValue().c_str();
   }
}
//---------------------------------------------------------------------------
void TGJVQSOLogFrame::setDTGNotValid(ScreenContact */*vcct*/)
{
// nothing needed
}
//---------------------------------------------------------------------------

