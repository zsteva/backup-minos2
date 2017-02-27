#ifndef ALSAVOLUME_H
#define ALSAVOLUME_H

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#endif

typedef double PxVolume; /* 0.0 (min) --> 1.0 (max) */

class PxSelem
{
public:
   snd_mixer_elem_t *elem;
   snd_mixer_elem_t *vol;
   unsigned int index;
   unsigned int item;
   QByteArray name;
};

class PxDev
{
public:
#ifdef Q_OS_LINUX
   snd_mixer_t *handle;
#endif
   int card;
   int playback;
   int source;

   int numselems;
   QVector<PxSelem> selems;
};
class PxInfo
{
public:
   int numMixers;
   //char *mixers;

   PxDev playback;
   PxDev capture;
};

class px_mixer
{
    public:
    px_mixer(){}
    PxInfo info;

};

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

    QString mixer_device_name;

    int currCard;
    QVector<Card> cards;
    QVector<Card> getCardList();
#ifdef Q_OS_LINUX
    QVector<Device> getDeviceList(Card &card, snd_pcm_stream_t stream);
#endif

public:
    AlsaVolume();
    ~AlsaVolume();
    QVector<Card> init();
    void switchCard(int index)
    {
        currCard = index;
    }
    bool GetInputCard(int *card);
    bool GetOutputCard( int *card);

private:
#ifdef Q_OS_LINUX

#endif
};
int OpenMixer_Linux_ALSA(px_mixer *Px);

extern AlsaVolume av;
#endif // ALSAVOLUME_H
