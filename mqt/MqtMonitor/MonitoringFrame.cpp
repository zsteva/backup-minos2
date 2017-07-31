#include "base_pch.h"

#include "MonitoringFrame.h"
#include "ui_MonitoringFrame.h"

MonitoringFrame::MonitoringFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MonitoringFrame)
{
    ui->setupUi(this);
}

MonitoringFrame::~MonitoringFrame()
{
    delete ui;
}

void MonitoringFrame::initialise( BaseContestLog * pcontest )
{
    // we don't currently have the required structures to include here...
   contest = pcontest;
   //LogMonitorFrame->initialise( pcontest );
}

void MonitoringFrame::showQSOs()
{
   //LogMonitorFrame->showQSOs();
}
