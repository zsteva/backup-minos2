#include "addradiodialog.h"
#include "ui_addradiodialog.h"
#include <QMessageBox>

AddRadioDialog::AddRadioDialog(QStringList _availRadios, RigControl* rig, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddRadioDialog)
{
    ui->setupUi(this);
    availRadios = _availRadios;


    rig->getRigList(ui->radioModel);
    connect (ui->radioName, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    connect (ui->radioModel, SIGNAL(currentIndexChanged(int)), this, SLOT(radioModelSelect(int)));
    connect (ui->buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));
}

AddRadioDialog::~AddRadioDialog()
{
    delete ui;
}


void AddRadioDialog::editingFinished()
{
    if (ui->radioName->text() != "")
    {
        if (availRadios.contains(ui->radioName->text()))
        {
            QMessageBox msgBox;
            msgBox.setText("Radio name already exists,\n please use another name");
            msgBox.exec();
            ui->radioName->setFocus();
        }
    }

    radioName = ui->radioName->text().trimmed();

}


void AddRadioDialog::radioModelSelect(int /*index*/)
{

    radioModel = ui->radioModel->currentText();

}


void AddRadioDialog::accepted()
{

    if (radioModel == "")
    {
        radioModel = ui->radioModel->currentText();
    }

}



QString AddRadioDialog::getRadioName()
{
    return radioName;
}

QString AddRadioDialog::getRadioModel()
{
    return radioModel;
}
