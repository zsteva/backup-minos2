#ifndef DXCCFRAME_H
#define DXCCFRAME_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QSortFilterProxyModel>

namespace Ui {
class DXCCFrame;
}

class BaseContestLog;

class DXCCGridModel: public QAbstractItemModel
{
    public:
        DXCCGridModel();
        ~DXCCGridModel();

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
class DXCCSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    int scrolledCountry;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    DXCCSortFilterProxyModel(): scrolledCountry(-1)
    {
    }
};

class DXCCFrame : public QFrame
{
    //Q_OBJECT

    DXCCGridModel model;
    DXCCSortFilterProxyModel proxyModel;

public:
    explicit DXCCFrame(QWidget *parent = 0);
    ~DXCCFrame();

    void setContest(BaseContestLog *contest);
    void reInitialiseCountries();
    void scrollToCountry( int ctry_ind, bool makeVisible );

private:
    Ui::DXCCFrame *ui;
};

#endif // DXCCFRAME_H
