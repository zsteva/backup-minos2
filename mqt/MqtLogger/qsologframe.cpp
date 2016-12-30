#include "logger_pch.h"

#include "tlogcontainer.h"
#include "tqsoeditdlg.h"
#include "tforcelogdlg.h"
#include "SendRPCDM.h"

#include "qsologframe.h"
#include "ui_qsologframe.h"

QSOLogFrame::QSOLogFrame(QWidget *parent) :
    QFrame(parent)
    , ui(new Ui::QSOLogFrame)
    , contest(0)
    , selectedContact(0)
    , current(0)
    , partialContact(0)
    , edit(false)
    , overstrike(false)
    , oldTimeOK(true)
{
    ui->setupUi(this);

    CallsignFW = new FocusWatcher(ui->CallsignEdit);
    CallsignLabelString = ui->Callsignlabel->text();
    ui->CallsignEdit->setValidator(new UpperCaseValidator(true));
    ui->CallsignEdit->installEventFilter(this);

    RSTTXFW = new FocusWatcher(ui->RSTTXEdit);
    RSTTXLabelString = ui->RSTTXLabel->text();
    ui->RSTTXEdit->setValidator(new UpperCaseValidator(true));
    ui->RSTTXEdit->installEventFilter(this);

    SerTXFW = new FocusWatcher(ui->SerTXEdit);
    SerTXLabelString = ui->SerTXLabel->text();
    ui->SerTXEdit->setValidator(new UpperCaseValidator(true));
    ui->SerTXEdit->installEventFilter(this);

    RSTRXFW = new FocusWatcher(ui->RSTRXEdit);
    RSTRXLabelString = ui->RSTRXLabel->text();
    ui->RSTRXEdit->setValidator(new UpperCaseValidator(true));
    ui->RSTRXEdit->installEventFilter(this);

    SerRXFW = new FocusWatcher(ui->SerRXEdit);
    SerRXLabelString = ui->SerRXLabel->text();
    ui->SerRXEdit->setValidator(new UpperCaseValidator(true));
    ui->SerRXEdit->installEventFilter(this);

    LocFW = new FocusWatcher(ui->LocEdit);
    LocLabelString = ui->LocLabel->text();
    ui->LocEdit->setValidator(new UpperCaseValidator(true));
    ui->LocEdit->installEventFilter(this);

    QTHFW = new FocusWatcher(ui->QTHEdit);
    QTHLabelString = ui->QTHLabel->text();
    ui->QTHEdit->setValidator(new UpperCaseValidator(true));
    ui->QTHEdit->installEventFilter(this);

    CommentsFW = new FocusWatcher(ui->CommentsEdit);
    CommentsLabelString = ui->CommentsLabel->text();
    ui->CommentsEdit->installEventFilter(this);

    MainOpFW = new FocusWatcher(ui->MainOpComboBox);
    SecondOpFW = new FocusWatcher(ui->SecondOpComboBox);

    connect(CallsignFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));
    connect(RSTTXFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));
    connect(SerTXFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));
    connect(RSTRXFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));
    connect(SerRXFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));
    connect(LocFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));
    connect(QTHFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));
    connect(CommentsFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));
    connect(MainOpFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));
    connect(SecondOpFW, SIGNAL(focusChanged(QObject *, bool, QFocusEvent * )), this, SLOT(focusChange(QObject *, bool, QFocusEvent *)));

    ui->TimeEdit->installEventFilter(this);

    ui->ModeComboBoxGJV->addItem("A1A");
    ui->ModeComboBoxGJV->addItem("J3E");

    connect(&MinosLoggerEvents::mle, SIGNAL(TimerDistribution()), this, SLOT(on_TimeDisplayTimer()));
    connect(&MinosLoggerEvents::mle, SIGNAL(AfterTabFocusIn(QLineEdit*)), this, SLOT(on_AfterTabFocusIn(QLineEdit*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(Validated()), this, SLOT(on_Validated()));
    connect(&MinosLoggerEvents::mle, SIGNAL(ValidateError(int)), this, SLOT(on_ValidateError(int)));
    connect(&MinosLoggerEvents::mle, SIGNAL(ShowOperators()), this, SLOT(on_ShowOperators()));

    QFontMetrics metrics(QApplication::font());
    ui->BrgSt->setFixedWidth(metrics.width("(8888)MT"));
}
bool QSOLogFrame::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = dynamic_cast<QKeyEvent *>(event);
        return doKeyPressEvent(ke);
    }
    else if (event->type() == QEvent::MouseButtonDblClick)
    {
        if (obj == ui->SerTXEdit || (edit && (obj == ui->TimeEdit)))
        {
            mouseDoubleClickEvent(obj);
        }
   }

   return false;
}
bool QSOLogFrame::doKeyPressEvent( QKeyEvent* event )
{
    if (!event)
        return false;

    int Key = event->key();

    Qt::KeyboardModifiers mods = event->modifiers();
    bool shift = mods & Qt::ShiftModifier;
    bool ctrl = mods & Qt::ControlModifier;
    //bool alt = mods & Qt::AltModifier;

    if (Key == Qt::Key_Return)
    {
        QMetaObject::invokeMethod(ui->GJVOKButton, "clicked", Qt::QueuedConnection);
        return true;
    }
    if (Key == Qt::Key_Escape)
    {
        QMetaObject::invokeMethod(ui->GJVCancelButton, "clicked", Qt::QueuedConnection);
        return true;
    }

    if (( ctrl || shift ) && Key >= Qt::Key_F1 && Key <= Qt::Key_F12 )
    {
        if ( ctrl && shift )
        {
            // keyer record keys
            TSendDM::sendKeyerRecord( Key - Qt::Key_F1 + 1 );
        }
        else
        {
            // Keyer play keys
            TSendDM::sendKeyerPlay( Key - Qt::Key_F1 + 1 );
        }
        return true;
    }

    if ( ( Key == Qt::Key_F1 || Key == Qt::Key_F2 || Key == Qt::Key_F3 || Key == Qt::Key_F4 || Key == Qt::Key_F5 || Key == Qt::Key_F6) )
    {
        setActiveControl( &Key );
        return true;
    }

    bool doReturn = false;
    if ( ( Key == Qt::Key_Insert ) && !shift && !ctrl )
    {
        overstrike = !overstrike;
        doReturn = true;
    }
    bool ovr = overstrike;

    QLineEdit *ed = dynamic_cast<QLineEdit *>( current );


    MinosLoggerEvents::SendReportOverstrike(ovr, contest);  // queued

    if (doReturn)
        return true;

    if (ed)
    {

        if (ovr && ((Key&0xff) == Key))    // QT keeps 8 bit ASCII range for real keys
        {
            int cpos = ed->cursorPosition();
            int edLen = ed->text().size();
            if (cpos < edLen)
            {
                QString edText = ed->text();
                if ( Key != Qt::Key_Delete )
                {
                    // just delete the chracter at cursor pos, and let new char be inserted
                    edText = edText.left(cpos) + edText.right(edLen - cpos - 1);
                    ed->setText(edText);
                    ed->setCursorPosition(cpos);
                }
                else
                {
                    // need to overstrike with space, but move cursor back one
                    edText = edText.left(cpos) + " " + edText.right(edLen - cpos);
                    ed->setText(edText);
                    ed->setCursorPosition(cpos);
                }
            }
        }
        if (ed == ui->CallsignEdit)
        {
            QString ss("QLineEdit { background-color: white ; border-style: outset ; border-width: 1px ; border-color: black ; }");
            ui->CallsignEdit->setStyleSheet(ss);
        }
    }
    return false;
}

QSOLogFrame::~QSOLogFrame()
{
    delete ui;
    for ( QVector <ValidatedControl *>::iterator vcp = vcs.begin(); vcp != vcs.end(); vcp++ )
    {
       delete ( *vcp );
    }
}
void QSOLogFrame::on_TimeDisplayTimer()
{
    updateQSOTime(true);
}

