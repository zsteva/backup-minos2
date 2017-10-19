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


namespace Ui {
    class RigControlFrame;
}

class RigControlFrame;
class RunMemoryButton : public QObject
{
    Q_OBJECT

public:
    explicit RunMemoryButton(QToolButton *b, RigControlFrame *rcf, int no);
    ~RunMemoryButton();

    RigControlFrame *rigControlFrame;
    QToolButton* memButton;
    QMenu* memoryMenu;
    QShortcut* shortKey;
    QAction* readAction;
    QAction* writeAction;
    QAction* editAction;
    QAction* clearAction;

    int memNo;
private slots:
    void memoryUpdate();

    void memoryShortCutSelected();
    void readActionSelected();
    void editActionSelected();
    void writeActionSelected();
    void clearActionSelected();
signals:
    void clearActionSelected(int);

};

class RigControlFrame : public QFrame
{
    Q_OBJECT

public:
    explicit RigControlFrame(QWidget *parent);
    ~RigControlFrame();

    Ui::RigControlFrame *ui;

    void setContest(BaseContestLog *);

    void transferDetails(memoryData::memData &m);
    void getDetails(memoryData::memData &m);


    void setRadioLoaded();
    void setMode(QString);
    void setFreq(QString);
    void setRadioName(QString);
    void setRadioState(QString);
    bool isRadioLoaded();
    void setRxPBFlag(QString);

    void exitFreqEdit();
    void runButtonUpdate(int);
    void runButReadActSel(int buttonNumber);
    void runButWriteActSel(int buttonNumber);
    void runButEditActSel(int buttonNumber);

signals:
    void sendFreqControl(QString);
    void noRadioSendFreq(QString);
    void sendModeToControl(QString);
    void sendPassBandStateToControl(int);

private slots:
    void freqLineEditInFocus();
    void radioBandFreq(int index);
    void noRadioSetFreq(QString);
    void freqEditSelected();
public slots:
    void changeRadioFreq();

    void returnChangeRadioFreq();

    void runButClearActSel(int buttonNumber);

    void passBandRadioSelected(int button);
private:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;

    // memory buttons
    memoryData::memData getRunMemoryData(int memoryNumber);

    void setRunMemoryData(int memoryNumber, memoryData::memData m);

    LoggerContestLog *ct;

    QMap<int, RunMemoryButton *> runButtonMap;


    QShortcut* freqEditShortKey;

    QLabel *freqLabel;

    QRadioButton* pBandButton[3];

    bool radioLoaded;
    bool freqEditOn;
    QString curFreq;
    QString curMode;
    int curpbState;
    QString radioName;
    QString radioState;
    bool rxPBFlag;

    void sendModeToRadio(QString);
    void freqLineEditBkgnd(bool status);
    void freqLineEditFrameColour(bool status);

    void initRigFrame(QWidget *parent);
    void initPassBandRadioButtons();
    void noRadioSendOutFreq(QString f);

    void initRunMemoryButton();
    void loadRunButtonLabels();

    void traceMsg(QString msg);

    QString extractKhz(QString f);
    void loadMemories();
    void mgmLabelVisible(bool state);
};
class FreqLineEdit : public QLineEdit
{
    Q_OBJECT

public:

    FreqLineEdit(QWidget *parent);
    ~FreqLineEdit();

signals:
    void receivedFocus() ;
    void lostFocus();
    void newFreq();
private:

    void changeFreq(bool direction);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    QString convertFreqString(double frequency);
} ;

#endif // RIGCONTROLFRAME_H
