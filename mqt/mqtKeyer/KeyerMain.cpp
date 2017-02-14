#include "KeyerMain.h"
#include "ui_KeyerMain.h"
#include "keyctrl.h"
#include "KeyerRPCServer.h"
#include "VKMixer.h"
#include "AlsaVolume.h"

KeyerMain *keyerMain = 0;

void lcallback( bool pPTT, bool pkeyline, bool pPTTRef, bool pL1Ref, bool pL2Ref )
{
    keyerMain->setLines(pPTT, pPTTRef, pL1Ref, pL2Ref, pkeyline);
}
//---------------------------------------------------------------------------
void recvolcallback( unsigned int vol )
{
    keyerMain->recvolcallback(vol);
}
//---------------------------------------------------------------------------
void outvolcallback( unsigned int vol )
{
    keyerMain->outvolcallback(vol);
}
void KeyerMain::outvolcallback( unsigned int vol )
{
   ui->commonLevelMeter->levelChanged( vol / 65536.0, vol / 65536.0, 200 );
}
//---------------------------------------------------------------------------
void KeyerMain::recvolcallback( unsigned int vol )
{
    ui->commonLevelMeter->levelChanged( vol / 65536.0, vol / 65536.0, 200 );
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
    PTT(false), keyline(false), PTTRef(false), L1Ref(false), L2Ref(false),
    recordWait(false),
    recording(false),
    inVolChange(false)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
//    connect(m_engine, SIGNAL(levelChanged(qreal, qreal, int)),
//            m_levelMeter, SLOT(levelChanged(qreal, qreal, int)));

    enableTrace( "./TraceLog", "MinosKeyer_" );

    createCloseEvent();

    QList<QAudioDeviceInfo> audioInputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

    foreach(const QAudioDeviceInfo indev, audioInputDevices)
    {
        ui->inputComboBox->addItem(indev.deviceName());
    }

    QList<QAudioDeviceInfo> audioOutputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach(const QAudioDeviceInfo outdev, audioOutputDevices)
    {
        ui->outputComboBox->addItem(outdev.deviceName());
    }

    keyerMain = this;
    setLineCallBack( lcallback );
    setVUCallBack( &::recvolcallback, &::outvolcallback );

    loadKeyers();

    connect(&LineTimer, SIGNAL(timeout()), this, SLOT(LineTimerTimer()));
    LineTimer.start(100);

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
    delete ui;
}

void KeyerMain::closeEvent(QCloseEvent * /*event*/)
{
    unloadKeyers();
}
const char *msets[] = {"emsUnloaded", "emsPassThroughNoPTT", "emsPassThroughPTT",
                 "emsReplay", "emsReplayPip", "emsReplayT1", "emsReplayT2",
                 "emsVoiceRecord",
                 "emsCWTransmit", "emsCWPassThrough",
                 "emsMicMonitor", "emsReplayMonitor",
                 "emsMaxMixerSet"
                };
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
   eMixerSets m = GetCurrentMixerSet();

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

   inVolChange = true;

   qreal inVol = getKeyerRecordVolume();
   qreal outVol = getKeyerPlaybackVolume();

   ui->inputLevelSlider->setValue(inVol * ui->inputLevelSlider->maximum());
   ui->outputLevelSlider->setValue(outVol * ui->outputLevelSlider->maximum());

   inVolChange = false;
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

void KeyerMain::on_inputComboBox_currentTextChanged(const QString &/*arg1*/)
{

}

void KeyerMain::on_outputComboBox_currentTextChanged(const QString &/*arg1*/)
{

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


void KeyerMain::on_inputLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        setKeyerRecordVolume(1.0*position/ui->inputLevelSlider->maximum());
    }
}

void KeyerMain::on_outputLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        setKeyerPlaybackVolume(1.0*position/ui->outputLevelSlider->maximum());
    }
}
