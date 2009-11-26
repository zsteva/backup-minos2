/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef TreeUtilsH
#define TreeUtilsH
//---------------------------------------------------------------------------
class GridColumn
{
   public:
      int fieldId;
      String width;
      String title;
      Classes::TAlignment alignment;
      GridColumn( int fieldId, String width, String title, Classes::TAlignment alignment ) :
            fieldId( fieldId ), width( width ), title( title ), alignment( alignment )
      {}
}
;
//---------------------------------------------------------------------------
enum eLogGridCols {egTime,
                   egCall,
                   egRSTTx, egSNTx, egRSTRx, egSNRx,
                   egLoc, egBrg, egScore,
                   egExchange, egComments,
                   egLogMaxCol
                  };
//---------------------------------------------------------------------------
#define LOGTREECOLS egLogMaxCol
#define THISMATCHTREECOLS egLogMaxCol
#define OTHERMATCHTREECOLS egLogMaxCol - 5
#define ARCHIVEMATCHTREECOLS egLogMaxCol - 5
extern GridColumn QSOTreeColumns[ LOGTREECOLS ];

#endif
