#ifndef ALSAVOLUME_H
#define ALSAVOLUME_H

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#endif

class SubDevice
{
public:
    int index;
    QString subdevName;
};

class Device
{
public:
    snd_pcm_stream_t stream;
    QString index;
    QString devId;
    QString devName;
    QVector<SubDevice> subdevs;
};
class Card
{
public:
    QString indexstr;
    QString name;
    QString device_name;
    QVector<Device> capture;
    QVector<Device> playback;
};

class AlsaVolume
{
private:
    QVector<Card> getCardList();
    QVector<Device> getDeviceList(Card &card, snd_pcm_stream_t stream);
public:
    AlsaVolume();
    void init();
    void SetPlaybackMasterVolume(long volume);
    void SetRecordMasterVolume(long volume);

    QVector<Card> cards;
};

#endif // ALSAVOLUME_H
