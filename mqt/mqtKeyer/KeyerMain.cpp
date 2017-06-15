#include <QFileDialog>

#include "KeyerMain.h"
#include "ui_KeyerMain.h"
#include "keyctrl.h"
#include "KeyerRPCServer.h"
#include "VKMixer.h"

KeyerMain *keyerMain = 0;

QString alsaStore("store");
QString alsaRestore("restore");
bool inhibitCallbacks = false;

// texts for displaying the current mixer set

const char *msets[emsMaxMixerSet] = {"Unloaded", "PassThrough(NoPTT)", "PassThrough(PTT)",
                 "Replay", "Replay Pip", "Replay Tone1", "Replay Tone2",
                 "Voice Record",
                 "CW Transmit", "CW PassThrough"
                };

void lcallback( bool pPTT, bool pkeyline, bool pPTTRef, bool pL1Ref, bool pL2Ref )
{
    if (!inhibitCallbacks)
        keyerMain->setLines(pPTT, pPTTRef, pL1Ref, pL2Ref, pkeyline);
}

//---------------------------------------------------------------------------
void recvolcallback( unsigned int rmsvol, unsigned int peakvol, int samples )
{
    if (!inhibitCallbacks)
        keyerMain->recvolcallback(rmsvol, peakvol, samples);
}
//---------------------------------------------------------------------------
void outvolcallback( unsigned int rmsvol, unsigned int peakvol, int samples )
{
    if (!inhibitCallbacks)
        keyerMain->outvolcallback(rmsvol, peakvol, samples);
}
void KeyerMain::outvolcallback( unsigned int rmsvol, unsigned int peakvol, int samples )
{
    if (!inhibitCallbacks)
        ui->outputLevelMeter->levelChanged( rmsvol / 32768.0, peakvol / 32768.0, samples );
}
//---------------------------------------------------------------------------
void KeyerMain::recvolcallback(unsigned int rmsvol , unsigned int peakvol, int samples)
{
    if (!inhibitCallbacks)
        ui->inputLevelMeter->levelChanged( rmsvol / 32768.0, peakvol / 32768.0, samples );
}

//---------------------------------------------------------------------------
void KeyerMain::setLines( bool PTTOut, bool PTTIn, bool L1, bool L2, bool key )
{
    PTT = PTTOut;
    PTTRef = PTTIn;
    L1Ref = L1;
    L2Ref = L2;
    keyline = key;
}
void KeyerMain::syncSetLines()
{
   ui->PTTOutCheckBox->setChecked(PTT);
   ui->KeyCheckBox->setChecked(keyline);
   ui->PTTReflectCheckBox->setChecked(PTTRef);
   ui->L1ReflectCheckBox->setChecked(L1Ref);
   ui->L2ReflectCheckBox->setChecked(L2Ref);
}



KeyerMain::KeyerMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KeyerMain),
    mixer(0),
    PTT(false), keyline(false), PTTRef(false), L1Ref(false), L2Ref(false),
    recordWait(false),
    recording(false),
    inVolChange(false),
    runner(0)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QSettings settings;
    QByteArray geometry = settings.value("KeyerMain/geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    enableTrace( "./TraceLog", "MinosKeyer_" );

    createCloseEvent();

    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    QString alsaFileName = keyerSettings.value("AlsaCtlFile", "").toString();
    ui->setupScriptEdit->setText(alsaFileName);

    runAlsaScript(alsaFileName, alsaRestore);

    mixer = VKMixer::OpenMixer();

    keyerMain = this;
    setLineCallBack( lcallback );
    setVUCallBack( &::recvolcallback, &::outvolcallback );

    loadKeyers();

    connect(&LineTimer, SIGNAL(timeout()), this, SLOT(LineTimerTimer()));
    LineTimer.start(100);

    // NB CaptionTimer only runs after something changes - the line timer triggers it
    connect(&CaptionTimer, SIGNAL(timeout()), this, SLOT(CaptionTimerTimer()));

    ui->PipCheckBox->setChecked(getPipEnabled());

    ui->delayEdit->setValue(getAutoRepeatDelay());
    ui->AutoRepeatCheckBox->setChecked( getEnableAutoRepeat());

    for (int i = 1; i < 10; i++)
    {
        ui->keyCombo->addItem(QString::number(i));
    }
    ui->keyCombo->setCurrentIndex(0);
}

