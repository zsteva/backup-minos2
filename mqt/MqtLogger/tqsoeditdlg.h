#ifndef TQSOEDITDLG_H
#define TQSOEDITDLG_H

#include <QDialog>

class BaseContact;
class BaseContestLog;
class ScreenContact;
class DisplayContestContact;
class QTreeWidgetItem;

namespace Ui {
class TQSOEditDlg;
}

class TQSOEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TQSOEditDlg( QWidget* Owner, bool catchup, bool unfilled );

    ~TQSOEditDlg();

    int exec();

private:
    Ui::TQSOEditDlg *ui;

//    MinosEventListener  EL_AfterSelectContact;
//    void AfterSelectContact_Event ( MinosEventBase & Event );

    BaseContestLog * contest;
    QSharedPointer<BaseContact> firstContact;
    bool catchup;
    bool unfilled;
    void refreshOps( ScreenContact &screenContact );
    void keyPressEvent( QKeyEvent* event );
    void addTreeRoot(QSharedPointer<BaseContact> lct);
    void addTreeChild(QTreeWidgetItem *parent, QString text);
    void doCloseEvent();


 public: 		// User declarations
    void selectContact(BaseContestLog * contest, QSharedPointer<BaseContact> lct );
    void setContest( BaseContestLog * c )
    {
       contest = c;
    }
    void selectCatchup( BaseContestLog * contest );
    void setFirstContact( QSharedPointer<BaseContact> c )
    {
       firstContact = c;
    }
private slots:
    void on_EditFrameCancelled();
    void on_AfterSelectContact(QSharedPointer<BaseContact> lct, BaseContestLog *contest);
public Q_SLOTS:
    virtual void accept() override;
    virtual void reject() override;
};

#endif // TQSOEDITDLG_H
