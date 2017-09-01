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

    void setAsEdit();
    void setBandMapLoaded();
    void setKeyerLoaded();

    bool savePartial( void );
    bool restorePartial( void );
    void killPartial( void );
    void startNextEntry( );
    bool doKeyPressEvent( QKeyEvent* event );
    virtual void selectEntry(QSharedPointer<BaseContact> lct );
    void setXferEnabled(bool);

    virtual void getScreenEntry();
    ScreenContact screenContact;  // contact being edited on screen
    void calcLoc( );

    QSharedPointer<BaseContact> selectedContact;   // contact from log list selected
    bool catchup;
    bool unfilled;

    void setActiveControl( int *Key );
    void clearCurrentField();
    void lgTraceerr( int err );

    virtual void selectField( QWidget *v );
    virtual void initialise(BaseContestLog * contest, bool catchup );
    virtual void refreshOps();
    virtual void refreshOps(ScreenContact &screenContact);
    virtual void updateQSOTime(bool fromTimer = false);
    virtual void updateQSODisplay();
    virtual void closeContest();

    void doGJVCancelButton_clicked();

    void transferDetails(const QSharedPointer<BaseContact> lct, const BaseContestLog *matct );
    void transferDetails(const ListContact *lct, const ContactList *matct );

    void setFirstUnfilledButtonEnabled(bool);
    void logTabChanged();



    void modeSentFromRig(QString mode);
    QString getBearing();
private:
    ScreenContact *partialContact; // contact being edited on screen
    virtual bool eventFilter(QObject *obj, QEvent *event) override;


    QString oldloc;
    bool locValid;

    bool oldTimeOK;

    void EditControlEnter( QObject *Sender, QFocusEvent *event );
    void EditControlExit( QObject *Sender );

    virtual void logScreenEntry( );
    virtual void logCurrentContact( );
    virtual void doGJVEditChange(QObject * );

    void setScoreText( int dist, bool partial, bool xband );
    bool dlgForced();
    bool validateControls( validTypes command );
    void contactValid( void );

    void mouseDoubleClickEvent(QObject *w);
    QSharedPointer<BaseContact> getPriorContact();
    QSharedPointer<BaseContact> getNextContact();

    QVector <ValidatedControl *> vcs;

    BaseContestLog * contest;
    bool overstrike;
    QWidget *current;
    bool updateTimeAllowed;
    bool valid( validTypes command );
    void doAutofill( );
    void fillRst(QLineEdit *rIl, QString &rep, const QString &mode );
    virtual void showScreenEntry( );
    virtual void getScreenContactTime();
    virtual void showScreenContactTime( );
    bool checkAndLogEntry(bool checkDTG );
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

    //bool rotatorLoaded;
    //bool isRotatorLoaded();

    bool bandMapLoaded;
    bool isBandMapLoaded();

    bool keyerLoaded;
    bool isKeyerLoaded();

    void MainOpComboBox_Exit();
    void SecondOpComboBox_Exit();

    QString mode;
    QString oldMode;
    bool qsoLogModeFlag = false;
    QString curFreq;

    void setMode(QString m);
signals:
    void QSOFrameCancelled();
    //void sendRotator(rpcConstants::RotateDirection direction, int angle );
    void sendBandMap( QString freq, QString call, QString utc, QString loc, QString qth );
    void sendModeControl(QString);

private slots:
    void focusChange(QObject *, bool, QFocusEvent *event);
    void on_CatchupButton_clicked();
    void on_FirstUnfilledButton_clicked();
    void on_GJVOKButton_clicked();
    void on_GJVForceButton_clicked();
    void on_GJVCancelButton_clicked();
    void on_MatchXferButton_clicked();
    void on_QTHEdit_textChanged(const QString &arg1);
    void on_CallsignEdit_textChanged(const QString &arg1);
    void on_LocEdit_textChanged(const QString &arg1);
    void on_ModeButton_clicked();
    void on_InsertBeforeButton_clicked();
    void on_InsertAfterButton_clicked();
    void on_PriorButton_clicked();
    void on_NextButton_clicked();

    void on_TimeDisplayTimer();
    void on_AfterTabFocusIn(QLineEdit *tle);
    void on_Validated();
    void on_ValidateError (int mess_no );
    void on_ShowOperators();

//    void on_ModeComboBoxGJV_currentIndexChanged(int index);
    void on_ModeComboBoxGJV_activated(int index);
    void on_RSTTXEdit_textChanged(const QString &arg1);
    void on_RSTRXEdit_textChanged(const QString &arg1);

};

#endif // QSOLOGFRAME_H
