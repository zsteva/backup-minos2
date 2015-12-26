#ifndef LOCFRAME_H
#define LOCFRAME_H

#include <QFrame>

namespace Ui {
class LocFrame;
}
class BaseContestLog;
class LocFrame : public QFrame
{
    Q_OBJECT

public:
    explicit LocFrame(QWidget *parent = 0);
    ~LocFrame();

    void reInitialiseLocators();
    void setContest(BaseContestLog *contest);

private:
    Ui::LocFrame *ui;
    BaseContestLog *ct;
};

#endif // LOCFRAME_H
