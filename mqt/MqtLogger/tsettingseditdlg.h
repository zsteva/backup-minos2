#ifndef TSETTINGSEDITDLG_H
#define TSETTINGSEDITDLG_H

#include <QDialog>

namespace Ui {
class TSettingsEditDlg;
}

class SettingsBundle;
class TSettingsEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TSettingsEditDlg(QWidget *parent , SettingsBundle *bundle);
    ~TSettingsEditDlg();

private:
    Ui::TSettingsEditDlg *ui;
    SettingsBundle *bundle;
};

#endif // TSETTINGSEDITDLG_H
