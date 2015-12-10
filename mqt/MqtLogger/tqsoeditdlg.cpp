#include "tqsoeditdlg.h"
#include "ui_tqsoeditdlg.h"

TQSOEditDlg::TQSOEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TQSOEditDlg)
{
    ui->setupUi(this);
}

TQSOEditDlg::~TQSOEditDlg()
{
    delete ui;
}
