#ifndef TCLOCKDLG_H
#define TCLOCKDLG_H

#include <QDialog>

namespace Ui {
class TClockDlg;
}

class TClockDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TClockDlg(QWidget *parent = 0);
    ~TClockDlg();

private:
    Ui::TClockDlg *ui;
};

#endif // TCLOCKDLG_H
