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

signals:
    void showLogDialog();


private slots:


    void on_logDirBrowsePb_clicked();
    void on_logFilenameEdit_textChanged(const QString &arg1);
    void loadLogConfig();

private:
    Ui::LogDialog *ui;
    QString directory;
    QString filename;
    int bearingDiff;
    bool logEnabled;
    int polltime;       // file write interval

    void saveLogConfig();
    void readLogConfig();
};

#endif // LOGDIALOG_H
