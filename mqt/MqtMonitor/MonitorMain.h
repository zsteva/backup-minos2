#ifndef MONITORMAIN_H
#define MONITORMAIN_H

#include "base_pch.h"

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
    void logMessage( const QString &s );
    void notifyCallback( bool err, MinosRPCObj *mro, const QString &from );
    void loggerSubscribeClientCallback( bool err, MinosRPCObj *mro, const QString &from );

private slots:
    void on_closeButton_clicked();
    void on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
    void on_response( bool err, QSharedPointer<MinosRPCObj> , const QString &from );
    void on_request( bool err, QSharedPointer<MinosRPCObj>, const QString &from );

    void on_monitorTimeout();


    void on_monitorTree_doubleClicked(const QModelIndex &index);

private:
    Ui::MonitorMain *ui;

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

    StdInReader stdinReader;
    QString localServerName;

    MonitorTreeModel *treeModel;

    QTimer *monitorTimer;

    QVector<MonitoredStation *> stationList;
//    PVirtualNode MonitorTreeClickNode;

    bool syncstat;
    void syncStations();
    void addSlot( MonitoredLog *ct );
    MonitoringFrame *findCurrentLogFrame();
    MonitoringFrame *findContestPage( BaseContestLog *ct );
    void showContestScore( const QString &score );
};

#endif // MONITORMAIN_H
