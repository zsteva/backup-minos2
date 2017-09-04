#ifndef MATCHTREESFRAME_H
#define MATCHTREESFRAME_H

#include "logger_pch.h"

class FocusWatcher;

namespace Ui {
class MatchTreesFrame;
}
enum MatchType {ThisMatch, OtherMatch, ArchiveMatch};
class MatchTreeItem
{
    BaseMatchContest *matchContest;
    QSharedPointer<MatchContact> matchContact;

    MatchTreeItem *parent;
    QVector<MatchTreeItem *> children;
    int row;

public:
    MatchTreeItem(MatchTreeItem *parent, BaseMatchContest *matchContest, QSharedPointer<MatchContact> matchContact);
    ~MatchTreeItem();

    void addChild(MatchTreeItem *mi)
    {
        children.push_back(mi);
        mi->setRow(children.size() - 1);
    }
    int childCount()
    {
        return children.size();
    }

    bool isMatchLine();
    QSharedPointer<MatchContact> getMatchContact();
    BaseMatchContest *getMatchContest();
    MatchTreeItem *getParent();
    MatchTreeItem *child(int i)
    {
        return children[i];
    }
    int getRow()
    {
        return row;
    }
    void setRow(int r)
    {
        row = r;
    }
};

class QSOMatchGridModel: public QAbstractItemModel
{
    protected:
        SharedMatchCollection match;
        MatchTreeItem * rootItem;
        MatchType type;

    public:
        QSOMatchGridModel();
        ~QSOMatchGridModel();

        QModelIndex firstIndex;
        bool currentModel;

        void initialise( MatchType, SharedMatchCollection pmatch );
        QVariant data( const QModelIndex &index, int role ) const Q_DECL_OVERRIDE;
        QVariant headerData( int section, Qt::Orientation orientation,
                             int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;
        QModelIndex index( int row, int column,
                           const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
        QModelIndex parent( const QModelIndex &index ) const Q_DECL_OVERRIDE;

        int rowCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
        int columnCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
};

class MatchTreesFrame : public QFrame
{
    Q_OBJECT

public:
    explicit MatchTreesFrame(QWidget *parent = 0);
    ~MatchTreesFrame();

    void setContest(BaseContestLog *);
    void restoreColumns();
    void getSplitters();

    MatchTreeItem * getXferItem();

    bool logColumnsChanged;

    QModelIndex matchTreeClickIndex;
    QModelIndex otherTreeClickIndex;
    QModelIndex archiveTreeClickIndex;
    QTreeView *xferTree;

private:
    Ui::MatchTreesFrame *ui;
    int splitterHandleWidth;
    BaseContestLog *contest;

    QSOMatchGridModel thisMatchModel;
    QSOMatchGridModel otherMatchModel;
    QSOMatchGridModel archiveMatchModel;

    FocusWatcher *OtherMatchTreeFW;
    FocusWatcher *ArchiveMatchTreeFW;

    void showThisMatchQSOs(SharedMatchCollection matchCollection );
    void showOtherMatchQSOs( SharedMatchCollection matchCollection );
    void showMatchList(SharedMatchCollection matchCollection );
private slots:
    void on_MatchStarting(BaseContestLog*);
    void on_ReplaceThisLogList( SharedMatchCollection matchCollection, BaseContestLog* );
    void on_ReplaceOtherLogList(SharedMatchCollection matchCollection, BaseContestLog* );
    void on_ReplaceListList( SharedMatchCollection matchCollection, BaseContestLog* );
    void on_ArchiveSplitter_splitterMoved(int pos, int index);
    void on_sectionResized(int, int, int);

    void onArchiveMatchTreeFocused(QObject *, bool, QFocusEvent * );
    void onOtherMatchTreeFocused(QObject *, bool, QFocusEvent * );

    void on_OtherMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &);
    void on_ArchiveMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &);
    void on_ThisMatchTree_doubleClicked(const QModelIndex &index);
    void on_OtherMatchTree_doubleClicked(const QModelIndex &index);
    void on_ArchiveMatchTree_doubleClicked(const QModelIndex &index);

signals:
    void setXferEnabled(bool);
    void editContact(QSharedPointer<BaseContact> bct);
    void xferPressed();

};

#endif // MATCHTREESFRAME_H
