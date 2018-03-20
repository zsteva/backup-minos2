#include "addtransverterdialog.h"
#include "ui_addtransverterdialog.h"
#include "BandList.h"

AddTransVerterDialog::AddTransVerterDialog(QVector<BandDetail*> _bands, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTransVerterDialog)
{
    ui->setupUi(this);
    bands = _bands;

    loadBandSel();
    connect (ui->bandSel, SIGNAL(activated(int)), this, SLOT(bandSelected(int)));

}



AddTransVerterDialog::~AddTransVerterDialog()
{
    delete ui;
}


void AddTransVerterDialog::loadBandSel()
{
    ui->bandSel->clear();
    ui->bandSel->addItem("");
    for (int i = 0; i < bands.count(); i++ )
    {
        ui->bandSel->addItem(bands[i]->name);
    }
}



void AddTransVerterDialog::bandSelected(int idx)
{
    if (idx != -1 && idx < bands.count() + 1)
    {
        bName = bands[idx-1]->name;
        fLow = bands[idx-1]->fLow;
        fHigh = bands[idx-1]->fHigh;
    }

}



QString AddTransVerterDialog::getTransVerterName()
{
    return bName;
}


double AddTransVerterDialog::getfLow()
{
    return fLow;
}

double AddTransVerterDialog::getfHigh()
{
    return fHigh;

}
