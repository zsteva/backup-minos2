#ifndef TLOGCONTAINER_H
#define TLOGCONTAINER_H

#include <QMainWindow>
#include <QMenu>
#include <QSettings>
#include <QTimer>
#include <QLabel>
#include "SpinBoxAction.h"

namespace Ui {
class TLogContainer;
}

class ContestDetails;
class BaseContestLog;
class BaseContact;
class TSingleLogFrame;
class TContactListDetails;
class ContactList;
class MatchContact;

class SetMemoryAction: public QAction
{
public:
    SetMemoryAction(QString t, QObject *p):QAction(t, p)
    {}
    BaseContestLog *ct = 0;
    QString call;
    QString loc;
};
class TLogContainer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TLogContainer(QWidget *parent = 0);
    ~TLogContainer();

    void subscribeApps();

    void selectContest(BaseContestLog *pc, QSharedPointer<BaseContact> pct );
    bool show(int argc, char *argv[]);
    TSingleLogFrame *getCurrentLogFrame();

    QLabel *sblabel0;
    QLabel *sblabel1;
    QLabel *sblabel2;

    TSingleLogFrame *findContest( const QString &pubname );
    TSingleLogFrame *findContest(BaseContestLog *ct );

    QString getDefaultDirectory( bool IsList );

    bool isShowOperators();
    bool isNextContactDetailsOnLeft();

    void setCaption( QString );

    void getCurrSession();
    QStringList getSessions();
    void updateSessionActions();
    void selectSession(QString sessName);

    SetMemoryAction *setMemoryAction;
private:
    Ui::TLogContainer *ui;


    QTimer TimerUpdateQSOTimer;


    QMenu TabPopup;

    enum { MaxRecentFiles = 5 };
    QVector<QAction *> recentFileActs;
    QMenu *recentFilesMenu;
    QVector<QAction *> sessionActs;
    QMenu *sessionsMenu;
    QMenu *keyerRecordMenu;
    QMenu *keyerPlaybackMenu;

    void enableActions();

    QString getCurrentFile();
    void setCurrentFile(const QString &fileName);
    void removeCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    BaseContestLog * addSlot(ContestDetails *ced, const QString &fname, bool newfile, int slotno );
    void closeSlot(int t, bool addToMRU );
    TSingleLogFrame *findLogFrame(int t);
    void selectTab(int t);

    QAction *lastSessionSelected;

    QAction *newAction(const QString &text, QMenu *m, const char *atype );
    SetMemoryAction *newMemoryAction(const QString &text, QMenu *m, const char *atype );
    QAction *newCheckableAction( const QString &text, QMenu *m, const char *atype );
    void setupMenus();

    QAction *HelpAboutAction;

    QAction *FileOpenAction;
    QAction *ListOpenAccept;
    QAction *ContestDetailsAction;
    QAction *FileCloseAction;
    QAction *CloseAllAction;
    QAction *CloseAllButAction;

    QAction *sessionManagerAction;

    QAction *ExitAction;
    QAction *ExitClearAction;
    QAction *MakeEntryAction;
    QAction *FileNewAction;
    QAction *ListOpenAction;
    QAction *ManageListsAction;
    QAction *ShiftTabLeftAction;
    QAction *ShiftTabRightAction;

    QAction *startConfigAction;
    QAction *LocCalcAction;
    QAction *AnalyseMinosLogAction;
    QAction *CorrectDateTimeAction;
    SpinBoxAction *NumberAuxiliaryAction;
    QAction *ShowOperatorsAction;
    QAction *OptionsAction;
    QAction *FontEditAcceptAction;
    QAction *ReportAutofillAction;

    QAction *GoToSerialAction;
    QAction *NextUnfilledAction;

    QAction *NextContactDetailsOnLeftAction;

    QAction *KeyerToneAction;
    QAction *KeyerTwoToneAction;
    QAction *KeyerStopAction;

    QAction *KeyerRecordAction;
    QAction *KeyerPlaybackAction;

    QVector< QSharedPointer<QAction> > menuLogsActions;

    BaseContestLog *loadSession(QString sessName);
    void preloadLists( );
    void preloadFiles( const QString &conarg );
    void addListSlot(const QString &fname, int slotno , bool preload);

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

private slots:
    void CancelClick();
    void HelpAboutActionExecute();

    void selectSession();
    void openRecentFile();
    void FileOpenActionExecute();
    void ListOpenActionExecute();
    void ManageListsActionExecute();
    void ContestDetailsActionExecute();
    void FileCloseActionExecute();
    void CloseAllActionExecute();
    void CloseAllButActionExecute();
    void ExitClearActionExecute();
    void ExitActionExecute();

    void sessionManageExecute();

    void MakeEntryActionExecute();
    void onSetMemoryActionExecute();
    void FileNewActionExecute();
    void ShiftTabLeftActionExecute();
    void ShiftTabRightActionExecute();

    void LocCalcActionExecute();
    void AnalyseMinosLogActionExecute();
    void CorrectDateTimeActionExecute();
    void ShowOperatorsActionExecute();
    void OptionsActionExecute();
    void FontEditAcceptActionExecute();
    void ReportAutofillActionExecute();

    void GoToSerialActionExecute();
    void NextUnfilledActionExecute();

    void NextContactDetailsOnLeftActionExecute();

    void KeyerToneActionExecute();
    void KeyerTwoToneActionExecute();
    void KeyerStopActionExecute();
    void KeyerRecordActionExecute();
    void KeyerPlaybackActionExecute();

    void menuLogsActionExecute();

    void StartConfigActionExecute();

    void on_ContestPageControl_currentChanged(int index);
    void on_ContestPageControl_tabBarDoubleClicked(int index);
    void on_ContestPageControl_customContextMenuRequested(const QPoint &pos);
    void onTabClosebutton(int);

    void on_TimeDisplayTimer( );
    void on_ReportOverstrike(bool , BaseContestLog * );
    void onTabMoved(int, int);

    void AuxSpinboxValueChanged(int);

signals:
    void sendKeyerPlay( int fno );
    void sendKeyerRecord( int fno );
    void sendKeyerTone();
    void sendKeyerTwoTone();
    void sendKeyerStop();

    void setAuxWindows();

};
extern TLogContainer *LogContainer;

#endif // TLOGCONTAINER_H
