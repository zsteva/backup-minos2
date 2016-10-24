#include "logger_pch.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "tlogcontainer.h"

#include "tcalendarform.h"
#include "ui_tcalendarform.h"

#define LOWYEAR -1
#define LOWURLYEAR -1
#define HIGHYEAR 1

static int curYear = 0;

//enum CalType {ectVHF, ectHF, ectMwave, ectVHFOther, ectHFOther, ectHFBARTG};
QString calString[] =
    {
        "vhfcontests",
        "hfcontests",
        "microcontests",
        "VHFContestsOther",
        "HFContestsOther",
        "bartgcontests"
    };
class CalendarYear
{
        virtual QString getSite() = 0;
        QString yearString()
        {
            QString y = QString::number( curYear + yearOffset );
            y = y.midRef( 2, 2 ).toString();
            return y;
        }
    public:
        CalendarYear ( CalType t, int y ) : type ( t ), yearOffset ( y )
        {
            if ( curYear == 0 )
            {
                curYear = QDate::currentDate().year();
            }

        }
        virtual ~CalendarYear(){}

        virtual bool downloadFile ( bool showError );

        bool loaded;
        int yearOffset;
        CalType type;
        virtual QString getPath();
        virtual QString getURL();
};
class HFCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        HFCalendarYear ( int year ) : CalendarYear ( ectHF, year )
        {
        }
};
class HFBARTGCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        HFBARTGCalendarYear ( int year ) : CalendarYear ( ectHFBARTG, year )
        {
        }
};
class VHFCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        VHFCalendarYear ( int year ) : CalendarYear ( ectVHF, year )
        {
        }
};
class MicroCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        MicroCalendarYear ( int year ) : CalendarYear ( ectMwave, year )
        {
        }
};
class HFOtherCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        HFOtherCalendarYear ( int year ) : CalendarYear ( ectHFOther, year )
        {
        }
        virtual QString getPath();
        virtual QString getURL();
};
class VHFOtherCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        VHFOtherCalendarYear ( int year ) : CalendarYear ( ectVHFOther, year )
        {
        }
        virtual QString getPath();
        virtual QString getURL();
};
class CTYCalendarYear : public CalendarYear
{
        virtual QString getSite();
    public:
        CTYCalendarYear ( int year ) : CalendarYear ( ectVHFOther, year )
        {
        }
        virtual QString getPath();
        virtual QString getURL();
};
//---------------------------------------------------------------------------
QString CalendarYear::getPath()
{
    QString path =  "./Configuration/" + calString[ type ] + yearString() + ".xml";
    return path;
}
QString CalendarYear::getURL()
{
    QString url = getSite() + calString[ type ] + yearString() + ".xml";
    return url;

}
//---------------------------------------------------------------------------
QString VHFCalendarYear::getSite()
{
    return "http://www.rsgbcc.org/vhf/";
}
//---------------------------------------------------------------------------
QString HFCalendarYear::getSite()
{
    // Yes, the HF calendar is under the VHF directory!
    return "http://www.rsgbcc.org/vhf/";
}
//---------------------------------------------------------------------------
QString HFBARTGCalendarYear::getSite()
{
    return "http://bartg.rsgbcc.org/";
}
//---------------------------------------------------------------------------
QString MicroCalendarYear::getSite()
{
    return "http://microwave.rsgbcc.org/";
}
//---------------------------------------------------------------------------
QString VHFOtherCalendarYear::getSite()
{
    return "";
}
QString VHFOtherCalendarYear::getPath()
{
    QString p = "./Configuration/" + calString[ type ] + ".xml";
    return p;
}
QString VHFOtherCalendarYear::getURL()
{
    // No URL
    return QString();
}
//---------------------------------------------------------------------------
QString HFOtherCalendarYear::getSite()
{
    return "";
}
QString HFOtherCalendarYear::getPath()
{
    QString p = "./Configuration/" + calString[ type ] + ".xml";
    return p;
}
QString HFOtherCalendarYear::getURL()
{
    // No URL
    return QString();
}
//---------------------------------------------------------------------------
QString CTYCalendarYear::getSite()
{
    return "http://www.country-files.com/cty/cty.dat";
}
QString CTYCalendarYear::getPath()
{
    QString p = "./Configuration/cty.dat";
    return p;
}
QString CTYCalendarYear::getURL()
{
    return getSite();
}

