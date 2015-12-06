#include "tcalendarform.h"
#include "ui_tcalendarform.h"

TCalendarForm::TCalendarForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TCalendarForm)
{
    ui->setupUi(this);
}

TCalendarForm::~TCalendarForm()
{
    delete ui;
}
