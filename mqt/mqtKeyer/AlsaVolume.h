#ifndef ALSAVOLUME_H
#define ALSAVOLUME_H

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#endif
#ifndef Q_OS_LINUX
class snd_mixer_elem_t{};
class snd_mixer_t{};
#endif

typedef double PxVolume; /* 0.0 (min) --> 1.0 (max) */

class PxSelem
{
public:
    PxSelem(): commvol(false), commswitch(false),
        cvol(false), pvol(false),
        cswitch(false), pswitch(false),
        elem(0), vol(0), index(0), item(0)
    {

    }
    bool commvol;
    bool cvol;
    bool pvol;
    bool commswitch;
    bool cswitch;
    bool pswitch;

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

   QVector<PxSelem> selems;
};
class PxInfo
{
public:
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

    int open_mixer(PxDev *dev, int card, int playback);
    int cleanup(px_mixer *Px);
    int generic_lookup(PxDev *dev, const char *generic);
    void close_mixer(px_mixer *Px);
    int get_num_mixers(px_mixer * /*Px*/);
    const char *get_mixer_name(px_mixer */*Px*/, int /*i*/);
    int supports_pcm_output_volume(px_mixer *Px);
    PxVolume get_pcm_output_volume(px_mixer *Px);
    void set_pcm_output_volume(px_mixer *Px, PxVolume volume);
    int get_num_output_volumes(px_mixer *Px);
    const char *get_output_volume_name(px_mixer *Px, int i);

    PxVolume get_volume(PxDev *dev, const char *name);
    void set_volume(PxDev *dev, const char *name, PxVolume volume);

    PxVolume get_output_volume(px_mixer *Px, int i);
    void set_output_volume(px_mixer *Px, int i, PxVolume volume);
    PxVolume get_master_volume(px_mixer *Px);
    void set_master_volume(px_mixer *Px, PxVolume volume);
    PxVolume get_input_volume(px_mixer *Px);
    void set_input_volume(px_mixer *Px, PxVolume volume);

    int get_num_input_sources(px_mixer *Px);
    const char *get_input_source_name(px_mixer *Px, int i);
    int get_current_input_source(px_mixer *Px);
    void set_current_input_source(px_mixer *Px, int i);
#endif

public:
    AlsaVolume();
    ~AlsaVolume();
    QVector<Card> init();
    void switchCard(int index)
    {
        currCard = index;
    }

    bool control_values_changed;
    bool controls_changed;

    void timer(PxDev &dev);
    int OpenMixer_Linux_ALSA(px_mixer *Px);
    bool GetInputCard(int *card);
    bool GetOutputCard( int *card);
    int elem_callback(snd_mixer_elem_t *elem, unsigned int mask);
    int mixer_callback(snd_mixer_t *mixer, unsigned int mask, snd_mixer_elem_t *elem);

    bool has_volume_indexed(PxDev *dev, int i);
    PxVolume get_volume_indexed(PxDev *dev, int i);
    void set_volume_indexed(PxDev *dev, int i, PxVolume volume);

    bool has_switch_indexed(PxDev *dev, int i);
    bool get_switch_indexed(PxDev *dev, int i);
    void set_switch_indexed(PxDev *dev, int i, bool set);
};
int OpenMixer_Linux_ALSA(px_mixer *Px);

#endif // ALSAVOLUME_H