void QSOLogFrame::focusChange(QObject *obj, bool in, QFocusEvent *event)
{
    if (in)
        EditControlEnter(obj, event);
    else
        EditControlExit(obj);

    QColor colour = in?Qt::blue:Qt::black;
    QString colStr = HtmlFontColour(colour);

    if (obj == ui->CallsignEdit)
    {
        ui->Callsignlabel->setText(colStr + CallsignLabelString);
    }

    if (obj == ui->RSTTXEdit)
    {
        ui->RSTTXLabel->setText(colStr + RSTTXLabelString);
    }

    if (obj == ui->SerTXEdit)
    {
        ui->SerTXLabel->setText(colStr + SerTXLabelString);
    }

    if (obj == ui->RSTRXEdit)
    {
        ui->RSTRXLabel->setText(colStr + RSTRXLabelString);
    }

    if (obj == ui->SerRXEdit)
    {
        ui->SerRXLabel->setText(colStr + SerRXLabelString);
    }

    if (obj == ui->LocEdit)
    {
        ui->LocLabel->setText(colStr + LocLabelString);
    }

    if (obj == ui->QTHEdit)
    {
        ui->QTHLabel->setText(colStr + QTHLabelString);
    }

    if (obj == ui->CommentsEdit)
    {
        ui->CommentsLabel->setText(colStr + CommentsLabelString);
    }
    if (obj == ui->MainOpComboBox && in == false)
    {
        MainOpComboBox_Exit();
    }
    if (obj == ui->SecondOpComboBox && in == false)
    {
        SecondOpComboBox_Exit();
    }
}
void QSOLogFrame::setAsEdit()
{
    edit = true;
    ui->GJVCancelButton->setText("Return to Log");
}

void QSOLogFrame::initialise( BaseContestLog * pcontest, bool bf )
{
   catchup = bf;

   contest = pcontest;
   screenContact.initialise( contest ); // get ops etc correct

   csIl = new ValidatedControl( ui->CallsignEdit, vtCallsign );
   vcs.push_back( csIl );
   rsIl = new ValidatedControl( ui->RSTTXEdit, vtRST );
   vcs.push_back( rsIl );
   ssIl = new ValidatedControl( ui->SerTXEdit, vtSN );
   vcs.push_back( ssIl );
   rrIl = new ValidatedControl( ui->RSTRXEdit, vtRST );
   vcs.push_back( rrIl );
   srIl = new ValidatedControl( ui->SerRXEdit, vtSN );
   vcs.push_back( srIl );
   locIl = new ValidatedControl( ui->LocEdit, vtLoc );
   vcs.push_back( locIl );
   qthIl = new ValidatedControl( ui->QTHEdit, vtQTH );
   vcs.push_back( qthIl );
   cmntIl = new ValidatedControl( ui->CommentsEdit, vtComments );
   vcs.push_back( cmntIl );

   ui->BrgSt->clear();
   ui->DistSt->clear();

   if (!edit)
   {
       ui->EditFrame->setVisible(false);

       QString ss("QLineEdit {  border: none ; }");
       ui->DateEdit->setStyleSheet(ss);
       ui->TimeEdit->setStyleSheet(ss);
       ui->DateEdit->setReadOnly(true);
       ui->TimeEdit->setReadOnly(true);
   }
   else
   {
       ui->MatchXferButton->setVisible(false);
       ui->AutoBandmapTime->setVisible(false);
       ui->AutoBandmapTune->setVisible(false);
       ui->BandMapButton->setVisible(false);
   }

   updateQSODisplay();
   refreshOps();

//   SerTXEdit->Color = clBtnFace;
   QString ss("QLineEdit { background-color: silver ; border-style: outset ; border-width: 1px ; border-color: black ; }");
   ui->SerTXEdit->setStyleSheet(ss);
   current = 0;
   updateTimeAllowed = true;
   oldTimeOK = true;
   updateQSOTime();
   MinosLoggerEvents::SendReportOverstrike(overstrike, contest);

}
void QSOLogFrame::setXferEnabled(bool s)
{
    ui->MatchXferButton->setEnabled(s);
}

void QSOLogFrame::on_CatchupButton_clicked()
{
    TQSOEditDlg qdlg(this, true, false );
    qdlg.selectCatchup( contest );

    qdlg.exec();

    contest->scanContest();

    screenContact.initialise(contest);
    showScreenEntry();

    refreshOps();
    selectField( 0 );

}

void QSOLogFrame::on_FirstUnfilledButton_clicked()
{
    // Go to the first unfilled QSO
    // If there aren't any then it needs to be made invisible
    // ScanContest can work out how many there are - and we can display that on the button
       MinosLoggerEvents::SendNextUnfilled(contest);
}
void QSOLogFrame::setFirstUnfilledButtonEnabled(bool state)
{
    ui->FirstUnfilledButton->setEnabled(state);
}

void QSOLogFrame::MainOpComboBox_Exit()
{
    if (contest)
     {
        QString op1 = ui->MainOpComboBox->currentText();
        contest->currentOp1.setValue( op1 );
        if ( op1.size() )
        {
           contest->oplist.insert( op1, op1 );
        }
        contest->commonSave(false);
        refreshOps();
     }
}

void QSOLogFrame::SecondOpComboBox_Exit()
{
    if (contest)
    {
       QString op2 = ui->SecondOpComboBox->currentText();
       contest->currentOp2.setValue( op2 );
       if ( op2.size() )
       {
          contest->oplist.insert( op2, op2 );
       }
       contest->commonSave(false);
       refreshOps();
    }
}

