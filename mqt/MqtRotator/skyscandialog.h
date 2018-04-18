/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////




#ifndef SKYSCANDIALOG_H
#define SKYSCANDIALOG_H

#include <QDialog>

namespace Ui {
class SkyScanDialog;
}

class SkyScanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SkyScanDialog(QWidget *parent = nullptr);
    ~SkyScanDialog();

private:
    Ui::SkyScanDialog *ui;
};

#endif // SKYSCANDIALOG_H
