#include "KeyerMain.h"
#include "ui_KeyerMain.h"
#include "keyctrl.h"
#include "KeyerRPCServer.h"
#include "VKMixer.h"
#include "AlsaVolume.h"

KeyerMain *keyerMain = 0;

class MixerSet
{
   public:
      MixerSet( bool PassThruMute, bool MasterMute )
            : PassThruMute( PassThruMute ), MasterMute( MasterMute )
      {}
      // we need a line setting, stereo volume + mute
      bool PassThruMute;
      bool MasterMute;
};
//---------------------------------------------------------------------------

MixerSet MixerSets[ emsMaxMixerSet ] =
   {
      // revise to MicOut mute, Speaker mute as only ones we need to drive
      // and we need to mute or drive to zero if no mute.
      // MixerSet(MicRec, MicOut,  Rec,   Master)
      // MixerSet(bool MicOutMute, bool MasterMute)

      MixerSet( false, false ),         //emsUnloaded
      MixerSet( false, false ),         //emsPassThroughNoPTT
      MixerSet( false, false ),         //emsPassThroughPTT
      MixerSet( true , false ),         //emsReplay
      MixerSet( true , false ),         //emsReplayPip
      MixerSet( true , false ),         //emsReplayT1
      MixerSet( true , false ),         //emsReplayT2
      MixerSet( false, true ),         //emsVoiceRecord
      MixerSet( true , false ),         //emsCWTransmit
      MixerSet( true , false )         //emsCWPassThrough
   };
eMixerSets GetCurrentMixerSet()
{
    return keyerMain->GetCurrentMixerSet();
}

void SetCurrentMixerSet( eMixerSets cms )
{
    keyerMain->SetCurrentMixerSet(cms);
}

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
   ui->outputLevelMeter->levelChanged( vol / 65536.0, vol / 65536.0, 200 );
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
eMixerSets KeyerMain::GetCurrentMixerSet()
{
    return CurrMixerSet;
}

