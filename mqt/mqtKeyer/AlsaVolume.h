#ifndef ALSAVOLUME_H
#define ALSAVOLUME_H


class AlsaVolume
{
public:
    AlsaVolume();
    void SetAlsaPlaybackMasterVolume(long volume);
    void SetAlsaRecordMasterVolume(long volume);
};

#endif // ALSAVOLUME_H
