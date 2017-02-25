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
   ui->masterLevelMeter->levelChanged( vol / 65536.0, vol / 65536.0, 200 );
}
//---------------------------------------------------------------------------
void KeyerMain::recvolcallback( unsigned int vol )
{
    ui->inputLevelMeter->levelChanged( vol / 65536.0, vol / 65536.0, 200 );
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

void KeyerMain::setMixerCombo(QComboBox *combo, QList<QAudioDeviceInfo> audioDevices, QAudioFormat *qaf)
{
    int cwidth = combo->width();
    foreach(const QAudioDeviceInfo indev, audioDevices)
    {
        if (!qaf || indev.isFormatSupported(*qaf))
        {
            QString name = indev.deviceName();
            combo->addItem(name);
            QFontMetrics fm(combo->fontMetrics());
            int pixelsWide = fm.width(name);
            cwidth = qMax(cwidth,pixelsWide );
        }
    }
    combo->view()->setMinimumWidth(cwidth);
}
void KeyerMain::setMixerCombo(QComboBox *combo, QVector<Device> devices)
{
    int cwidth = combo->width();
    foreach(const Device indev, devices)
    {
        QString name = indev.devName;
        combo->addItem(name);
        QFontMetrics fm(combo->fontMetrics());
        int pixelsWide = fm.width(name);
        cwidth = qMax(cwidth,pixelsWide );
    }
    combo->view()->setMinimumWidth(cwidth);
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

    AlsaVolume av;
    QVector<Card> cards = av.init();

    foreach (Card card, cards)
    {
        ui->cardCombo->addItem(card.name);
    }

    QAudioFormat qaf;
    qaf.setChannelCount(1);
    qaf.setSampleRate(22050);
    qaf.setSampleSize(16);
    qaf.setSampleType(QAudioFormat::SignedInt);
    qaf.setByteOrder(QAudioFormat::LittleEndian);
    qaf.setCodec("audio/pcm");

    // This gives us alsa AND pulse devices. I think we only want alsa
    // device given is the frst
/*
    QList<QAudioDeviceInfo> audioInputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    setMixerCombo(ui->inputCombo, audioInputDevices, &qaf);

    //QList<QAudioDeviceInfo> audioInputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    setMixerCombo(ui->passthruCombo, audioInputDevices, 0);

    QList<QAudioDeviceInfo> audioOutputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    setMixerCombo(ui->outputCombo, audioOutputDevices, &qaf);
*/
    setMixerCombo(ui->inputCombo, cards[0].capture);
    setMixerCombo(ui->outputCombo, cards[0].playback);

    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
/*
    applyMixerSetting(keyerSettings, "Card", ui->cardCombo);

    applyMixerSetting(keyerSettings, "PCMInput", ui->inputCombo);
    applyMixerSetting(keyerSettings, "PCMInput", ui->inputLevelCombo);
    applyMixerSetting(keyerSettings, "PCMInput", ui->inputMuteCombo);

    applyMixerSetting(keyerSettings, "PCMInput", ui->outputCombo);
    applyMixerSetting(keyerSettings, "PCMInput", ui->outputLevelCombo);
    applyMixerSetting(keyerSettings, "PCMInput", ui->outputMuteCombo);

    applyMixerSetting(keyerSettings, "PCMInput", ui->passthruCombo);
    applyMixerSetting(keyerSettings, "PCMInput", ui->passthruLevelCombo);
    applyMixerSetting(keyerSettings, "PCMInput", ui->passthruMuteCombo);
*/
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

   QSettings settings;
   bool inpok;
   bool outok;
   qreal invol = 0.0;
   qreal outvol = 0.0;

   invol = settings.value("Volume/input", 0.0).toReal(&inpok);
   outvol = settings.value("Volume/output", 0.0).toReal(&outok);;
   ui->inputLevelSlider->setValue(invol * ui->inputLevelSlider->maximum());
   ui->masterLevelSlider->setValue(outvol * ui->masterLevelSlider->maximum());

   setKeyerPlaybackVolume(invol);
   setKeyerPlaybackVolume(outvol);

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
        qreal vol = 1.0*position/ui->inputLevelSlider->maximum();
        setKeyerRecordVolume(vol);
        QSettings settings;
        settings.setValue("Volume/input", vol);
    }
}

void KeyerMain::on_masterLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        qreal vol = 1.0*position/ui->masterLevelSlider->maximum();
        setKeyerPlaybackVolume(vol);
        QSettings settings;
        settings.setValue("Volume/output", vol);
    }
}

void KeyerMain::on_passthruLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        qreal vol = 1.0*position/ui->passthruLevelSlider->maximum();
        setKeyerPassthruVolume(vol);
        QSettings settings;
        settings.setValue("Volume/passthru", vol);
    }
}

void KeyerMain::on_alsaTestButton_clicked()
{
    AlsaVolume av;
    av.init();
}

void KeyerMain::applyMixerSetting(QSettings &keyerSettings, QString key, QComboBox *combo)
{
    QString name = keyerSettings.value(key, "").toString();
    combo->setCurrentText(name);
}

void KeyerMain::saveMixerSetting(QSettings &keyerSettings, QString key, QComboBox *combo)
{
    QString name = combo->currentText();
    keyerSettings.setValue(key, name);
}

void KeyerMain::on_mixerApplyButton_clicked()
{
    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;

    saveMixerSetting(keyerSettings, "Card", ui->cardCombo);

    saveMixerSetting(keyerSettings, "PCMInput", ui->inputCombo);
    saveMixerSetting(keyerSettings, "InputLevel", ui->inputLevelCombo);
    saveMixerSetting(keyerSettings, "InputMute", ui->inputMuteCombo);

    saveMixerSetting(keyerSettings, "PCMOutput", ui->outputCombo);
    saveMixerSetting(keyerSettings, "OutputLevel", ui->outputLevelCombo);
    saveMixerSetting(keyerSettings, "OutputMute", ui->outputMuteCombo);

    saveMixerSetting(keyerSettings, "PassThru", ui->passthruCombo);
    saveMixerSetting(keyerSettings, "PassThruLevel", ui->passthruLevelCombo);
    saveMixerSetting(keyerSettings, "PassThruMute", ui->passthruMuteCombo);

    close();
}
