#include "tcontactlistdetails.h"
#include "ui_tcontactlistdetails.h"

TContactListDetails::TContactListDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TContactListDetails)
{
    ui->setupUi(this);
}

TContactListDetails::~TContactListDetails()
{
    delete ui;
}
