/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MultDispH
#define MultDispH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "VirtualTrees.hpp"
#include <ExtCtrls.hpp>
#include "StatsDisp.h"
#include "UWhisperSplitter.hpp"
#include <Graphics.hpp>
#include <ComCtrls.hpp>
#include "CompDisp.h"
//---------------------------------------------------------------------------
class TMultDispFrame : public TFrame
{
   __published:  	// IDE-managed Components
      TPageControl *MultPages;
      TTabSheet *CountriesTab;
      TTabSheet *DistrictTab;
      TTabSheet *LocatorsTab;
      TVirtualStringTree *CountryMultTree;
      TVirtualStringTree *DistrictMultTree;
      TVirtualStringTree *LocatorMultTree;
      TTabSheet *StatsTab;
      TStatsDispFrame *StatsDispFrame;
      TTabSheet *ErrTab;
      TListBox *ErrList;
      TPanel *FilterPanel;
      TSplitter *FilterSplitter;
      TCheckBox *UnworkedCB;
      TCheckBox *ContEU;
      TCheckBox *ContAS;
      TCheckBox *ContAF;
      TCheckBox *ContOC;
      TCheckBox *ContSA;
      TCheckBox *ContNA;
      TTimer *FilterTimer;
      TCheckBox *WorkedCB;
   TImage *Splitter_Image;
   TTabSheet *CompSheet;
   TCompFrame *CompFrame;
   TTabSheet *KeyerTab;
   TButton *Button1;
   TButton *Button2;
   TButton *Button3;
      void __fastcall CountryMultTreeGetNodeDataSize( TBaseVirtualTree *Sender,
            int &NodeDataSize );
      void __fastcall CountryMultTreeGetText( TBaseVirtualTree *Sender,
                                              PVirtualNode Node, TColumnIndex Column, TVSTTextType TextType,
                                              WideString &CellText );
      void __fastcall DistrictMultTreeGetText( TBaseVirtualTree *Sender,
            PVirtualNode Node, TColumnIndex Column, TVSTTextType TextType,
            WideString &CellText );
      void __fastcall DistrictMultTreeGetNodeDataSize(
         TBaseVirtualTree *Sender, int &NodeDataSize );
      void __fastcall LocatorMultTreeGetNodeDataSize( TBaseVirtualTree *Sender,
            int &NodeDataSize );
      void __fastcall CountryMultTreeInitNode( TBaseVirtualTree *Sender,
            PVirtualNode ParentNode, PVirtualNode Node,
            TVirtualNodeInitStates &InitialStates );
      void __fastcall DistrictMultTreeInitNode( TBaseVirtualTree *Sender,
            PVirtualNode ParentNode, PVirtualNode Node,
            TVirtualNodeInitStates &InitialStates );
      void __fastcall LocatorMultTreeInitNode( TBaseVirtualTree *Sender,
            PVirtualNode ParentNode, PVirtualNode Node,
            TVirtualNodeInitStates &InitialStates );
      void __fastcall LocatorMultTreeInitChildren( TBaseVirtualTree *Sender,
            PVirtualNode Node, DWORD &ChildCount );
      void __fastcall CountryMultTreeInitChildren( TBaseVirtualTree *Sender,
            PVirtualNode Node, DWORD &ChildCount );
      void __fastcall DistrictMultTreeInitChildren( TBaseVirtualTree *Sender,
            PVirtualNode Node, DWORD &ChildCount );
      void __fastcall LocatorMultTreeGetText( TBaseVirtualTree *Sender,
                                              PVirtualNode Node, TColumnIndex Column, TVSTTextType TextType,
                                              WideString &CellText );
      void __fastcall FilterClick( TObject *Sender );
      void __fastcall FilterTimerTimer( TObject *Sender );
   void __fastcall FilterSplitterMoved(TObject *Sender);
   void __fastcall CountryMultTreeMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
   void __fastcall DistrictMultTreeMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
   void __fastcall LocatorMultTreeMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
   private:  	// User declarations
      MinosEventListener  EL_ShowErrorList;
      void ShowErrorList_Event ( MinosEventBase & Event );

      MinosEventListener  EL_ValidateError;
      void ValidateError_Event ( MinosEventBase & Event );
      ErrorList errs;

      BaseContestLog *ct;
      TWhisperSplitter *WFilterSplitter;
      void initFilters();
      bool filterClickEnabled;
      void saveFilters();
      void reInitialiseCountries();
      void reInitialiseDistricts();
      void reInitialiseLocators();
      void reInitialiseStats();
      void reInitialiseComp();
   public:  		// User declarations
      __fastcall TMultDispFrame( TComponent* Owner );
      void refreshMults();
      void scrollToDistrict( unsigned int district_ind, bool makeVisible );
      void scrollToCountry( unsigned int ctry_ind, bool makeVisible );
      void setContest( BaseContestLog *ct );
      void showErrorList( ErrorList &errs );
};
//---------------------------------------------------------------------------
#endif
