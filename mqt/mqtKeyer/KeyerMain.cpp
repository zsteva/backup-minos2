#include "KeyerMain.h"
#include "ui_KeyerMain.h"
#include "keyctrl.h"
#include "KeyerRPCServer.h"
#include "VKMixer.h"

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



KeyerMain::KeyerMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KeyerMain),
    mixer(0),
    PTT(false), keyline(false), PTTRef(false), L1Ref(false), L2Ref(false),
    recordWait(false),
    recording(false),
    inVolChange(false),
    inInit(true)

{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QSettings settings;
    QByteArray geometry = settings.value("KeyerMain/geometry").toByteArray();
    if (geometry.size() > 0)
        restoreGeometry(geometry);

    enableTrace( "./TraceLog", "MinosKeyer_" );

    createCloseEvent();

    mixer = VKMixer::GetVKMixer();

    QVector<Card> cards = VKMixer::getCardList();

    foreach (Card card, cards)
    {
        ui->cardCombo->addItem(card.name);
    }
    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    applyMixerSetting(keyerSettings, "Card", ui->cardCombo);

    VKMixer::OpenMixer(ui->cardCombo->currentText());

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
    mixer->switchCard(ui->cardCombo->currentText());

    if (inInit)
        return;

    QSettings keyerSettings( GetCurrentDir() + "/Configuration/MixerSettings.ini" , QSettings::IniFormat ) ;
    saveMixerSetting(keyerSettings, "Card", ui->cardCombo);
}

const char *msets[] = {"emsUnloaded", "emsPassThroughNoPTT", "emsPassThroughPTT",
                 "emsReplay", "emsReplayPip", "emsReplayT1", "emsReplayT2",
                 "emsVoiceRecord",
                 "emsCWTransmit", "emsCWPassThrough",
                 "emsMicMonitor", "emsReplayMonitor",
                 "emsMaxMixerSet"
                };
/*
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
*/
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

   mixer->timer();

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
        mixer->setInputLevel(vol);
        QSettings settings;
        settings.setValue("Volume/input", vol);
    }
}

void KeyerMain::on_outputLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        qreal vol = 1.0*position/ui->outputLevelSlider->maximum();
        mixer->setOutputLevel(vol);
        QSettings settings;
        settings.setValue("Volume/output", vol);
    }
}

void KeyerMain::on_passthruLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        qreal vol = 1.0*position/ui->passthruLevelSlider->maximum();
        mixer->setPassThruLevel(vol);
        QSettings settings;
        settings.setValue("Volume/passthru", vol);
    }
}
void KeyerMain::on_masterLevelSlider_sliderMoved(int position)
{
    if (!inVolChange)
    {
        qreal vol = 1.0*position/ui->masterLevelSlider->maximum();
        mixer->setMasterLevel(vol);
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
    mixer->setInputMute(inmute);
}

void KeyerMain::on_masterMute_toggled(bool /*checked*/)
{
    bool mastermute = ui->masterMute->isChecked();
    mixer->setMasterMute(mastermute);
}

void KeyerMain::on_passthruMute_toggled(bool /*checked*/)
{
    bool passmute = ui->passthruMute->isChecked();
    mixer->setPassThruMute(passmute);

}


void KeyerMain::on_outputMute_toggled(bool /*checked*/)
{
    bool outputmute = ui->outputMute->isChecked();
    mixer->setOutputMute(outputmute);
}
