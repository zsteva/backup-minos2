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
#include "rigcommon.h"
#include "rigutils.h"
#include "BandList.h"
#include <QLineEdit>
#include <QCheckBox>


TransVertSetupForm::TransVertSetupForm(TransVertParams* _transvertData, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::transVertSetupForm)
{

    ui->setupUi(this);
    transVertData = _transvertData;


    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    fillPortsInfo(ui->locTVComPortSel);

    connect(ui->targetFreq, SIGNAL(editingFinished()), this, SLOT(calcOffset()));
    connect(ui->radioFreq, SIGNAL(editingFinished()), this, SLOT(calcOffset()));
    connect(ui->enableTransVertSw, SIGNAL(clicked(bool)), this, SLOT(enableTransVertSwSel(bool)));
    connect(ui->transVertSwNum, SIGNAL(editingFinished()), this, SLOT(transVertSwNumSel()));
    connect(ui->locTvConChk, SIGNAL(clicked(bool)), this, SLOT(localTransVertSwSel(bool)));
    connect(ui->locTVComPortSel, SIGNAL(activated(int)), this, SLOT(locTVComPortSel(int)));

}







/********************* TransVert Offset Freq  *********************************/


void TransVertSetupForm::calcOffset()
{
    //if (ui->radioFreq->text().isEmpty() || ui->targetFreq->text().isEmpty())
    //{
    //    return;  // wait till both are filled
    //}

    transVertOffsetOk = false;

    if (ui->targetFreq->text().isEmpty() || ui->radioFreq->text().isEmpty())
    {
        transVertData->transVertOffset = 0.0;
        transVertData->transVertOffsetStr = convertFreqStrDispSingle(convertFreqToStr(transVertData->transVertOffset));
        // display
        ui->offsetFreq->setText(transVertData->transVertOffsetStr);
        return;
    }

/*

    if (ui->targetFreq->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please enter a target frequency.");
        msgBox.exec();
        return;
    }
    else if (ui->radioFreq->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please enter a radio frequency.");
        msgBox.exec();
        return;
    }

*/
    // check freq valid format
    QString txf = ui->radioFreq->text().trimmed().remove( QRegExp("^[0]*"));
    QString targetf = ui->targetFreq->text().trimmed().remove(QRegExp("^[0]*"));

    if (valInputFreq(txf, RADIO_FREQ_EDIT_ERR_MSG) && valInputFreq(targetf, TARGET_FREQ_EDIT_ERR_MSG))
    {


        // convert radio freq
        txf = convertSinglePeriodFreqToFullDigit(txf).remove('.');
        transVertData->radioFreqStr = txf;
        transVertData->radioFreq = txf.toDouble();
        // convert target freq
        targetf = convertSinglePeriodFreqToFullDigit(targetf).remove('.');
        transVertData->targetFreqStr = targetf;
        transVertData->targetFreq = targetf.toDouble();

        // check target freq in band
        if (transVertData->targetFreq >= transVertData->fLow && transVertData->targetFreq <= transVertData->fHigh)
        {
            // calculate offset
            if (transVertData->transVertNegative)
            {
               transVertData->transVertOffset =  transVertData->radioFreq - transVertData->targetFreq;
            }
            else
            {
                transVertData->transVertOffset = transVertData->targetFreq - transVertData->radioFreq;

            }

            transVertData->transVertOffsetStr = convertFreqStrDispSingle(convertFreqToStr(transVertData->transVertOffset));
            // display
            ui->offsetFreq->setText(transVertData->transVertOffsetStr);

            // check for negative offset
            if (transVertData->radioFreq > transVertData->targetFreq)
            {
                transVertData->transVertNegative = true;
            }
            else
            {
                transVertData->transVertNegative = false;
            }

            transVertOffsetOk = true;
            transVertValueChanged = true;
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText(QString("Target Freq. is out of band for %1").arg(transVertData->band));
            msgBox.exec();
            return;
        }


    }

}


