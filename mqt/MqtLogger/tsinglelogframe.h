#ifndef TSINGLELOGFRAME_H
#define TSINGLELOGFRAME_H

#include "logger_pch.h"

namespace Ui {
class TSingleLogFrame;
}

extern bool showWorked;
extern bool showUnworked;

class TMatchCollection;
class MatchNodeData;
class ProtoContest;
class BaseContestLog;
class BaseContact;
class ContactList;
class ListContact;
class FocusWatcher;
class TSendDM;

// We may need to define our own validation controls with valid methods
// for each needed type...
//==========================================================


class BaseMatchContest;
class MatchContact;

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
        TMatchCollection *match;
        MatchTreeItem * rootItem;
        MatchType type;

    public:
        QSOMatchGridModel();
        ~QSOMatchGridModel();

        QModelIndex firstIndex;
        bool currentModel;

        void initialise( MatchType, TMatchCollection *pmatch );
        QVariant data( const QModelIndex &index, int role ) const Q_DECL_OVERRIDE;
        QVariant headerData( int section, Qt::Orientation orientation,
                             int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;
        QModelIndex index( int row, int column,
                           const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
        QModelIndex parent( const QModelIndex &index ) const Q_DECL_OVERRIDE;

        int rowCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
        int columnCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
};
class TSingleLogFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TSingleLogFrame(QWidget *parent, BaseContestLog *contest);
    ~TSingleLogFrame();

    void showQSOs();
    void getSplitters();
    void goSerial( );
    BaseContestLog * getContest();
    void closeContest();

    void setActiveControl( int *Key );
    QString makeEntry( bool saveMinos );
    void exportContest();
    void EditContact(QSharedPointer<BaseContact> lct );
    void EditMatchContact();
    void QSOTreeSelectContact( QSharedPointer<BaseContact> lct );

    ScreenContact &getScreenEntry();

    void refreshMults();
    TSendDM *sendDM;

    bool logColumnsChanged;
    bool splittersChanged;

    bool isBandMapLoaded();
    bool bandMapLoaded;

    bool rotatorLoaded;
    bool isRotatorLoaded();

    bool keyerLoaded;
    bool isKeyerLoaded();

    bool radioLoaded;
    bool isRadioLoaded();

    void setRotatorState( QString f );
    void setRotatorBearing( QString f );

    void updateTrees();
    void updateQSODisplay();

    bool getStanza( unsigned int stanza, QString &stanzaData );

    QModelIndex matchTreeClickIndex;
    QModelIndex otherTreeClickIndex;
    QModelIndex archiveTreeClickIndex;
    QTreeView *xferTree;

    void goNextUnfilled();
    void doNextContactDetailsOnLeftClick(bool keepSizes);

private:
    Ui::TSingleLogFrame *ui;

    int splitterHandleWidth;

    BaseContestLog * contest;
    QSOGridModel qsoModel;

    QSOMatchGridModel thisMatchModel;
    QSOMatchGridModel otherMatchModel;
    QSOMatchGridModel archiveMatchModel;

    FocusWatcher *OtherMatchTreeFW;
    FocusWatcher *ArchiveMatchTreeFW;

    int lastStanzaCount;

    long long currFreq;
    long long oldFreq;
    QString sCurrFreq;
    QString sOldFreq;

    bool filterClickEnabled;

    void initFilters();
    void saveFilters();

    void transferDetails( MatchTreeItem *MatchTreeIndex );

    void keyPressEvent( QKeyEvent* event );

    void showThisMatchQSOs( TMatchCollection *matchCollection );
    void showOtherMatchQSOs( TMatchCollection *matchCollection );
    void showMatchList( TMatchCollection *matchCollection );
    void restoreColumns();


private slots:
    void on_ContestPageChanged();
    void on_XferPressed();
    void NextContactDetailsTimerTimer();
    void PublishTimerTimer();
    void HideTimerTimer();
    void on_MakeEntry(BaseContestLog*);
    void on_QSOTable_doubleClicked(const QModelIndex &index);
    void on_MatchStarting(BaseContestLog*);
    void on_ReplaceThisLogList( TMatchCollection *matchCollection, BaseContestLog* );
    void on_ReplaceOtherLogList( TMatchCollection *matchCollection, BaseContestLog* );
    void on_ReplaceListList( TMatchCollection *matchCollection, BaseContestLog* );
    void on_ScrollToDistrict( const QString &qth, BaseContestLog* );
    void on_ScrollToCountry( const QString &csCs, BaseContestLog* );
    void on_AfterSelectContact(QSharedPointer<BaseContact> lct, BaseContestLog *contest);
    void on_AfterLogContact( BaseContestLog *ct);
    void on_NextContactDetailsOnLeft();
    void on_NextUnfilled(BaseContestLog*);
    void on_GoToSerial(BaseContestLog*);

    void on_WorkedCB_clicked();
    void on_UnworkedCB_clicked();
    void on_ContEU_clicked();
    void on_ContOC_clicked();
    void on_ContAS_clicked();
    void on_ContSA_clicked();
    void on_ContAF_clicked();
    void on_ContNA_clicked();


    void on_StackedMults_currentChanged(int arg1);
    void onLogColumnsChanged();
    void onSplittersChanged();
    void onFiltersChanged();
    void on_LogAreaSplitter_splitterMoved(int pos, int index);
    void on_ArchiveSplitter_splitterMoved(int pos, int index);
    void on_TopSplitter_splitterMoved(int pos, int index);
    void on_CribSplitter_splitterMoved(int pos, int index);
    void on_MultSplitter_splitterMoved(int pos, int index);
    void on_LocatorSplitterMoved(int pos, int index);
    void on_sectionResized(int, int, int);

    void on_OtherMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &);
    void on_ArchiveMatchTreeSelectionChanged(const QItemSelection &, const QItemSelection &);
    void on_ThisMatchTree_doubleClicked(const QModelIndex &index);
    void on_OtherMatchTree_doubleClicked(const QModelIndex &index);
    void on_ArchiveMatchTree_doubleClicked(const QModelIndex &index);

    void on_KeyerLoaded();

    void on_BandMapLoaded();

    void on_SetMode(QString);
    void on_SetFreq(QString);
    void on_RadioLoaded();
    void on_SetRadioName(QString);
    void on_SetRadioState(QString);

    void on_RotatorLoaded();
    void on_RotatorState(QString);
    void on_RotatorBearing(QString);
    void on_RotatorMaxAzimuth(QString);
    void on_RotatorMinAzimuth(QString);
    void on_RotatorAntennaName(QString);


    void onArchiveMatchTreeFocused(QObject *, bool, QFocusEvent * );
    void onOtherMatchTreeFocused(QObject *, bool, QFocusEvent * );

    void sendKeyerPlay( int fno );
    void sendKeyerRecord( int fno );
    void sendBandMap( QString freq, QString call, QString utc, QString loc, QString qth );
    void sendKeyerTone();
    void sendKeyerTwoTone();
    void sendKeyerStop();
    void sendRotator(rpcConstants::RotateDirection direction, int angle );

};

#endif // TSINGLELOGFRAME_H
