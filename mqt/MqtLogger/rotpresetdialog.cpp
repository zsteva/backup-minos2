/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2018
//
// Interprocess Control Logic
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2017
//
//
//
/////////////////////////////////////////////////////////////////////////////


#include <QMessageBox>
#include "rotpresetdialog.h"
#include "ui_rotpresetdialog.h"


RotPresetDialog::RotPresetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RotPresetDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->bearingEdit, SIGNAL(editingFinished()), this, SLOT(bearingEditFinished()));

}

RotPresetDialog::~RotPresetDialog()
{
    delete ui;
}




void RotPresetDialog::bearingEditFinished()
{
    int b = ui->bearingEdit->text().toInt();
    if (b < 0 || b > 360)
    {
        QMessageBox::information(this, tr("Bearing Out of range"),
                                 tr("Bearing: %1, out of range, should be 0-360 \nPlease enter another bearing").arg(b),
                                  QMessageBox::Ok|QMessageBox::Default,
                                  QMessageBox::NoButton, QMessageBox::NoButton);
    }
}


void RotPresetDialog::setPresetDialogData(RotPresetData& _editData, RotPresetData& _curData )
{

    editData = _editData;
    ui->nameEdit->setText(_curData.name);
    ui->bearingEdit->setText(_curData.bearing);
    ui->nameEdit->setFocus();
}
