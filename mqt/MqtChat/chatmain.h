#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class TMinosChatForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit TMinosChatForm(QWidget *parent = 0);
    ~TMinosChatForm();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
