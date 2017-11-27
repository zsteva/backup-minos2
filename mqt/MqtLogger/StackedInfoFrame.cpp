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

    connect(&MinosLoggerEvents::mle, SIGNAL(ScrollToCountry(QString,BaseContestLog*)), this, SLOT(on_ScrollToCountry(QString,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(ScrollToDistrict(QString,BaseContestLog*)), this, SLOT(on_ScrollToDistrict(QString,BaseContestLog*)), Qt::QueuedConnection);
    connect(&MinosLoggerEvents::mle, SIGNAL(FontChanged()), this, SLOT(on_FontChanged()), Qt::QueuedConnection);

}

StackedInfoFrame::~StackedInfoFrame()
{
    delete ui;
}
void StackedInfoFrame::on_infoCombo_currentIndexChanged(int arg1)
{
    ui->StackedMults-> setCurrentIndex(arg1);
    if (contest)
    {
        contest->currentStackItem.setValue(ui->infoCombo->currentText());
        contest->commonSave(false);
    }
}

void StackedInfoFrame::setContest(LoggerContestLog *ct)
{
    contest = ct;
    initFilters();

    ui->dxccFrame->setContest(contest);
    ui->districtFrame->setContest(contest);
    ui->StatsFrame->setContest(contest);
    ui->locFrame->setContest(contest);
    ui->locTreeFrame->setContest(contest);
    ui->clockFrame->setContest(contest);
    ui->rigMemFrame->setContest(contest);

    if (contest)
        ui->infoCombo->setCurrentText(contest->currentStackItem.getValue());   // start up on the clock - useful outside the contest!

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

   if (contest)
   {
       contlist[ 0 ].allow = contest->showContinentEU.getValue();
       contlist[ 1 ].allow = contest->showContinentAS.getValue();
       contlist[ 2 ].allow = contest->showContinentAF.getValue();
       contlist[ 3 ].allow = contest->showContinentOC.getValue();
       contlist[ 4 ].allow = contest->showContinentSA.getValue();
       contlist[ 5 ].allow = contest->showContinentNA.getValue();

       ui->ContEU->setChecked(contlist[ 0 ].allow);
       ui->ContAS->setChecked(contlist[ 1 ].allow);
       ui->ContAF->setChecked(contlist[ 2 ].allow);
       ui->ContOC->setChecked(contlist[ 3 ].allow);
       ui->ContSA->setChecked(contlist[ 4 ].allow);
       ui->ContNA->setChecked(contlist[ 5 ].allow);
       ui->WorkedCB->setChecked(contest->showWorked.getValue());
       ui->UnworkedCB->setChecked(contest->showUnworked.getValue());
    }
   filterClickEnabled = true;
}
void StackedInfoFrame::filtersChanged()
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
        contest->showWorked.setValue(ui->WorkedCB->isChecked());
        contest->showUnworked.setValue(ui->UnworkedCB->isChecked());

        contest->showContinentEU.setValue(contlist[ 0 ].allow);
        contest->showContinentAS.setValue(contlist[ 1 ].allow);
        contest->showContinentAF.setValue(contlist[ 2 ].allow);
        contest->showContinentOC.setValue(contlist[ 3 ].allow);
        contest->showContinentSA.setValue(contlist[ 4 ].allow);
        contest->showContinentNA.setValue(contlist[ 5 ].allow);

        contest->commonSave(false);
        filtersChanged();
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
