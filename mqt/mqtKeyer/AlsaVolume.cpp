#include "base_pch.h"

#include "AlsaVolume.h"

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#endif

AlsaVolume::AlsaVolume()
{

}
void AlsaVolume::init()
{
#ifdef Q_OS_LINUX

    // use amixer -c 1 controls to get the control names
    // e.g. for one of the cmedia dongles
    /*
mjg@mjg-Aspire-5920:~$ amixer -c 1 controls
numid=3,iface=MIXER,name='Mic Playback Switch'
numid=4,iface=MIXER,name='Mic Playback Volume'
numid=7,iface=MIXER,name='Mic Capture Switch'
numid=8,iface=MIXER,name='Mic Capture Volume'
numid=9,iface=MIXER,name='Auto Gain Control'
numid=5,iface=MIXER,name='Speaker Playback Switch'
numid=6,iface=MIXER,name='Speaker Playback Volume'
numid=2,iface=PCM,name='Capture Channel Map'
numid=1,iface=PCM,name='Playback Channel Map'
    */
    // So here is the minimum code needed to duplicate the following command line:

//    > amixer -c 1 cset name='Speaker Playback Volume' 66,77
//    The control has the following features (after the above call):

//    > amixer -c 1 cget name='Speaker Playback Volume'
//    numid=6,iface=MIXER,name='Speaker Playback Volume'
//      ; type=INTEGER,access=rw---R--,values=2,min=0,max=151,step=0
//      : values=66,77
//      | dBminmax-min=-28.37dB,max=-0.06dB

//    The coloured parameters are the same colour in the code.

//    We will use the high-level control API. First we need to open an hctl and load its data:


    int err;

    snd_hctl_t *hctl;
    err = snd_hctl_open(&hctl, "hw:1", 0);
    err = snd_hctl_load(hctl);

//    Now we need to initialise a snd_ctl_elem_id_t structure which alsa can use to find the control we want.


    snd_ctl_elem_id_t *id;
    snd_ctl_elem_id_alloca(&id);
    snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);

    //We must specify the interface and either the numeric id

    //snd_ctl_elem_id_set_numid(id, 0);

    //or the name of the control.

    snd_ctl_elem_id_set_name(id, "Speaker Playback Volume");

    //With this initialised we can use it to get a snd_hctl_elem_t object and a snd_ctl_elem_value_t object. The elem is retrieved as follows:


    snd_hctl_elem_t *elem = snd_hctl_find_elem(hctl, id);

    if (elem)
    {

        //The elem_value is initialised with the id of the element and the value we want to set like this:

        snd_ctl_elem_value_t *control;
        snd_ctl_elem_value_alloca(&control);
        snd_ctl_elem_value_set_id(control, id);

        // try reading the value(s)
//        int snd_hctl_elem_read(snd_hctl_elem_t *elem, snd_ctl_elem_value_t * value);
        snd_ctl_elem_value_t *val;
        snd_ctl_elem_value_alloca(&val);
        int vals[2];
        if ((err = snd_hctl_elem_read(elem,val)) == 0)
        {
            for (int idx=0; idx<2; idx++)
            {
                vals[idx]=snd_ctl_elem_value_get_integer(val, idx);
            }
        }
        //Finally we are ready to write our values to the mixer control:

        snd_ctl_elem_value_set_integer(control, 0, 66);
        err = snd_hctl_elem_write(elem, control);

        snd_ctl_elem_value_set_integer(control, 1, 77);
        err = snd_hctl_elem_write(elem, control);

        //And if we don't want to do anything else we can close the high level control handle:
    }

    snd_hctl_close(hctl);


    //We don't have to cleanup any memory, cause all elements are allocated on the stack and freed automatically when our function returns to its caller.
#endif
}