KeyerMain::~KeyerMain()
{
    inhibitCallbacks = true;
    delete ui;
}

void KeyerMain::closeEvent(QCloseEvent *event)
{
    inhibitCallbacks = true;
    unloadKeyers();

    QWidget::closeEvent(event);
}
void KeyerMain::moveEvent(QMoveEvent *event)
{
    QSettings settings;
    settings.setValue("KeyerMain/geometry", saveGeometry());
    QWidget::moveEvent(event);
}
void KeyerMain::resizeEvent(QResizeEvent *event)
{
    QSettings settings;
    settings.setValue("KeyerMain/geometry", saveGeometry());
    QWidget::resizeEvent(event);
}
void KeyerMain::changeEvent( QEvent* e )
{
    if( e->type() == QEvent::WindowStateChange )
    {
        QSettings settings;
        settings.setValue("geometry", saveGeometry());
    }
}

void KeyerMain::LineTimerTimer( )
{
    static bool closed = false;
    if ( !closed )
    {
       if ( checkCloseEvent() )
       {
          closed = true;
          close();
          return;
       }
    }

    syncSetLines();
   bool PTT = getPTT();
   if ( recordWait && PTT )
   {
      ui->recind->setText("Release PTT to stop Recording");
      recordWait = false;
      recording = true;
   }
   else
      if ( recording && !PTT )
      {
         ui->recind->setText("");
         recordWait = false;
         recording = false;
      }
   KeyerServer::publishCommand( ui->recind->text() );
   eMixerSets m = mixer->GetCurrentMixerSet();

   QString astate;
   getActionState( astate );

   // This isn't quite what we want - needs to be better english and slower changing

   QString kstatus;
   if ( getKeyerStatus( kstatus ) )
      setWindowTitle(QString( msets[ m ] ) + " : " + astate + " : " + kstatus);
   else
      setWindowTitle(QString( msets[ m ] ) + " : " + astate);

   static QString old;

   if ( windowTitle() != old )
   {
      old = windowTitle();
      CaptionTimer.start(200);
   }
}
void KeyerMain::CaptionTimerTimer( )
{
   CaptionTimer.stop();

   KeyerServer::publishState( windowTitle() );
}
void KeyerMain::on_recordButton_clicked()
{
    trace("Record Button");
    int fno = ui->keyCombo->currentText().toInt();
    if ( fno >= 1 && fno <= 12 )
    {
       startRecordDVPFile( fno );
       ui->recind->setText("Push PTT to Commence Recording");
       KeyerServer::publishCommand( ui->recind->text() );
       recordWait = true;
    }
}

void KeyerMain::on_playButton_clicked()
{
    trace("Play Button");
    int fno = ui->keyCombo->currentText().toInt( );
    if ( fno >= 1 && fno <= 12 )
    {
       ui->recind->setText("");
       playKeyerFile( fno, false );
    }
    KeyerServer::publishCommand( ui->recind->text() );
}

void KeyerMain::on_showButton_clicked()
{
    trace("Show Button");
    /*
    if ( !WaveShowForm )
     {
        WaveShowForm = new TWaveShowForm( this );
        WaveShowForm->Top = Top + 50;
        WaveShowForm->Left = Left + 50;
        int fno = KeyCombo->Text.ToIntDef( 1 );
        WaveShowForm->setSamples( fno );
        WaveShowForm->Show();
     }
     else
     {
        WaveShowForm->Close();
        delete WaveShowForm;
     }
     */
}

