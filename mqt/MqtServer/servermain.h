#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "MinosLink.h"
#include "minoslistener.h"
#include "clientThread.h"
#include "serverThread.h"

class TZConf;
class TPubSubMain;

namespace Ui {
class MainWindow;
}

class ServerMain : public QMainWindow
{
    Q_OBJECT

    QTimer LogTimer;
    QTimer ScanTimer;

public:
    explicit ServerMain(QWidget *parent = 0);
    ~ServerMain();
    void logMessage( const QString &s );
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

private slots:
    void on_CloseButton_clicked();
    void LogTimerTimer( );
    void ScanTimerTimer( );

private:
    Ui::MainWindow *ui;
    QSharedPointer<MinosClientListener> clientListener;
    QSharedPointer<MinosServerListener> serverListener;
    QSharedPointer<TZConf> ZConf;
    QSharedPointer<TPubSubMain> PubSubMain;
};
extern ServerMain *MinosMainForm;
extern bool closeApp;

#endif // MAINWINDOW_H
