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
    bool L3Checked();
    bool L4Checked();
    bool PTTChecked();

    void checkL1(bool checked);
    void checkL2(bool checked);
    void checkL3(bool checked);
    void checkL4(bool checked);
    void checkPTT(bool checked);

private:
    Ui::windowMonitor *ui;
    QTimer setPosTimer;

    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;

private slots:
    void setPosition();

};

#endif // WINDOWMONITOR_H
