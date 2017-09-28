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





#ifndef RUNBUTTONDIALOG_H
#define RUNBUTTONDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QKeyEvent>
#include "rigmemcommondata.h"

namespace Ui {
class RunButtonDialog;
}

class RunButtonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunButtonDialog(QString, QString, QWidget *parent = 0);
    ~RunButtonDialog();

    void setDialogTitle(QString number);
    void setLogData(memoryData::memData*, int buttonNumber);
    void setRadioName(QString);
    void setRadioState(QString);
    int readAllMemories();
    memoryData::memData getMemoryData(int memoryNumber);
    void clearMemory(memoryData::memData* ldata, int memoryLoc);
    void setFocusFreq();
    void editMemory(int memoryLoc);
    void setRxPbFlag(bool flag);

signals:
    void runButtonSaved(int memoryNumber);

private slots:
    void saveButtonPushed();
    void cancelButtonPushed();

    void escapePushed();
    void closeMemory();

private:
    Ui::RunButtonDialog *ui;
    memoryData::memData memoryList[runButData::NUM_RUNBUTTONS];
    QString radioName;
    QString radioState;
    int memoryNumber;
    bool  rxPbFlag;
    int readSettings();
    int saveMemory(int memoryLoc);
    int readMemory(int memoryLoc);


    void keyPressEvent(QKeyEvent*);

signals:
    void escapePressed();
    void returnPressed();

};

#endif // RUNBUTTONDIALOG_H
