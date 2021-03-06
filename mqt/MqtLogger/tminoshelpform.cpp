#include "tminoshelpform.h"
#include "ui_tminoshelpform.h"

TMinosHelpForm::TMinosHelpForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TMinosHelpForm)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

TMinosHelpForm::~TMinosHelpForm()
{
    delete ui;
}
void TMinosHelpForm::setText(const QString &text)
{
    ui->helpText->setText(text);
}

void TMinosHelpForm::on_CloseButton_clicked()
{
    accept();
}
