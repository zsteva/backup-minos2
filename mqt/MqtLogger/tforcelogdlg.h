#ifndef TFORCELOGDLG_H
#define TFORCELOGDLG_H

#include <QDialog>

namespace Ui {
class TForceLogDlg;
}

class TForceLogDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TForceLogDlg(QWidget *parent = 0);
    ~TForceLogDlg();

private:
    Ui::TForceLogDlg *ui;
};

#endif // TFORCELOGDLG_H