void KeyerMain::SetCurrentMixerSet( eMixerSets cms )
{
    CurrMixerSet = cms;
    // and now we need to apply the settings...
    // and we need to mute or drive to zero if no mute.
    // BUT we need to get the correct level to reset it to...
    // and hope it doesn't change while we are busy?

    av.set_switch_indexed(&Px.info.playback, ui->inputControlCombo->currentIndex(), MixerSets[ CurrMixerSet ].PassThruMute);

    av.set_switch_indexed(&Px.info.playback, ui->masterControlCombo->currentIndex(), MixerSets[ CurrMixerSet ].MasterMute);

    adjustDeviceControls(&Px.info.capture, ui->inputControlCombo, ui->inputLevelSlider, ui->inputMute);
    adjustDeviceControls(&Px.info.playback, ui->outputControlCombo, ui->outputLevelSlider, ui->outputMute);
    adjustDeviceControls(&Px.info.playback, ui->passthruControlCombo, ui->passthruLevelSlider, ui->passthruMute);
    adjustDeviceControls(&Px.info.playback, ui->masterControlCombo, ui->masterLevelSlider, ui->masterMute);

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
        QString name = indev.devId;
        combo->addItem(name);
        QFontMetrics fm(combo->fontMetrics());
        int pixelsWide = fm.width(name);
        cwidth = qMax(cwidth,pixelsWide );
    }
    combo->view()->setMinimumWidth(cwidth);
}
void KeyerMain::setMixerCombo(QComboBox *combo, PxDev &devices)
{
    int cwidth = combo->width();
    foreach(const PxSelem selem, devices.selems)
    {
        QString name = selem.name;
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
    inVolChange(false),
    currCardIndex(0),
    currInputIndex(0),
    currOutputIndex(0),
    inInit(true),
    CurrMixerSet(emsUnloaded)

{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QSettings settings;
    QByteArray geometry = settings.value("KeyerMain/geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    enableTrace( "./TraceLog", "MinosKeyer_" );

    createCloseEvent();

    cards = av.init();

    foreach (Card card, cards)
    {
        ui->cardCombo->addItem(card.name);
    }
    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    applyMixerSetting(keyerSettings, "Card", ui->cardCombo);

    av.OpenMixer_Linux_ALSA(&Px);

    setMixerCombo(ui->inputControlCombo, Px.info.capture);
    setMixerCombo(ui->passthruControlCombo, Px.info.playback);
    setMixerCombo(ui->outputControlCombo, Px.info.playback);
    setMixerCombo(ui->masterControlCombo, Px.info.playback);


    applyMixerSetting(keyerSettings, "PCMInput", ui->inputDeviceCombo);
    applyMixerSetting(keyerSettings, "PCMOutput", ui->outputDeviceCombo);

    applyMixerSetting(keyerSettings, "InputControl", ui->inputControlCombo);
    applyMixerSetting(keyerSettings, "OutputControl", ui->outputControlCombo);
    applyMixerSetting(keyerSettings, "PassThruControl", ui->passthruControlCombo);
    applyMixerSetting(keyerSettings, "MasterControl", ui->masterControlCombo);

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

    inInit = false;
}

KeyerMain::~KeyerMain()
{
    delete ui;
}

void KeyerMain::closeEvent(QCloseEvent *event)
{
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
void KeyerMain::on_cardCombo_currentIndexChanged(int index)
{
    currCardIndex = index;

    av.switchCard(currCardIndex);
    setMixerCombo(ui->inputDeviceCombo, cards[currCardIndex].capture);
    setMixerCombo(ui->outputDeviceCombo, cards[currCardIndex].playback);

    if (inInit)
        return;

    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    saveMixerSetting(keyerSettings, "Card", ui->cardCombo);
}

void KeyerMain::on_inputDeviceCombo_currentIndexChanged(int index)
{
    currInputIndex = index;

    //Fill in the capture channels

    if (inInit)
        return;
    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    saveMixerSetting(keyerSettings, "PCMInput", ui->inputDeviceCombo);


}

void KeyerMain::on_outputDeviceCombo_currentIndexChanged(int index)
{
    currOutputIndex = index;
    // fill in the playback channels

    // (is this input or output?) fill in the passthru channels

    if (inInit)
        return;
    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    saveMixerSetting(keyerSettings, "PCMOutput", ui->outputDeviceCombo);

}


void KeyerMain::on_inputControlCombo_currentIndexChanged(int /*index*/)
{

    if (inInit)
        return;
    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    saveMixerSetting(keyerSettings, "InputControl", ui->inputControlCombo);

}

void KeyerMain::on_outputControlCombo_currentIndexChanged(int /*index*/)
{

    if (inInit)
        return;
    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    saveMixerSetting(keyerSettings, "OutputControl", ui->outputControlCombo);

}

void KeyerMain::on_passthruControlCombo_currentIndexChanged(int /*index*/)
{

    if (inInit)
        return;
    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    saveMixerSetting(keyerSettings, "PassThruControl", ui->passthruControlCombo);

}
void KeyerMain::on_masterControlCombo_currentIndexChanged(int /*index*/)
{
    if (inInit)
        return;
    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    saveMixerSetting(keyerSettings, "MasterControl", ui->masterControlCombo);
}

const char *msets[] = {"emsUnloaded", "emsPassThroughNoPTT", "emsPassThroughPTT",
                 "emsReplay", "emsReplayPip", "emsReplayT1", "emsReplayT2",
                 "emsVoiceRecord",
                 "emsCWTransmit", "emsCWPassThrough",
                 "emsMicMonitor", "emsReplayMonitor",
                 "emsMaxMixerSet"
                };

void KeyerMain::adjustDeviceControls( PxDev *dev, QComboBox *devCombo, QSlider *slider, QCheckBox *muteBox)
{
    int index = devCombo->currentIndex();
    qreal vol = av.get_volume_indexed(dev, index);
    bool mute = av.get_switch_indexed(dev, index);

    if (av.has_volume_indexed(dev, index))
    {
        slider->setValue(vol * slider->maximum());
        slider->setEnabled(true);
    }
    else
    {
        slider->setValue(0);
        slider->setEnabled(false);
    }
    if (av.has_volume_indexed(dev, index))
    {
        muteBox->setChecked(mute);
        muteBox->setEnabled(true);
    }
    else
    {
        muteBox->setChecked(false);
        muteBox->setEnabled(false);
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

   av.timer(Px.info.capture);
   av.timer(Px.info.playback);

   inVolChange = true;

   if (av.control_values_changed)
   {
       av.control_values_changed = false;

        adjustDeviceControls(&Px.info.capture, ui->inputControlCombo, ui->inputLevelSlider, ui->inputMute);
        adjustDeviceControls(&Px.info.playback, ui->outputControlCombo, ui->outputLevelSlider, ui->outputMute);
        adjustDeviceControls(&Px.info.playback, ui->passthruControlCombo, ui->passthruLevelSlider, ui->passthruMute);
        adjustDeviceControls(&Px.info.playback, ui->masterControlCombo, ui->masterLevelSlider, ui->masterMute);
   }

/*
 *    QSettings settings;
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
*/
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
        av.set_volume_indexed(&Px.info.capture, ui->inputControlCombo->currentIndex(), vol);
//        setKeyerRecordVolume(vol);
        QSettings settings;
        settings.setValue("Volume/input", vol);
    }
}

void KeyerMain::on_outputLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        qreal vol = 1.0*position/ui->outputLevelSlider->maximum();
        av.set_volume_indexed(&Px.info.playback, ui->outputControlCombo->currentIndex(), vol);
//        setKeyerPlaybackVolume(vol);
        QSettings settings;
        settings.setValue("Volume/output", vol);
    }
}

void KeyerMain::on_passthruLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        qreal vol = 1.0*position/ui->passthruLevelSlider->maximum();
        av.set_volume_indexed(&Px.info.playback, ui->passthruControlCombo->currentIndex(), vol);
//        setKeyerPassthruVolume(vol);
        QSettings settings;
        settings.setValue("Volume/passthru", vol);
    }
}
void KeyerMain::on_masterLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        qreal vol = 1.0*position/ui->masterLevelSlider->maximum();
        av.set_volume_indexed(&Px.info.playback, ui->masterControlCombo->currentIndex(), vol);
//        setKeyerPlaybackVolume(vol);
        QSettings settings;
        settings.setValue("Volume/master", vol);
    }
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


void KeyerMain::on_inputMute_toggled(bool /*checked*/)
{
    bool inmute = ui->inputMute->isChecked();
    av.set_switch_indexed(&Px.info.playback, ui->inputControlCombo->currentIndex(), inmute);
}

void KeyerMain::on_masterMute_toggled(bool /*checked*/)
{
    bool mastermute = ui->masterMute->isChecked();
    av.set_switch_indexed(&Px.info.playback, ui->masterControlCombo->currentIndex(), mastermute);
}

void KeyerMain::on_passthruMute_toggled(bool /*checked*/)
{
    bool passmute = ui->passthruMute->isChecked();
    av.set_switch_indexed(&Px.info.playback, ui->passthruControlCombo->currentIndex(), passmute);

}


void KeyerMain::on_outputMute_toggled(bool /*checked*/)
{
    bool outputmute = ui->outputMute->isChecked();
    av.set_switch_indexed(&Px.info.playback, ui->outputControlCombo->currentIndex(), outputmute);
}
