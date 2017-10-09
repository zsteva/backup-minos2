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

class RigControlFrame;
class RigMemoryButton : public QObject
{
    Q_OBJECT

public:
    explicit RigMemoryButton(QWidget *parent, RigControlFrame *rcf, int no);
    ~RigMemoryButton();

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

    LoggerContestLog *ct;

public:
    explicit RigControlFrame(QWidget *parent);
    ~RigControlFrame();

    Ui::RigControlFrame *ui;

    void setContest(BaseContestLog *);

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

    void exitFreqEdit();

    void on_FontChanged();

    void on_newMemoryButton_clicked();
public slots:
    void memoryUpdate(int);

    void readActionSelected(int);
    void editActionSelected(int buttonNumber);
    void writeActionSelected(int);
    void clearActionSelected(int);

    void runButtonUpdate(int);

    void runButReadActSel(int buttonNumber);
    void runButWriteActSel(int buttonNumber);
    void runButEditActSel(int buttonNumber);
    void runButClearActSel(int buttonNumber);

    void passBandRadioSelected(int button);
private:

    // memory buttons
    memoryData::memData getRigMemoryData(int memoryNumber);
    memoryData::memData getRunMemoryData(int memoryNumber);

    void setRigMemoryData(int memoryNumber, memoryData::memData m);
    void setRunMemoryData(int memoryNumber, memoryData::memData m);

    QMap<int, RigMemoryButton *> memButtonMap;
    QMap<int, RunMemoryButton *> runButtonMap;


    QShortcut *freqEditKey;
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
    void keyPressEvent(QKeyEvent *event);

    void initRigFrame(QWidget *parent);
    void loadMemoryButtonLabels();
    void initPassBandRadioButtons();
    void noRadioSendOutFreq(QString f);

    void initRunMemoryButton();
    void loadRunButtonLabels();

    void traceMsg(QString msg);

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