void QSOLogFrame::on_GJVOKButton_clicked()
{
    ui->SerTXEdit->setReadOnly(true);
    //SerTXEdit->Color = clBtnFace;

    if ( contest->isReadOnly() )
    {
       return;
    }
    //GJVEditChange( Sender );   // start the match thread

    getScreenEntry(); // make sure it is saved

    if ( screenContact.contactFlags & ( LOCAL_COMMENT | DONT_PRINT | COMMENT_ONLY ) )
    {
       logCurrentContact( );
       return;
    }

    // validate the entry; if still invalid, spin round the invalid
    // controls (this should really be the job of tab, but...)

    QWidget *currn = current;
    if ( !valid( cmCheckValid ) || ( currn == ui->RSTTXEdit ) || ( currn == ui->RSTRXEdit ) )
       // make sure all single and cross field
       // validation has been done
    {
       doAutofill();
    }
    bool was_unfilled = screenContact.contactFlags & TO_BE_ENTERED;
    if ( !valid( cmCheckValid ) )   // make sure all single and cross field
       // validation has been done
    {
       QWidget * firstInvalid = 0;
       QWidget *nextInvalid = 0;
       bool onCurrent = false;
       bool pastCurrent = false;
       for ( QVector <ValidatedControl *>::iterator vcp = vcs.begin(); vcp != vcs.end(); vcp++ )
       {
          if ( /*( *vcp ) ->wc->ReadOnly ||*/ !( *vcp ) ->wc->isVisible() )
          {
 //         #error but if date or time are invalid...
             continue;
          }
          if ( onCurrent )
             pastCurrent = true;
          if ( ( *vcp ) ->wc == current )
             onCurrent = true;
          if ( !( *vcp ) ->valid( cmValidStatus ) )
          {
             if ( !firstInvalid )
                firstInvalid = ( *vcp ) ->wc;
             if ( pastCurrent )
             {
                if ( !nextInvalid )
                {
                   nextInvalid = ( *vcp ) ->wc;
                   break;
                }
             }
          }
       }

       // make sure we go to the invalid field

       QWidget *nextf = ( nextInvalid ) ? nextInvalid : firstInvalid;

       // but if it is DTG, probably want CS instead (Unless post entry)


       if ( nextf )
       {
          if ( nextf == current )
          {
             if ( firstInvalid != nextf )
                selectField( firstInvalid );
             else
                dlgForced();   // repeated attack on same faulty field
          }
          else
             selectField( nextf );
       }
       // Show on errList on multdisp frame
       MinosLoggerEvents::SendShowErrorList();
       return;
    }
    else
    {
       screenContact.contactFlags &= ~( TO_BE_ENTERED | FORCE_LOG );
    }

    // all is OK (or we will have executed a return statement)
    // so do it!

    // we have to check if we need to save it
    // checkLogEntry does the log action as well

    if ( !was_unfilled && !catchup && selectedContact )  // AND if we are logging "current" then we don't want to do this
    {
       if ( !checkLogEntry(true) )  // if it is the same, then don't log
       {
          return;
       }
    }
    else
    {
       logCurrentContact( );
    }
    if (edit && !catchup && !was_unfilled)
    {
        emit QSOFrameCancelled();
    }
    else
    {
        if ( edit && unfilled )
        {
           // If Uri mode then continue to the next...
           QSharedPointer<BaseContact> nuc = contest->findNextUnfilledContact( );
           selectEntry(nuc);

//           on_FirstUnfilledButton_clicked( );
           bool stillUnfilled = screenContact.contactFlags & TO_BE_ENTERED;

           if (!stillUnfilled)
           {
                emit QSOFrameCancelled();
           }
           else
           {
                selectField( 0 );             // make sure we move off the "Log" default button
           }
        }
        if (edit && catchup)
        {
            LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );
            int ctmax = ct->maxSerial + 1;
            QSharedPointer<BaseContact> lct = ct->addContact( ctmax, 0, false, catchup );
            selectEntry( lct );
        }
    }
    return;

}
bool QSOLogFrame::dlgForced()
{
    getScreenEntry();
    valid( cmCheckValid );       // This adds errors to the MAIN dialog error list, not our own

    TForceLogDlg ForceDlg(this);

    int res = ForceDlg.doexec(contest, screenContact, errs);
    if ( res == QDialog::Accepted )
    {
        // make sure marked on main screen

        ui->NonScoreCheckBox->setChecked(screenContact.contactFlags & NON_SCORING);
        ui->DeletedCheckBox->setChecked(screenContact.contactFlags & DONT_PRINT);
        if ( screenContact.contactFlags & NON_SCORING )
        {
            screenContact.multCount = 0;
            screenContact.bonus = 0;
        }

        // if no dtg then autofill dtg

        logCurrentContact( );
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void QSOLogFrame::on_GJVForceButton_clicked()
{
    if ( contest->isReadOnly() )
    {
       return;
    }
    ui->SerTXEdit->setReadOnly(true);
//    SerTXEdit->Color = clBtnFace;
    MinosLoggerEvents::SendShowErrorList();
    dlgForced();
}
bool QSOLogFrame::savePartial()
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
bool QSOLogFrame::restorePartial( void )
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
void QSOLogFrame::killPartial( void )
{
   if ( partialContact )
   {
      delete partialContact;
      partialContact = 0;
   }
}
void QSOLogFrame::on_Validated()
{
    killPartial();
}

void QSOLogFrame::startNextEntry( )
{
   if (contest->unfilledCount <= 0 || contest->isReadOnly())
   {
      ui->FirstUnfilledButton->setVisible(false);
   }
   else
   {
      ui->FirstUnfilledButton->setVisible(true);
   }

   if ( !restorePartial() )
   {
      updateTimeAllowed = true;
      screenContact.initialise( contest );
   }

   updateQSOTime();
   showScreenEntry();

   MinosLoggerEvents::SendAfterSelectContact(QSharedPointer<BaseContact>(), contest);
}
void QSOLogFrame::doGJVCancelButton_clicked()
{
    if (edit)
    {
        catchup = false;
        checkLogEntry(false);

        emit QSOFrameCancelled();

    }
    else
    {
        ui->SerTXEdit->setReadOnly(true);

        ScreenContact *temp = 0;
        if ( !partialContact )
        {
           savePartial();
           temp = partialContact;
           partialContact = 0;
        }
        updateTimeAllowed = true;
        startNextEntry();
        ui->CallsignEdit->setFocus();

        if ( temp )
           partialContact = temp;

        MinosLoggerEvents::SendShowErrorList();
    }
}

void QSOLogFrame::on_GJVCancelButton_clicked()
{
    doGJVCancelButton_clicked();
}

void QSOLogFrame::on_MatchXferButton_clicked()
{
    MinosLoggerEvents::SendXferPressed();
}

void QSOLogFrame::on_BandMapButton_clicked()
{
    MinosLoggerEvents::SendBandMapPressed();
}

void QSOLogFrame::on_QTHEdit_textChanged(const QString &/*arg1*/)
{
    doGJVEditChange( ui->QTHEdit );
}

void QSOLogFrame::on_CallsignEdit_textChanged(const QString &/*arg1*/)
{
    doGJVEditChange( ui->CallsignEdit );
}

void QSOLogFrame::on_LocEdit_textChanged(const QString &/*arg1*/)
{
   // force bearing calc
   getScreenEntry();
   calcLoc();
   doGJVEditChange( ui->LocEdit );
}
void QSOLogFrame::mouseDoubleClickEvent(QObject *w)
{
    // How to find what was double clicked?

    if (w == ui->SerTXEdit)
    {
        ui->SerTXEdit->setReadOnly(false);
    }
    if (edit && w == ui->TimeEdit)
    {
        ui->TimeEdit->setReadOnly(false);
        ui->DateEdit->setReadOnly(false);
    }
    /*
    if ( contest->isReadOnly() )
    {
       return ;
    }
    SerTXEdit->ReadOnly = false;
    SerTXEdit->Color = clWindow;
    */
}
void QSOLogFrame::setActiveControl( int *Key )
{
   switch ( *Key )
   {
      case Qt::Key_F1:
         selectField( ui->CallsignEdit );
         *Key = 0;
         break;
      case Qt::Key_F2:
         selectField( ui->RSTTXEdit );
         *Key = 0;
         break;
      case Qt::Key_F3:
         selectField( ui->RSTRXEdit );
         *Key = 0;
         break;
      case Qt::Key_F4:
         selectField( ui->SerRXEdit );
         *Key = 0;
         break;
      case Qt::Key_F5:
         selectField( ui->LocEdit );
         *Key = 0;
         break;
      case Qt::Key_F6:
         selectField( ui->QTHEdit );
         *Key = 0;
         break;
   }
}
//---------------------------------------------------------------------------
void QSOLogFrame::getScreenEntry()
{
   getScreenContactTime();
   screenContact.cs.fullCall.setValue( ui->CallsignEdit->text().trimmed() );
   screenContact.cs.valRes = CS_NOT_VALIDATED;
   screenContact.cs.validate( );

   screenContact.reps = ui->RSTTXEdit->text().trimmed();
   screenContact.serials = ui->SerTXEdit->text().trimmed();
   screenContact.repr = ui->RSTRXEdit->text().trimmed();
   screenContact.serialr = ui->SerRXEdit->text().trimmed();

   screenContact.loc.loc.setValue( ui->LocEdit->text().trimmed() );
   screenContact.bearing = -1;		// force a recalc
   screenContact.loc.validate();
   screenContact.extraText = ui->QTHEdit->text().trimmed();
   screenContact.comments = ui->CommentsEdit->text().trimmed();

   screenContact.mode = ui->ModeComboBoxGJV->currentText().trimmed();
   screenContact.contactFlags &= ~NON_SCORING;

   // op1/op2 get set when the attached combos change - I hope :)

   if ( ui->NonScoreCheckBox->isChecked() )
   {
      screenContact.contactFlags |= NON_SCORING;
   }
   screenContact.contactFlags &= ~DONT_PRINT;
   if ( ui->DeletedCheckBox->isChecked() )
   {
      screenContact.contactFlags |= DONT_PRINT;
   }
}
//---------------------------------------------------------------------------
void QSOLogFrame::logTabChanged()
{
    MinosLoggerEvents::SendScreenContactChanged(&screenContact, contest);
}
void QSOLogFrame::showScreenEntry( void )
{
   // display the contents of the contest->screenContact

   if ( contest )
   {
      // we only validate this contact up to the validation point

      contest->validationPoint = selectedContact?selectedContact->getLogSequence():0;
      ScreenContact temp;
      temp.copyFromArg( screenContact ); // as screen contact gets corrupted by auto changes
      // op1, op2 in ScreenContact ge corrupted as well
      showScreenContactTime(temp);
      ui->CallsignEdit->setText(temp.cs.fullCall.getValue().trimmed());
      ui->RSTTXEdit->setText(temp.reps.trimmed());
      ui->SerTXEdit->setText(temp.serials.trimmed());
      ui->RSTRXEdit->setText(temp.repr.trimmed());
      ui->SerRXEdit->setText(temp.serialr.trimmed());
      ui->LocEdit->setText(temp.loc.loc.getValue().trimmed());;  // also forces update of score etc
      ui->QTHEdit->setText(temp.extraText.trimmed());;
      ui->CommentsEdit->setText(temp.comments.trimmed());;
      ui->NonScoreCheckBox->setChecked(temp.contactFlags & NON_SCORING);
      ui->DeletedCheckBox->setChecked(temp.contactFlags & DONT_PRINT);

      setMode(temp.mode.trimmed());

      // and now we want to put the selection on each at the END of the text
      for ( QVector <ValidatedControl *>::iterator vcp = vcs.begin(); vcp != vcs.end(); vcp++ )
      {
         int selpt = ( *vcp ) ->wc->text().length();
         ( *vcp )->wc ->setSelection(selpt, 0);
      }

      valid( cmCheckValid ); // make sure contact is valid - display any errors
      if ( temp.contactFlags & ( COMMENT_ONLY | LOCAL_COMMENT ) )
      {
         selectField( ui->CommentsEdit );
      }
      else
         selectField( 0 );

      ui->SerTXEdit->setReadOnly(true);
      MinosLoggerEvents::SendShowErrorList();
      MinosLoggerEvents::SendScreenContactChanged(&screenContact, contest);
   }
}
//---------------------------------------------------------------------------
void QSOLogFrame::EditControlEnter(QObject *Sender , QFocusEvent *event)
{
   current = dynamic_cast<QWidget *>(Sender);
   QLineEdit *tle = dynamic_cast<QLineEdit *>(Sender);

   if (event->reason() == Qt::TabFocusReason || event->reason() == Qt::BacktabFocusReason)
   {
       MinosLoggerEvents::SendAfterTabFocusIn(tle);
   }
}
void QSOLogFrame::on_AfterTabFocusIn(QLineEdit *tle)
{
    int endpos = tle->text().size();
    if (endpos < 0)
    {
        endpos = 0;
    }
    tle->setCursorPosition(endpos);
    tle->deselect();

    bool ovr = overstrike;
    MinosLoggerEvents::SendReportOverstrike(ovr, contest);
}

//---------------------------------------------------------------------------

void QSOLogFrame::EditControlExit( QObject * /*Sender*/ )
{
   if (!contest)
   {
      return;
   }
   ui->SerTXEdit->setReadOnly(true);

   ui->TimeEdit->setReadOnly(!edit);
   ui->DateEdit->setReadOnly(!edit);

   if ( current == ui->LocEdit )
   {
      // do any required character substitutions, but only when we have a full
      // locator

      // This does something to correct sloppy typing

      QString loc = ui->LocEdit->text().trimmed().toUpper();
      if (loc.size() == 6 || loc.size() == 8)
      {
         if (loc[0] == '1')
         {
            loc[0] = 'I';
         }
         if (loc[0] == '0')
         {
            loc[0] = '1';
         }

         if (loc[1] == '1')
         {
            loc[1] = 'I';
         }
         if (loc[1] == '0')
         {
            loc[1] = 'O';
         }

         if (loc[2] == 'I')
         {
            loc[2] = '1';
         }
         if (loc[2] == 'O')
         {
            loc[2] = '0';
         }

         if (loc[3] == 'I')
         {
            loc[3] = '1';
         }
         if (loc[3] == 'O')
         {
            loc[3] = '0';
         }

         if (loc[4] == '1')
         {
            loc[4] = 'I';
         }
         if (loc[4] == '0')
         {
            loc[4] = 'O';
         }

         if (loc[5] == '1')
         {
            loc[5] = 'I';
         }
         if (loc[5] == '0')
         {
            loc[5] = 'O';
         }
         if (loc != ui->LocEdit->text().trimmed())
         {
            ui->LocEdit->setText(loc);
            doGJVEditChange( ui->LocEdit );   // start the match thread
         }
      }
   }
   if ( ( current == ui->CallsignEdit ) || ( current == ui->LocEdit ) )
   {
      valid( cmCheckValid ); // make sure all single and cross field
      doAutofill();           // should only be time to be filled
   }
   MinosLoggerEvents::SendShowErrorList();

   // make sure the mode button shows the correct "flip" value
   if (ui->ModeComboBoxGJV->currentText() == "A1A")
   {
      ui->ModeButton->setText("J3E");
   }
   else
   {
      ui->ModeButton->setText("A1A");
   }
}
    //---------------------------------------------------------------------------
void QSOLogFrame::setScoreText( int dist, bool partial, bool xband )
{
   QString b;
   QString s;
   s =  QString::number(dist );

   if ( partial )
      b += "(";
   b += s;
   if ( partial )
      b += ")";
   if ( xband )
      b += "X";
   b += "  ";
   ui->DistSt->setText(b);
}
//---------------------------------------------------------------------------
void QSOLogFrame::calcLoc( )
{
   QString gridref = ui->LocEdit->text().trimmed();

   if ( gridref.compare(oldloc, Qt::CaseInsensitive ) != 0 )
   {
      oldloc = gridref;
      locValid = true;
      double latitude;
      double longitude;
      double dist;
      int brg;

      int locValres = lonlat( gridref, longitude, latitude );
      if ( ( locValres ) != LOC_OK )
         locValid = false;

      ScreenContact &sct = screenContact;
      if ( ( sct.contactFlags & MANUAL_SCORE ) &&
           !( sct.contactFlags & DONT_PRINT ) )
      {
         int thisscore = sct.contactScore;
         setScoreText( thisscore, false, sct.contactFlags & XBAND );
         ui->BrgSt->setText("MANUAL");

      }
      else
         if ( ( locValid
                || ( locValres == LOC_PARTIAL ) )
              && contest->locValid )
         {
            contest->disbear( longitude, latitude, dist, brg );

            if ( locValid )  	// just 4CHAR not enough
            {
               sct.bearing = brg;

               if ( !( sct.contactFlags & ( MANUAL_SCORE | NON_SCORING | LOCAL_COMMENT | COMMENT_ONLY | DONT_PRINT ) ) )
               {
                  sct.contactScore = dist;
               }

            }

            int offset = contest->bearingOffset.getValue();

            QString rev;
            if (offset)
            {
                rev += "O";
            }
            rev += ( MinosParameters::getMinosParameters() ->getMagneticVariation() ) ? "M" : "T" ;
            int vb = varBrg( brg + offset);
            if ( TContestApp::getContestApp() ->reverseBearing )
            {
               vb = normBrg( vb - 180 );
               rev += "R";
            }
            setScoreText( static_cast< int> ( dist), ( locValres == LOC_PARTIAL ), sct.contactFlags & XBAND );
            QString brgbuff;
            const QChar degreeChar(0260); // octal value
            if ( locValres == LOC_PARTIAL )
            {
               brgbuff = QString( "(%1%2%3)").arg(vb).arg(degreeChar).arg(rev );
            }
            else
            {
                brgbuff = QString( "%1%2%3").arg(vb).arg(degreeChar).arg(rev );
            }
            ui->BrgSt->setText(brgbuff);
         }
         else
         {
            ui->DistSt->clear();
            ui->BrgSt->clear();
            sct.contactScore = -1;
            sct.bearing = -1;
         }
   }
}
//---------------------------------------------------------------------------
bool QSOLogFrame::validateControls( validTypes command )   // do control validation
{
   // spin round all controls, and validate them
   // return true if all valid

   for ( QVector <ValidatedControl *>::iterator vcp = vcs.begin(); vcp != vcs.end(); vcp++ )
   {
      if ( !( *vcp ) ->valid( command ) )
         return false;
   }
   return true;
}
//---------------------------------------------------------------------------
bool QSOLogFrame::valid( validTypes command )
{
   lgTraceerr(-1); // clear the error list

   if ( contest->isReadOnly() )
      return true;

   bool pvalid = validateControls( command ); // do control validation

   if ( command == cmCheckValid )   // our own command!
      contactValid();

   // re-validate, as we may have changed things
   pvalid = validateControls( cmValidStatus ); // look at current validity

   return pvalid;
}
//---------------------------------------------------------------------------
void QSOLogFrame::selectField( QWidget *v )
{
    int dtgne = -1;

    if ( catchup )
    {
       dtgne = screenContact.time.notEntered();
       //   0;   // neither entered, will fill in when cs entered
       //   1;   // time, no date
       //   2;   // date, no time
    }

    if ( v == 0 )
    {
       if ( contest->isReadOnly() )
       {
          v = ui->CallsignEdit;
       }
       else
          if ( catchup )
          {
             v = ( ( dtgne != -1 ) && ( dtgne <= 1 ) ) ? ui->DateEdit : ui->TimeEdit;
          }
          else
             if ( screenContact.contactFlags & TO_BE_ENTERED )
                v = ui->TimeEdit;
             else
                v = ui->CallsignEdit;

    }
    if ( !v || ( current == v ) )
    {
       if (v)
          v->setFocus();
       return ;
    }

    if ( ( current == ui->CallsignEdit ) || ( current == ui->LocEdit ) )
    {
       valid( cmCheckValid ); // make sure all single and cross field
       doAutofill();
    }

    if ( v == ui->TimeEdit )
    {
       ( static_cast< QLineEdit * > (v) ) ->setReadOnly(false);
       if (dtgne == 0 || dtgne == 1)
       {
//          TimeEdit->SelStart = 0;
//          TimeEdit->SelLength = 1;
       }
    }
    if ( v == ui->DateEdit )
    {

       ( static_cast< QLineEdit * > ( v) ) ->setReadOnly(false);
       if (dtgne == 0 || dtgne == 2)
       {
//          DateEdit->SelStart = 0;
//          DateEdit->SelLength = 1;
       }
    }
    if ( v == ui->SerTXEdit )
    {
        ( static_cast< QLineEdit * > (v) ) ->setReadOnly(false);
    }
    if ( v->isEnabled() )
    {
       v->setFocus();
       current = v;
    }
    MinosLoggerEvents::SendShowErrorList();

/*
   if ( v == 0 )
   {
      v = ui->CallsignEdit;

   }
   if ( !v || ( current == v ) )
   {
      if (v)
         v->setFocus();
      return ;
   }

   if ( ( current == ui->CallsignEdit ) || ( current == ui->LocEdit ) )
   {
      valid( cmCheckValid ); // make sure all single and cross field
      doAutofill();
   }

   if ( v == ui->SerTXEdit )
   {
      ( ( QLineEdit * ) v ) ->setReadOnly(false);
//      ( ( QLineEdit * ) v ) ->Color = clWindow;
   }
   if ( v->isEnabled() )
   {
      v->setFocus();
      current = v;
   }
   MinosLoggerEvents::SendShowErrorList();
   */
}
//==============================================================================
// check for embedded space or empty number

void QSOLogFrame::fillRst( QLineEdit *rIl, QString &rep, const QString &fmode )
{
   QString newrep;
   if ( current == rIl )
   {
      bool autoFill;
      TContestApp::getContestApp() ->loggerBundle.getBoolProfile( elpAutoFill, autoFill );
      if (autoFill)
      {
         if ( !Validator::validateRST( rIl->text().trimmed() ) )
         {
            if ( ( rep.size() == 1 ) && rep[ 0 ].isNumber() )
            {
               newrep = rep[ 0 ];
               newrep += "9";
               rIl->setText(newrep);
               rep = newrep;
            }
         }
      }
      if ( ( fmode.compare( "A1A", Qt::CaseInsensitive ) == 0 ) && Validator::validateRST( rIl->text().trimmed() )
           && ( rep.size() == 2 ) )
      {
         newrep = rep[ 0 ];
         newrep += rep[ 1 ];
         newrep += "9";
         rIl->setText(newrep);
         rep = newrep;
      }
   }
}
//==============================================================================
void QSOLogFrame::doAutofill( void )
{
   if ( contest->isReadOnly() )
      return ;
   ScreenContact *vcct = &screenContact;

   //rst sent (autofill S9)

   fillRst( ui->RSTTXEdit, vcct->reps, vcct->mode );
   fillRst( ui->RSTRXEdit, vcct->repr, vcct->mode );
}
//==============================================================================
void QSOLogFrame::lgTraceerr( int err )
{
   MinosLoggerEvents::SendValidateError(err);
}
//==============================================================================
void QSOLogFrame::contactValid( void )
{
   // this is where we need to do all of our cross field validation

   getScreenEntry();
   ScreenContact *vcct = &screenContact;

   if ( vcct->contactFlags & DONT_PRINT )
   {
      lgTraceerr(-1);
      lgTraceerr( ERR_26 );
   }
   else
   {
      if ( vcct->contactFlags & ( LOCAL_COMMENT | COMMENT_ONLY ) )
         lgTraceerr( ERR_25 );
      if ( vcct->contactFlags & NON_SCORING )
         lgTraceerr( ERR_24 );
   }
   if ( vcct->contactFlags & ( LOCAL_COMMENT | COMMENT_ONLY | NON_SCORING | DONT_PRINT ) )
   {
      return ;
   }
   // cs worked

   vcct->cs.valRes = CS_NOT_VALIDATED;

   // we only validate this contact up to the validation point of the contest
   contest->validationPoint = selectedContact?selectedContact->getLogSequence():0 ;

   int csret = vcct->cs.validate( );
   contest->DupSheet.clearCurDup();
   if ( csret == CS_OK )
   {
      if ( contest->DupSheet.checkCurDup( vcct, contest->validationPoint, false ) )
      {
         if ( contest->DupSheet.isCurDup( vcct ) )      // But vcct is screen contact... so it won't be curdup
         {
            csret = vcct->cs.valRes = ERR_DUPCS;
         }
      }
   }
   if ( csret != CS_OK )
   {
      csIl->tIfValid = false;
      switch ( csret )
      {
         case CS_NOT_VALIDATED:
            lgTraceerr( ERR_10 );
            break;

         case ERR_NOCS:
            lgTraceerr( ERR_11 );
            break;

         case ERR_DUPCS:
            lgTraceerr( ERR_12 );
            break;

         default:
            lgTraceerr( ERR_13 );
            break;
      }
   }

   // locator received

   int locrep = vcct->loc.validate();
   if ( locrep != 0 )
   {
      if ( contest->locatorField.getValue() )
         locIl->tIfValid = false;
      if ( contest->locatorField.getValue() && ( locrep == ERR_NOLOC ) )
      {
         lgTraceerr( ERR_18 );
      }
      else
         if ( locrep == ERR_LOC_RANGE )
         {
            lgTraceerr( ERR_19 );
         }
   }

   // If multiplier ContestLog, multiplier (e.g. district if needed)
   // check anyway, this keeps country and loc charts up to date

   vcct->checkScreenContact( );  // QSOLogFrame::contactValid, check multiplier, don't log it yet!

   if (vcct->ctryMult)
   {

   // and look up in squares list for country
   // look for square against main prefix in LocSquares.ini

      QString sloc = vcct->loc.loc.getValue().left(4);
      if (sloc.size())
      {
         bool LocOK;

         QString prefix = vcct->ctryMult->basePrefix;
         TContestApp::getContestApp() ->locsBundle.openSection(prefix);
         if (TContestApp::getContestApp() ->locsBundle.isSectionPresent() )
         {
            TContestApp::getContestApp() ->locsBundle.getBoolProfile( sloc, LocOK, false );
            if (!LocOK)
            {
               lgTraceerr( ERR_15 );
               locIl->tIfValid = false;
            }
         }
      }
   }
   if ( contest->districtMult.getValue() && !vcct->screenQSOValid )
   {
      // no district when required
      // No CS means we should go to QTH, as its likely to be needed
      if ( csret == ERR_NOCS || ( vcct->ctryMult && vcct->ctryMult->hasDistricts() && !vcct->districtMult ) )
      {
         lgTraceerr( ERR_20 );    // "Invalid district multiplier"
         qthIl->tIfValid = false;
      }
      // What do we allow as valid? Should we error wrong postcode/country combination?
   }
   else
      if ( contest->otherExchange .getValue() && !contest->districtMult.getValue() && ( vcct->extraText.trimmed().size() == 0 ) )
      {
         // no QTH info if required

         lgTraceerr( ERR_21 );            // QTH required
         qthIl->tIfValid = false;
      }
}

//---------------------------------------------------------------------------
bool QSOLogFrame::checkLogEntry(bool checkDTG)
{

   if ( contest->isReadOnly() )
      return true;

   // check if the screen contact and selected log contact differ
   bool retval = true;
   getScreenEntry();
   QSharedPointer<BaseContact> sct = selectedContact ;
   if ( sct->ne( screenContact, checkDTG ) )
   {
      bool mresp = true;

      // Dont check with op if not entered, and e.g. ESC pressed
      // Also allows for partial saving when in Uri mode
      if ( !( screenContact.contactFlags & TO_BE_ENTERED ) && !catchup )
      {
         mresp = mShowYesNoMessage( this,
                             "This Contact has changed: Shall I log the changes?\n"
                             "\n"
                             "Yes         - Log as shown\n"
                             "No          - Discard changes"
                              );
      }
      if ( mresp )
      {
         //Yes - log and continue
         logScreenEntry( );
         retval = true;
      }
      else
      {
         //Cancel - Discard changes, continue action
         screenContact.copyFromArg( selectedContact );  // we have to ACTUALLY revert, as the action may not conmplete
         showScreenEntry();
         retval = false;	// stay where we are
      }
   }
   return retval;
}

void QSOLogFrame::setMode( QString m )
{
   ui->ModeComboBoxGJV->setCurrentText(m);
   // make sure the mode button shows the correct "flip" value
   if (ui->ModeComboBoxGJV->currentText() == "A1A")
   {
      ui->ModeButton->setText("J3E");
   }
   else
   {
      ui->ModeButton->setText("A1A");
   }
}

void QSOLogFrame::clearCurrentField()
{
   current = 0;
}
//---------------------------------------------------------------------------
void QSOLogFrame::updateQSODisplay()
{
   if ( contest->districtMult.getValue() )
   {
//      ui->QTHEdit->CharCase = ecUpperCase;
   }
   else
   {
//      ui->QTHEdit->CharCase = ecNormal;
   }
   ui->RSTTXEdit->setEnabled(!contest->isReadOnly());
   ui->SerTXEdit->setEnabled(!contest->isReadOnly());
   ui->RSTRXEdit->setEnabled(!contest->isReadOnly());
   ui->SerRXEdit->setEnabled(!contest->isReadOnly());
   //CallsignEdit->Enabled = false;
   //LocEdit->Enabled = false;
   //QTHEdit->Enabled = false;
   ui->CommentsEdit->setEnabled(!contest->isReadOnly());
   ui->ModeComboBoxGJV->setEnabled(!contest->isReadOnly());
   ui->NonScoreCheckBox->setEnabled(!contest->isReadOnly());
   ui->DeletedCheckBox->setEnabled(!contest->isReadOnly());
   ui->GJVOKButton->setEnabled(!contest->isReadOnly());
   ui->GJVForceButton->setEnabled(!contest->isReadOnly());

   ui->QTHEdit->setEnabled( contest->otherExchange .getValue() || contest->districtMult.getValue() );

   ui->ModeButton->setEnabled(!contest->isReadOnly());
   ui->SecondOpComboBox->setEnabled(!contest->isReadOnly());
   ui->MainOpComboBox->setEnabled(!contest->isReadOnly());

   ui->InsertBeforeButton->setEnabled(!contest->isReadOnly());
   ui->InsertAfterButton->setEnabled(!contest->isReadOnly());
}

//---------------------------------------------------------------------------

void QSOLogFrame::refreshOps()
{
   // refill the op combo boxes from the current contest, and select the correct op

   if (contest)
   {

       QString mainOp = contest->currentOp1.getValue();
       QString secondOp = contest->currentOp2.getValue();
//       QString mainOp = ui->MainOpComboBox->currentText();
//       QString secondOp = ui->SecondOpComboBox->currentText();

       ui->MainOpComboBox->clear();
       ui->SecondOpComboBox->clear();

       QStringList ops;
       for ( OperatorIterator i = contest->oplist.begin(); i != contest->oplist.end(); i++ )
       {
           if (!(*i).isEmpty())
             ops.append(*i);
       }
       ops.append(mainOp);
       ops.append(secondOp);

       ops.append("");

       ops.sort();
       ops.removeDuplicates();

       ui->MainOpComboBox->addItems(ops);
       ui->SecondOpComboBox->addItems(ops);

       ui->MainOpComboBox->setCurrentText(mainOp);
       ui->SecondOpComboBox->setCurrentText(secondOp);

   }
}
void QSOLogFrame::refreshOps( ScreenContact &screenContact )
{
    if (contest)
    {
        QString mainOp = screenContact.op1;
        QString secondOp = screenContact.op2;

        ui->MainOpComboBox->clear();
        ui->SecondOpComboBox->clear();

        BaseContestLog * contest = TContestApp::getContestApp() ->getCurrentContest();
        QStringList ops;
        for ( OperatorIterator i = contest->oplist.begin(); i != contest->oplist.end(); i++ )
        {
            if (!(*i).isEmpty())
              ops.append(*i);
        }
        ops.append(mainOp);
        ops.append(secondOp);

        ops.append("");

        ops.sort();
        ops.removeDuplicates();

        ui->MainOpComboBox->addItems(ops);
        ui->SecondOpComboBox->addItems(ops);

        ui->MainOpComboBox->setCurrentText(mainOp);
        ui->SecondOpComboBox->setCurrentText(secondOp);

       // and if this is the last contact, the ops should also propogate into the contest
       // for the NEXT contact
    }
}
void QSOLogFrame::on_ShowOperators ( )
{
   bool so = LogContainer->isShowOperators();
   ui->SecondOpComboBox->setVisible(so);
   ui->SecondOpLabel->setVisible(so);
   ui->MainOpComboBox->setVisible(so);
   ui->OperatorLabel->setVisible(so);
}
//---------------------------------------------------------------------------
void QSOLogFrame::closeContest()
{
   BaseContestLog * currentContest = TContestApp::getContestApp() ->getCurrentContest();
   if (contest == currentContest)
   {
      ui->GJVCancelButton->setEnabled(true);
      ui->GJVCancelButton->setFocus();
   }
   contest = 0;
}
//---------------------------------------------------------------------------
void QSOLogFrame::doGJVEditChange( QObject *Sender )
{
   // sensitive field changed - trigger match scan
   if ( contest )
   {
      getScreenEntry();

      if ( current == ui->CallsignEdit || Sender == ui->CallsignEdit )
      {
         // clear the error list
         contest->DupSheet.clearCurDup();	// as edited, no longer a dup(?)

         if ( ( current == ui->CallsignEdit ))
         {
            valid( cmCheckValid ); // make sure all single and cross field

            if (screenContact.cs.valRes == ERR_DUPCS)
            {
                QString ss("QLineEdit { background-color: red ; border-style: outset ; border-width: 1px ; border-color: black ; }");
                ui->CallsignEdit->setStyleSheet(ss);
            }
            else
            {
                QString ss("QLineEdit { background-color: white ; border-style: outset ; border-width: 1px ; border-color: black ; }");
                ui->CallsignEdit->setStyleSheet(ss);
            }
         }

      }
      if ( current == ui->LocEdit || Sender == ui->LocEdit )
      {
         // force bearing calc
         calcLoc();
      }
      MinosLoggerEvents::SendScreenContactChanged(&screenContact, contest);
   }
}

void QSOLogFrame::on_ModeButton_clicked()
{
    ui->ModeComboBoxGJV->setCurrentText(ui->ModeButton->text());
    EditControlExit(ui->ModeButton);
}
void QSOLogFrame::logScreenEntry( )
{
   if (!contest || contest->isReadOnly() )
      return ;

   int ctmax = screenContact.serials.toInt();

   if ( ctmax > contest->maxSerial )
      contest->maxSerial = ctmax;

   LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );
   if ( !ct )
   {
      return ;
   }
   QSharedPointer<BaseContact> lct = selectedContact;
   if (!lct)
   {
        lct = ct->addContact( ctmax, 0, false, false );	// "current" doesn't get flag, don't save ContestLog yet
   }

   bool contactmodeCW = ( screenContact.reps.size() == 3 && screenContact.repr.size() == 3 );
   bool curmodeCW = ( screenContact.mode.compare( "A1A", Qt::CaseInsensitive ) == 0 );

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
   ct->mode.setValue( screenContact.mode );
   screenContact.op1 = ct->currentOp1.getValue() ;
   screenContact.op2 = ct->currentOp2.getValue();

   lct->copyFromArg( screenContact );
   lct->time.setDirty(); // As we may have created the contact with the same time as the screen contact
                         // This then becomes "not dirty", so we end up not saving the dtg.
                         // But this only happens when seconds are :00, as the main log
                         // is only to a minute resolution

   lct->commonSave(lct);				// which also saves the ContestLog

   killPartial();

   MinosLoggerEvents::SendAfterLogContact(ct);
   startNextEntry( );	// select the "next"
}
//---------------------------------------------------------------------------
void QSOLogFrame::getScreenContactTime()
{
   updateQSOTime();
   screenContact.time.setDate( ui->DateEdit->text(), DTGDISP );
   screenContact.time.setTime( ui->TimeEdit->text(), DTGDISP );
}
//---------------------------------------------------------------------------
void QSOLogFrame::showScreenContactTime( ScreenContact &temp )
{
   // display the contents of the contest->screenContact

   ui->DateEdit->setText(temp.time.getDate( DTGDISP ));
   ui->TimeEdit->setText(temp.time.getTime( DTGDISP ));
}
//==============================================================================
void QSOLogFrame::logCurrentContact( )
{
   if (!contest || contest->isReadOnly() )
      return ;

   // copy the display into the correct log entry, etc

   getScreenEntry();

   // first check for extra lines to be added
   int ctno = screenContact.serials.toInt();
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
            ct->addContact( nct_no, orflag, true, false ); // last contact
            nct_no++;
         }
         while ( nct_no < ctno ) ;
      }
      else
      {
         // reset focus to serial sent
         //????????????? But its disabled...
         selectField( ui->SerTXEdit );
         return ;
      }
   }

   logScreenEntry( );  // true => move on to next contact
}
void QSOLogFrame::updateQSOTime(bool fromTimer)
{
    // Check if dtg is within the contest time
    // If not we wish to show as red
    // We need to do this in log displays as well
    if (!edit)
    {
        dtg tnow( true );
        ui->DateEdit->setText(tnow.getDate( DTGDISP ));
        QString t = tnow.getTime( DTGDISP );
        ui->TimeEdit->setText(t);
    }
    dtg time(false);
    time.setDate( ui->DateEdit->text(), DTGDISP );
    time.setTime( ui->TimeEdit->text().left( 5), DTGDISP );

    bool timeOK = false;

    if (contest)
    {
        timeOK = contest->checkTime(time);
    }

    if (fromTimer && timeOK != oldTimeOK)
    {
        oldTimeOK = timeOK;
        if (timeOK)
        {
            QString ss("QLineEdit { background-color: white ; border: none ; color: black ; }");
            ui->DateEdit->setStyleSheet(ss);
            ui->TimeEdit->setStyleSheet(ss);
        }
        else
        {
            QString ss("QLineEdit { background-color: white ; border: none ; color: red ; }");
            ui->DateEdit->setStyleSheet(ss);
            ui->TimeEdit->setStyleSheet(ss);
        }
    }
    ui->bandMapFrame->setVisible( LogContainer->isBandMapLoaded());

    ui->Rotate->setVisible(LogContainer->isRotatorLoaded());
    ui->RotateLeft->setVisible(LogContainer->isRotatorLoaded());
    ui->RotateRight->setVisible(LogContainer->isRotatorLoaded());
    ui->StopRotate->setVisible(LogContainer->isRotatorLoaded());
    ui->rotatorState->setVisible(LogContainer->isRotatorLoaded());
}

