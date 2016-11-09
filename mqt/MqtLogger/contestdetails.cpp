#include "logger_pch.h"
#include "CalendarList.h"
#include "BandList.h"
#include "tentryoptionsform.h"
#include "tminoshelpform.h"
#include "tcalendarform.h"

#include "tlogcontainer.h"

#include "contestdetails.h"
#include "ui_contestdetails.h"

ContestDetails::ContestDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContestDetails),
    contest(0), inputcontest(0),
    saveContestOK(false), suppressProtectedOnClick(false)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->setupUi(this);

    QSettings settings;
    QByteArray geometry = settings.value("ContestDetails/geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    ui->ExchangeComboBox->addItem("No Exchange Required");
    ui->ExchangeComboBox->addItem("PostCode Multipliers");
    ui->ExchangeComboBox->addItem("Other Exchange Multiplier");
    ui->ExchangeComboBox->addItem("Exchange Required (no multiplier)");

    ui->BonusComboBox->addItem("None");
    ui->BonusComboBox->addItem("UKAC Bonuses");

    for ( int i = 0; i < 24; i++ )
    {
        QString cbText = QString("%1:").arg(i, 2, 10, QChar('0'));
        QString hour = cbText + "00 UTC";
        QString halfhour = cbText + "30 UTC";
        ui->StartTimeCombo->addItem( hour );
        ui->StartTimeCombo->addItem ( halfhour );
        ui->EndTimeCombo->addItem( hour );
        ui->EndTimeCombo->addItem ( halfhour );
    }
    ui->CallsignEdit->setValidator(new UpperCaseValidator());
    ui->LocatorEdit->setValidator(new UpperCaseValidator());
}
void ContestDetails::doCloseEvent()
{
    QSettings settings;
    settings.setValue("ContestDetails/geometry", saveGeometry());
}
void ContestDetails::reject()
{
    doCloseEvent();
    QDialog::reject();
}
void ContestDetails::accept()
{
    doCloseEvent();
    QDialog::accept();
}

int ContestDetails::exec()
{
    ui->QTHBundleFrame->initialise( this, "QTH", &contest->QTHBundle, &contest->QTHBundleName );
    ui->StationBundleFrame->initialise(this,  "Station", &contest->stationBundle, &contest->stationBundleName );
    ui->EntryBundleFrame->initialise(this,  "Entry", &contest->entryBundle, &contest->entryBundleName );
    ui->ContestNameSelected->setText(contest->VHFContestName.getValue());

    contest->initialiseINI();

    QWidget *nextD = getDetails( );
    if ( nextD )
    {
       nextD->setFocus();
    }

    return QDialog::exec();
}

