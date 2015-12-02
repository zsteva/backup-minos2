#ifndef TSINGLELOGFRAME_H
#define TSINGLELOGFRAME_H

#include <QFrame>

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
    void showMatchHeaders( );
    void showMatchQSOs( TMatchCollection *matchCollection );
    void showMatchList( TMatchCollection *matchCollection );
    void matchDistrict( const std::string &qth );
    void matchCountry( const std::string &csCs );
    void replaceContestList( TMatchCollection *matchCollection );
    void replaceListList( TMatchCollection *matchCollection );
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
    void updateQSOTime(bool fromTimer = false);
    void updateQSODisplay();

    bool getStanza( unsigned int stanza, std::string &stanzaData );

//    PVirtualNode QSOTreeClickNode;
//    PVirtualNode matchTreeClickNode;
//    PVirtualNode otherTreeClickNode;
//    PVirtualNode archiveTreeClickNode;
//    TVirtualStringTree *xferTree;

    void GoNextUnfilled();
    void doNextContactDetailsOnLeftClick( );

private:
    Ui::TSingleLogFrame *ui;

    BaseContestLog * contest;
    int lastStanzaCount;

    long long currFreq;
    long long oldFreq;
    QString sCurrFreq;
    QString sOldFreq;

    void transferDetails( MatchNodeData *MatchTreeIndex );
    void transferDetails( MatchNodeListData *MatchTreeIndex );

};

#endif // TSINGLELOGFRAME_H
