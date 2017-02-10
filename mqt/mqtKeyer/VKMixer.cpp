#include "base_pch.h"
#include "VKMixer.h"

eMixerSets CurrMixerSet = emsUnloaded;
eMixerSets GetCurrentMixerSet()
{
    return CurrMixerSet;
}

void SetCurrentMixerSet( eMixerSets cms )
{
    CurrMixerSet = cms;
}
