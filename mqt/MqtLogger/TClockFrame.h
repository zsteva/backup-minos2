#ifndef TCLOCKFRAME_H
#define TCLOCKFRAME_H

#include <QFrame>

namespace Ui {
class TClockFrame;
}

class TClockFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TClockFrame(QWidget *parent = 0);
    ~TClockFrame();

private:
    Ui::TClockFrame *ui;

private slots:
    void RecheckTimerTimer( );
};

#endif // TCLOCKFRAME_H
