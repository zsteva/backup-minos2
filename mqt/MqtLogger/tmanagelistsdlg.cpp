#include "tmanagelistsdlg.h"
#include "ui_tmanagelistsdlg.h"

TManageListsDlg::TManageListsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TManageListsDlg)
{
    ui->setupUi(this);
}

TManageListsDlg::~TManageListsDlg()
{
    delete ui;
}
