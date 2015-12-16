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
    explicit QSOLogFrame(QWidget *parent = 0);
    ~QSOLogFrame();

private:
    ScreenContact *partialContact; // contact being edited on screen


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

     void keyPressEvent( QKeyEvent* event );

     void mouseDoubleClickEvent(QMouseEvent *event);

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
     virtual void getScreenEntry();
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

     ScreenContact screenContact;  // contact being edited on screen
     BaseContact *selectedContact;   // contact from log list selected
     bool catchup;
     bool unfilled;

     void setActiveControl( int *Key );
     void setMode( QString m );
     void clearCurrentField();
     void lgTraceerr( int err );

     virtual void selectField( QWidget *v );
     virtual void initialise( BaseContestLog * contest, bool catchup );
     virtual void refreshOps();
     virtual void updateQSOTime(bool fromTimer = false);
     virtual void updateQSODisplay();
     virtual void closeContest();


private:
    Ui::QSOLogFrame *ui;

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

    ErrorList errs;

    ValidatedControl *csIl;
    ValidatedControl *rsIl, *ssIl, *rrIl, *srIl;
    ValidatedControl *locIl;
    ValidatedControl *qthIl;
    ValidatedControl *cmntIl;

private slots:
    void focusChange(QObject *, bool);
    void on_CatchupButton_clicked();
    void on_FirstUnfilledButton_clicked();
    void on_MainOpComboBox_currentTextChanged(const QString &arg1);
    void on_SecondOpComboBox_currentTextChanged(const QString &arg1);
    void on_GJVOKButton_clicked();
    void on_GJVForceButton_clicked();
    void on_GJVCancelButton_clicked();
    void on_MatchXferButton_clicked();
    void on_BandMapButton_clicked();
    void on_QTHEdit_textChanged(const QString &arg1);
    void on_CallsignEdit_textChanged(const QString &arg1);
    void on_LocEdit_textChanged(const QString &arg1);
    void on_ModeButton_clicked();
};

#endif // QSOLOGFRAME_H
