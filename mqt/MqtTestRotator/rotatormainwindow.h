#ifndef ROTATORMAINWINDOW_H
#define ROTATORMAINWINDOW_H

#include "base_pch.h"

namespace Ui {
class RotatorMainWindow;
}

class RotatorLogic;
class RotatorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RotatorMainWindow(QWidget *parent = 0);
    ~RotatorMainWindow();

private:
    Ui::RotatorMainWindow *ui;

    RotatorLogic *rl;

    QTimer LogTimer;


    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);


    void logMessage( QString s );

private slots:
    void LogTimerTimer( );
    void onSetRotation(int direction, int angle);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};

#endif // ROTATORMAINWINDOW_H
