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
#include "base_pch.h"


#include "rigmemcommondata.h"

namespace Ui {
class RunButtonDialog;
}

class RunButtonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunButtonDialog(QWidget *parent = nullptr);
    ~RunButtonDialog();

    void setLogData(memoryData::memData*, int buttonNumber);


private slots:
    void on_okButton_clicked();
    void on_cancelbutton_clicked();

    void onFreqEditFinish();
private:
    Ui::RunButtonDialog *ui;
    memoryData::memData *logdata = nullptr;
    int memoryNumber;

    bool  rxPbFlag;
};

#endif // RUNBUTTONDIALOG_H