void QSOLogFrame::transferDetails(const QSharedPointer<BaseContact> lct, const BaseContestLog *matct )
{
   ui->CallsignEdit->setText(lct->cs.fullCall.getValue());
   ui->LocEdit->setText(lct->loc.loc.getValue());  // also forces update of score etc

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
            QString exch = lct->extraText.getValue();
            if (exch.size())
            {
               ui->QTHEdit->setText(exch);
            }
         }
      }
   }
   valid( cmCheckValid ); // make sure all single and cross field
   // validation has been done

   // make sure dtg gets entered if needed
   ui->CallsignEdit->setFocus();
   doAutofill();
   ui->CallsignEdit->setFocus();

   doGJVEditChange(ui->CallsignEdit);
   doGJVEditChange(ui->LocEdit);
   doGJVEditChange(ui->QTHEdit);
}
void QSOLogFrame::transferDetails( const ListContact *lct, const ContactList * /*matct*/ )
{
   ui->CallsignEdit->setText(lct->cs.fullCall.getValue());
   ui->LocEdit->setText(lct->loc.loc.getValue());

   // only transfer qth info if required for this ContestLog
   // and it might be valid...
   if ( contest->districtMult.getValue() || contest->otherExchange.getValue() )
   {
      if ( contest->districtMult.getValue() || contest->otherExchange.getValue() )
      {
         if ( contest->QTHField.getValue() )
         {
            QString exch = lct->extraText;
            if (exch.size())
            {
               ui->QTHEdit->setText(exch);
            }
         }
      }
   }
   valid( cmCheckValid ); // make sure all single and cross field
   // validation has been done

   // make sure dtg gets entered if needed
   ui->CallsignEdit->setFocus();
   doAutofill();
   ui->CallsignEdit->setFocus();

   doGJVEditChange(ui->CallsignEdit);
   doGJVEditChange(ui->LocEdit);
   doGJVEditChange(ui->QTHEdit);
}
void QSOLogFrame::selectEntry( QSharedPointer<BaseContact> slct )
{
   selectedContact = slct;   // contact from log list selected

   ui->FirstUnfilledButton->setVisible(false);
   ui->CatchupButton->setVisible(false);

   screenContact.copyFromArg( slct );
   showScreenEntry();

   ui->PriorButton->setEnabled(getPriorContact());
   ui->NextButton->setEnabled(getNextContact());
   ui->InsertAfterButton->setEnabled(getNextContact() && !contest->isReadOnly());  // dont allow insert after last contact
   ui->InsertBeforeButton->setEnabled(getPriorContact() && !contest->isReadOnly());  // dont allow insert after last contact

   ui->MainOpComboBox->setCurrentText(screenContact.op1);
   ui->SecondOpComboBox->setCurrentText(screenContact.op2);
   if ( !contest->isReadOnly() && ((screenContact.contactFlags & TO_BE_ENTERED) || catchup))
   {
      // Uri Mode - catchuping QSOs from paper while logging current QSOs
      // and we need to set the date/time from the previous contact
      ui->TimeEdit->setReadOnly(false);
      ui->DateEdit->setReadOnly(false);

      int tne = screenContact.time.notEntered(); // partial dtg will give +fe
      // full dtg gives -ve, none gives 0
      if ( tne == 0 )
      {
         QSharedPointer<BaseContact> pct = getPriorContact();
         if ( pct )
         {
            screenContact.time = pct->time;
            ui->DateEdit->setText(screenContact.time.getDate( DTGDISP ));
            ui->TimeEdit->setText(screenContact.time.getTime( DTGDISP ));
         }
         else
         {
            // use contest start time
            QDateTime DTGStart = CanonicalToTDT(contest->DTGStart.getValue());
            ui->DateEdit->setText(DTGStart.toString("dd/MM/yy"));
            ui->TimeEdit->setText(DTGStart.toString("hh:mm"));
            dtg time(false);
            time.setDate( ui->DateEdit->text(), DTGDISP );
            time.setTime( ui->TimeEdit->text().left(5), DTGDISP );
            screenContact.time = time;
         }
         int selpt = ui->DateEdit->text().length();
         if ( selpt > 2 )
         {
            ui->DateEdit->setSelection(1, 1);
         }

         selpt = ui->TimeEdit->text().length();
         if ( selpt > 0 )
         {
             ui->DateEdit->setSelection(selpt - 1, 1);
         }
      }
   }
   else
   {
      ui->TimeEdit->setReadOnly(true);
      ui->DateEdit->setReadOnly(true);
   }
   ui->SerTXEdit->setReadOnly(true);

   bool timeOK = false;
   int tne = screenContact.time.notEntered(); // partial dtg will give +fe
   // full dtg gives -ve, none gives 0

   if (tne < 0)
   {
      dtg time(false);
      time.setDate( ui->DateEdit->text(), DTGDISP );
      time.setTime( ui->TimeEdit->text().left( 5), DTGDISP );

      timeOK = contest->checkTime(time);
   }

   if (timeOK)
   {
//      DateEdit->Font->Color = clWindowText;
//      TimeEdit->Font->Color = clWindowText;
   }
   else
   {
//      DateEdit->Font->Color = clRed;
//      TimeEdit->Font->Color = clRed;
   }

   MinosLoggerEvents::SendAfterSelectContact(catchup?QSharedPointer<BaseContact>():slct, contest);
   selectField( 0 );
}
//---------------------------------------------------------------------------
QSharedPointer<BaseContact> QSOLogFrame::getPriorContact()
{
   for ( LogIterator i = contest->ctList.begin(); i != contest->ctList.end(); i++ )
   {
      if ( i->wt->getLogSequence() == screenContact.getLogSequence() )
      {
         if ( i != contest->ctList.begin() )
         {
            i--;
            return ( i->wt ) ;
         }
         return QSharedPointer<BaseContact>();
      }
   }
   return QSharedPointer<BaseContact>();
}


