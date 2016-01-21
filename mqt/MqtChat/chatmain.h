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

private:
    Ui::MainWindow *ui;

    bool connected;
    bool subscribed;
    std::vector<Server> serverList;

    QTimer LogTimer;
    QTimer ConnectTimer;


    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);


    void logMessage( QString s );
    void chatClientCallback( bool err, MinosRPCObj *mro, const QString &from );
    void chatServerCallback( bool err, MinosRPCObj *mro, const QString &from );
    static void makeRPCObjects();
    void syncChat();
    void syncStations();
    void keyPressEvent( QKeyEvent* event );

 public: 		// User declarations
    void notifyCallback( bool err, MinosRPCObj *mro, const QString &from );
    void addChat(const QString &mess);

private slots:
    void LogTimerTimer( );
    void ConnectTimerTimer( );


    void on_SendButton_clicked();
};

#endif // MAINWINDOW_H
