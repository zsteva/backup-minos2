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
    explicit RunButtonDialog(QWidget *parent = 0);
    ~RunButtonDialog();

    void setLogData(memoryData::memData*, int buttonNumber);


    void setFreqCurPos(int pos);
private slots:
    void on_okButton_clicked();
    void on_cancelbutton_clicked();

private:
    Ui::RunButtonDialog *ui;
    memoryData::memData *logdata = 0;
    int memoryNumber;

    bool  rxPbFlag;
};

#endif // RUNBUTTONDIALOG_H
