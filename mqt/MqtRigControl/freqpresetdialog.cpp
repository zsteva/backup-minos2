/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2018
//
//
//
//
/////////////////////////////////////////////////////////////////////////////






#include <QMessageBox>
#include "freqpresetdialog.h"
#include "ui_freqpresetdialog.h"

FreqPresetDialog::FreqPresetDialog(QStringList& _presetFreq, const QVector<BandDetail*> _band, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FreqPresetDialog)
{
    ui->setupUi(this);

    bands = _band;
    presetFreq = _presetFreq;

    loadSettingsToDialog();

    connect (ui->lineEdit_10m, SIGNAL(editingFinished()), this, SLOT(b_10mSelected()));
    connect (ui->lineEdit_6m, SIGNAL(editingFinished()), this, SLOT(b_6mSelected()));
    connect (ui->lineEdit_4m, SIGNAL(editingFinished()), this, SLOT(b_4mSelected()));
    connect (ui->lineEdit_2m, SIGNAL(editingFinished()), this, SLOT(b_2mSelected()));
    connect (ui->lineEdit_70cm, SIGNAL(editingFinished()), this, SLOT(b_70cmSelected()));

    connect (ui->lineEdit_23cm, SIGNAL(editingFinished()), this, SLOT(b_23cmSelected()));
    connect (ui->lineEdit_13cm, SIGNAL(editingFinished()), this, SLOT(b_13cmSelected()));
    connect (ui->lineEdit_9cm, SIGNAL(editingFinished()), this, SLOT(b_9cmSelected()));
    connect (ui->lineEdit_6cm, SIGNAL(editingFinished()), this, SLOT(b_6cmSelected()));
    connect (ui->lineEdit_3cm, SIGNAL(editingFinished()), this, SLOT(b_3cmSelected()));
    connect (ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect (ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelSettings()));


}

FreqPresetDialog::~FreqPresetDialog()
{
    delete ui;
}



void FreqPresetDialog::b_10mSelected()
{

    getFreq(ui->lineEdit_10m, _10M);

}


void FreqPresetDialog::b_6mSelected()
{
    getFreq(ui->lineEdit_6m, _6M);
}

void FreqPresetDialog::b_4mSelected()
{
    getFreq(ui->lineEdit_4m, _4M);
}


void FreqPresetDialog::b_2mSelected()
{
    getFreq(ui->lineEdit_2m, _2M);
}

void FreqPresetDialog::b_70cmSelected()
{
    getFreq(ui->lineEdit_70cm, _70CM);
}
void FreqPresetDialog::b_23cmSelected()
{
    getFreq(ui->lineEdit_23cm, _23CM);
}


void FreqPresetDialog::b_13cmSelected()
{
    getFreq(ui->lineEdit_13cm, _13CM);
}

void FreqPresetDialog::b_9cmSelected()
{
    getFreq(ui->lineEdit_9cm, _9CM);
}


void FreqPresetDialog::b_6cmSelected()
{
    getFreq(ui->lineEdit_6cm, _6CM);
}


void FreqPresetDialog::b_3cmSelected()
{
    getFreq(ui->lineEdit_3cm, _3CM);
}


void FreqPresetDialog::getFreq(QLineEdit* f_box, bandOffSet band)
{

    QString freq = f_box->text().trimmed().remove( QRegExp("^[0]*"));
    if (valInputFreq(freq, RADIO_FREQ_EDIT_ERR_MSG))
    {
       freq = convertSinglePeriodFreqToFullDigit(freq).remove('.');


       // check in band
       if (checkInBand(freq.toDouble(), band))
       {
           presetFreq[band] = freq;
           freqChanged = true;
       }
    }

}




// check in band

bool FreqPresetDialog::checkInBand(double freq, bandOffSet band)
{
    if (freq >= bands[band]->fLow && freq <= bands[band]->fHigh)
    {
        return true;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Freq. is out of band for %1").arg(bands[band]->name));
        msgBox.exec();
        return false;
    }
}






void FreqPresetDialog::saveSettings()
{

    if (!freqChanged)
    {
        return;
    }

    freqChanged = false;

    QString fileName;
    fileName = RADIO_PATH_LOGGER + FILENAME_FREQ_PRESETS;

    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup("FreqPresets");
    config.setValue("10M", presetFreq[_10M]);
    config.setValue("6M", presetFreq[_6M]);
    config.setValue("4M", presetFreq[_4M]);
    config.setValue("2M", presetFreq[_2M]);
    config.setValue("70CM", presetFreq[_70CM]);
    config.setValue("23CM", presetFreq[_23CM]);
    config.setValue("13CM", presetFreq[_13CM]);
    config.setValue("9CM", presetFreq[_9CM]);
    config.setValue("6CM", presetFreq[_6CM]);
    config.setValue("3CM", presetFreq[_3CM]);
    config.endGroup();

}


void FreqPresetDialog::readSettings(QStringList& _presetFreq)  // static
{
    QString fileName;
    fileName = RADIO_PATH_LOGGER + FILENAME_FREQ_PRESETS;

    QSettings config(fileName, QSettings::IniFormat);

    _presetFreq.clear();
    config.beginGroup("FreqPresets");

    _presetFreq.append(config.value("10M", bandFreq[_10M]).toString());
    _presetFreq.append(config.value("6M", bandFreq[_6M]).toString());
    _presetFreq.append(config.value("4M", bandFreq[_4M]).toString());
    _presetFreq.append(config.value("2M", bandFreq[_2M]).toString());
    _presetFreq.append(config.value("70CM", bandFreq[_70CM]).toString());
    _presetFreq.append(config.value("23CM", bandFreq[_23CM]).toString());
    _presetFreq.append(config.value("13CM", bandFreq[_13CM]).toString());
    _presetFreq.append(config.value("9CM", bandFreq[_9CM]).toString());
    _presetFreq.append(config.value("6CM", bandFreq[_6CM]).toString());
    _presetFreq.append(config.value("3CM", bandFreq[_3CM]).toString());

    config.endGroup();

}


void FreqPresetDialog::cancelSettings()
{

    if (!freqChanged)
    {
        return;
    }

    freqChanged = false;
    readSettings(presetFreq);
    loadSettingsToDialog();



}


void FreqPresetDialog::loadSettingsToDialog()
{
    ui->lineEdit_10m->setText(convertFreqStrDispSingle(presetFreq[_10M]));
    ui->lineEdit_6m->setText(convertFreqStrDispSingle(presetFreq[_6M]));
    ui->lineEdit_4m->setText(convertFreqStrDispSingle(presetFreq[_4M]));
    ui->lineEdit_2m->setText(convertFreqStrDispSingle(presetFreq[_2M]));
    ui->lineEdit_70cm->setText(convertFreqStrDispSingle(presetFreq[_70CM]));
    ui->lineEdit_23cm->setText(convertFreqStrDispSingle(presetFreq[_23CM]));
    ui->lineEdit_13cm->setText(convertFreqStrDispSingle(presetFreq[_13CM]));
    ui->lineEdit_9cm->setText(convertFreqStrDispSingle(presetFreq[_9CM]));
    ui->lineEdit_6cm->setText(convertFreqStrDispSingle(presetFreq[_6CM]));
    ui->lineEdit_3cm->setText(convertFreqStrDispSingle(presetFreq[_3CM]));
}
