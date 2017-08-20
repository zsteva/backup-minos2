/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2017
//
//
//
/////////////////////////////////////////////////////////////////////////////



#ifndef RIGCONTROLFRAME_H
#define RIGCONTROLFRAME_H

#include "logger_pch.h"
#include "RPCCommandConstants.h"
#include "rigmempushbutton.h"

#define NUM_MEMORIES 6

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
enum freqMask_lookup {
  KHZ = 0,              /* 9 Khz   */
  KHZ99,        		/* 99 khz  */
  KHZ999,        		/* 999 khz */
  MHZ,                  /* 9 Mhz   */
  MHZ99,        		/* 99 Mhz  */
  MHZ999,       		/* 999 Mhz */
  GHZ,                  /* Ghz     */
  GHZ99                 /* Ghz 99  */
};


}

namespace bandSelData {

const QStringList bandNames = { "",
                                "10m",
                                "6m",
                                "4m",
                                "2m",
                                "70cm",
                                "23cm",
                                "GHz"};

const QStringList bandFreq = { "",
                               "28.170.000",
                               "50.170.000",
                               "70.210.000",
                               "144.290.000",
                               "432.170.000",
                               "1.296.210.000",
                               "10.000.000.000"};

const QStringList freqDialZero =  { "",
                                    "00.000.000",
                                    "00.000.000",
                                    "00.000.000",
                                    "000.000.000",
                                    "000.000.000",
                                    "0.000.000.000",
                                    "00.000.000.000"};

const maskData::freqMask_lookup bandMaskIdx[] = { maskData::MHZ99,      /* Not used */
                                                    maskData::MHZ99,    /* 10m  */
                                                    maskData::MHZ99,    /* 6m  */
                                                    maskData::MHZ99,    /* 4m  */
                                                    maskData::MHZ999,   /* 2m  */
                                                    maskData::MHZ999,   /* 70cm  */
                                                    maskData::GHZ,      /* 23cm  */
                                                    maskData::GHZ99};   /* GHz  */


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
    void sendFreqControl(QString);
    void sendModeToControl(QString);
    void sendModeLogFrame(QString);

private slots:
    void freqLineEditInFocus();
    void changeRadioFreq();
    void radioBandFreq(int index);

    void clickedMemory(int);

private:

    QPushButton* memButtons[NUM_MEMORIES];
    bool radioLoaded;
    bool isRadioLoaded();
    bool freqEditOn;
    QString curFreq;
    QString curMode;
    QString radioName;
    QString radioState;

    void sendModeToRadio(QString);
    void freqLineEditBkgnd(bool status);
    void freqLineEditFrameColour(bool status);
    void exitFreqEdit();
    void keyPressEvent(QKeyEvent *event);

    void initRigFrame();
    void initMemoryButtons();
signals:
    void escapePressed();

};

#endif // RIGCONTROLFRAME_H