void QSOLogFrame::on_PriorButton_clicked()
{
   current = 0;            // make sure the focus moves off this button
   if ( !checkLogEntry(true) )
   {
      return ;
   }
   QSharedPointer<BaseContact> lct = getPriorContact();
   if ( lct )
   {
      selectEntry( lct );
   }
   else
   {
      mShowMessage( "Start of QSOs", this );
   }
}
//---------------------------------------------------------------------------
QSharedPointer<BaseContact> QSOLogFrame::getNextContact()
{
   for ( LogIterator i = contest->ctList.begin(); i != contest->ctList.end(); i++ )
   {
      if ( i->wt->getLogSequence() == screenContact.getLogSequence() )
      {
         i++;
         if ( i != contest->ctList.end() )
         {
            return i->wt;
         }
         return QSharedPointer<BaseContact>();
      }
   }
   return QSharedPointer<BaseContact>();
}

void QSOLogFrame::on_NextButton_clicked()
{
   current = 0;            // make sure the focus moves off this button
   if ( !checkLogEntry(true) )
   {
      return ;
   }
   QSharedPointer<BaseContact> lct = getNextContact();
   if ( lct )
   {
      selectEntry( lct );
   }
   else
   {
      mShowMessage( "End of QSOs", this );
   }
}

void QSOLogFrame::on_InsertBeforeButton_clicked()
{
    QSharedPointer<BaseContact> pct = getPriorContact();
    LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );
    QSharedPointer<BaseContact> newct = ct->addContactBetween(pct, selectedContact);
    newct->contactFlags.setValue(newct->contactFlags.getValue()|TO_BE_ENTERED);
    selectEntry(newct);
}