void AlsaVolume::SetAlsaPlaybackMasterVolume(long volume)
{
#ifdef Q_OS_LINUX
    long min = 0;
    long max = 0;
    snd_mixer_t *handle = 0;
    snd_mixer_selem_id_t *sid = 0;
    const char *card = "default";
    const char *selem_name = "Master";

    int res;

    res = snd_mixer_open(&handle, 0);     // return 0 or -ve for error; param is mode
    if (res < 0)
        trace(QString("snd_mixer_open ") + snd_strerror(res));
    res = snd_mixer_attach(handle, card);// atach ctl to mixer
    if (res < 0)
        trace(QString("snd_mixer_attach ") + snd_strerror(res));
    res = snd_mixer_selem_register(handle, NULL, NULL);
    if (res < 0)
        trace(QString("snd_mixer_selem_register ") + snd_strerror(res));
    res = snd_mixer_load(handle);
    if (res < 0)
        trace(QString("snd_mixer_load ") + snd_strerror(res));

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);
    if (elem == 0)
        trace(QString("snd_mixer_find_selem failed"));

    res = snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    if (res < 0)
        trace(QString("snd_mixer_selem_get_playback_volume_range ") + snd_strerror(res));
    res = snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);
    if (res < 0)
        trace(QString("snd_mixer_selem_set_playback_volume_all ") + snd_strerror(res));

    res = snd_mixer_close(handle);
    if (res < 0)
        trace(QString("snd_mixer_close") + snd_strerror(res));
#else
    Q_UNUSED(volume)
#endif
}
void AlsaVolume::SetAlsaRecordMasterVolume(long volume)
{
#ifdef Q_OS_LINUX
    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "Master";

    int res;
    res = snd_mixer_open(&handle, 0);
    if (res < 0)
        trace(QString("snd_mixer_open ") + snd_strerror(res));
    res = snd_mixer_attach(handle, card);
    if (res < 0)
        trace(QString("snd_mixer_attach ") + snd_strerror(res));
    res = snd_mixer_selem_register(handle, NULL, NULL);
    if (res < 0)
        trace(QString("snd_mixer_selem_register ") + snd_strerror(res));
    res = snd_mixer_load(handle);
    if (res < 0)
        trace(QString("snd_mixer_load ") + snd_strerror(res));

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);
    if (elem == 0)
        trace(QString("snd_mixer_find_selem failed"));

    res = snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
    if (res < 0)
        trace(QString("snd_mixer_selem_get_capture_volume_range ") + snd_strerror(res));
    res = snd_mixer_selem_set_capture_volume_all(elem, volume * max / 100);
    if (res < 0)
        trace(QString("snd_mixer_selem_set_capture_volume_all ") + snd_strerror(res));

    res = snd_mixer_close(handle);
    if (res < 0)
        trace(QString("snd_mixer_close ") + snd_strerror(res));
#else
    Q_UNUSED(volume)
#endif
}
QVector<Card> AlsaVolume::getCardList()
{
    QVector<Card> cards;
#ifdef Q_OS_LINUX

    Card card;
    card.indexstr = "-";
    card.name = "(default)";
    card.device_name = "default";

    cards.push_back(card);

    snd_ctl_card_info_t *info;
    snd_ctl_card_info_alloca(&info);
    int number = -1;
    for (;;)
    {
        int err = snd_card_next(&number);
        if (err < 0)
        {
            trace("cannot enumerate sound cards" + QString::number(err));
            return cards;
        }
        if (number < 0)
            break;

        QString buf = "hw:" + QString::number(number);
        snd_ctl_t *ctl;
        err = snd_ctl_open(&ctl, buf.toLatin1(), 0);
        if (err < 0)
            continue;

        err = snd_ctl_card_info(ctl, info);
        snd_ctl_close(ctl);
        if (err < 0)
            continue;

        card.indexstr = QString::number(number);
        card.name = snd_ctl_card_info_get_name(info);
        card.device_name = buf;
        cards.push_back(card);
    }

#endif
    return cards;
}
