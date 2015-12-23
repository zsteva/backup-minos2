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

private slots:
    void on_CloseListButton_clicked();

    void on_ExitButton_clicked();

private:
    Ui::TManageListsDlg *ui;
    void DrawList();
};

#endif // TMANAGELISTSDLG_H
