#include "logger_pch.h"
#include "tentryoptionsform.h"
#include "ui_tentryoptionsform.h"


TEntryOptionsForm::TEntryOptionsForm( QWidget* Owner, LoggerContestLog * cnt, bool saveMinos ):
    QDialog(Owner),
    ui(new Ui::TEntryOptionsForm),
    ct( cnt ), minosSave( saveMinos ), opsQSOLine1(-1), opsQSOLine2(-1),
    opsEntryLine1(-1), opsEntryLine2(-1)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if ( !ct )
        return ;

    QSettings settings;
    QByteArray geometry = settings.value("EntryOptions/geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);


    QStringList vlabels =
    {
        "Date Range (Calculated)",
        "Contest Name",
        "Band",
        "Entrant name (or group)",
        "Station QTH 1",
        "Station QTH 2",
        "Section",
        "Callsign",
        "Locator",
        "Exchange/code/QTH sent",
        "Transmitter",
        "Transmit Power",
        "Receiver",
        "Antenna",
        "Height above ground",
        "Height above sea level",
        "(From QSOs) Operators Line 1",
        "(From QSOs) Operators Line 2",
        "(Entry)Operators Line 1",
        "(Entry)Operators Line 2",
        "Conditions/Comments",
        "Conditions/Comments",
        "Conditions/Comments",
        "Conditions/Comments",
        "Name for Correspondence",
        "Callsign for Correspondence",
        "Address 1 for Correspondence",
        "Address 2 for Correspondence",
        "City for Correspondence",
        "Country for Correspondence",
        "Postcode for Correspondence",
        "Phone number for queries",
        "email address for queries"
    };

    ui->OptionsScrollBox->setColumnCount(1);
    ui->OptionsScrollBox->setRowCount(vlabels.count());

    int r = 0;

    ui->OptionsScrollBox->setVerticalHeaderLabels(vlabels);

    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->dateRange( DTGDISP )));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->name.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->band.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entrant.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->sqth1.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->sqth2.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entSect.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->mycall.fullCall.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->myloc.loc.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->location.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entTx.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->power.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entRx.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entAnt.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entAGL.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entASL.getValue()));

    getContestOperators();
    opsQSOLine1 = r;
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->opsQSO1));
    opsQSOLine2 = r;
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->opsQSO2));
    opsEntryLine1 = r;
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->ops1.getValue()));
    opsEntryLine2 = r;
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->ops1.getValue()));

    // Focus here
    //ActiveControl = options[ r ]->OptionEdit;

    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entCondx1.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entCondx2.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entCondx3.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entCondx4.getValue()));

    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entName.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entCall.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entAddr1.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entAddr2.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entCity.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entCountry.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entPostCode.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entPhone.getValue()));
    ui->OptionsScrollBox->setItem(r++, 0, new QTableWidgetItem(ct->entEMail.getValue()));

    ui->OptionsScrollBox->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->OptionsScrollBox->horizontalHeader()->hide();
    ui->OptionsScrollBox->verticalHeader()->show();

    ui->EntryGroup->setId( ui->enrb0, 0 );
    ui->EntryGroup->setId( ui->enrb1, 1 );
    ui->EntryGroup->setId( ui->enrb2, 2 );
    ui->EntryGroup->setId( ui->enrb3, 3 );
    ui->EntryGroup->setId( ui->enrb4, 4 );
    ui->EntryGroup->setId( ui->enrb5, 5 );

    if ( minosSave )
    {
        ui->EntryGroup->buttons().at( EMINOS ) ->setChecked( true );
        ui->EntryGroupBox->setVisible( false);
    }
    else
    {
        ui->EntryGroup->buttons().at( EREG1TEST ) ->setChecked( true );
    }
}

TEntryOptionsForm::~TEntryOptionsForm()
{
    delete ui;
}
void TEntryOptionsForm::doCloseEvent()
{
    QSettings settings;
    settings.setValue("EntryOptions/geometry", saveGeometry());
}
void TEntryOptionsForm::reject()
{
    doCloseEvent();
    QDialog::reject();
}
void TEntryOptionsForm::accept()
{
    doCloseEvent();
    QDialog::accept();
}

