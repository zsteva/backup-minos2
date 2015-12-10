#include "tminosbandchooser.h"
#include "ui_tminosbandchooser.h"

TMinosBandChooser::TMinosBandChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TMinosBandChooser)
{
    ui->setupUi(this);
}

TMinosBandChooser::~TMinosBandChooser()
{
    delete ui;
}
