/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) D. G. Balharrie M0DGB/G8FKH
//
//
/////////////////////////////////////////////////////////////////////////////



#include <QSignalMapper>
#include <QMessageBox>
#include <QSettings>
#include <QPushButton>
#include <QKeyEvent>

#include "editpresetsdialog.h"
#include "ui_editpresetsdialog.h"

#include <QtDebug>

EditPresetsDialog::EditPresetsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditPresetsDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    presetNameLineEdit[0] = ui->presetNameEdit1;
    presetNameLineEdit[1] = ui->presetNameEdit2;
    presetNameLineEdit[2] = ui->presetNameEdit3;
    presetNameLineEdit[3] = ui->presetNameEdit4;
    presetNameLineEdit[4] = ui->presetNameEdit5;
    presetNameLineEdit[5] = ui->presetNameEdit6;
    presetNameLineEdit[6] = ui->presetNameEdit7;
    presetNameLineEdit[7] = ui->presetNameEdit8;
    presetNameLineEdit[8] = ui->presetNameEdit9;
    presetNameLineEdit[9] = ui->presetNameEdit10;




    presetBearingLineEdit[0] = ui->presetBearingEdit1;
    presetBearingLineEdit[1] = ui->presetBearingEdit2;
    presetBearingLineEdit[2] = ui->presetBearingEdit3;
    presetBearingLineEdit[3] = ui->presetBearingEdit4;
    presetBearingLineEdit[4] = ui->presetBearingEdit5;
    presetBearingLineEdit[5] = ui->presetBearingEdit6;
    presetBearingLineEdit[6] = ui->presetBearingEdit7;
    presetBearingLineEdit[7] = ui->presetBearingEdit8;
    presetBearingLineEdit[8] = ui->presetBearingEdit9;
    presetBearingLineEdit[9] = ui->presetBearingEdit10;



    clearPresets();

    QSignalMapper *name_mapper = new QSignalMapper(this);


    for (int i = 0; i < NUM_PRESETS; i++ )
    {
        name_mapper->setMapping(presetNameLineEdit[i], i);
        connect(presetNameLineEdit[i], SIGNAL(editingFinished()), name_mapper, SLOT(map()));


    }
    connect(name_mapper, SIGNAL(mapped(int)), this, SLOT(nameReturnPressed(int)));

    QSignalMapper *bearing_mapper = new QSignalMapper(this);



    for (int i=0; i < NUM_PRESETS; i++)
    {
        bearing_mapper->setMapping(presetBearingLineEdit[i], i);
        connect(presetBearingLineEdit[i], SIGNAL(editingFinished()), bearing_mapper, SLOT(map()));
    }
    connect(bearing_mapper, SIGNAL(mapped(int)), this, SLOT(bearingReturnPressed(int)));

    ui->editPresetbuttonBox->button(QDialogButtonBox::Ok)->setText("Save");

    connect(ui->editPresetbuttonBox, SIGNAL(accepted()), this, SLOT(saveButtonPushed()));
    connect(ui->editPresetbuttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonPushed()));
}


EditPresetsDialog::~EditPresetsDialog()
{
    delete ui;
}


void EditPresetsDialog::nameReturnPressed(int boxNumber)
{

    if (presetNameLineEdit[boxNumber]->text() != rotPresets[boxNumber]->name)
    {
        rotPresets[boxNumber]->name = presetNameLineEdit[boxNumber]->text();
        presetNameUpdated[boxNumber] = true;
        presetValueChanged = true;

    }
}



void EditPresetsDialog::bearingReturnPressed(int boxNumber)
{
    bool ok;

    if (presetBearingLineEdit[boxNumber]->text() != "")
    {
        int bearing = presetBearingLineEdit[boxNumber]->text().toInt(&ok, 10);
        if (bearing >= 0 && bearing <= 359 && ok)
        {
            if (presetBearingLineEdit[boxNumber]->text() != rotPresets[boxNumber]->bearing)
            {
                rotPresets[boxNumber]->bearing = presetBearingLineEdit[boxNumber]->text();
                presetBearingUpdated[boxNumber] = true;
                presetValueChanged = true;

            }
        }
        else
        {
            QMessageBox::critical(this, tr("Bearing Error"), tr("Invalid Bearing\nPlease enter 0 - 359"));
        }
     }
    else
    {
        // clear text store
        rotPresets[boxNumber]->bearing = presetBearingLineEdit[boxNumber]->text();
        presetBearingUpdated[boxNumber] = true;
    }
}





void EditPresetsDialog:: savePresets()
{


    QSettings config("./Configuration/MinosRotatorConfig.ini", QSettings::IniFormat);
    config.beginGroup("Presets");

    for (int i = 0; i < NUM_PRESETS; i++)
    {
        if (presetNameUpdated[i])
        {

            config.setValue("preset" + QString::number(i+1), rotPresets[i]->name);
        }

        if (presetBearingUpdated[i])
        {
           config.setValue("bearing" + QString::number(i+1), rotPresets[i]->bearing);
        }

    }

    config.endGroup();
    emit updatePresetButtonLabels();
}


void EditPresetsDialog::lineEditReturn()
{
    if (presetValueChanged)
    {
        savePresets();
        closePresets();
    }

}



void EditPresetsDialog::readPresets()
{
    QSettings config("./Configuration/MinosRotatorConfig.ini", QSettings::IniFormat);
    config.beginGroup("Presets");

    for (int i = 0; i < NUM_PRESETS; i++)
    {
        rotPresets.append(new RotPresetData(i, config.value("preset" +  QString::number(i+1)).toString(),
                                        config.value("bearing" +  QString::number(i+1)).toString()));
        //presetName[i] = config.value("preset" +  QString::number(i+1)).toString();
        //presetBearing[i] = config.value("bearing" +  QString::number(i+1)).toString();
    }

    config.endGroup();
}



void EditPresetsDialog::loadPresetEditFieldsShow()
{

    readPresets();
    for (int i = 0; i < NUM_PRESETS; i++)
    {
        presetNameLineEdit[i]->setText(rotPresets[i]->name);
        presetBearingLineEdit[i]->setText(rotPresets[i]->bearing);
    }
    emit showEditPresetDialog();

}




void EditPresetsDialog::saveButtonPushed()
{
    savePresets();
}


void EditPresetsDialog::cancelButtonPushed()
{

    clearPresets();
}


void EditPresetsDialog::clearPresets()
{
    rotPresets.clear();

    for (int i = 0; i < NUM_PRESETS; i++)
    {
        //presetName[i] = EMPTY_PRESET_NAME;
        //presetBearing[i] = "";
        presetBearingUpdated[i] = false;
        presetNameUpdated[i] = false;
        presetValueChanged = false;
    }


}

void EditPresetsDialog::closePresets()
{

    clearPresets();
    close();


}


void EditPresetsDialog::keyPressEvent(QKeyEvent *event)
{

    switch (event->key()) {
    case Qt::Key_Escape:
        closePresets();
    break;
    default:
        break;
    }
}
