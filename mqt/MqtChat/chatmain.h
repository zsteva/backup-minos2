#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "base_pch.h"

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}
class Server
{
   public:
      QString name;
      QString ip;
      PublishState state;
};
class TMinosChatForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit TMinosChatForm(QWidget *parent = 0);
    ~TMinosChatForm();
    void addChat(const QString &mess);

private:
    Ui::MainWindow *ui;

    QTimer SyncTimer;

    QVector<Server> serverList;

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

    void logMessage( QString s );
    void syncChat();
    void syncStations();
    void keyPressEvent( QKeyEvent* event );

private slots:
    void SyncTimerTimer( );

    void on_SendButton_clicked();

    void on_notify(bool err, QSharedPointer<MinosRPCObj> mro, const QString &from );
    void on_response( bool err, QSharedPointer<MinosRPCObj> , const QString &from );
    void on_request( bool err, QSharedPointer<MinosRPCObj>, const QString &from );
};

#endif // MAINWINDOW_H