TCalendarForm::
TCalendarForm( QWidget *parent, CalType calType ) :
        QDialog( parent ),
        ui( new Ui::TCalendarForm ),
        vhf ( 2000, ectVHF ),
        hf ( 2000, ectHF ), hfother ( 2000, ectHFOther ),
        vhfother ( 2000, ectVHFOther ), mwave ( 2000, ectMwave ),
        hfbartg( 2000, ectHFBARTG ), calType ( calType )
{
    ui->setupUi( this );
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}
int TCalendarForm::exec()
{
    ui->YearEdit->setText( QDate::currentDate().toString ( "yyyy" ) );

    FormShow();

    QSettings settings;
    QString sname = "Calendar/grid_geometry_" + calString[calType];
    QByteArray state = settings.value(sname).toByteArray();
    if (state.size())
    {
        ui->CalendarGrid->horizontalHeader()->restoreState(state);
    }

    QByteArray geometry = settings.value("Calendar/form_geometry_" + calString[calType]).toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    return QDialog::exec();
}

TCalendarForm::~TCalendarForm()
{
    delete ui;
}
void TCalendarForm::reject()
{
    doCloseEvent();
    QDialog::reject();
}
void TCalendarForm::accept()
{
    doCloseEvent();
    QDialog::accept();
}

void TCalendarForm::doCloseEvent()
{
    QSettings settings;

    settings.setValue("Calendar/grid_geometry_" + calString[calType], ui->CalendarGrid->horizontalHeader()->saveState());

    settings.setValue("Calendar/form_geometry_" + calString[calType], saveGeometry());
}