void TransVertSetupForm::setRadioFreqBox(QString f)
{
    ui->radioFreq->setText(f);
}

void TransVertSetupForm::setTargetFreqBox(QString f)
{
    ui->targetFreq->setText(f);
}


void TransVertSetupForm::setOffsetFreqLabel(QString f)
{
    ui->offsetFreq->setText(f);
}




/********************* TransVert Switch Enable  *********************************/

void TransVertSetupForm::enableTransVertSwSel(bool /*flag*/)
{
    bool checked = ui->enableTransVertSw->isChecked();
    if (transVertData->enableTransSwitch != checked)
    {
        transVertData->enableTransSwitch = checked;
        setEnableTransVertSwBoxVisible(checked);
        setEnableLocalTransVertSwVisible(checked);
        setLocTVSWComportVisible(false);
        transVertValueChanged = true;
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
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (re.exactMatch(numSel))
    {
        transVertData->transSwitchNum = numSel;
        transVertValueChanged = true;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Digits only!"));
        msgBox.exec();
        return;
    }

}

QString TransVertSetupForm::getTransVerSwNum()
{
    return ui->transVertSwNum->text().trimmed();
}

void TransVertSetupForm::setTransVerSwNum(QString s)
{
    ui->transVertSwNum->setText(s);
}



/**************** Local Transvert Switch Control *****************************/


void TransVertSetupForm::localTransVertSwSel(bool /*flag*/)
{

    bool checked = ui->locTvConChk->isChecked();
    if (transVertData->enableLocTVSwMsg != checked)
    {
        transVertData->enableLocTVSwMsg = checked;
        setLocTVSWComportVisible(checked);
        transVertValueChanged = true;
    }

}

bool TransVertSetupForm::getEnableLocalTransVertSw()
{
    return ui->locTvConChk->isChecked();
}


void TransVertSetupForm::setEnableLocalTransVertSw(bool b)
{
    ui->locTvConChk->setChecked(b);

}


void TransVertSetupForm::setEnableLocalTransVertSwVisible(bool visible)
{
     ui->locTvConChk->setVisible(visible);


}



/**************** Local Transvert Switch Comport **************************/



void TransVertSetupForm::locTVComPortSel(int /*index*/)
{

    if (ui->locTVComPortSel->currentText() != transVertData->locTVSwComport)
    {
        transVertData->locTVSwComport = ui->locTVComPortSel->currentText();
        transVertValueChanged = true;
    }
}

QString TransVertSetupForm::getLocTVSwComport()
{
    return ui->locTVComPortSel->currentText();
}

void TransVertSetupForm::setLocTVSwComport(QString p)
{
    ui->locTVComPortSel->setCurrentIndex(ui->locTVComPortSel->findText(p));
}


void TransVertSetupForm::setLocTVSWComportVisible(bool visible)
{
     ui->locTVComPortSel->setVisible(visible);
     ui->locComportSwLbl->setVisible(visible);

}


/***************** Radio Antenna Switch Number  ********************************/

/*
void TransVertSetupForm::antennaNumSwSel()
{
    QString numSel = ui->radioAntSwNum->text().trimmed();
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (re.exactMatch(numSel))
    {
        transVertData->antSwitchNum = numSel;
        transVertValueChanged = true;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Digits only!"));
        msgBox.exec();
        return;
    }
}


void TransVertSetupForm::antSwNumVisible(bool visible)
{

    ui->radioAntSwNum->setVisible(visible);
    ui->antSwNumLbl->setVisible(visible);
}

*/



void TransVertSetupForm::setUiItemsVisible(bool visible)
{
    //ui->bandSel->setVisible(visible);
    ui->enableTransVertSw->setVisible(visible);
    ui->radioFreq->setVisible(visible);
    ui->transVertSwNum->setVisible(visible);
    //ui->BandLabel->setVisible(visible);
    ui->OffsetLabel->setVisible(visible);
}



