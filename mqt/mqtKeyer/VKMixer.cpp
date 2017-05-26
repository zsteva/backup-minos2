#include "base_pch.h"

#include "VKMixer.h"

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

/*
  Need INI file settings for mixer and controls
  Need the mixer name and the control name

  Plus, we should have combos for all these

  Speaker out volume
  Speaker mute
  Capture in volume
  Capture mute
  Bypass (mic monitor)(in?) volume
  Bypass (mic monitor) mute

  PCM speaker channel
  PCM capture channel


  Then we need to remove the passthrough code - mic needs to
  go via bypass, as before, with pip tail ending it.

  */
//===================================================================
VKMixer * VKMixer::currentMixer = 0;

/*static*/ VKMixer * VKMixer::OpenMixer(const QString &mixerCard)
{
    VKMixer *mixer = GetVKMixer();
    mixer->switchCard(mixerCard);
    return mixer;
}
/*static*/ VKMixer *VKMixer::GetVKMixer()
{
    if (!currentMixer)
    {
        currentMixer = new VKMixer();
    }
    return currentMixer;
}

VKMixer::VKMixer():
    CurrMixerSet(emsUnloaded)

{
}
VKMixer::~VKMixer()
{
}
#ifdef Q_OS_LINUX
AlsaVolume VKMixer::av;
/*static*/ QVector<Card> VKMixer::getCardList()
{
    return AlsaVolume::getCardList();
}
#else
    #ifdef Q_OS_WIN
    /*static*/ QVector<Card> VKMixer::getCardList()
    {
            return QVector<Card>();
    }
    #endif
#endif


bool VKMixer::switchCard(const QString &mixerCard)
{
    if (currentCard == mixerCard)
        return true;

    // No we need to open the correct interface to the card

    return false;
}

void VKMixer::SetMasterMute( bool )
{

}

void VKMixer::SetMicOutMute( bool )
{

}


eMixerSets VKMixer::GetCurrentMixerSet()
{
    return CurrMixerSet;
}

void VKMixer::SetCurrentMixerSet( eMixerSets cms )
{
    CurrMixerSet = cms;
    // and now we need to apply the settings...
    // and we need to mute or drive to zero if no mute.
    // BUT we need to get the correct level to reset it to...
    // and hope it doesn't change while we are busy?
/*
    av.set_switch_indexed(&Px.info.playback, ui->inputControlCombo->currentIndex(), MixerSets[ CurrMixerSet ].PassThruMute);

    av.set_switch_indexed(&Px.info.playback, ui->masterControlCombo->currentIndex(), MixerSets[ CurrMixerSet ].MasterMute);

    adjustDeviceControls(&Px.info.capture, ui->inputControlCombo, ui->inputLevelSlider, ui->inputMute);
    adjustDeviceControls(&Px.info.playback, ui->outputControlCombo, ui->outputLevelSlider, ui->outputMute);
    adjustDeviceControls(&Px.info.playback, ui->passthruControlCombo, ui->passthruLevelSlider, ui->passthruMute);
    adjustDeviceControls(&Px.info.playback, ui->masterControlCombo, ui->masterLevelSlider, ui->masterMute);
*/
}

void VKMixer::timer()
{

}
void VKMixer::setInputLevel(qreal vol)
{

}

void VKMixer::setInputMute(bool mute)
{

}

void VKMixer::setOutputLevel(qreal vol)
{

}

void VKMixer::setOutputMute(bool mute)
{

}

void VKMixer::setMasterLevel(qreal vol)
{

}

void VKMixer::setMasterMute(bool mute)
{

}

void VKMixer::setPassThruLevel(qreal vol)
{

}

void VKMixer::setPassThruMute(bool mute)
{

}
