#include "tloccalcform.h"
#include "ui_tloccalcform.h"

TLocCalcForm::TLocCalcForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TLocCalcForm)
{
    ui->setupUi(this);
}

TLocCalcForm::~TLocCalcForm()
{
    delete ui;
}
