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

#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QRadioButton>
#include "logger_pch.h"
#include "RPCCommandConstants.h"
#include "rigmemcommondata.h"
#include "rigmemdialog.h"



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
    void setRxPBFlag(QString);

signals:
    void sendFreqControl(QString);
    void noRadioSendFreq(QString);
    void sendModeToControl(QString);
    void sendPassBandStateToControl(int);

private slots:
    void freqLineEditInFocus();
    void changeRadioFreq();
    void radioBandFreq(int index);
    void noRadioSetFreq(QString);

    void memoryUpdate(int);

    void exitFreqEdit();
    void readActionSelected(int);
    void editActionSelected(int buttonNumber);
    void writeActionSelected(int);
    void clearActionSelected(int);
    void passBandRadioSelected(int button);




private:

    QToolButton* memButtons[memoryData::NUM_MEMORIES];
    QMenu* memoryMenu[memoryData::NUM_MEMORIES];
    QAction* readAction[memoryData::NUM_MEMORIES];
    QAction* writeAction[memoryData::NUM_MEMORIES];
    QAction* editAction[memoryData::NUM_MEMORIES];
    QAction* clearAction[memoryData::NUM_MEMORIES];



    QRadioButton* pBandButton[3];
    RigMemDialog* memDialog;

    bool radioLoaded;
    bool isRadioLoaded();
    bool freqEditOn;
    //bool memFlag;
    QString curFreq;
    QString curMode;
    int curpbState;
    QString radioName;
    QString radioState;
    bool rxPBFlag;

    memoryData::memData logData;

    void sendModeToRadio(QString);
    void freqLineEditBkgnd(bool status);
    void freqLineEditFrameColour(bool status);
    void keyPressEvent(QKeyEvent *event);

    void initRigFrame();
    void initMemoryButtons();
    void loadMemoryButtonLabels();
    void initPassBandRadioButtons();
    void noRadioSendOutFreq(QString f);
signals:
    void escapePressed();

};


class FreqLineEdit : public QLineEdit
{
    Q_OBJECT

public:

    FreqLineEdit(QWidget *parent);
    ~FreqLineEdit();


    void focusInEvent( QFocusEvent* ) ;

signals:
    void receivedFocus() ;

private:


} ;


#endif // RIGCONTROLFRAME_H
