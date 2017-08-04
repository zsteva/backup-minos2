#ifndef MONITORMAIN_H
#define MONITORMAIN_H

#include "base_pch.h"
#include <QAction>
#include <QMenu>

class MonitoredStation;
class MonitoringFrame;
class MonitoredLog;
class MonitorTreeModel;

namespace Ui {
class MonitorMain;
}

class MonitorMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit MonitorMain(QWidget *parent = 0);
    ~MonitorMain();

    QVector<MonitoredStation *> stationList;

    void logMessage( const QString &s );
    void notifyCallback( bool err, MinosRPCObj *mro, const QString &from );
    void loggerSubscribeClientCallback( bool err, MinosRPCObj *mro, const QString &from );

    void closeTab(MonitoringFrame *tab);

private slots:
    void on_closeButton_clicked();
    void on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
    void on_clientCall( bool err, QSharedPointer<MinosRPCObj> , const QString &from );
    void on_serverCall( bool err, QSharedPointer<MinosRPCObj>, const QString &from );

    void onStdInRead(QString cmd);

    void on_monitorTimeout();

    void on_monitorTree_doubleClicked(const QModelIndex &index);
    void on_monitorSplitter_splitterMoved(int /*pos*/, int /*index*/);
    void on_closeMonitoredLog();
    void on_contestPageControl_customContextMenuRequested(const QPoint &pos);
    void CancelClick();

private:
    Ui::MonitorMain *ui;

    QAction *newAction( const QString &text, QMenu *m, const char *atype );
    QMenu TabPopup;
    QAction *closeMonitoredLog;


    int splitterHandleWidth;

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

    StdInReader stdinReader;
    QString localServerName;

    MonitorTreeModel *treeModel;

    QTimer *monitorTimer;

    bool syncstat;
    void syncStations();
    void addSlot( MonitoredLog *ct );
    MonitoringFrame *findCurrentLogFrame();
    MonitoringFrame *findContestPage( BaseContestLog *ct );
    void showContestScore( const QString &score );
};

#endif // MONITORMAIN_H