bool TCalendarForm::loadYear ( Calendar &cal, int year )
{
    bool loaded = false;

    for ( int i = yearList.size() - 1; i >= 0; i-- )
    {
        if ( !loaded && FileExists ( yearList[ i ] ->getPath() ) && year >= curYear + yearList[ i ] ->yearOffset )
        {
            loaded = cal.parseFile ( yearList[ i ] ->getPath() );
        }
    }
    return loaded;
}
//---------------------------------------------------------------------------
void TCalendarForm::LoadGrid ( Calendar &cal )
{
    ui->CalendarVersionLabel->setText( ( "File Version " + cal.version ).c_str() );
    ui->CalendarGrid->setRowCount( cal.calendar.size() + 1 );
    int cc = ( cal.calType == ectVHF ? 8 : 5 );

    ui->CalendarGrid->setColumnCount( cc );
    int col = 0;
    ui->CalendarGrid->setHorizontalHeaderItem( col++, new QTableWidgetItem( "Contest Name" ) );
    ui->CalendarGrid->setHorizontalHeaderItem( col++, new QTableWidgetItem( "Band" ) );
    ui->CalendarGrid->setHorizontalHeaderItem( col++, new QTableWidgetItem( "Start (UTC)" ) );
    ui->CalendarGrid->setHorizontalHeaderItem( col++, new QTableWidgetItem( "Finish (UTC)" ) );
    if ( cal.calType == ectVHF )
    {
        ui->CalendarGrid->setHorizontalHeaderItem( col++, new QTableWidgetItem( "Scoring" ) );
        ui->CalendarGrid->setHorizontalHeaderItem( col++, new QTableWidgetItem( "Mults" ) );
    }
    ui->CalendarGrid->setHorizontalHeaderItem( col++, new QTableWidgetItem( "Sections" ) );
    if ( cal.calType == ectVHF )
    {
        ui->CalendarGrid->setHorizontalHeaderItem( col++, new QTableWidgetItem( "Special Rules" ) );
    }
    int row = 0;
    int nextContest = 0;
    QDateTime now = QDateTime::currentDateTime();
    for ( std::vector<IndividualContest>::iterator i = cal.calendar.begin(); i != cal.calendar.end(); i++ )
    {
        col = 0;
        ui->CalendarGrid->setItem( row, col++, new QTableWidgetItem( ( *i ).description.c_str() ) );
        ui->CalendarGrid->setItem( row, col++, new QTableWidgetItem( ( *i ).bands.c_str() ) );
        ui->CalendarGrid->setItem( row, col++, new QTableWidgetItem( ( *i ).start.toString ( "dd/MM/yyyy hh:mm" ) ) );
        ui->CalendarGrid->setItem( row, col++, new QTableWidgetItem( ( *i ).finish.toString ( "dd/MM/yyyy hh:mm" ) ) );

        if ( cal.calType == ectVHF )
        {
            ui->CalendarGrid->setItem( row, col++, new QTableWidgetItem( ( *i ).ppKmScoring ? "1Pt/Km" : "1Pt/QSO" ) );
            ui->CalendarGrid->setItem( row, col++, new QTableWidgetItem( ( *i ).mults.c_str() ) );
        }
        ui->CalendarGrid->setItem( row, col++, new QTableWidgetItem( ( *i ).sections.c_str() ) );
        if ( cal.calType == ectVHF )
        {
            ui->CalendarGrid->setItem( row, col++, new QTableWidgetItem( ( *i ).specialRules.c_str() ) );
        }

        if (!description.isEmpty())
         {
            if (nextContest == 0 && description == (*i).description.c_str())
            {
               nextContest = row;
            }
         }
         else
         {
            if (nextContest == 0 && now <= (*i).finish)
            {
               nextContest = row;
            }
         }
        row++;
    }
    if (nextContest)
    {
        ui->CalendarGrid->selectRow(nextContest);
    }
}
void TCalendarForm::FormShow ( )
{
    bool ok;
    int year = ui->YearEdit->text().toInt(&ok);
    if (!ok)
        year = 2000;

    bool loaded = false;
    if ( calType == ectHF )
    {
        setWindowTitle ( "Select Contest from HF Calendar" );
        hf = Calendar ( year, ectHF );

        yearList.clear();
        for ( int i = LOWYEAR; i <= HIGHYEAR; i++ )
        {
            yearList.push_back ( QSharedPointer<CalendarYear> ( new HFCalendarYear ( i ) ) );
        }

        loaded = loadYear ( hf, year );

        if ( !loaded )
        {
            mShowMessage ( "Failed to load the HF calendar file", this );
            ui->CalendarVersionLabel->setText( "No file loaded" );
            ui->CalendarGrid->setRowCount( 0 );
            return ; // don't close - they need a chance to download
        }
        LoadGrid ( hf );
    }
    else
        if ( calType == ectHFBARTG )
        {
            setWindowTitle ( "Select Contest from BARTG Calendar" );
            hfbartg = Calendar ( year, ectHFBARTG );

            yearList.clear();
            for ( int i = LOWYEAR; i <= HIGHYEAR; i++ )
            {
                yearList.push_back ( QSharedPointer<CalendarYear> ( new HFBARTGCalendarYear ( i ) ) );
            }

            loaded = loadYear ( hfbartg, year );

            if ( !loaded )
            {
                mShowMessage ( "Failed to load the BARTG calendar file", this );
                ui->CalendarVersionLabel->setText( "No file loaded" );
                ui->CalendarGrid->setRowCount( 0 );
                return ; // don't close - they need a chance to download
            }
            LoadGrid ( hfbartg );
        }
        else
            if ( calType == ectVHF )
            {
                setWindowTitle ( "Select Contest from VHF Calendar" );
                vhf = Calendar ( year, ectVHF );

                yearList.clear();
                for ( int i = LOWYEAR; i <= HIGHYEAR; i++ )
                {
                    yearList.push_back ( QSharedPointer<CalendarYear> ( new VHFCalendarYear ( i ) ) );
                }

                loaded = loadYear ( vhf, year );

                if ( !loaded )
                {
                    mShowMessage ( "Failed to load the VHF calendar file", this );
                    ui->CalendarVersionLabel->setText( "No file loaded" );
                    ui->CalendarGrid->setRowCount( 0 );
                    return ; // don't close - they need a chance to download
                }
                LoadGrid ( vhf );
            }
            else
                if ( calType == ectHFOther )
                {
                    setWindowTitle ( "Select Contest from HF other Calendar" );
                    hfother = Calendar ( year, ectHFOther );
                    yearList.clear();
                    for ( int i = LOWYEAR; i <= HIGHYEAR; i++ )
                    {
                        yearList.push_back ( QSharedPointer<CalendarYear> ( new HFOtherCalendarYear ( i ) ) );
                    }

                    loaded = loadYear ( hfother, year );

                    if ( !loaded )
                    {
                        mShowMessage ( "Failed to load the HF other calendar file", this );
                        ui->CalendarVersionLabel->setText( "No file loaded" );
                        ui->CalendarGrid->setRowCount( 0 );
                        return ; // don't close - they need a chance to download
                    }
                    LoadGrid ( hfother );
                }
                else
                    if ( calType == ectVHFOther )
                    {
                        setWindowTitle ( "Select Contest from VHF other Calendar" );
                        vhfother = Calendar ( year, ectVHFOther );

                        yearList.clear();
                        for ( int i = LOWYEAR; i <= HIGHYEAR; i++ )
                        {
                            yearList.push_back ( QSharedPointer<CalendarYear> ( new VHFOtherCalendarYear ( i ) ) );
                        }

                        loaded = loadYear ( vhfother, year );
                        if ( !loaded )
                        {
                            mShowMessage ( "Failed to load the VHF other calendar file", this );
                            ui->CalendarVersionLabel->setText( "No file loaded" );
                            ui->CalendarGrid->setRowCount( 0 );
                            return ; // don't close - they need a chance to download
                        }
                        LoadGrid ( vhfother );
                    }
                    else
                        if ( calType == ectMwave )
                        {
                            setWindowTitle ( "Select Contest from Microwave Calendar" );
                            mwave = Calendar ( year, ectMwave );

                            yearList.clear();
                            for ( int i = LOWYEAR; i <= HIGHYEAR; i++ )
                            {
                                yearList.push_back ( QSharedPointer<CalendarYear> ( new MicroCalendarYear ( i ) ) );
                            }

                            loaded = loadYear ( mwave, year );

                            if ( !loaded )
                            {
                                mShowMessage ( "Failed to load the Microwave calendar file for " + QString::number ( year ), this );
                                ui->CalendarVersionLabel->setText( "No file loaded" );
                                ui->CalendarGrid->setRowCount( 0 );
                                return ; // don't close - they need a chance to download
                            }
                            LoadGrid ( mwave );
                        }
}
//---------------------------------------------------------------------------
bool CalendarYear::downloadFile ( bool showError )
{
    QString calendarURL = getURL();


    QNetworkAccessManager m_NetworkMngr;

    QUrl qurl( calendarURL );
    QNetworkRequest qnr( qurl );

    qnr.setRawHeader( "User-Agent" , "Mozilla/4.0 (compatible;Adjsql)" );

    QNetworkReply *reply = m_NetworkMngr.get( qnr );

    QEventLoop loop;
    QObject::connect( reply, SIGNAL( finished() ), &loop, SLOT( quit() ) );
    loop.exec();

    if ( reply->error() == QNetworkReply::NoError )
    {
        int raw = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (raw == 301)
        {
            QUrl redirect =  reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

            QNetworkRequest qnr1( redirect );
            qnr1.setRawHeader( "User-Agent" , "Mozilla/4.0 (compatible;Adjsql)" );

            delete reply;
            reply = m_NetworkMngr.get( qnr1 );
            QEventLoop loop;
            QObject::connect( reply, SIGNAL( finished() ), &loop, SLOT( quit() ) );
            loop.exec();
            raw = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        }
        QByteArray data = reply->readAll();
        if (data.size() > 0)
        {
            QUrl aUrl( calendarURL );
            QFileInfo fileInfo = aUrl.path();

            QFile file( getPath() );
            file.open( QIODevice::WriteOnly );
            file.write( data );
            trace ( "HTPP Get of " + calendarURL + " OK" );
        }
        else
        {
           trace ( "HTPP Get of " + calendarURL + " failed - zero length data returned with attribute " + QString::number(raw));
        }
        delete reply;
        return true;
    }
    else
    {
        trace ( QString( "HTPP Get of " ) + calendarURL + " failed: " + reply->errorString() );
        if ( showError )
        {
            mShowMessage ( QString( "HTPP Get of " ) + calendarURL + " failed: " + reply->errorString(), LogContainer );
        }
    }

    delete reply;
    return false;
}
void TCalendarForm::downloadFiles()
{

    QString fpath = "./Configuration";

    int fileCount = 0;

    std::vector<QSharedPointer<CalendarYear> > yearList;

    yearList.push_back ( QSharedPointer<CalendarYear> ( new CTYCalendarYear ( 0 ) ) );
    for ( int i = LOWURLYEAR; i <= HIGHYEAR; i++ )
    {
        yearList.push_back ( QSharedPointer<CalendarYear> ( new VHFCalendarYear ( i ) ) );
//        yearList.push_back ( QSharedPointer<CalendarYear> ( new HFCalendarYear ( i ) ) );
//        yearList.push_back ( QSharedPointer<CalendarYear> ( new HFBARTGCalendarYear ( i ) ) );
//        yearList.push_back ( QSharedPointer<CalendarYear> ( new MicroCalendarYear ( i ) ) );
    }

    for ( unsigned int i = 0; i < yearList.size(); i++ )
    {
        if ( yearList[ i ] ->downloadFile ( false ) )
        {
            fileCount++;
        }
    }

    mShowMessage( QString::number ( fileCount ) + " of " + QString::number( yearList.size() ) + " files downloaded. We don't expect to load them all.", LogContainer );
}
//---------------------------------------------------------------------------

