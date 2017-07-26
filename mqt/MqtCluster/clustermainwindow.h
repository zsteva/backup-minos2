#ifndef CLUSTERMAINWINDOW_H
#define CLUSTERMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class ClusterMainWindow;
}

class ClusterMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClusterMainWindow(QWidget *parent = 0);
    ~ClusterMainWindow();

private:
    Ui::ClusterMainWindow *ui;
};

#endif // CLUSTERMAINWINDOW_H
