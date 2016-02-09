#ifndef DISTRICTFRAME_H
#define DISTRICTFRAME_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QSortFilterProxyModel>

namespace Ui {
class DistrictFrame;
}

class BaseContestLog;

class DistrictGridModel: public QAbstractItemModel
{
    public:
        DistrictGridModel();
        ~DistrictGridModel();

        BaseContestLog *ct;

        void reset();
        void initialise( );
        QVariant data( const QModelIndex &index, int role ) const Q_DECL_OVERRIDE;
        QVariant headerData( int section, Qt::Orientation orientation,
                             int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;
        QModelIndex index( int row, int column,
                           const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
        QModelIndex parent( const QModelIndex &index ) const Q_DECL_OVERRIDE;

        int rowCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
        int columnCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
};

class DistrictSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    int scrolledDistrict;
    DistrictSortFilterProxyModel(): scrolledDistrict(-1)
    {
    }
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

class DistrictFrame : public QFrame
{
    Q_OBJECT

    DistrictGridModel model;
    DistrictSortFilterProxyModel proxyModel;

public:
    explicit DistrictFrame(QWidget *parent = 0);
    ~DistrictFrame();

    void setContest(BaseContestLog *contest);
    void reInitialiseDistricts();
    void scrollToDistrict( int district_ind, bool makeVisible );
private:
    Ui::DistrictFrame *ui;
private slots:
    void on_sectionResized(int, int , int);
    void on_DistrictTable_clicked(const QModelIndex &index);
};

#endif // DISTRICTFRAME_H
