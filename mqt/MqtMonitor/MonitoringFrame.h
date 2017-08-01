#ifndef MONITORINGFRAME_H
#define MONITORINGFRAME_H

#include "base_pch.h"

namespace Ui {
class MonitoringFrame;
}
class BaseContestLog;

class MonitoringFrame : public QFrame
{
    Q_OBJECT

public:
    explicit MonitoringFrame(QWidget *parent = 0);
    ~MonitoringFrame();

    void initialise( BaseContestLog * contest );
    void showQSOs();
    BaseContestLog * getContest()
    {
       return contest;
    }

    void update();

private:
    Ui::MonitoringFrame *ui;
    BaseContestLog *contest;

    QSOGridModel qsoModel;

    void restoreColumns();

private slots:
    void on_sectionResized(int, int, int);


};

#endif // MONITORINGFRAME_H
