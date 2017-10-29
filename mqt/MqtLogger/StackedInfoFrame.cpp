#include "StackedInfoFrame.h"
#include <QSignalMapper>
#include <QSplitter>
#include "tlogcontainer.h"
#include "tsinglelogframe.h"

#include "ui_StackedInfoFrame.h"

ContList contlist[ CONTINENTS ] =
   {
      {"EU", true},
      {"AS", false},
      {"AF", false},
      {"OC", false},
      {"SA", false},
      {"NA", false},
   };
bool showWorked = false;
bool showUnworked = false;

StackedInfoFrame::StackedInfoFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::StackedInfoFrame),
    contest(0),
    filterClickEnabled(false)
{
    ui->setupUi(this);

    initFilters();

#ifdef Q_OS_ANDROID
    splitterHandleWidth = 20;
#else
    splitterHandleWidth = 6;
#endif

    QStringList infoList =
    {
        "Clock",
        "DXCC",
        "District",
        "Filter",
        "Memories",
        "Locator Map",
        "Locator Tree",
        "Stats"
    };
    ui->infoCombo->addItems(infoList);
    connect(&MinosLoggerEvents::mle, SIGNAL(FiltersChanged()), this, SLOT(onFiltersChanged()));

    connect(&MinosLoggerEvents::mle, SIGNAL(ScrollToCountry(QString,BaseContestLog*)), this, SLOT(on_ScrollToCountry(QString,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ScrollToDistrict(QString,BaseContestLog*)), this, SLOT(on_ScrollToDistrict(QString,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(FontChanged()), this, SLOT(on_FontChanged()), Qt::QueuedConnection);

    ui->infoCombo->setCurrentIndex(0);   // start up on the clock - useful outside the contest!

}

StackedInfoFrame::~StackedInfoFrame()
{
    delete ui;
}
void StackedInfoFrame::on_infoCombo_currentIndexChanged(int arg1)
{
    ui->StackedMults-> setCurrentIndex(arg1);
}

void StackedInfoFrame::setContest(BaseContestLog *ct)
{
    contest = ct;

    ui->dxccFrame->setContest(contest);
    ui->districtFrame->setContest(contest);
    ui->StatsFrame->setContest(contest);
    ui->locFrame->setContest(contest);
    ui->locTreeFrame->setContest(contest);
    ui->clockFrame->setContest(contest);
    ui->rigMemFrame->setContest(contest);
}
void StackedInfoFrame::on_ScrollToDistrict( const QString &qth, BaseContestLog* )
{
    QSharedPointer<DistrictEntry> dist = MultLists::getMultLists() ->searchDistrict( qth );
    if ( dist )
    {
       unsigned int district_ind = MultLists::getMultLists() ->getDistListIndexOf( dist );
       ui->districtFrame->scrollToDistrict( district_ind, true );
    }
}

void StackedInfoFrame::on_ScrollToCountry( const QString &csCs, BaseContestLog* )
{
    callsign cs( csCs );
    cs.validate( );	// we don't use the result

    QSharedPointer<CountryEntry> ctryMult = findCtryPrefix( cs );
    if ( ctryMult )
    {
       int ctry_ind = MultLists::getMultLists() ->getCtryListIndexOf( ctryMult );
       ui->dxccFrame->scrollToCountry( ctry_ind, true );
    }
}
void StackedInfoFrame::refreshMults()
{
    ui->locFrame->reInitialiseLocators();
    ui->locTreeFrame->reInitialiseLocators();
    ui->dxccFrame->reInitialiseCountries();
    ui->districtFrame->reInitialiseDistricts();
    ui->rigMemFrame->reInitialiseMemories();
}
void StackedInfoFrame::on_FontChanged()
{
    refreshMults();
}

void StackedInfoFrame::initFilters()
{
   filterClickEnabled = false;  // stop them being saved while we are setting up

   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentEU, contlist[ 0 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentAS, contlist[ 1 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentAF, contlist[ 2 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentOC, contlist[ 3 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentSA, contlist[ 4 ].allow );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowContinentNA, contlist[ 5 ].allow );

   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowWorked, showWorked );
   MinosParameters::getMinosParameters() ->getBoolDisplayProfile( edpShowUnworked, showUnworked );

   ui->ContEU->setChecked(contlist[ 0 ].allow);
   ui->ContAS->setChecked(contlist[ 1 ].allow);
   ui->ContAF->setChecked(contlist[ 2 ].allow);
   ui->ContOC->setChecked(contlist[ 3 ].allow);
   ui->ContSA->setChecked(contlist[ 4 ].allow);
   ui->ContNA->setChecked(contlist[ 5 ].allow);
   ui->WorkedCB->setChecked(showWorked);
   ui->UnworkedCB->setChecked(showUnworked);


   filterClickEnabled = true;
}
void StackedInfoFrame::onFiltersChanged()
{
    if (contest)
    {
        initFilters();
        ui->dxccFrame->reInitialiseCountries();
        ui->districtFrame->reInitialiseDistricts();
        ui->locFrame->reInitialiseLocators();
        ui->locTreeFrame->reInitialiseLocators();
        ui->StatsFrame->reInitialiseStats();
        ui->rigMemFrame->reInitialiseMemories();
    }
}

void StackedInfoFrame::saveFilters()
{
    if ( filterClickEnabled )
    {
        contlist[ 0 ].allow = ui->ContEU->isChecked();
        contlist[ 1 ].allow = ui->ContAS->isChecked();
        contlist[ 2 ].allow = ui->ContAF->isChecked();
        contlist[ 3 ].allow = ui->ContOC->isChecked();
        contlist[ 4 ].allow = ui->ContSA->isChecked();
        contlist[ 5 ].allow = ui->ContNA->isChecked();
        showWorked = ui->WorkedCB->isChecked();
        showUnworked = ui->UnworkedCB->isChecked();

        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentEU, contlist[ 0 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentAS, contlist[ 1 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentAF, contlist[ 2 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentOC, contlist[ 3 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentSA, contlist[ 4 ].allow );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowContinentNA, contlist[ 5 ].allow );

        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowWorked, showWorked );
        MinosParameters::getMinosParameters() ->setBoolDisplayProfile( edpShowUnworked, showUnworked );
        MinosParameters::getMinosParameters() ->flushDisplayProfile();

        MinosLoggerEvents::SendFiltersChanged();
    }
}

void StackedInfoFrame::on_WorkedCB_clicked()
{
    saveFilters();
}

void StackedInfoFrame::on_UnworkedCB_clicked()
{
    saveFilters();
}

void StackedInfoFrame::on_ContEU_clicked()
{
    saveFilters();
}

void StackedInfoFrame::on_ContOC_clicked()
{
    saveFilters();
}

void StackedInfoFrame::on_ContAS_clicked()
{
    saveFilters();
}

void StackedInfoFrame::on_ContSA_clicked()
{
    saveFilters();
}

void StackedInfoFrame::on_ContAF_clicked()
{
    saveFilters();
}

void StackedInfoFrame::on_ContNA_clicked()
{
    saveFilters();
}
void StackedInfoFrame::on_StackedMults_currentChanged(int /*arg1*/)
{
    ui->StatsFrame->reInitialiseStats();
}
