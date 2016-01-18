#include "logger_pch.h"

#include "dxccframe.h"
#include "ui_dxccframe.h"

#include "tsinglelogframe.h"

extern ContList contlist[ CONTINENTS ];

static GridColumn CountryTreeColumns[ ectMultMaxCol ] =
   {
      GridColumn( ectCall, "XXXXXX", "Call", taLeftJustify ),
      GridColumn( ectWorked, "Wk CtX", "Wkd", taCenter ),
      GridColumn( ectLocator, "MM00MM00", "Locator", taLeftJustify ),
      GridColumn( ectBearing, "BRGXXX", "brg", taCenter ),
      GridColumn( ectName, "This is a very long country", "Country", taLeftJustify ),
      GridColumn( ectOtherCalls, "This is a very very very very long country name", "Other calls", taLeftJustify /*taRightJustify*/ )
   };

DXCCFrame::DXCCFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DXCCFrame)
{
    ui->setupUi(this);
}

DXCCFrame::~DXCCFrame()
{
    delete ui;
}
void DXCCFrame::setContest(BaseContestLog *contest)
{
    model.ct = contest;
    proxyModel.setSourceModel(&model);
    ui->DXCCTable->setModel(&proxyModel);
    reInitialiseCountries();
    connect( ui->DXCCTable->horizontalHeader(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));
}
void DXCCFrame::reInitialiseCountries()
{
    model.reset();
    QSettings settings;
    QByteArray state;

    state = settings.value("DXCCTable/state").toByteArray();
    ui->DXCCTable->horizontalHeader()->restoreState(state);
}
void DXCCFrame::scrollToCountry( int ctry_ind, bool makeVisible )
{
    if (makeVisible)
        proxyModel.scrolledCountry = ctry_ind;
    else
        proxyModel.scrolledCountry = -1;
   reInitialiseCountries();
}

void DXCCFrame::on_sectionResized(int, int , int)
{
    QSettings settings;
    QByteArray state;

    state = ui->DXCCTable->horizontalHeader()->saveState();
    settings.setValue("DXCCTable/state", state);
}

DXCCGridModel::DXCCGridModel()
{}
DXCCGridModel::~DXCCGridModel()
{
}
void DXCCGridModel::reset()
{
    beginResetModel();

    endResetModel();
}

void DXCCGridModel::initialise( )
{
   beginResetModel();

   // pick up the correct DXCC list

   endResetModel();
}
QVariant DXCCGridModel::data( const QModelIndex &index, int role ) const
{

    if (role == Qt::BackgroundRole)
    {
        return QVariant();
    }
    if ( role != Qt::DisplayRole && role != Qt::EditRole )
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        QString disp = MultLists::getMultLists() ->getCtryListText( index.row(), CountryTreeColumns[ index.column() ].fieldId, ct );
        return disp;
    }
    return QVariant();
}
QVariant DXCCGridModel::headerData( int section, Qt::Orientation orientation,
                     int role ) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        QString cell;

        cell = CountryTreeColumns[section].title;

        return cell;
    }
    return QVariant();
}

QModelIndex DXCCGridModel::index( int row, int column, const QModelIndex &parent) const
{
    if ( row < 0 || row >= rowCount()  )
        return QModelIndex();

    return createIndex( row, column );
}

QModelIndex DXCCGridModel::parent( const QModelIndex &index ) const
{
    return QModelIndex();
}

int DXCCGridModel::rowCount( const QModelIndex &parent ) const
{
    return MultLists::getMultLists() ->getCtryListSize();
}

int DXCCGridModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return ectMultMaxCol;
}
bool DXCCSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (scrolledCountry == sourceRow)
        return true;
    BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
    int worked = MultLists::getMultLists()->getCountryWorked(sourceRow, ct) ;

    CountryEntry * ce = MultLists::getMultLists() ->getCtryListAt( sourceRow );
    bool makeVisible = false;
    for ( int i = 0; i < CONTINENTS; i++ )
    {
        if ( ce->continent == contlist[ i ].continent )
        {
            makeVisible = contlist[ i ].allow;
            break;
        }
    }

    if ( worked && showUnworked && !showWorked )
    {
        makeVisible = false;
    }
    else
        if ( !worked && !showUnworked && showWorked )
        {
            makeVisible = false;
        }


    return makeVisible;
}
