#include "logger_pch.h"

#include "rigcontrolframe.h"
#include "ui_rigcontrolframe.h"
#include "SendRPCDM.h"
#include "rigControlcommonconstants.h"

RigControlFrame::RigControlFrame(QWidget *parent):
    QFrame(parent)
    , ui(new Ui::RigControlFrame)
    , radioLoaded(false)
{

    ui->setupUi(this);

//   for (int i = 0; i < hamlibData::supModeList.count(); i++)
//    {
//        ui->ModeComboBoxGJV->addItem(hamlibData::supModeList[i]);
//    }

//    setMode(hamlibData::USB);


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
    ui->freqDispVFOA->setText(f);
}

/*
void RigControlFrame::setMode(QString m)
{
   ui->ModeComboBoxGJV->setCurrentText(m);
    ui->ModeButton->setText(m);
   // make sure the mode button shows the correct "flip" value


   if (ui->ModeComboBoxGJV->currentText() == "CW")
   {
      ui->ModeButton->setText("USB");
   }
   else
   {
      ui->ModeButton->setText("CW");
   }


}
*/
void RigControlFrame::setRadioName(QString n)
{
    ui->radioName->setText(n);
}


void RigControlFrame::setRadioState(QString s)
{
    ui->rigState->setText(s);
}



// needto sort this **************************************

//ui->ModeComboBoxGJV->addItem("A1A");
//ui->ModeComboBoxGJV->addItem("J3E");
//ui->ModeComboBoxGJV->addItem("F3E");
//ui->ModeComboBoxGJV->addItem("MGM");
