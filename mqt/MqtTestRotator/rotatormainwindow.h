#ifndef ROTATORMAINWINDOW_H
#define ROTATORMAINWINDOW_H

#include "base_pch.h"

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class RotatorMainWindow;
}

class RotatorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RotatorMainWindow(QWidget *parent = 0);
    ~RotatorMainWindow();

private:
    Ui::RotatorMainWindow *ui;

    bool connected;
    bool subscribed;

    QTimer LogTimer;
    QTimer ConnectTimer;


    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);


    void logMessage( QString s );

    void rotatorClientCallback( bool err, MinosRPCObj *mro, const QString &from );
    void rotatorServerCallback( bool err, MinosRPCObj *mro, const QString &from );

    static void makeRPCObjects();

private slots:
    void LogTimerTimer( );
    void ConnectTimerTimer( );
};

#endif // ROTATORMAINWINDOW_H
