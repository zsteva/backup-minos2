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

signals:
    void sendFreqControl(QString);
    void sendModeToControl(QString);
    void sendModeLogFrame(QString);

private slots:
    void freqLineEditInFocus();
    void changeRadioFreq();
    void radioBandFreq(int index);

    void memoryUpdate(int);


    void readActionSelected(int);
    void writeActionSelected(int);
    void clearActionSelected(int);

private:

    QToolButton* memButtons[memoryData::NUM_MEMORIES];
    QMenu* memoryMenu[memoryData::NUM_MEMORIES];
    QAction* readAction[memoryData::NUM_MEMORIES];
    QAction* writeAction[memoryData::NUM_MEMORIES];
    QAction* clearAction[memoryData::NUM_MEMORIES];
    RigMemDialog* memDialog;

    bool radioLoaded;
    bool isRadioLoaded();
    bool freqEditOn;
    bool memReadFlag;
    QString curFreq;
    QString curMode;
    QString radioName;
    QString radioState;

    memoryData::memData logData;

    void sendModeToRadio(QString);
    void freqLineEditBkgnd(bool status);
    void freqLineEditFrameColour(bool status);
    void exitFreqEdit();
    void keyPressEvent(QKeyEvent *event);

    void initRigFrame();
    void initMemoryButtons();
    void loadMemoryButtonLabels();
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
