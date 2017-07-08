#ifndef MQTKTMAINWINDOW_H
#define MQTKTMAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>

namespace Ui {
class mqtktMainWindow;
}

class mqtktMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit mqtktMainWindow(QWidget *parent = 0);
    ~mqtktMainWindow();

private:
    Ui::mqtktMainWindow *ui;
};

#endif // MQTKTMAINWINDOW_H
