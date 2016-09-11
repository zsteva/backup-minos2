#include "skyscandialog.h"
#include "ui_skyscandialog.h"

SkyScanDialog::SkyScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SkyScanDialog)
{
    ui->setupUi(this);
}

SkyScanDialog::~SkyScanDialog()
{
    delete ui;
}
