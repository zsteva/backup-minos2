#ifndef MONITORINGFRAME_H
#define MONITORINGFRAME_H

#include "base_pch.h"

namespace Ui {
class MonitoringFrame;
}
class BaseContestLog;
class MonitorMain;

class MonitoringFrame : public QFrame
{
    Q_OBJECT

    MonitorMain *mparent;

public:
    explicit MonitoringFrame(MonitorMain *parent = nullptr);
    ~MonitoringFrame();

    void initialise( BaseContestLog * contest );
    void showQSOs();
    void setScore();
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
