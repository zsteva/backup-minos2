#include "tclockdlg.h"
#include "ui_tclockdlg.h"

TClockDlg::TClockDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TClockDlg)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

TClockDlg::~TClockDlg()
{
    delete ui;
}
