/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////



#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QWidget *parent = 0);
    ~LogDialog();

private slots:


    void on_logDirBrowsePb_clicked();
    void on_logFileBrowsePb_clicked();

private:
    Ui::LogDialog *ui;

    int polltime;       // file write interval

};

#endif // LOGDIALOG_H
