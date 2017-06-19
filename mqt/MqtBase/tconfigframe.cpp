#include "base_pch.h"

#include "tconfigframe.h"
#include "ui_tconfigframe.h"

#include "ConfigDM.h"

TConfigFrame::TConfigFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TConfigFrame)
{
    ui->setupUi(this);
}

TConfigFrame::~TConfigFrame()
{
    delete ui;
}
void TConfigFrame::initialise(QWidget *p, ConfigCloseCallBack ccb, bool doAutoStart)
{
    if (!doAutoStart)
        ui->autoStartCheckBox->setVisible(false);

    closeCb = ccb;
    parent = p;

    for (int i = 0; i < TMConfigDM::getConfigDM( 0 ) ->elelist.size(); i++)
    {
       QSharedPointer<TConfigElement> c = TMConfigDM::getConfigDM( 0 ) ->elelist[i];
       QString sect = c ->name;
       QListWidgetItem *listItem = new QListWidgetItem(sect, ui->ElementListBox);
       if (c->commandLine.isEmpty())
       {
//            Qt::ItemFlags flags = listItem->flags();
//            flags = flags ^ Qt::ItemIsUserCheckable;
//            listItem->setFlags(flags);
            listItem->setData(Qt::CheckStateRole, QVariant());
       }
       else
       {
           listItem->setCheckState(c->run?Qt::Checked: Qt::Unchecked);
       }
    }
    ui->StationIdEdit->setText(TMConfigDM::getConfigDM( 0 ) ->getCircleOfHell());
    ui->HideCheckBox->setChecked(TMConfigDM::getConfigDM( 0 )->getHideServers());
    ui->autoStartCheckBox->setChecked(TMConfigDM::getConfigDM( 0 )->getAutoStart());

    if (doAutoStart && TMConfigDM::getConfigDM( 0 )->getAutoStart())
    {
        connect(&startTimer, SIGNAL(timeout()), this, SLOT(startTimer_Timeout()));
        startTimer.start(100);
    }
    ui->HideCheckBox->setChecked(TMConfigDM::getConfigDM( 0 )->getHideServers());
}
void TConfigFrame::setup(bool started)
{
    ui->StartButton->setEnabled(!started);
    ui->StopButton->setVisible(false);
}
void TConfigFrame::startTimer_Timeout()
{
    start();
}

void TConfigFrame::start()
{
    on_SetButton_clicked();    // set the circle of hell
    for (int i = 0; i < TMConfigDM::getConfigDM( 0 ) ->elelist.size(); i++)
    {
       TMConfigDM::getConfigDM( 0 ) ->elelist[i] ->setRun(ui->ElementListBox->item(i) ->checkState());
    }

    TMConfigDM::getConfigDM( 0 ) ->start();
}

void TConfigFrame::on_StartButton_clicked()
{
    start();
}

void TConfigFrame::on_StopButton_clicked()
{
    TMConfigDM::getConfigDM( 0 ) ->stop();
}

void TConfigFrame::on_HideCheckBox_clicked()
{
    TMConfigDM::getConfigDM( 0 ) ->setHideServers(ui->HideCheckBox->isChecked());
    // Make this active - need a hide/show event that is signalled for show
    // and then all "server" apps need to honour this.
    setShowServers(!ui->HideCheckBox->isChecked());
}
void TConfigFrame::on_autoStartCheckBox_clicked()
{
    TMConfigDM::getConfigDM( 0 ) ->setAutoStart(ui->autoStartCheckBox->isChecked());
}

void TConfigFrame::on_SetButton_clicked()
{
    QString coh = ui->StationIdEdit->text();
    TMConfigDM::getConfigDM( 0 ) ->setCircleOfHell( coh );
}

void TConfigFrame::on_ClearAllButton_clicked()
{
    for ( int i = 0; i < TMConfigDM::getConfigDM( 0 ) ->elelist.size(); i++)
    {
       ui->ElementListBox->item(i) ->setCheckState(Qt::Unchecked);
    }
}

void TConfigFrame::on_ModifyButton_clicked()
{
    // we really need to have accumulated changes, so that we can cancel - or we action them instantly
    // Change the INI file to match what is shown
    for (int i = 0; i < TMConfigDM::getConfigDM( 0 ) ->elelist.size(); i++)
    {
       TMConfigDM::getConfigDM( 0 ) ->elelist[i] ->setRun(ui->ElementListBox->item(i) ->checkState());
    }
}

void TConfigFrame::on_CancelButton_clicked()
{
    // need to pass up to parent
    closeCb(parent);
}


