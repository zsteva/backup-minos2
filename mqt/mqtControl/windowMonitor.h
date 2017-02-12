#ifndef WINDOWMONITOR_H
#define WINDOWMONITOR_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class windowMonitor;
}

class windowMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit windowMonitor(QWidget *parent);
    ~windowMonitor();

    bool L1Checked();
    bool L2Checked();
    bool PTTChecked();

    void checkL1(bool checked);
    void checkL2(bool checked);
    void checkPTT(bool checked);

private:
    Ui::windowMonitor *ui;
    QTimer setPosTimer;
private slots:
    void setPosition();

};

#endif // WINDOWMONITOR_H
