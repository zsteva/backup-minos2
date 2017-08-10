#include "logger_pch.h"

#include "rigcontrolframe.h"
#include "ui_rigcontrolframe.h"
#include "rotatorCommonConstants.h"

RigControlFrame::rigControlFrame(QWidget *parent):
    QFrame(parent)
    , ui(new Ui::RigControlFrame)
    , radioLoaded(false)
{

    ui->setupUi(this);


}

RigControlFrame::~RigControlFrame()
{
    delete ui;

}


void RigControlFrame::setRadioLoaded()
{
    radioLoaded = true;
}

bool RigControlFrame::isRadioLoaded()
{
    return radioLoaded;
}

void RigControlFrame::setFreq(QString f)
{
    ui->freqDisplay->setText(f);
}


void RigControlFrame::setMode(QString m)
{
   //ui->ModeComboBoxGJV->setCurrentText(m);
    ui->ModeButton->setText(m);
   // make sure the mode button shows the correct "flip" value

/*
   if (ui->ModeComboBoxGJV->currentText() == "A1A")
   {
      ui->ModeButton->setText("J3E");
   }
   else
   {
      ui->ModeButton->setText("A1A");
   }

*/
}

void RigControlFrame::setRadioName(QString n)
{
    ui->radioName->setText(n);
}


void RigControlFrame::setRadioState(QString s)
{
    ui->rigState->setText(s);
}





ui->ModeComboBoxGJV->addItem("A1A");
ui->ModeComboBoxGJV->addItem("J3E");
ui->ModeComboBoxGJV->addItem("F3E");
ui->ModeComboBoxGJV->addItem("MGM");
