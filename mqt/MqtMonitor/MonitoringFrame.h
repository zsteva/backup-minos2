#ifndef MONITORINGFRAME_H
#define MONITORINGFRAME_H

#include <QFrame>

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


private:
    Ui::MonitoringFrame *ui;
    BaseContestLog *contest;
};

#endif // MONITORINGFRAME_H