ContestDetails::~ContestDetails()
{
    delete ui;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
      onShow - needs to produce a copy of the contest structure containing relevant
               fields
      setDetails (both) to populate display from those details

      getDetails to get from screen into copy

      Then OK Button to actually copy the detail over - then cancel
      can just discard the details
*/
void ContestDetails::setDetails( LoggerContestLog * pcont )
{
   if ( !pcont )
      return ;
   inputcontest = pcont;
   contest = new LoggerContestLog();
   *contest = *pcont;                // is this safe? not with the QSO vector... although it won't get changed!
   sectionList = contest->sectionList.getValue(); // the combo will then be properly set up in setDetails()
   setDetails();
}
void ContestDetails::setDetails(  )
{
   setWindowTitle( ("Details of Contest Entry - " + contest->cfileName));

   ui->ContestNameEdit->setText(contest->name.getValue());

   ui->BandComboBox->clear();
   // need to get legal bands from ContestLog
   if ( contest->bands.count() )
   {
      ui->BandComboBox->addItems(contest->bands);
   }
   else
   {
      BandList &blist = BandList::getBandList();
      for (std::vector<BandInfo>::iterator i = blist.bandList.begin(); i != blist.bandList.end(); i++)
      {
         if ((*i).getType() != "HF")
         {
            ui->BandComboBox->addItem( (*i).uk );
         }
      }
   }
   QString cb = contest->band.getValue().trimmed();
   BandList &blist = BandList::getBandList();
   BandInfo bi;
   bool bandOK = blist.findBand(cb, bi);
   if (bandOK)
   {
      cb = bi.uk;
   }

   int b = ui->BandComboBox->findText( cb );        // contest

   if ( b >= 0 )
   {
      ui->BandComboBox->setCurrentIndex( b);
   }
   else
   {
      ui->BandComboBox->setCurrentText(contest->band.getValue());
   }

   ui->SectionComboBox->clear();
   if ( sectionList.size() )
   {
       QStringList sl = sectionList.split(",", QString::SkipEmptyParts);
       ui->SectionComboBox->addItems(sl);
   }

   int s = ui->SectionComboBox->findText( contest->entSect.getValue() );        // contest

   if ( s >= 0 )
   {
      ui->SectionComboBox->setCurrentIndex(s);
   }
   else
   {
      ui->SectionComboBox->setCurrentText(contest->entSect.getValue());
   }

   // start/end of ContestLog
   // if attempt to log QSO BEFORE, complain (but allow) (give set time offset option)
   // if AFTER at load time, set "post entry"
   // if attempt to log QSO AFTER and NOT post entry, complain (give set time offset option, post entry option)
   //      std::string contest->DTGStart;  //ccccmmsshhmm
   //      std::string contest->DTGEnd;    //ccccmmsshhmm

   if ( contest->DTGStart.getValue().size() )
   {
      QString s = contest->DTGStart.getValue();
      QDateTime t = QDateTime::fromString(s, "yyyyMMddhhmm");
      ui->StartDateEdit->setDate(t.date());
      QString stc = t.time().toString( "hh:mm UTC" );
      ui->StartTimeCombo->setCurrentText(stc);
   }
   else
   {
      //         StartDateEdit->Date = "";
      ui->StartTimeCombo->setCurrentText("");
   }
   if ( contest->DTGEnd.getValue().size() )
   {
       QString s = contest->DTGEnd.getValue();
       QDateTime t = QDateTime::fromString(s, "yyyyMMddhhmm");
      ui->EndDateEdit->setDate(t.date()); // short date format, hours:minutes
      QString etc = t.time().toString( "hh:mm UTC" );
      ui->EndTimeCombo->setCurrentText(etc); // short date format, hours:minutes
   }
   else
   {
      //         EndDateEdit->Date = "";
      ui->EndTimeCombo->setCurrentText("");
   }

   QString call = contest->mycall.fullCall.getValue();
   if ( !call.size() )                                       // Entry
   {
      contest->entryBundle.getStringProfile( eepCall, call );

      // STL version of strupr
      call = call.toUpper();
      contest->mycall.fullCall.setValue( call );
   }
   ui->CallsignEdit->setText(call);
   if (contest->currentOp1.getValue().size()== 0)
   {
      contest->currentOp1.setValue( call);
   }

   contest->validateLoc();
   if ( !contest->locValid && contest->myloc.loc.getValue().size() == 0 )
   {
      QString temp;
      contest->QTHBundle.getStringProfile( eqpLocator, temp );
      contest->myloc.loc.setValue( temp );
      contest->validateLoc();
   }
   ui->LocatorEdit->setText(contest->myloc.loc.getValue());

   ui->AllowLoc4CB->setChecked(contest->allowLoc4.getValue());    // bool               // ?? contest
   ui->AllowLoc8CB->setChecked(contest->allowLoc8.getValue());    // bool               // ?? contest

   ui->ExchangeEdit->setText(contest->location.getValue()); // QTH/if contest specifies - but disp anyway

   switch (contest->scoreMode.getValue())
   {
   case 0:
       ui->commencedKRB->setChecked(true);
       break;
   case 1:
       ui->PPQSORB->setChecked(true);
       break;
   }

   /*
      ExchangeComboBox:

      No Exchange Required
      PostCode Multipliers
      Other Exchange Multiplier
      Exchange Required (no multiplier)
   */

   if ( contest->districtMult.getValue() )
   {
      ui->ExchangeComboBox->setCurrentIndex( 1);
   }
   else
      if ( contest->otherExchange.getValue() )
      {
          ui->ExchangeComboBox->setCurrentIndex( 3);
      }
      else
      {
          ui->ExchangeComboBox->setCurrentIndex( 0);
      }
   ui->DXCCMult->setChecked( contest->countryMult.getValue()) ;
   ui->NonGCtryMult->setChecked( contest->nonGCountryMult.getValue()) ;

   ui->M7LocatorMults->setChecked(contest->M7Mults.getValue());

   bool UKACBonus = contest->UKACBonus.getValue();
   ui->BonusComboBox->setCurrentIndex(UKACBonus?1:0);

   ui->LocatorMult->setChecked(contest->locMult.getValue()) ;
   ui->GLocMult->setChecked(contest->GLocMult.getValue());

   ui->PowerEdit->setText(contest->power.getValue());

   if ( contest->isMinosFile() )
   {
      suppressProtectedOnClick = true;
      ui->ProtectedOption->setChecked(contest->isProtected() && !contest->isProtectedSuppressed());
      suppressProtectedOnClick = false;
   }
   else
   {
      ui->ProtectedOption->setEnabled(false);
   }
   ui->RSTField->setChecked(contest->RSTField.getValue()) ;   // bool                   // contest
   ui->SerialField->setChecked(contest->serialField.getValue()) ;   // bool             // contest
   ui->LocatorField->setChecked(contest->locatorField.getValue()) ;   // bool         // contest
   ui->QTHField->setChecked( contest->QTHField.getValue()) ;   // bool                   // contest

   ui->AntOffsetEdit->setText(QString::number(contest->bearingOffset.getValue()));	// int
   refreshOps();

   enableControls();
}
void ContestDetails::refreshOps()
{
   // refill the op combo boxes from the current contest, and select the correct op
   if (contest)
   {
      ui->MainOpComboBox->clear();
      ui->SecondOpComboBox->clear();
      //bool addCall = true;
      QStringList ops;
      for ( OperatorIterator i = contest->oplist.begin(); i != contest->oplist.end(); i++ )
      {
          if (!(*i).isEmpty())
            ops.append(*i);
      }
      ops.append(contest->currentOp1.getValue());
      ops.append(contest->currentOp2.getValue());

      ops.append("");

      ops.sort();
      ops.removeDuplicates();

      ui->MainOpComboBox->addItems(ops);
      ui->SecondOpComboBox->addItems(ops);

      ui->MainOpComboBox->setCurrentText(contest->currentOp1.getValue());
      ui->SecondOpComboBox->setCurrentText(contest->currentOp2.getValue());
   }
}
void ContestDetails::setDetails( const IndividualContest &ic )
{

   setWindowTitle("Details of Contest Entry - " + contest->cfileName );

   ui->ContestNameEdit->setText(ic.description.c_str());                      // contest
   contest->VHFContestName.setValue(ic.description.c_str());

   // need to get legal bands from ContestLog
   ui->BandComboBox->clear();

   ui->BandComboBox->addItem( ic.reg1band.c_str() );
   ui->BandComboBox->setCurrentIndex(0);

   ui->SectionComboBox->clear();

   sectionList = ic.sections.c_str(); // the combo will then be properly set up in setDetails()
   if ( sectionList.size() )
   {
      QStringList sl = sectionList.split(",");
      ui->SectionComboBox->addItems(sl);
   }

   int s = ui->SectionComboBox->findText( contest->entSect.getValue() );        // contest

   if ( s >= 0 )
   {
      ui->SectionComboBox->setCurrentIndex(s);
   }
   else
   {
      ui->SectionComboBox->setCurrentText(contest->entSect.getValue());
   }

   ui->StartDateEdit->setDate(ic.start.date());
   ui->StartTimeCombo->setCurrentText(ic.start.toString( "hh:mm" ) + " UTC");

   ui->EndDateEdit->setDate(ic.finish.date()); // short date format, hours:minutes
   ui->EndTimeCombo->setCurrentText(ic.finish.toString( "hh:mm" ) + " UTC"); // short date format, hours:minutes

   if ( ic.mults == "M1" )
   {
      // PC, DXCC
      contest->districtMult.setValue( true );
      contest->countryMult.setValue( true );
      contest->locMult.setValue( false );
      contest->GLocMult.setValue( false );
      contest->nonGCountryMult.setValue( false );

      contest->M7Mults.setValue(false);

      contest->UKloc_mult = false;
      contest->NonUKloc_mult = false;
      contest->UKloc_multiplier = 0;
      contest->NonUKloc_multiplier = 0;
   }
   else if ( ic.mults == "M2" )
   {
      // Loc
      contest->districtMult.setValue( false );
      contest->countryMult.setValue( false );
      contest->locMult.setValue( true );
      contest->GLocMult.setValue( false );
      contest->nonGCountryMult.setValue( false );

      contest->M7Mults.setValue(false);

      contest->UKloc_mult = true;
      contest->NonUKloc_mult = true;
      contest->UKloc_multiplier = 1;
      contest->NonUKloc_multiplier = 1;
   }
   else if ( ic.mults == "M3" )
   {
      // PC, DXCC, LOC
      contest->districtMult.setValue( true );
      contest->countryMult.setValue( true );
      contest->locMult.setValue( true );
      contest->GLocMult.setValue( false );
      contest->nonGCountryMult.setValue( false );

      contest->M7Mults.setValue(false);

      contest->UKloc_mult = true;
      contest->NonUKloc_mult = true;
      contest->UKloc_multiplier = 1;
      contest->NonUKloc_multiplier = 1;
   }
   else if ( ic.mults == "M4" )
   {
      // DXCC, LOC
      contest->districtMult.setValue( false );
      contest->countryMult.setValue( true );
      contest->locMult.setValue( true );
      contest->GLocMult.setValue( false );
      contest->nonGCountryMult.setValue( false );

      contest->M7Mults.setValue(false);

      contest->UKloc_mult = true;
      contest->NonUKloc_mult = true;
      contest->UKloc_multiplier = 1;
      contest->NonUKloc_multiplier = 1;
   }
   else if ( ic.mults == "M5" )
   {
      // G Locs only
      contest->districtMult.setValue( false );
      contest->countryMult.setValue( false );
      contest->locMult.setValue( true );
      contest->GLocMult.setValue( true );
      contest->nonGCountryMult.setValue( false );

      contest->M7Mults.setValue(false);

      contest->UKloc_mult = true;
      contest->NonUKloc_mult = false;
      contest->UKloc_multiplier = 1;
      contest->NonUKloc_multiplier = 0;
   }
   else if ( ic.mults == "M6" )
   {
      // G Locs only  + DXCC
      contest->districtMult.setValue( false );
      contest->countryMult.setValue( false );
      contest->locMult.setValue( true );
      contest->GLocMult.setValue( true );
      contest->nonGCountryMult.setValue( true );

      contest->M7Mults.setValue(false);

      contest->UKloc_mult = true;
      contest->NonUKloc_mult = false;
      contest->UKloc_multiplier = 1;
      contest->NonUKloc_multiplier = 0;
   }
   else if ( ic.mults == "M7" )
   {
      // Modified M5; non UK 1 mult, UK 2 mults
      contest->districtMult.setValue( false );
      contest->countryMult.setValue( false );
      contest->locMult.setValue( true );
      contest->GLocMult.setValue( true );
      contest->nonGCountryMult.setValue( false );

      contest->M7Mults.setValue(true);

      contest->UKloc_mult = true;
      contest->NonUKloc_mult = true;
      contest->UKloc_multiplier = 2;
      contest->NonUKloc_multiplier = 1;
   }
   else
   {
      contest->districtMult.setValue( false );
      contest->countryMult.setValue( false );
      contest->locMult.setValue( false );
      contest->GLocMult.setValue( false );
      contest->nonGCountryMult.setValue( false );

      contest->M7Mults.setValue(false);

      contest->UKloc_mult = false;
      contest->NonUKloc_mult = false;
      contest->UKloc_multiplier = 0;
      contest->NonUKloc_multiplier = 0;
   }
   /*
      ExchangeComboBox:

      No Exchange Required
      PostCode Multipliers
      Other Exchange Multiplier
      Exchange Required (no multiplier)
   */

   if ( contest->districtMult.getValue() )
   {
      ui->ExchangeComboBox->setCurrentIndex(1);
   }
   else
      if ( contest->otherExchange.getValue() )
      {
         ui->ExchangeComboBox->setCurrentIndex(3);
      }
      else
      {
         ui->ExchangeComboBox->setCurrentIndex(0);
      }
   ui->NonGCtryMult->setChecked(contest->nonGCountryMult.getValue()) ;
   ui->DXCCMult->setChecked(contest->countryMult.getValue()) ;

   ui->LocatorMult->setChecked(contest->locMult.getValue()) ;
   ui->GLocMult->setChecked(contest->GLocMult.getValue()) ;
   ui->M7LocatorMults->setChecked(contest->M7Mults.getValue()) ;

   bool UKACBonus = contest->UKACBonus.getValue();
   ui->BonusComboBox->setCurrentIndex(UKACBonus?1:0);


   ui->RSTField->setChecked(true) ;
   ui->SerialField->setChecked(true) ;
   ui->LocatorField->setChecked(true) ;
   ui->QTHField->setChecked(true) ;

   contest->scoreMode.setValue( ( SCOREMODE ) ( ic.ppKmScoring ? 0 : 1 ) );  // combo

   switch (( ic.ppKmScoring ? 0 : 1 ))
   {
   case 0:
       ui->commencedKRB->setChecked(true);
       break;
   case 1:
       ui->PPQSORB->setChecked(true);
       break;
   }
//   setDetails();
}
//---------------------------------------------------------------------------
QWidget * ContestDetails::getDetails( )
{
    QWidget *nextD = getNextFocus();

    contest->name.setValue( ui->ContestNameEdit->text() );
    contest->band.setValue( ui->BandComboBox->currentText() );
    contest->entSect.setValue( ui->SectionComboBox->currentText() );
    contest->sectionList.setValue( sectionList );


    if (ui->StartDateEdit->text().isEmpty())
    {
        ui->StartDateEdit->setDate(QDate::currentDate());
    }
    contest->DTGStart.setValue(TDTToCanonical( ui->StartDateEdit->text() + " " + ui->StartTimeCombo->currentText())) ;

    if (ui->EndDateEdit->text().isEmpty())
    {
        ui->EndDateEdit->setDate(QDate::currentDate());
    }
    contest->DTGEnd.setValue(  TDTToCanonical(ui->EndDateEdit->text() + " " + ui->EndTimeCombo->currentText())) ;

    contest->mycall.fullCall.setValue( ui->CallsignEdit->text() );
    contest->mycall.valRes = CS_NOT_VALIDATED;
    contest->mycall.validate();
    if ( contest->mycall.valRes != CS_OK )
    {
        nextD = (nextD?nextD:ui->CallsignEdit);
    }
    contest->myloc.loc.setValue( ui->LocatorEdit->text() );
    contest->myloc.validate();
    if ( contest->myloc.valRes != LOC_OK )
    {
        nextD = (nextD?nextD:ui->LocatorField);
    }
    if ( contest->currentOp1.getValue().size() == 0 )
    {
        nextD = (nextD?nextD:ui->MainOpComboBox);
    }
    contest->allowLoc4.setValue( ui->AllowLoc4CB->isChecked() );    // bool
    contest->allowLoc8.setValue( ui->AllowLoc8CB->isChecked() );    // bool
    contest->location.setValue( ui->ExchangeEdit->text() );
    contest->scoreMode.setValue( ( SCOREMODE ) ui->PPQSORB->isChecked()?1:0 );  // combo

    if (ui->NonGCtryMult->isChecked())
    {
        ui->DXCCMult->setChecked(true);
    }
    contest->countryMult.setValue( ui->DXCCMult->isChecked() );   // bool
    contest->nonGCountryMult.setValue( ui->NonGCtryMult->isChecked() );   // bool

    if (ui->GLocMult->isChecked() || ui->M7LocatorMults->isChecked())
    {
        ui->LocatorMult->setChecked(true);
    }

    contest->locMult.setValue( ui->LocatorMult->isChecked() ) ;   // bool
    contest->GLocMult.setValue( ui->GLocMult->isChecked() ) ;   // bool
    contest->M7Mults.setValue( ui->M7LocatorMults->isChecked() ) ;   // bool
    contest->UKACBonus.setValue(ui->BonusComboBox->currentIndex() == 1);

    if (contest->M7Mults.getValue())
    {
        contest->UKloc_mult = true;
        contest->NonUKloc_mult = true;
        contest->UKloc_multiplier = 2;
        contest->NonUKloc_multiplier = 1;
    }
    else
    {
        if (contest->locMult.getValue())
        {
            contest->UKloc_mult = true;
            contest->UKloc_multiplier = 1;

            if (contest->GLocMult.getValue())
            {
                contest->NonUKloc_mult = false;
                contest->NonUKloc_multiplier = 0;
            }
            else
            {
                contest->NonUKloc_mult = true;
                contest->NonUKloc_multiplier = 1;
            }
        }
        else
        {
            contest->UKloc_mult = false;
            contest->NonUKloc_mult = false;
            contest->UKloc_multiplier = 0;
            contest->NonUKloc_multiplier = 0;
        }
    }
    if (ui->ProtectedOption->isChecked() && contest->isProtected() && contest->isProtectedSuppressed())
    {
        contest->setProtectedSuppressed(false);
    }
    else
    {
        if (ui->ProtectedOption->isChecked() && !contest->isProtected())
        {
            contest->setProtected( true ) ;
            saveContestOK  = true;
        }
        else if (!ui->ProtectedOption->isChecked() && contest->isProtected())
        {
            contest->setProtected( false ) ;
            saveContestOK  = true;
        }
        else if (!contest->isReadOnly()) // not protected, not unwriteable, protected but suppressed
        {
            saveContestOK  = true;
        }
    }
    /*
      ExchangeComboBox:

      No Exchange Required
      PostCode Multipliers
      Other Exchange Multiplier
      Exchange Required (no multiplier)
   */
    switch ( ui->ExchangeComboBox->currentIndex() )
    {
    case 0:
        contest->otherExchange.setValue( false );
        contest->districtMult.setValue( false );
        break;

    case 1:
        contest->otherExchange.setValue( true );
        contest->districtMult.setValue( true );
        break;

    case 2:
        contest->otherExchange.setValue( true );
        contest->districtMult.setValue( false );
        break;

    case 3:
        contest->otherExchange.setValue( true );
        contest->districtMult.setValue( false );
        break;

    }
    contest->RSTField.setValue( ui->RSTField->isChecked() ) ;   // bool
    contest->serialField.setValue( ui->SerialField->isChecked() ) ;   // bool
    contest->locatorField.setValue( ui->LocatorField->isChecked() ) ;   // bool
    contest->QTHField.setValue( ui->QTHField->isChecked() ) ;   // bool

    contest->power.setValue( ui->PowerEdit->text() );
    contest->bearingOffset.setValue(ui->AntOffsetEdit->text().toInt());	// int

    contest->currentOp1.setValue(ui->MainOpComboBox->currentText());
    contest->currentOp2.setValue(ui->SecondOpComboBox->currentText());
    contest->oplist.insert(contest->currentOp1.getValue());
    contest->oplist.insert(contest->currentOp2.getValue());

    contest->validateLoc();

    return nextD;
}
//---------------------------------------------------------------------------
QWidget * ContestDetails::getNextFocus()
{

   if ( ui->ContestNameEdit->text().trimmed().isEmpty() )
   {
      return ui->ContestNameEdit;
   }
   if ( ui->BandComboBox->currentText().trimmed().isEmpty() )
   {
      return ui->BandComboBox;
   }
   if ( ui->CallsignEdit->text().trimmed().isEmpty() )
   {
      return ui->CallsignEdit;
   }
   if ( ui->LocatorEdit->text().trimmed().isEmpty() )
   {
      return ui->LocatorEdit;
   }
   if ( ui->PowerEdit->text().trimmed().isEmpty() )
   {
      return ui->PowerEdit;
   }
   return 0;
}

//---------------------------------------------------------------------------
void ContestDetails::enableControls()
{
// Should protected be disabled if the contest is unwriteable?
   bool protectedChecked = ui->ProtectedOption->isChecked();
// enable/disable relevant fields based on protected
   ui->ContestNameEdit->setEnabled(!protectedChecked);
   ui->BandComboBox->setEnabled(!protectedChecked);
   ui->CallsignEdit->setEnabled(!protectedChecked);
   ui->LocatorEdit->setEnabled(!protectedChecked);
   ui->ExchangeEdit->setEnabled(!protectedChecked);
   ui->ScoreGroupBox->setEnabled(!protectedChecked);
   ui->MultGroupBox->setEnabled(!protectedChecked);
   ui->DXCCMult->setEnabled(!protectedChecked);
   ui->LocatorMult->setEnabled(!protectedChecked);
   ui->FieldsGroupBox->setEnabled(!protectedChecked);
   ui->RSTField->setEnabled(!protectedChecked);
   ui->SerialField->setEnabled(!protectedChecked);
   ui->LocatorField->setEnabled(!protectedChecked);
   ui->QTHField->setEnabled(!protectedChecked);

   ui->QTHBundleFrame->enableBundle(!protectedChecked);
   ui->StationBundleFrame->enableBundle(!protectedChecked);
   ui->EntryBundleFrame->enableBundle(!protectedChecked);

   ui->SectionComboBox->setEnabled(!protectedChecked);
   ui->StartTimeCombo->setEnabled(!protectedChecked);
   ui->EndTimeCombo->setEnabled(!protectedChecked);
   ui->ExchangeComboBox->setEnabled(!protectedChecked);
   ui->VHFCalendarButton->setEnabled(!protectedChecked);
   ui->ContestNameSelected->setEnabled(!protectedChecked);
   ui->LocatorGroupBox->setEnabled(!protectedChecked);
   ui->AllowLoc8CB->setEnabled(!protectedChecked);
   ui->AllowLoc4CB->setEnabled(!protectedChecked);
   ui->StartDateEdit->setEnabled(!protectedChecked);
   ui->EndDateEdit->setEnabled(!protectedChecked);
   ui->MainOpComboBox->setEnabled(!protectedChecked);
   ui->SecondOpComboBox->setEnabled(!protectedChecked);
   ui->AntOffsetEdit->setEnabled(!protectedChecked);
}
//---------------------------------------------------------------------------

void ContestDetails::on_OKButton_clicked()
{
    // make sure we have the minimum required information

    if (ui->ProtectedOption->isChecked() && ! contest->isProtected())
    {
       if (!mShowYesNoMessage(this, "This contest will be marked as protected.\r\n"
                                     "This is a permanent change that may be temporarily overridden.\r\n"
                                     "Please confirm this change by pressing \"Yes\"." ))
       {
          return;
       }
    }

    QWidget *nextD = getDetails( );
    if ( nextD )
    {
       nextD->setFocus();
    }
    else
    {
       if (saveContestOK)
       {
          bool temp = contest->isProtectedSuppressed();
          contest->setProtectedSuppressed(true);
          contest->commonSave( false );
          contest->setProtectedSuppressed(temp);
       }
       *inputcontest = *contest;
       accept();
    }

}

void ContestDetails::on_EntDetailButton_clicked()
{
    getDetails( );   // override from the window
    TEntryOptionsForm EntryDlg( this, contest, false );
    if ( EntryDlg.exec() == QDialog::Accepted )
       setDetails( );

}

void ContestDetails::on_CancelButton_clicked()
{
    reject();
}


QString BSHelpText =
   "These settings are groups of settings that can "
   "be applied to a contest all in one go."
   "\r\n\r\n"
   "There are four basic groups: - \r\n\r\n"
   "Contest - for the description, bands, multipliers and time of a contest\r\n"
   "Entry - all the extra bits for a real entry - callsign, group, contact details.\r\n"
   "Station - Rig details, antenna, antenna height.\r\n"
   "QTH - where the station is, height above sea level, Locator.\r\n"
   "\r\n"
   "To use them select from the drop down lists, or for Contest, use the "
   "\"VHF Calendar\" button.\r\n"
   "Any group set to \"<none>\" will be ignored.\r\n"
   "\r\n"
   "If the setting you want isn't there, press the \"Edit\" "
   "button for the group.\r\n"
   "\r\n"
   "This brings up a dialog where you can define a new setting, "
   "copy an existing setting, or delete an existing setting\r\n"
   "\r\n"
   "Click on the setting name on the left to select an existing setting "
   "and then its components are shown in the right hand pane, and "
   "can be edited individually.\r\n"
   "\r\n"
   "Move between components of a group using the mouse or up/down arrow keys.\r\n"
   ;

void ContestDetails::on_BSHelpButton_clicked()
{
    // Put up the help text on bundled settings
     TMinosHelpForm HelpForm( this );
     HelpForm.setText(BSHelpText);
     HelpForm.exec();}

void ContestDetails::on_VHFCalendarButton_clicked()
{
    TCalendarForm CalendarDlg(this, ectVHF);

    CalendarDlg.setWindowTitle( "VHF Calendar");
    CalendarDlg.description = ui->ContestNameSelected->text();

    if ( CalendarDlg.exec() == QDialog::Accepted )
    {
       // set up all the details that we can from the calendar
       ui->ContestNameSelected->setText(CalendarDlg.ic.description.c_str());
       setDetails( CalendarDlg.ic );
    }
    QWidget *next = getNextFocus();
    if (next)
    {
       next->setFocus();
    }
    else
    {
       ui->OKButton->setFocus();
    }
}

void ContestDetails::on_CallsignEdit_editingFinished()
{
    if (ui->MainOpComboBox->currentText().isEmpty())
    {
       ui->MainOpComboBox->addItem( ( ui->CallsignEdit->text() ) );
       ui->MainOpComboBox->setCurrentText( ui->CallsignEdit->text());
    }

}
void ContestDetails::on_DXCCMult_clicked()
{
    if (ui->DXCCMult->isChecked())
    {
       ui->NonGCtryMult->setChecked(false);
    }
}

void ContestDetails::on_NonGCtryMult_clicked()
{
    if (ui->NonGCtryMult->isChecked())
    {
       ui->DXCCMult->setChecked(false);
    }
}

void ContestDetails::on_LocatorMult_clicked()
{
    if (!ui->LocatorMult->isChecked())
    {
       ui->GLocMult->setChecked(false);
       ui->M7LocatorMults->setChecked(false);
    }
}

void ContestDetails::on_GLocMult_clicked()
{
    if (ui->GLocMult->isChecked())
    {
       ui->LocatorMult->setChecked(true);
       ui->M7LocatorMults->setChecked(false);
    }
}

void ContestDetails::on_M7LocatorMults_clicked()
{
    if (ui->M7LocatorMults->isChecked())
    {
       ui->LocatorMult->setChecked(true);
       ui->GLocMult->setChecked(false);
    }
}

void ContestDetails::on_ProtectedOption_clicked()
{
    if (!suppressProtectedOnClick)
    {
       if (ui->ProtectedOption->isChecked() )
       {
          // move to protected
          if (!mShowYesNoMessage(this, "Are you sure you want to protect this contest?" ))
          {
             ui->ProtectedOption->setChecked(contest->isProtected() && !contest->isProtectedSuppressed());
          }
       }
       else // unchecked
       {
          if (!mShowYesNoMessage(this, "Are you sure you want to disable protection for this contest?" ))
          {
             ui->ProtectedOption->setChecked(contest->isProtected() && !contest->isProtectedSuppressed());
          }
       }
       enableControls();
    }
}
void ContestDetails::bundleChanged()
{
    getDetails( );   // override from the window

    contest->setINIDetails();
    setDetails( );
    QWidget *next = getNextFocus();
    if (next)
    {
        next->setFocus();
    }
    else
    {
        ui->OKButton->setFocus();
    }
}
