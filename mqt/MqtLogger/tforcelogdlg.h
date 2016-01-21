#ifndef TFORCELOGDLG_H
#define TFORCELOGDLG_H

#include "logger_pch.h"
#include <QDialog>

//class BaseContestLog;
//class ScreenContact;
//class ErrorList;

namespace Ui {
class TForceLogDlg;
}

class TForceLogDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TForceLogDlg(QWidget *parent);
    ~TForceLogDlg();

    int doexec(BaseContestLog *contest, ScreenContact &screenContact, ErrorList &errs);

private slots:
    void on_OKButton_clicked();

    void on_LocCalcButton_clicked();

    void on_CancelButton_clicked();

private:
    Ui::TForceLogDlg *ui;
};

#endif // TFORCELOGDLG_H