void TCalendarForm::on_CloseButton_clicked()
{
    reject();
}

void TCalendarForm::on_SelectButton_clicked()
{
    int row = ui->CalendarGrid->currentRow();
    if ( calType == ectHF )
    {
        if ( row >= 0 && row < ( int ) hf.calendar.size() )
        {
            ic = hf.calendar[ row ];
            accept();
        }
    }
    else
        if ( calType == ectHFOther )
        {
            if ( row >= 0 && row < ( int ) hfother.calendar.size() )
            {
                ic = hfother.calendar[ row ];
                accept();
            }
        }
        else
            if ( calType == ectVHF )
            {
                if ( row >= 0 && row < ( int ) vhf.calendar.size() )
                {
                    ic = vhf.calendar[ row ];
                    accept();
                }
            }
            else
                if ( calType == ectVHFOther )
                {
                    if ( row >= 0 && row < ( int ) vhfother.calendar.size() )
                    {
                        ic = vhfother.calendar[ row ];
                        accept();
                    }
                }
                else
                    if ( calType == ectMwave )
                    {
                        if ( row >= 0 && row < ( int ) mwave.calendar.size() )
                        {
                            ic = mwave.calendar[ row ];
                            accept();
                        }

                    }
                    else
                        if ( calType == ectHFBARTG )
                        {
                            if ( row >= 0 && row < ( int ) hfbartg.calendar.size() )
                            {
                                ic = hfbartg.calendar[ row ];
                                accept();
                            }

                        }
}

void TCalendarForm::on_GetCalendarButton_clicked()
{
    downloadFiles();
    FormShow ( );
}

void TCalendarForm::on_YearDownButton_clicked()
{
    bool ok;
    int year = ui->YearEdit->text().toInt(&ok);
    if (!ok)
        year = curYear;

    year--;

    if ( year < curYear + LOWYEAR )
    {
        year = curYear + HIGHYEAR;
    }
    ui->YearEdit->setText( QString::number( year ) );

    FormShow ( );

}

void TCalendarForm::on_YearUpButton_clicked()
{
    bool ok;
    int year = ui->YearEdit->text().toInt(&ok);
    if (!ok)
        year = curYear;
    year++;

    if ( year > curYear + HIGHYEAR )
    {
        year = curYear + LOWYEAR;
    }
    ui->YearEdit->setText( QString::number( year ) );

    FormShow ( );
}

void TCalendarForm::on_CalendarGrid_doubleClicked(const QModelIndex &/*index*/)
{
    on_SelectButton_clicked();
}

