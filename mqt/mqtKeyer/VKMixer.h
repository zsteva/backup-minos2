/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#ifndef VKMixerH
#define VKMixerH

//======================================================================================
enum eMixerSets {emsUnloaded, emsPassThroughNoPTT, emsPassThroughPTT,
                 emsReplay, emsReplayPip, emsReplayT1, emsReplayT2,
                 emsVoiceRecord,
                 emsCWTransmit, emsCWPassThrough,
                 emsMaxMixerSet};
//======================================================================================

class VKMixer
{
private:
    static VKMixer *currentMixer;
public:

    eMixerSets CurrMixerSet;

      VKMixer();

      virtual ~VKMixer();

      static VKMixer *GetVKMixer();

      eMixerSets GetCurrentMixerSet();
      void SetCurrentMixerSet( eMixerSets cms );

      static VKMixer *OpenMixer();
};
//======================================================================================
#endif
