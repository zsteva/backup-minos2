#ifndef TLOGCONTAINER_H
#define TLOGCONTAINER_H

#include <QMainWindow>
#include <QMenu>
#include <QSettings>
#include <QTimer>
#include <QLabel>

namespace Ui {
class TLogContainer;
}

class ContestDetails;
class BaseContestLog;
class BaseContact;
class TSingleLogFrame;
class TContactListDetails;
class ContactList;

class TLogContainer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TLogContainer(QWidget *parent = 0);
    ~TLogContainer();

    void selectContest(BaseContestLog *pc, QSharedPointer<BaseContact> pct );
    bool show(int argc, char *argv[]);
    TSingleLogFrame *getCurrentLogFrame();

    QLabel *sblabel0;
    QLabel *sblabel1;
    QLabel *sblabel2;

    TSingleLogFrame *findContest( const QString &pubname );
    QString getDefaultDirectory( bool IsList );

    bool isShowOperators();
    bool isNextContactDetailsOnLeft();
    bool isScrollingContestTabs();

    void setCaption( QString );
    void setMode( QString );
    void setFreq( QString );

    void setRotatorState( QString );

    void setBandMapLoaded();
    bool isBandMapLoaded();
    bool bandMapLoaded;

    void setKeyerLoaded();
    bool isKeyerLoaded();
    bool keyerLoaded;

    void setRotatorLoaded();
    bool isRotatorLoaded();
    bool rotatorLoaded;

    void getCurrSession();
    QStringList getSessions();
    void updateSessionActions();
    void selectSession(QString sessName);

private:
    Ui::TLogContainer *ui;


    QTimer TimerUpdateQSOTimer;


    QMenu TabPopup;

    enum { MaxRecentFiles = 5 };
    QVector<QAction *> recentFileActs;
    QMenu *recentFilesMenu;
    QVector<QAction *> sessionActs;
    QMenu *sessionsMenu;

    void enableActions();

    QString getCurrentFile();
    void setCurrentFile(const QString &fileName);
    void removeCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    BaseContestLog * addSlot(ContestDetails *ced, const QString &fname, bool newfile, int slotno );
    void closeSlot(int t, bool addToMRU );
    TSingleLogFrame *findLogFrame(int t);

    QAction *lastSessionSelected;

    QAction *newAction(const QString &text, QMenu *m, const char *atype );
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
    QAction *MakeEntryAction;
    QAction *FileNewAction;
    QAction *ListOpenAction;
    QAction *ManageListsAction;
    QAction *ShiftTabLeftAction;
    QAction *ShiftTabRightAction;

    QAction *LocCalcAction;
    QAction *AnalyseMinosLogAction;
    QAction *CorrectDateTimeAction;
    QAction *ScrollingContestTabsAction;
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

    QVector< QSharedPointer<QAction> > menuLogsActions;

    BaseContestLog *loadSession(QString sessName);
    void preloadLists( );
    void preloadFiles( const QString &conarg );
    void addListSlot(const QString &fname, int slotno , bool preload);

    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

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
    void ExitActionExecute();

    void sessionManageExecute();

    void MakeEntryActionExecute();
    void FileNewActionExecute();
    void ShiftTabLeftActionExecute();
    void ShiftTabRightActionExecute();

    void LocCalcActionExecute();
    void AnalyseMinosLogActionExecute();
    void CorrectDateTimeActionExecute();
    void ScrollingContestTabsActionExecute();
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

    void menuLogsActionExecute();

    void on_ContestPageControl_currentChanged(int index);
    void on_ContestPageControl_tabBarDoubleClicked(int index);
    void on_ContestPageControl_customContextMenuRequested(const QPoint &pos);

    void on_TimeDisplayTimer( );
    void on_ReportOverstrike(bool , BaseContestLog * );
};
extern TLogContainer *LogContainer;

#endif // TLOGCONTAINER_H
