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

namespace Ui {
class RotPresetDialog;
}


class RotPresetData
{

public:

    RotPresetData(int _num = 0, QString _name = "", QString _bearing = "")
    {
        num = _num;
        name = _name;
        bearing = _bearing;
    }

    int num;
    QString name;
    QString bearing;

};



class RotPresetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RotPresetDialog(RotPresetData& editData, RotPresetData& curData, QWidget *parent = 0);
    ~RotPresetDialog();

private slots:
    void bearingEditFinished();
private:
    Ui::RotPresetDialog *ui;
    RotPresetData editData;

};

#endif // ROTPRESETDIALOG_H
