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
#include <QShortcut>
#include "logger_pch.h"
#include "RPCCommandConstants.h"
#include "rigmemcommondata.h"
#include "rigmemdialog.h"
#include "runbuttondialog.h"


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
    bool isRadioLoaded();
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
    void memoryShortCutSelected(int buttonNumber);
    void readActionSelected(int);
    void editActionSelected(int buttonNumber);
    void writeActionSelected(int);
    void clearActionSelected(int);
    void passBandRadioSelected(int button);

    void runButShortCutSel(int buttonNumber);
    void runButReadActSel(int buttonNumber);
    void runButWriteActSel(int buttonNumber);
    void runButEditActSel(int buttonNumber);
    void runButClearActSel(int buttonNumber);
    void runButtonUpdate(int buttonNumber);



    void on_FontChanged();




private:

    // memory buttons

    QToolButton* memButtons[memoryData::NUM_MEMORIES];
    QMenu* memoryMenu[memoryData::NUM_MEMORIES];
    QShortcut* shortKey[memoryData::NUM_MEMORIES];
    QAction* readAction[memoryData::NUM_MEMORIES];
    QAction* writeAction[memoryData::NUM_MEMORIES];
    QAction* editAction[memoryData::NUM_MEMORIES];
    QAction* clearAction[memoryData::NUM_MEMORIES];

    // run button
    QToolButton* runButton[runButData::NUM_RUNBUTTONS];
    QMenu* runButMenu[runButData::NUM_RUNBUTTONS];
    QShortcut* runButShortKey[runButData::NUM_RUNBUTTONS];
    QAction* runButReadAct[runButData::NUM_RUNBUTTONS];
    QAction* runButWriteAct[runButData::NUM_RUNBUTTONS];
    QAction* runButEditAct[runButData::NUM_RUNBUTTONS];
    QAction* runButClearAct[runButData::NUM_RUNBUTTONS];


    QShortcut *freqEditKey;
    QLabel *freqLabel;

    QRadioButton* pBandButton[3];
    RigMemDialog* memDialog;
    RunButtonDialog* runDialog;

    bool radioLoaded;
    bool freqEditOn;
    //bool memFlag;
    QString curFreq;
    QString curMode;
    int curpbState;
    QString radioName;
    QString radioState;
    bool rxPBFlag;

    memoryData::memData logData;
    memoryData::memData runData;

    void sendModeToRadio(QString);
    void freqLineEditBkgnd(bool status);
    void freqLineEditFrameColour(bool status);
    void keyPressEvent(QKeyEvent *event);

    void initRigFrame(QWidget *parent);
    void initMemoryButtons(QWidget *parent);
    void loadMemoryButtonLabels();
    void initPassBandRadioButtons();
    void noRadioSendOutFreq(QString f);

    void initRunMemoryButton(QWidget *parent);


    void traceMsg(QString msg);
    void loadRunButtonLabels();

    QString extractKhz(QString f);
    void loadMemories();
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
