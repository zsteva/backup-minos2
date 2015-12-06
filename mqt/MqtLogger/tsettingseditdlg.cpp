#include "tsettingseditdlg.h"
#include "ui_tsettingseditdlg.h"

TSettingsEditDlg::TSettingsEditDlg(QWidget *parent, SettingsBundle *bundle) :
    QDialog(parent),
    ui(new Ui::TSettingsEditDlg),
    bundle(bundle)

{
    ui->setupUi(this);
}

TSettingsEditDlg::~TSettingsEditDlg()
{
    delete ui;
}
