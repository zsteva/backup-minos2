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

    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

    void logMessage( QString s );

private slots:
    void LogTimerTimer( );
    void onSetRotation(int direction, int angle);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};

#endif // ROTATORMAINWINDOW_H
