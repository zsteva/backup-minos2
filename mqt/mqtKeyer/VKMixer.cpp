#include "base_pch.h"
#include "vkmixer.h"

eMixerSets CurrMixerSet = emsUnloaded;
eMixerSets GetCurrentMixerSet()
{
    return CurrMixerSet;
}

void SetCurrentMixerSet( eMixerSets cms )
{
    CurrMixerSet = cms;
}
