#ifndef TSINGLELOGFRAME_H
#define TSINGLELOGFRAME_H

#include <QFrame>
#include <QAbstractItemModel>
#include <QTreeView>
#include <QTreeWidget>

namespace Ui {
class TSingleLogFrame;
}

class TMatchCollection;
class MatchNodeData;
class ProtoContest;
class BaseContestLog;
class BaseContact;
class ContactList;
class ListContact;

// We may need to define our own validation controls with valid methods
// for each needed type...
//==========================================================
class MatchNodeData
{
   public:
      MatchNodeData();

      bool top;
      BaseContestLog *matchedContest;
      BaseContact *matchedContact;
};
class MatchNodeListData
{
   public:
      MatchNodeListData();

      bool top;
      ContactList *matchedList;
      ListContact *matchedContact;
};
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
    void EditContact( BaseContact *lct );
    void EditMatchContact();
    void QSOTreeSelectContact( BaseContact * lct );

    bool logColumnsChanged;
    bool splittersChanged;

    void setMode( QString m );
    void setFreq( QString f );
    void updateTrees();
    void updateQSODisplay();

    bool getStanza( unsigned int stanza, std::string &stanzaData );

    QModelIndex QSOTreeClickIndex;
    QModelIndex matchTreeClickIndex;
    QModelIndex otherTreeClickIndex;
    QModelIndex archiveTreeClickIndex;
    QTreeView *xferTree;

    void GoNextUnfilled();
    void doNextContactDetailsOnLeftClick( );

private:
    Ui::TSingleLogFrame *ui;

    BaseContestLog * contest;
    QSOGridModel qsoModel;

    int lastStanzaCount;

    long long currFreq;
    long long oldFreq;
    QString sCurrFreq;
    QString sOldFreq;

    void transferDetails( MatchNodeData *MatchTreeIndex );
    void transferDetails( MatchNodeListData *MatchTreeIndex );

    void keyPressEvent( QKeyEvent* event );

    QTreeWidgetItem *addTreeRoot(QTreeWidget *tree, QString text);
    void addTreeChild(QTreeWidgetItem *parent, QString text);

//    void showMatchHeaders( );
    void showThisMatchQSOs( TMatchCollection *matchCollection );
    void showOtherMatchQSOs( TMatchCollection *matchCollection );
    void showMatchList( TMatchCollection *matchCollection );

private slots:
    void on_ContestPageChanged();
    void on_XferPressed();
    void on_BandMapPressed();
    void NextContactDetailsTimerTimer();
    void on_MakeEntry(BaseContestLog*);
    void on_QSOTable_doubleClicked(const QModelIndex &index);
    void on_MatchStarting(BaseContestLog*);
    void on_ReplaceThisLogList( TMatchCollection *matchCollection, BaseContestLog* );
    void on_ReplaceOtherLogList( TMatchCollection *matchCollection, BaseContestLog* );
    void on_ReplaceListList( TMatchCollection *matchCollection, BaseContestLog* );
    void on_ScrollToDistrict( const QString &qth, BaseContestLog* );
    void on_ScrollToCountry( const QString &csCs, BaseContestLog* );
    void on_AfterSelectContact(BaseContact *lct, BaseContestLog *contest);
    void on_AfterLogContact( BaseContestLog *ct);

};

#endif // TSINGLELOGFRAME_H
