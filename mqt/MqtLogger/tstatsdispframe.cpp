#include "logger_pch.h"

#include "tstatsdispframe.h"
#include "ui_tstatsdispframe.h"

const int checkInterval = 10;
TStatsDispFrame::TStatsDispFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TStatsDispFrame),
    ct(0), lastCheckTime(QDateTime::currentDateTime().addSecs(-1000))
{
    ui->setupUi(this);
    connect(&MinosLoggerEvents::mle, SIGNAL(TimerDistribution()), this, SLOT(RecheckTimerTimer()));
}

TStatsDispFrame::~TStatsDispFrame()
{
    delete ui;
}
void TStatsDispFrame::setContest( BaseContestLog *pct )
{
   ct = pct;
}
void TStatsDispFrame::RecheckTimerTimer(  )
{
   if ( !isVisible() || !ct )
      return ;

   QDateTime now = QDateTime::currentDateTime();

   if (lastCheckTime.secsTo(now) >= checkInterval)
   {
       lastCheckTime = now;
       reInitialise();
   }
}

void TStatsDispFrame::on_P1Edit_valueChanged(int arg1)
{
    MinosParameters::getMinosParameters() ->setStatsPeriod1( arg1 );
    reInitialise();
}

void TStatsDispFrame::on_P2Edit_valueChanged(int arg1)
{
    MinosParameters::getMinosParameters() ->setStatsPeriod2( arg1 );
    reInitialise();
}
//---------------------------------------------------------------------------
void TStatsDispFrame::reInitialise()
{
   // we need to set the operator bundle to the current contest ops
   if ( !ui->P1Edit->hasFocus() && !ui->P2Edit->hasFocus() )
   {
      ui->P1Edit->setValue( MinosParameters::getMinosParameters() ->getStatsPeriod1() );
      ui->P2Edit->setValue( MinosParameters::getMinosParameters() ->getStatsPeriod2() );
   }

   ui->SLabel1->clear();

   if ( !ct )
   {
      // not there...
      return ;
   }

   BaseContact *bestdx = ct->getBestDX();

   if ( !bestdx || ct->maxSerial <= 0 )
      return ;

   int ltot = 0;
   if ( ct->countryMult.getValue() )
      ltot += ct->nctry ;
   if ( ct->districtMult.getValue() )
      ltot += ct->ndistrict;
   if ( ct->locMult.getValue() )
      ltot += ct->nlocs;

   ct->updateStats();
   int nvalid = ct->getValidQSOs();
   if (nvalid)
   {
      double avpts = ( double ) ct->contestScore / nvalid;
      QString tempcs;
      tempcs = bestdx->cs.fullCall.getValue().trimmed() ;

      QString buff = QString( "Best DX %1 Sn %2 %3(%4 kms). Average QSO is %5 Points" )
                           //#define MAX_DISPLAY_TEST 1
   #ifndef MAX_DISPLAY_TEST
                           .arg( tempcs) .arg(bestdx->serials.getValue())
                           .arg(bestdx->loc.loc.getValue()) .arg( bestdx->contactScore.getValue())
                           .arg(avpts, 5);
   #else
                           .arg("PE0/LX5ABC/P") .arg("9999") .arg( "XX99XX") .arg(29999) .arg(9999.9);
   #endif

      int sp1 = MinosParameters::getMinosParameters() ->getStatsPeriod1();
      int sp2 = MinosParameters::getMinosParameters() ->getStatsPeriod2();
      QDateTime  contestStart = CanonicalToTDT(ct->DTGStart.getValue());
      int fromContestStart = contestStart.secsTo(QDateTime::currentDateTime());

      if (sp1 > fromContestStart/2)
      {
         sp1 = fromContestStart/2;
      }
      if (sp2 > fromContestStart/2)
      {
         sp2 = fromContestStart/2;
      }
      if ( ltot )
      {
         double qmult = ( double ) nvalid / ltot;
         QString temp;

         long Qs = ct->QSO1 ? ct->QSO1 : ct->QSO2;
         if ( Qs )
         {
            // Why the selection of period in this way?  kms1 is the distance for the first period; use that unless it is zero
            int period = ct->kms1 ? sp1 : sp2;
            double qmins = ( qmult * period ) / Qs;
            temp = QString(" (%1 Mins)" ) .arg(qmins, 1) ;
         }

         QString lbuff = QString( "%1\nOne New Multiplier is equivalent to %2 Average QSOs%3" ) .arg(buff)
   #ifndef MAX_DISPLAY_TEST
                               .arg(qmult) .arg(temp);
   #else
                               .arg(9999.9) .arg(" (9999.9 Mins)");
   #endif
         buff = lbuff ;
      }

      QString lbuff = QString( "Last %1 Mins: %2 QSO %3 pts %4 mults\r\n(Previous %5; %6; %7)"
                             "\nLast %8 Mins: %9 QSO %10 pts %11 mults\r\n(Previous %12; %13; %14)" )
   #ifndef MAX_DISPLAY_TEST
                            .arg(sp1)  .arg(ct->QSO1) .arg(ct->kms1) .arg(ct->mults1)
                            .arg(ct->QSO1p) .arg(ct->kms1p) .arg(ct->mults1p)
                            .arg(sp2) .arg(ct->QSO2) .arg(ct->kms2) .arg(ct->mults2)
                            .arg(ct->QSO2p) .arg(ct->kms2p) .arg(ct->mults2p);
   #else

                            .arg(999) .arg(1999) .arg(999999L) .arg(999) .arg(1999) .arg(999999L) .arg(999) .arg(999999)
                            .arg(999) .arg(1999) .arg(999999L) .arg(999) .arg(1999) .arg(999999L) .arg(999) .arg(999999);
   #endif
      ui->SLabel1->setText( buff + "\r\n\r\n" + lbuff );
   }
}
//---------------------------------------------------------------------------

