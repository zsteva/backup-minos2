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

#ifdef Q_OS_LINUX
#include "AlsaVolume.h"
#endif
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
    px_mixer Px;        // internally, very Alsa oriented
#endif
      eMixerSets CurrMixerSet;

      QString currentCard;


      VKMixer();

      virtual ~VKMixer();

      static VKMixer *GetVKMixer();

      void SetMasterMute( bool );
      void SetMicOutMute( bool );

      eMixerSets GetCurrentMixerSet();
      void SetCurrentMixerSet( eMixerSets cms );

      static VKMixer *OpenMixer(const QString &currentCard);

      bool switchCard(const QString &currentCard);

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


#endif
