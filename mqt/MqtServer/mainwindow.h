#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QTimer LogTimer;
    QTimer ScanTimer;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void logMessage( const QString &s );

private slots:
    void on_CloseButton_clicked();
    void LogTimerTimer( );
    void ScanTimerTimer( );

private:
    Ui::MainWindow *ui;
};
extern MainWindow *MinosMainForm;
extern bool closeApp;

#endif // MAINWINDOW_H
