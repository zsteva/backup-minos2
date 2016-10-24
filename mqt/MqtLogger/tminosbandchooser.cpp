#include "tminosbandchooser.h"
#include "ui_tminosbandchooser.h"

TMinosBandChooser::TMinosBandChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TMinosBandChooser)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

TMinosBandChooser::~TMinosBandChooser()
{
    delete ui;
}