void QSOLogFrame::on_InsertAfterButton_clicked()
{
    QSharedPointer<BaseContact> nct = getNextContact();
    LoggerContestLog *ct = dynamic_cast<LoggerContestLog *>( contest );
    QSharedPointer<BaseContact> newct = ct->addContactBetween(selectedContact, nct);
    newct->contactFlags.setValue(newct->contactFlags.getValue()|TO_BE_ENTERED);
    selectEntry(newct);
}

void QSOLogFrame::on_ModeComboBoxGJV_currentIndexChanged(int /*index*/)
{
    if (ui->ModeComboBoxGJV->currentText() == "A1A")
    {
       ui->ModeButton->setText("J3E");
    }
    else
    {
       ui->ModeButton->setText("A1A");
    }
}
void QSOLogFrame::on_ValidateError (int mess_no )
{
      if ( mess_no == -1 )
      {
         errs.clear();
         return ;
      }

      // add the message into the error list
      errs.insert( &errDefs[ mess_no ], &errDefs[ mess_no ] );
}
int QSOLogFrame::getAngle()
{
    QString brgSt = ui->BrgSt->text();

    int brg = 0;

    for (int i = 0;i < brgSt.length(); i++)
    {
        if (brgSt[i].isDigit())
        {
            int j = 0;
            for (j = i; j < brgSt.length(); j++)
            {
                if (!brgSt[j].isDigit())
                {
                    break;
                }
            }
            brgSt = brgSt.mid(i, j - i);
            brg = brgSt.toInt();
            return brg;
        }
    }
    return brg;
}

void QSOLogFrame::on_RotateLeft_clicked()
{
    TSendDM::sendRotator(rpcConstants::eRotateLeft, getAngle());
}

void QSOLogFrame::on_Rotate_clicked()
{
    TSendDM::sendRotator(rpcConstants::eRotateDirect, getAngle());
}

void QSOLogFrame::on_RotateRight_clicked()
{
    TSendDM::sendRotator(rpcConstants::eRotateRight, getAngle());
}

void QSOLogFrame::on_StopRotate_clicked()
{
    TSendDM::sendRotator(rpcConstants::eRotateStop, 0);
}
void QSOLogFrame::setRotatorState(const QString &s)
{
    ui->rotatorState->setText(s);
}
