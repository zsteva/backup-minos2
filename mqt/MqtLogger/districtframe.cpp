#include "logger_pch.h"

#include "districtframe.h"
#include "ui_districtframe.h"

#include "tsinglelogframe.h"

static GridColumn DistrictTreeColumns[ ectMultMaxCol - 1 ] =
   {
      GridColumn( ectCall, "XXXXXXX", "Code", taLeftJustify ),
      GridColumn( ectWorked, "Wk CtX", "Wkd", taCenter ),
      GridColumn( ectLocator, "MM00MM00", "Locator", taLeftJustify ),
      GridColumn( ectBearing, "BRGXXX", "brg", taCenter ),
      GridColumn( ectName, "This is a Very Very long District", "District", taLeftJustify ),
   };

DistrictFrame::DistrictFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DistrictFrame)
{
    ui->setupUi(this);
}

DistrictFrame::~DistrictFrame()
{
    delete ui;
}
void DistrictFrame::setContest(BaseContestLog *contest)
{
    model.ct = contest;
    proxyModel.setSourceModel(&model);
    ui->DistrictTable->setModel(&proxyModel);
    reInitialiseDistricts();
    connect( ui->DistrictTable->horizontalHeader(), SIGNAL(sectionResized(int, int , int)),
             this, SLOT( on_sectionResized(int, int , int)));
}
void DistrictFrame::reInitialiseDistricts()
{
    model.reset();
    QSettings settings;
    QByteArray state;

    state = settings.value("DistrictTable/state").toByteArray();
    ui->DistrictTable->horizontalHeader()->restoreState(state);
}
void DistrictFrame::scrollToDistrict( int district_ind, bool makeVisible )
{
    if (makeVisible)
        proxyModel.scrolledDistrict = district_ind;
    else
        proxyModel.scrolledDistrict = -1;
   reInitialiseDistricts();
}
void DistrictFrame::on_sectionResized(int, int , int)
{
    QSettings settings;
    QByteArray state;

    state = ui->DistrictTable->horizontalHeader()->saveState();
    settings.setValue("DistrictTable/state", state);
}

DistrictGridModel::DistrictGridModel()
{}
DistrictGridModel::~DistrictGridModel()
{
}
void DistrictGridModel::reset()
{
    beginResetModel();

    endResetModel();
}

void DistrictGridModel::initialise( )
{
   beginResetModel();

   // pick up the correct District list

   endResetModel();
}
QVariant DistrictGridModel::data( const QModelIndex &index, int role ) const
{

    if (role == Qt::BackgroundRole)
    {
        return QVariant();
    }
    if ( role != Qt::DisplayRole && role != Qt::EditRole )
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        QString disp = MultLists::getMultLists() ->getDistListText( index.row(), DistrictTreeColumns[ index.column() ].fieldId, ct );
        return disp;
    }
    return QVariant();
}
QVariant DistrictGridModel::headerData( int section, Qt::Orientation orientation,
                     int role ) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        QString cell;

        cell = DistrictTreeColumns[section].title;

        return cell;
    }
    return QVariant();
}

QModelIndex DistrictGridModel::index( int row, int column, const QModelIndex &/*parent*/) const
{
    if ( row < 0 || row >= rowCount()  )
        return QModelIndex();

    return createIndex( row, column );
}

QModelIndex DistrictGridModel::parent( const QModelIndex &/*index*/ ) const
{
    return QModelIndex();
}

int DistrictGridModel::rowCount( const QModelIndex &/*parent*/ ) const
{
    return MultLists::getMultLists() ->getDistListSize();
}

int DistrictGridModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return ectMultMaxCol - 1;
}
bool DistrictSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &/*sourceParent*/) const
{
    if (scrolledDistrict == sourceRow)
        return true;
    BaseContestLog * ct = TContestApp::getContestApp() ->getCurrentContest();
    int worked = MultLists::getMultLists()->getDistWorked(sourceRow, ct) ;

    bool makeVisible = true;
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
