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

#include "base_pch.h"
//---------------------------------------------------------------------------
enum TAlignment { taLeftJustify, taRightJustify, taCenter };
class GridColumn
{
   public:
      int fieldId;
      QString width;
      QString title;
      TAlignment alignment;
      GridColumn( int fieldId, QString width, QString title, TAlignment alignment ) :
            fieldId( fieldId ), width( width ), title( title ), alignment( alignment )
      {}
};

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

class QSOGridModel: public QAbstractItemModel
{
    protected:
        BaseContestLog *contest;
    public:
        QSOGridModel();
        ~QSOGridModel();

        void reset();
        void initialise( BaseContestLog * pcontest );
        QVariant data( const QModelIndex &index, int role ) const Q_DECL_OVERRIDE;
        QVariant headerData( int section, Qt::Orientation orientation,
                             int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;
        QModelIndex index( int row, int column,
                           const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
        QModelIndex parent( const QModelIndex &index ) const Q_DECL_OVERRIDE;

        int rowCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
        int columnCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
};

#endif
