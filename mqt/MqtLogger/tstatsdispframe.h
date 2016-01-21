#ifndef TSTATSDISPFRAME_H
#define TSTATSDISPFRAME_H

#include <QFrame>
#include <QDateTime>

class BaseContestLog;

namespace Ui {
class TStatsDispFrame;
}

class TStatsDispFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TStatsDispFrame(QWidget *parent = 0);
    ~TStatsDispFrame();

    void reInitialiseStats();
    void setContest( BaseContestLog *ct );

private slots:
    void on_P1Edit_valueChanged(int arg1);
    void on_P2Edit_valueChanged(int arg1);
    void RecheckTimerTimer( );

private:
    Ui::TStatsDispFrame *ui;
    BaseContestLog *ct;
    QDateTime lastCheckTime;
};

#endif // TSTATSDISPFRAME_H
