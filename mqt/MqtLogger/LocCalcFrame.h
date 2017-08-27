#ifndef LOCCALCFRAME_H
#define LOCCALCFRAME_H

#include "logger_pch.h"
#include "focuswatcher.h"

namespace Ui {
class LocCalcFrame;
}

class LocCalcFrame : public QFrame
{
    Q_OBJECT

public:
    explicit LocCalcFrame(QWidget *parent = 0);
    ~LocCalcFrame();
    void doExec();
    void setContest(BaseContestLog *contest);

    bool Modal;
    QString S1Loc;
    QString S2Loc;
    QString Distance;

private slots:
    void on_CalcButton_clicked();

    void on_ExitButton_clicked();

    void on_CancelButton_clicked();

private:
    Ui::LocCalcFrame *ui;
    void doCloseEvent();
    bool handleExit( QLineEdit *Edit );

    BaseContestLog *contest;

    FocusWatcher *S1LocFW;
    FocusWatcher *S1LatLongFW;
    FocusWatcher *S1NGRFW;
    FocusWatcher *S2LocFW;
    FocusWatcher *S2LatLongFW;
    FocusWatcher *S2NGRFW;

public Q_SLOTS:
    void focusChange(QObject *, bool, QFocusEvent *event);
};

#endif // LOCCALCFRAME_H
