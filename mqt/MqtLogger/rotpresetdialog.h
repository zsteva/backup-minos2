/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2018
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2017
//
//
//
/////////////////////////////////////////////////////////////////////////////



#ifndef ROTPRESETDIALOG_H
#define ROTPRESETDIALOG_H

#include <QDialog>
#include "rotatorcommon.h"

namespace Ui {
class RotPresetDialog;
}






class RotPresetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RotPresetDialog(RotPreset& editData, RotPreset& curData, QWidget *parent = 0);
    ~RotPresetDialog();

private slots:
    void bearingEditFinished();
private:
    Ui::RotPresetDialog *ui;
    RotPreset editData;

};

#endif // ROTPRESETDIALOG_H
