#include "base_pch.h"
#include "htmldelegate.h"

#include "MonitoringFrame.h"
#include "ui_MonitoringFrame.h"

MonitoringFrame::MonitoringFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MonitoringFrame)
{
    ui->setupUi(this);
    ui->QSOTable->setItemDelegate( new HtmlDelegate );

    ui->QSOTable->resizeColumnsToContents();
    ui->QSOTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
//    connect( ui->QSOTable->horizontalHeader(), SIGNAL(sectionResized(int, int , int)),
//             this, SLOT( on_sectionResized(int, int , int)));
}

MonitoringFrame::~MonitoringFrame()
{
    delete ui;
}

void MonitoringFrame::initialise( BaseContestLog * pcontest )
{
    // we don't currently have the required structures to include here...
   contest = pcontest;
   qsoModel.initialise(contest);
   ui->QSOTable->setModel(&qsoModel);
}

void MonitoringFrame::showQSOs()
{
   //LogMonitorFrame->showQSOs();
}
void MonitoringFrame::update()
{
    qsoModel.reset();
}
