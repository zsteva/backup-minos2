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
/*
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
*/
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
VKMixer * VKMixer::currentMixer = nullptr;

/*static*/ VKMixer * VKMixer::OpenMixer()
{
    VKMixer *mixer = GetVKMixer();
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

eMixerSets VKMixer::GetCurrentMixerSet()
{
    return CurrMixerSet;
}

void VKMixer::SetCurrentMixerSet( eMixerSets cms )
{
    CurrMixerSet = cms;

}
