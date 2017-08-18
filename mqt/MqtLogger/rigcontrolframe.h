#ifndef RIGCONTROLFRAME_H
#define RIGCONTROLFRAME_H

#include "logger_pch.h"

namespace Ui {
    class RigControlFrame;
}

class RigControlFrame : public QFrame
{
    Q_OBJECT

public:
    explicit RigControlFrame(QWidget *parent);
    ~RigControlFrame();

    Ui::RigControlFrame *ui;

    void setRadioLoaded();
    void setMode(QString);
    void setFreq(QString);
    void setRadioName(QString);
    void setRadioState(QString);

signals:

    void sendModeLogFrame(QString);

private:

    bool radioLoaded;
    bool isRadioLoaded();


};

#endif // RIGCONTROLFRAME_H
