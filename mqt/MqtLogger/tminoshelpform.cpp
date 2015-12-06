#include "tminoshelpform.h"
#include "ui_tminoshelpform.h"

TMinosHelpForm::TMinosHelpForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TMinosHelpForm)
{
    ui->setupUi(this);
}

TMinosHelpForm::~TMinosHelpForm()
{
    delete ui;
}
