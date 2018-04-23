#include "addantennadialog.h"
#include "ui_addantennadialog.h"
#include <QMessageBox>

AddAntennaDialog::AddAntennaDialog(QStringList _availAntennas, RotControl* rot, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAntennaDialog)
{
    ui->setupUi(this);
    availAntennas = _availAntennas;

    rot->getRotatorList(ui->rotatorModel);
    connect (ui->antennaName, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    connect (ui->rotatorModel, SIGNAL(currentIndexChanged(int)), this, SLOT(rotatorModelSelect(int)));
    connect (ui->buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));

}

AddAntennaDialog::~AddAntennaDialog()
{
    delete ui;
}



void AddAntennaDialog::editingFinished()
{
    if (ui->antennaName->text() != "")
    {
        if (availAntennas.contains(ui->antennaName->text()))
        {
            QMessageBox msgBox;
            msgBox.setText("Radio name already exists,\n please use another name");
            msgBox.exec();
            ui->antennaName->setFocus();
        }
    }

    antennaName = ui->antennaName->text().trimmed();

}


void AddAntennaDialog::rotatorModelSelect(int /*index*/)
{

    rotatorModel = ui->rotatorModel->currentText();

}


void AddAntennaDialog::accepted()
{

    if (rotatorModel == "")
    {
        rotatorModel = ui->rotatorModel->currentText();
    }

}



QString AddAntennaDialog::getAntennaName()
{
    return antennaName;
}

QString AddAntennaDialog::getRotatorModel()
{
    return rotatorModel;
}

