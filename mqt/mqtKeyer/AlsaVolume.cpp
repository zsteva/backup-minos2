#include "base_pch.h"

#include "AlsaVolume.h"

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#endif

AlsaVolume::AlsaVolume()
{

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