void KeyerMain::on_stopButton_clicked()
{
    trace("Stop Button");
    if ( recordWait )
    {
       ui->recind->setText("");
       recordWait = false;
    }
    if ( recording )
    {
       finishRecordDVPFile();
       ui->recind->setText("");
       recording = false;
    }
    else
       stopKeyer();
    KeyerServer::publishCommand( ui->recind->text() );
}

void KeyerMain::on_AutoRepeatCheckBox_clicked()
{
    setEnableAutoRepeat( ui->AutoRepeatCheckBox->isChecked() );

}

void KeyerMain::on_PipCheckBox_clicked()
{
    setPipEnabled( ui->PipCheckBox->isChecked() );
}

void KeyerMain::on_delayEdit_valueChanged(const QString &/*arg1*/)
{
    QString temp = ui->delayEdit->text();
    int val = temp.toInt();
    if (val > 0)
    {
       setAutoRepeatDelay( val );
    }

}

void KeyerMain::on_tuneButton_clicked()
{
    trace("Tone 1 Button");
    sendTone1();
}

void KeyerMain::on_twoToneButton_clicked()
{
    trace("Tone 2 Button");
    sendTone2();
}

void KeyerMain::on_aboutButton_clicked()
{

}

void KeyerMain::runAlsaScript(const QString &alsaFileName, const QString &command)
{
    if (alsaFileName.isEmpty())
        return;

    if ( !runner)
    {
        runner = new QProcess(parent());
        connect (runner, SIGNAL(started()), this, SLOT(on_started()));
        connect (runner, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(on_finished(int, QProcess::ExitStatus)));
        connect (runner, SIGNAL(error(QProcess::ProcessError)), this, SLOT(on_error(QProcess::ProcessError)));

        connect (runner, SIGNAL(readyReadStandardError()), this, SLOT(on_readyReadStandardError()));
        connect (runner, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readyReadStandardOutput()));

        QString wdir = GetCurrentDir();
        runner->setWorkingDirectory(wdir);
    }


    QString commandLine = "alsactl -f " + alsaFileName + " " + command;// <card>
    runner->start(commandLine);
}
void KeyerMain::on_started()
{
    trace("AlsaCtl :started");
}

void KeyerMain::on_finished(int err, QProcess::ExitStatus exitStatus)
{
    trace("AlsaCtl :finished:" + QString::number(err) + ":" + QString::number(exitStatus));

    // we need to delete runner sometime - but not yet?
}

void KeyerMain::on_error(QProcess::ProcessError error)
{
    trace("AlsaCtl :error:" + QString::number(error));
}

void KeyerMain::on_readyReadStandardError()
{
    QString r = runner->readAllStandardError();
    trace("AlsaCtl :stdErr:" + r);
}

void KeyerMain::on_readyReadStandardOutput()
{
    QString r = runner->readAllStandardOutput();
    trace("AlsaCtl :stdOut:" + r);
}


void KeyerMain::on_setupBrowseButton_clicked()
{
    QString InitialDir = GetCurrentDir();

    QString Filter = "Alsa Control Files (*.txt);;"
                     "All Files (*.*)" ;

    QString alsaFileName = QFileDialog::getOpenFileName( this,
                       "File for mixer setup",
                       InitialDir,                   // opendir
                       Filter );

    if ( !alsaFileName.isEmpty() )
    {
        // run alsactl on the file
        // so we need the alsa card names :(
        // AND I think we need the QtAudio card names
        // unless we can always use the default card...

        // alsactl -f <filename> restore <card>

        // And save the file name for the next startup

        ui->setupScriptEdit->setText(alsaFileName);

        QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
        keyerSettings.setValue("AlsaCtlFile", alsaFileName);

        runAlsaScript(alsaFileName, alsaRestore);
    }
}

void KeyerMain::on_saveAlsaButton_clicked()
{
    QString alsaFileName = ui->setupScriptEdit->text();
    runAlsaScript(alsaFileName, alsaStore);
}

void KeyerMain::on_restoreAlsaButton_clicked()
{
    QString alsaFileName = ui->setupScriptEdit->text();
    runAlsaScript(alsaFileName, alsaRestore);
}
