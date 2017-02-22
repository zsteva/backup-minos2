#include "base_pch.h"
#include "VKMixer.h"

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
class MixerSet
{
   public:
      MixerSet( bool MicOutMute, bool MasterMute )
            : MicOutMute( MicOutMute ), MasterMute( MasterMute )
      {}
      // we need a line setting, stereo volume + mute
      bool MicOutMute;
      bool MasterMute;
};

eMixerSets CurrMixerSet = emsUnloaded;
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
    return CurrMixerSet;
}

void SetCurrentMixerSet( eMixerSets cms )
{
   CurrMixerSet = cms;
   // and now we need to apply the settings...
   VKMixer *m = VKMixer::GetInputVKMixer();
   if ( m )
   {
      // and we need to mute or drive to zero if no mute.
      // BUT we need to get the correct level to reset it to...
      // and hope it doesn't change while we are busy?
      m->SetMicOutMute( MixerSets[ CurrMixerSet ].MicOutMute );
   }
   m = VKMixer::GetOutputVKMixer();
   if ( m )
   {
      m->SetMasterMute( MixerSets[ CurrMixerSet ].MasterMute );
   }
}
//===================================================================
VKMixer * VKMixer::inputMixer = 0;
VKMixer * VKMixer::outputMixer = 0;
VKMixer::VKMixer()
{
}
VKMixer::~VKMixer()
{
}
/*static*/ VKMixer *VKMixer::GetInputVKMixer()
{
   return inputMixer;
}

/*static*/ VKMixer *VKMixer::GetOutputVKMixer()
{
   return outputMixer;
}
void VKMixer::SetMasterMute( bool )
{

}

void VKMixer::SetMicOutMute( bool )
{

}
