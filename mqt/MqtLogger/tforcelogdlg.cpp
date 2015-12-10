#include "tforcelogdlg.h"
#include "ui_tforcelogdlg.h"

TForceLogDlg::TForceLogDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TForceLogDlg)
{
    ui->setupUi(this);
}

TForceLogDlg::~TForceLogDlg()
{
    delete ui;
}
