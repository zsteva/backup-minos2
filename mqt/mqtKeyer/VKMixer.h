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

#include "AlsaVolume.h"

enum eMixerSets {emsUnloaded, emsPassThroughNoPTT, emsPassThroughPTT,
                 emsReplay, emsReplayPip, emsReplayT1, emsReplayT2,
                 emsVoiceRecord,
                 emsCWTransmit, emsCWPassThrough,
                 emsMaxMixerSet};


class VKMixer
{
    static VKMixer *currentMixer;
   public:
    static QVector<Card> getCardList();
#ifdef Q_OS_LINUX
    static AlsaVolume av;
#endif
        eMixerSets CurrMixerSet;


        px_mixer Px;

        int currCardIndex;
        int currInputIndex;
        int currOutputIndex;

      VKMixer();

      virtual ~VKMixer();

      static VKMixer *GetVKMixer();

      void SetMasterMute( bool );
      void SetMicOutMute( bool );

      eMixerSets GetCurrentMixerSet();
      void SetCurrentMixerSet( eMixerSets cms );

      static bool OpenMixer(const QString &currentCard);

      static bool switchCard(const QString &currentCard);

      void timer();

      void setInputLevel(qreal vol);
      void setInputMute(bool mute);
      void setOutputLevel(qreal vol);
      void setOutputMute(bool mute);
      void setMasterLevel(qreal vol);
      void setMasterMute(bool mute);
      void setPassThruLevel(qreal vol);
      void setPassThruMute(bool mute);

};

class AudioInjectorMixer: public VKMixer
{
private:
    AlsaVolume av;

public:
    AudioInjectorMixer();
    virtual ~AudioInjectorMixer();
};

#endif
