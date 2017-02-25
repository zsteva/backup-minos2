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
#ifdef Q_OS_LINUX
    snd_pcm_stream_t stream;
#endif
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
    QVector<Card> cards;
    QVector<Card> getCardList();
#ifdef Q_OS_LINUX
    QVector<Device> getDeviceList(Card &card, snd_pcm_stream_t stream);
#endif
public:
    AlsaVolume();
    QVector<Card> init();
    void SetPlaybackMasterVolume(long volume);
    void SetRecordMasterVolume(long volume);
};

#endif // ALSAVOLUME_H
