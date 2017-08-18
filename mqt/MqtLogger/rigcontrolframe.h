#ifndef RIGCONTROLFRAME_H
#define RIGCONTROLFRAME_H

#include "logger_pch.h"

namespace Ui {
    class RigControlFrame;
}

namespace maskData {

const QStringList freqMask = { "9.999",
                         "99.999",
                         "999.999",
                         "9.999.999",
                         "99.999.999",
                         "999.999.999",
                         "9.999.999.999",
                         "99.999.999.999"};
}


class FreqLineEdit : public QLineEdit
{
    Q_OBJECT

public:

    FreqLineEdit(QWidget *parent);
    ~FreqLineEdit();


    void focusInEvent( QFocusEvent* ) ;

signals:
    void receivedFocus() ;

} ;





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

private slots:
    void freqLineEditInFocus();
private:

    bool radioLoaded;
    bool isRadioLoaded();
    QString curFreq;
    QString curMode;
    QString radioName;
    QString radioState;


    void freqLineEditBkgnd(bool status);
    void freqLineEditFrameColour(bool status);
};

#endif // RIGCONTROLFRAME_H
