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




#include "transvertsetupform.h"
#include "rigutils.h"
#include <QLineEdit>
#include <QCheckBox>


TransVertSetupForm::TransVertSetupForm(TransVertParams *transvertData, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::transVertSetupForm),
    tansVertValueChanged(false),
    transVertNameChanged(false)
{

    ui->setupUi(this);
    transVertData = transvertData;

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    loadBands();

    connect(ui->bandSel, SIGNAL(activated(int)), this, SLOT(bandSelected()));
    connect(ui->TransVertOffset, SIGNAL(editingFinished()), this, SLOT(transVertOffsetSelected()));
    connect(ui->negCheckbox, SIGNAL(clicked(bool)), this, SLOT(negCheckBoxSelected(bool)));
    connect(ui->enableTransVertSw, SIGNAL(clicked(bool)), this, SLOT(enableTransVertSwSel(bool)));
    connect(ui->transVertSwNum, SIGNAL(editingFinished()), this, SLOT(transVertSwNumSel()));

}





/********************* Band Selection *********************************/


void TransVertSetupForm::bandSelected()
{
    if (ui->bandSel->currentText() != transVertData->band)
    {
        transVertData->band = ui->bandSel->currentText();
    }
}


void TransVertSetupForm::loadBands()
{
    BandList &blist = BandList::getBandList();
    ui->bandSel->addItem("");
    for (int i = 6; i < 15; i++)   // just load VHF/UHF bands
    {
        ui->bandSel->addItem(blist.bandList[i].adif);
    }

}

void TransVertSetupForm::setBand(QString b)
{
    ui->bandSel->setCurrentText(b);
}


QString TransVertSetupForm::getBand()
{
    return ui->bandSel->currentText();
}


/********************* TransVert Offset Freq  *********************************/


void TransVertSetupForm::transVertOffsetSelected()
{
    // check freq valid format
    QString f = ui->TransVertOffset->text().trimmed().remove( QRegExp("^[0]*"));
    if (f != "")
    {
        if (f.count('.') == 1)
        {
            QStringList fl = f.split('.');
            fl[1] = fl[1] + "000000";
            fl[1].truncate(6);
            f = fl[0] + "." + fl[1];
        }
        if (!validateFreqTxtInput(f))
        {
            // error
            QMessageBox msgBox;
            msgBox.setText(FREQ_EDIT_ERR_MSG);
            msgBox.exec();
            return;             //incorrect format
        }
        f =
        transVertData->transVertOffsetStr = convertSinglePeriodFreqToFullDigit(f).remove('.');
        transVertData->transVertOffset  =  convertStrToFreq(f);
        tansVertValueChanged = true;

    }



}

QString TransVertSetupForm::getTransVertOffsetFreq()
{
    return ui->TransVertOffset->text().trimmed();
}

void TransVertSetupForm::setTransVertOffsetFreq(QString s)
{
    ui->TransVertOffset->setText(convertFreqStrDispSingle(s));
}


/********************* TransVert Negative Offset Select  *********************************/

void TransVertSetupForm::negCheckBoxSelected(bool flag)
{
    bool checked = ui->negCheckbox->isChecked();
    if(transVertData->transVertNegative != checked)
    {
        transVertData->transVertNegative = flag;
        tansVertValueChanged = true;
    }

}

bool TransVertSetupForm::getNegCheckBox()
{
    return ui->negCheckbox->isChecked();
}

void TransVertSetupForm::setNegCheckBox(bool b)
{
    ui->negCheckbox->setChecked(b);
}

/********************* TransVert Switch Enable  *********************************/

void TransVertSetupForm::enableTransVertSwSel(bool /*flag*/)
{
    bool checked = ui->enableTransVertSw->isChecked();
    if (transVertData->enableTransSwitch != checked)
    {
        transVertData->enableTransSwitch = checked;
        setEnableTransVertSwBoxVisible(checked);
        tansVertValueChanged = true;
    }

}


bool TransVertSetupForm::getEnableTransVertSw()
{
    return ui->enableTransVertSw->isChecked();
}

void TransVertSetupForm::setEnableTransVertSw(bool b)
{
    ui->enableTransVertSw->setChecked(b);
    setEnableTransVertSwBoxVisible(b);
}

void TransVertSetupForm::setEnableTransVertSwBoxVisible(bool visible)
{
     ui->transVertSwNum->setVisible(visible);
     ui->transVertSwNumLbl->setVisible(visible);

}


/********************* TransVert Switch Number *********************************/

// need some validation here...

void TransVertSetupForm::transVertSwNumSel()
{
    QString numSel = ui->transVertSwNum->text().trimmed();
    transVertData->transSwitchNum = numSel;
    tansVertValueChanged = true;
}



QString TransVertSetupForm::getTransVerSwNum()
{
    return ui->transVertSwNum->text().trimmed();
}

void TransVertSetupForm::setTransVerSwNum(QString s)
{
    ui->transVertSwNum->setText(s);
}


void TransVertSetupForm::setUiItemsVisible(bool visible)
{
    ui->bandSel->setVisible(visible);
    ui->enableTransVertSw->setVisible(visible);
    ui->negCheckbox->setVisible(visible);
    ui->TransVertOffset->setVisible(visible);
    ui->transVertSwNum->setVisible(visible);
    ui->BandLabel->setVisible(visible);
    ui->OffsetLabel->setVisible(visible);
}



