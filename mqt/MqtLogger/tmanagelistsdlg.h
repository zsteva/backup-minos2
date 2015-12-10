#ifndef TMANAGELISTSDLG_H
#define TMANAGELISTSDLG_H

#include <QDialog>

namespace Ui {
class TManageListsDlg;
}

class TManageListsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TManageListsDlg(QWidget *parent = 0);
    ~TManageListsDlg();

private:
    Ui::TManageListsDlg *ui;
};

#endif // TMANAGELISTSDLG_H
