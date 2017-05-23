#include "logger_pch.h"

#include "TClockFrame.h"
#include "ui_TClockFrame.h"

TClockFrame::TClockFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TClockFrame)
{
    ui->setupUi(this);
    connect(&MinosLoggerEvents::mle, SIGNAL(TimerDistribution()), this, SLOT(RecheckTimerTimer()));
}

TClockFrame::~TClockFrame()
{
    delete ui;
}
void TClockFrame::RecheckTimerTimer(  )
{
   if ( !isVisible() )
      return ;

   QDateTime t = QDateTime::currentDateTimeUtc().addSecs( MinosParameters::getMinosParameters() ->getBigClockCorrection());
   QString disp = t.toString( "hh:mm:ss" );

   ui->clockLabel ->setText("<b><center><nobr><p><big><h1>"
                            + disp + "</p></h1>");
}
