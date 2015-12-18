#ifndef QSOLOGFRAME_H
#define QSOLOGFRAME_H

#include "logger_pch.h"

#include "focuswatcher.h"
#include "validators.h"


namespace Ui {
class QSOLogFrame;
}

class QSOLogFrame : public QFrame
{
    Q_OBJECT

public:
    explicit QSOLogFrame(QWidget *parent);
    ~QSOLogFrame();

    void setAsEdit()
    {
        edit = true;
    }

private:
    ScreenContact *partialContact; // contact being edited on screen
    virtual bool eventFilter(QObject *obj, QEvent *event) override;


     QString oldloc;
     bool locValid;

     void EditControlEnter( QObject *Sender );
     void EditControlExit( QObject *Sender );

     virtual void logScreenEntry( );
     virtual void logCurrentContact( );
     virtual void doGJVEditChange(QObject * );

     void setScoreText( int dist, bool partial, bool xband );
     bool dlgForced();
     bool validateControls( validTypes command );
     void contactValid( void );

     void mouseDoubleClickEvent(QObject *w);
     BaseContact *getPriorContact();
     BaseContact *getNextContact();


  protected: 	// User declarations
     std::vector <ValidatedControl *> vcs;

     BaseContestLog * contest;
     bool overstrike;
     QWidget *current;
     bool updateTimeAllowed;
     bool valid( validTypes command );
     void calcLoc( void );
     void doAutofill( void );
     void fillRst(QLineEdit *rIl, QString &rep, const QString &mode );
     virtual void showScreenEntry( );
     virtual void getScreenContactTime();
     virtual void showScreenContactTime( ScreenContact &);
     virtual void checkTimeEditEnter(QLineEdit *tle, bool &ovr);
     virtual bool isTimeEdit(QLineEdit *tle);
     virtual void checkTimeEditExit();
     bool checkLogEntry(bool checkDTG );
     virtual void setDTGNotValid(ScreenContact *vcct);
  public: 		// User declarations
     bool savePartial( void );
     bool restorePartial( void );
     void killPartial( void );
     void startNextEntry( );
     void keyPressEvent( QKeyEvent* event );
     virtual void selectEntry( BaseContact *lct );

     virtual void getScreenEntry();
     ScreenContact screenContact;  // contact being edited on screen
     BaseContact *selectedContact;   // contact from log list selected
     bool catchup;
     bool unfilled;

     void setActiveControl( int *Key );
     void setMode( QString m );
     void clearCurrentField();
     void lgTraceerr( int err );

     virtual void selectField( QWidget *v );
     virtual void initialise(BaseContestLog * contest, bool catchup , bool edit);
     virtual void refreshOps();
     virtual void updateQSOTime(bool fromTimer = false);
     virtual void updateQSODisplay();
     virtual void closeContest();

     void doGJVCancelButton_clicked();

     void transferDetails( const BaseContact * lct, const BaseContestLog *matct );
     void transferDetails( const ListContact * lct, const ContactList *matct );

private:
    Ui::QSOLogFrame *ui;

    bool edit;

    FocusWatcher *CallsignFW;
    QString CallsignLabelString;

    FocusWatcher *RSTTXFW;
    QString RSTTXLabelString;

    FocusWatcher *SerTXFW;
    QString SerTXLabelString;

    FocusWatcher *RSTRXFW;
    QString RSTRXLabelString;

    FocusWatcher *SerRXFW;
    QString SerRXLabelString;

    FocusWatcher *LocFW;
    QString LocLabelString;

    FocusWatcher *QTHFW;
    QString QTHLabelString;

    FocusWatcher *CommentsFW;
    QString CommentsLabelString;

    FocusWatcher *MainOpFW;
    FocusWatcher *SecondOpFW;

    ErrorList errs;

    ValidatedControl *csIl;
    ValidatedControl *rsIl, *ssIl, *rrIl, *srIl;
    ValidatedControl *locIl;
    ValidatedControl *qthIl;
    ValidatedControl *cmntIl;

    void MainOpComboBox_Exit();
    void SecondOpComboBox_Exit();
signals:
    void QSOFrameCancelled();
private slots:
    void focusChange(QObject *, bool);
    void on_CatchupButton_clicked();
    void on_FirstUnfilledButton_clicked();
    void on_GJVOKButton_clicked();
    void on_GJVForceButton_clicked();
    void on_GJVCancelButton_clicked();
    void on_MatchXferButton_clicked();
    void on_BandMapButton_clicked();
    void on_QTHEdit_textChanged(const QString &arg1);
    void on_CallsignEdit_textChanged(const QString &arg1);
    void on_LocEdit_textChanged(const QString &arg1);
    void on_ModeButton_clicked();
    void on_InsertBeforeButton_clicked();
    void on_InsertAfterButton_clicked();
    void on_PriorButton_clicked();
    void on_NextButton_clicked();
};

#endif // QSOLOGFRAME_H
