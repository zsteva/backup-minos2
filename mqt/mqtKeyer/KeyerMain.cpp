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
    recordWait(false),
    recording(false)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
//    connect(m_engine, SIGNAL(levelChanged(qreal, qreal, int)),
//            m_levelMeter, SLOT(levelChanged(qreal, qreal, int)));

    enableTrace( "./TraceLog", "MinosKeyer_" );

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

    loadKeyers();

    connect(&LineTimer, SIGNAL(timeout()), this, SLOT(LineTimerTimer()));
    LineTimer.start(100);
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

}
void KeyerMain::CaptionTimerTimer( )
{
   CaptionTimer.stop();
   KeyerServer::publishState( windowTitle() );
}
void KeyerMain::on_recordButton_clicked()
{
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
    sendTone1();
}

void KeyerMain::on_twoToneButton_clicked()
{
    sendTone2();
}

void KeyerMain::on_aboutButton_clicked()
{

}

