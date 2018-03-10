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
#include <QLineEdit>
#include <QCheckBox>


TransVertSetupForm::TransVertSetupForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::transVertSetupForm),
    tansVertValueChanged(false),
    transVertNameChanged(false)
{

    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->bandSel, SIGNAL(activated(int)), this, SLOT(bandSelected()));
    connect(ui->TransVertOffset, SIGNAL(editingFinished()), this, SLOT(transVertOffsetSelected()));
    connect(ui->negCheckbox, SIGNAL(clicked(bool)), this, SLOT(negCheckBoxSelected(bool)));
    connect(ui->enableTransVertSw, SIGNAL(clicked(bool)), this, SLOT(enableTransVertSwSel(bool)));
    connect(ui->transVertSwNum, SIGNAL(editingFinished()), this, SLOT(transVertSwNumSel(bool)));

}


/********************* Band Selection *********************************/


void TransVertSetupForm::bandSelected()
{




}



/********************* TransVert Offset Freq  *********************************/


void TransVertSetupForm::transVertOffsetSelected()
{




}

QString TransVertSetupForm::getTransVertOffsetFreq()
{
    return ui->TransVertOffset->text().trimmed();
}

void TransVertSetupForm::setTransVertOffsetFreq(QString s)
{
    ui->TransVertOffset->setText(s);
}


/********************* TransVert Negative Offset Select  *********************************/

void TransVertSetupForm::negCheckBoxSelected(bool)
{




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

void TransVertSetupForm::enableTransVertSwSel(bool)
{




}


bool TransVertSetupForm::getEnableTransVertSw()
{
    return ui->enableTransVertSw->isChecked();
}

void TransVertSetupForm::setEnableTransVertSw(bool b)
{
    ui->enableTransVertSw->setChecked(b);
}


/********************* TransVert Switch Number *********************************/


void TransVertSetupForm::transVertSwNumSel(bool)
{




}



QString TransVertSetupForm::getTransVerSwNum()
{
    return ui->TransVertOffset->text().trimmed();
}

void TransVertSetupForm::setTransVerSwNum(QString s)
{
    ui->TransVertOffset->setText(s);
}

/*

void TransVertSetupForm::transVertChecked(int boxNumber)
{
    if (!chkloadflg)
    {
        if (transVertCheck[boxNumber]->isChecked())
        {
            availRadios[boxNumber].transVertEnable = true;
            transVertEdit[boxNumber]->setEnabled(true);
            transNegCheck[boxNumber]->setEnabled(true);
        }
        else
        {
            availRadios[boxNumber].transVertEnable = false;
            transVertEdit[boxNumber]->setEnabled(false);
            transNegCheck[boxNumber]->setEnabled(false);
        }
        radioValueChanged[boxNumber] = true;
        radioChanged = true;


    }

}



void TransVertSetupForm::transVertEditFinished(int boxNumber)
{
    bool ok;
    QString offsetStr = transVertEdit[boxNumber]->text();
    offsetStr.remove(QChar('.'));

    qDebug() << "offset = " << offsetStr;

    double value = offsetStr.toDouble(&ok);
    if (!ok )
    {
         QMessageBox::critical(this, "TransVert Error", offsetStr + " Not a valid TransVert Offset value");
         transVertEdit[boxNumber]->text() = "<font color='Red'>" + offsetStr + "</font>";

    }
    else if (value != availRadios[boxNumber].transVertOffset)
    {
        qDebug() << "foffset = " << value;
        transVertEdit[boxNumber]->text() = "<font color='Black'>" + offsetStr + "</font>";
        availRadios[boxNumber].transVertOffset = value;
        availRadios[boxNumber].transVertOffsetStr = offsetStr;
        radioValueChanged[boxNumber] = true;
        radioChanged = true;
    }
}



void TransVertSetupForm::transNegChecked(int boxNumber)
{
    if (!chkloadflg)
    {
        availRadios[boxNumber].transVertNegative = transNegCheck[boxNumber]->isChecked();
        radioValueChanged[boxNumber] = true;
        radioChanged = true;

    }


}
*/