void TEntryOptionsForm::on_CloseButton_clicked()
{
    int r = 0;
    r++;  // date range not editable

    ct->name.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->band.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entrant.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->sqth1.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->sqth2.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entSect.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->mycall.fullCall.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->myloc.loc.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->location.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entTx.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->power.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entRx.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entAnt.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entAGL.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entASL.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );

    ct->opsQSO1 = ui->OptionsScrollBox->item(r++, 0)->text() ;
    ct->opsQSO2 = ui->OptionsScrollBox->item(r++, 0)->text() ;

    ct->ops1.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->ops2.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );

    ct->entCondx1.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entCondx2.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entCondx3.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entCondx4.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entName.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entCall.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entAddr1.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entAddr2.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entCity.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entCountry.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entPostCode.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entPhone.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );
    ct->entEMail.setValue( ui->OptionsScrollBox->item(r++, 0)->text() );

    //enum ExportType {EREG1TEST, EADIF, EG0GJV, EMINOS, EKML};
    expformat = static_cast< ExportType> (ui->EntryGroup->checkedId());

    accept();
}

void TEntryOptionsForm::on_CancelButton_clicked()
{
    reject();
}
//---------------------------------------------------------------------------
void TEntryOptionsForm::getContestOperators()
{
    OperatorList operators;

    for ( OperatorIterator op = ct->oplist.begin(); op != ct->oplist.end(); op++ )
    {
        operators.insert(*op, *op);
    }

    // now actual ops are a sorted list

    QString ops1;
    QString ops2;
    int ls = operators.size();
    int i = 0;
    foreach(QString op, operators)
    {
        if (i <= ls/2)
        {
            ops1 += op + " ";
        }
        else
        {
            ops2 += op + " ";
        }
        i++;
    }
    ct->opsQSO1 = ops1.trimmed();
    ct->opsQSO2 = ops2.trimmed();
}

QString TEntryOptionsForm::doFileSave( )
{
    QString InitialDir = ExtractFilePath( ct->cfileName );

    QString expName;
    QString fname = ExtractFileName( ct->cfileName );
    QString ext = ExtractFileExt( ct->cfileName );
    fname = fname.left( fname.size() - ext.size() );

    //enum ExportType {EREG1TEST, EADIF, EG0GJV, EMINOS, EKML, EPRINTFILE };
    QString defext;
    QString filter;
    switch ( expformat )
    {
    case EG0GJV:
        defext = "gjv";
        filter = "GJV ContestLog files (*.gjv);;All Files (*.*);;" ;
        break;
    case EMINOS:
        defext = "minos";
        filter = "Minos ContestLog files (*.minos *.Minos);;All Files (*.*);;" ;
        break;
    case EADIF:
        defext = "adi";
        filter = "ADIF files (*.adi);;All Files (*.*);;" ;
        break;
    case EKML:
        defext = "kml";
        filter = "KML(GoogleEarth) files (*.kml);;All Files (*.*);;" ;
        break;
    case EREG1TEST:
        defext = "edi";
        ct->mycall.valRes = CS_NOT_VALIDATED;
        ct->mycall.validate( );
        filter = "Region 1 EDI files (*.edi);;All Files (*.*);;" ;
        break;
    case EPRINTFILE:
    {
        defext = "txt";
        filter = "Text output (*.txt);;All Files (*.*);;" ;
    }
        break;
    }

    bool Ok = false;

    while (!Ok)
    {
        QString fileName = QFileDialog::getSaveFileName( this,
                                                         "Save contest as...",
                                                         InitialDir + "/" + fname + "." + defext,
                                                         filter,
                                                         0,
                                                         QFileDialog::DontConfirmOverwrite
                                                         );
        if ( !fileName.isEmpty() )
        {

            expName = fileName;

            // open the export file
            if ( FileAccessible(expName) && !FileWriteable(expName) )
            {
                    MinosParameters::getMinosParameters() ->mshowMessage( "File is Read Only", this );
                    continue;
            }
            if ( MinosParameters::getMinosParameters() ->isContestOpen( expName ) )
            {
                // then try again...
                continue;
            }

            QIODevice::OpenMode om = QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered;
            QSharedPointer<QFile> contestFile(new QFile(expName));

            if (!contestFile->open(om))
            {
               QString lerr = contestFile->errorString();
               QString emess = "Failed to open Contest export file " + expName + " : " + lerr;
               MinosParameters::getMinosParameters() ->mshowMessage( emess, this );
               continue;
            }


            int ret = ct->export_contest( contestFile, expformat );
            contestFile->close();

            if ( ret == -1 )
            {
                QFile::remove( expName );		// failure response, so delete file again
                expName = "";
            }
            break;
        }
        else
        {
            return "";
        }
    }
    return expName;
}
