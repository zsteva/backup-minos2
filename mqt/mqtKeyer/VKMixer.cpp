#include "base_pch.h"
#ifdef RUBBISH
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
#endif
