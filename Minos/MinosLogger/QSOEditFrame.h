/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#ifndef QSOEditFrameH
#define QSOEditFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "QSOFrame.h"
#include <ExtCtrls.hpp>
#include "BundleFrame.h"
//---------------------------------------------------------------------------
class TGJVQSOEditFrame : public TGJVEditFrame
{
   __published: 	// IDE-managed Components
      TButton *InsertBeforeButton;
      TButton *InsertAfterButton;
      TButton *PriorButton;
      TButton *NextButton;
      TTimer *ROFieldTimer;
   TLabeledEdit *TimeEdit;
   TLabeledEdit *DateEdit;
      void __fastcall InsertBeforeButtonClick( TObject *Sender );
      void __fastcall InsertAfterButtonClick( TObject *Sender );
      void __fastcall FirstUnfilledButtonClick( TObject *Sender );
      void __fastcall PriorButtonClick( TObject *Sender );
      void __fastcall NextButtonClick( TObject *Sender );
      void __fastcall ROFieldTimerTimer( TObject *Sender );
   void __fastcall MainOpComboBoxExit(TObject *Sender);
   void __fastcall SecondOpComboBoxExit(TObject *Sender);
   void __fastcall TimeEditDblClick(TObject *Sender);
   void __fastcall DateEditDblClick(TObject *Sender);
   void __fastcall TimeEditKeyPress(TObject *Sender, char &Key);
   void __fastcall DateEditChange(TObject *Sender);
   void __fastcall TimeEditChange(TObject *Sender);
   protected:
      virtual void getScreenContactTime();
      virtual void showScreenContactTime( ScreenContact &);
      virtual void checkTimeEditEnter(TLabeledEdit *tle, bool &ovr);
      virtual void checkTimeEditExit();
      virtual bool isTimeEdit(TLabeledEdit *tle);
      virtual void setDTGNotValid(ScreenContact *vcct);
   private: 	// User declarations
      ValidatedControl *dateIl;
      ValidatedControl *timeIl;
      void killPartial( void );
      void doGJVEditChange( TObject *Sender );
      void logCurrentContact( );
      void logScreenEntry( );
      BaseContact *getPriorContact();
      BaseContact *getNextContact();
   public: 		// User declarations
      __fastcall TGJVQSOEditFrame( TComponent* Owner );
      virtual void selectEntry( BaseContact *lct );
      virtual void initialise( BaseContestLog * contest, bool catchup );
      bool doGJVOKButtonClick( TObject *Sender );
      void doGJVCancelButtonClick( TObject *Sender );
      virtual void updateQSOTime(bool fromTimer = false);
      virtual void updateQSODisplay();
      virtual void selectField( TWinControl *v );
};
//---------------------------------------------------------------------------
#endif
