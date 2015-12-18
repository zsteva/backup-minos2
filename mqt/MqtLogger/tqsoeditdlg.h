#ifndef TQSOEDITDLG_H
#define TQSOEDITDLG_H

#include <QDialog>

class BaseContact;
class BaseContestLog;
class ScreenContact;
class DisplayContestContact;

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
    BaseContact *firstContact;
    bool catchup;
    bool unfilled;
    void refreshOps( ScreenContact &screenContact );
    void keyPressEvent( QKeyEvent* event );

 public: 		// User declarations
    void selectContact( BaseContestLog * contest, DisplayContestContact *lct );
    void setContest( BaseContestLog * c )
    {
       contest = c;
    }
    void selectCatchup( BaseContestLog * contest );
    void setFirstContact( BaseContact *c )
    {
       firstContact = c;
    }
private slots:
    void on_EditFrameCancelled();
};

#endif // TQSOEDITDLG_H
