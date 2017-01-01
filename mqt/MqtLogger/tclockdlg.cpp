#include "tclockdlg.h"
#include "ui_tclockdlg.h"

#include "logger_pch.h"

TClockDlg::TClockDlg(QWidget *parent) :
    QDialog(parent),
    initialised(false), initialCorrection(bigClockCorr),
    ui(new Ui::TClockDlg)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(&clockTick, SIGNAL(timeout()), this, SLOT(on_clockTicked()));
    clockTick.start(500);

    setEdits();
}

TClockDlg::~TClockDlg()
{
    delete ui;
}

void TClockDlg::setEdits()
{
   initialised = false;
   QDateTime corr = QDateTime::currentDateTimeUtc().addSecs( MinosParameters::getMinosParameters() ->getBigClockCorrection());

   ui->dateEdit->setDate(corr.date());
   ui->timeEdit->setTime(corr.time());

   initialised = true;
}
void TClockDlg::on_OKButton_clicked()
{
    // and save the correction to the correction file
    QFile lf( "./Configuration/time.correction");

    if (!lf.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream strm(&lf);

    strm << bigClockCorr;

    accept();
}

void TClockDlg::on_clearButton_clicked()
{
    bigClockCorr = 0;
    setEdits();
    on_clockTicked();
}

void TClockDlg::on_applyButton_clicked()
{
    setEdits();
    on_clockTicked();
}

void TClockDlg::on_cancelButton_clicked()
{
    bigClockCorr = initialCorrection;
    reject();
}

void TClockDlg::on_clockTicked()
{
    QString rawdisp = QDateTime::currentDateTimeUtc().toString("dd/MM/yyyy hh:mm:ss");
    QString disp = QDateTime::currentDateTimeUtc().addSecs( MinosParameters::getMinosParameters() ->getBigClockCorrection()).toString("dd/MM/yyyy HH:mm:ss");


    ui->rawTime->setText(rawdisp);
    ui->correctedTime->setText(disp);
}

void TClockDlg::handleEdit()
{
    if ( initialised )
     {
        QDateTime cttime = QDateTime::currentDateTimeUtc();
        QDateTime cttime2;
        cttime2.setDate(ui->dateEdit->date());
        cttime2.setTime(ui->timeEdit->time());

        bigClockCorr = cttime.secsTo(cttime2);	// I hope sign is right!

        on_clockTicked( );
     }
}

void TClockDlg::on_dateEdit_dateChanged(const QDate &/*date*/)
{
    handleEdit();
}

void TClockDlg::on_timeEdit_timeChanged(const QTime &/*time*/)
{
    handleEdit();
}
