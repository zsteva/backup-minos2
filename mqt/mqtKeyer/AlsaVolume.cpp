#include "base_pch.h"

#include "AlsaVolume.h"

/**
 * Reads audio devices from ALSA interface and returns count and array of
 * strings containing the devices.
 *
 * @param[out] count Number of devices found.
 * @param[out] Array of strings containing the device names.
 *
 */
AlsaVolume::AlsaVolume()
{

}
// What may be the top level - one day!
extern void create_controls();
void AlsaVolume::init()
{
//create_controls();

    cards = getCardList();
    foreach(Card card, cards)
    {
        card.capture = getDeviceList(card, SND_PCM_STREAM_CAPTURE);
        card.playback = getDeviceList(card, SND_PCM_STREAM_PLAYBACK);
    }

}

void AlsaVolume::SetPlaybackMasterVolume(long volume)
{
    Q_UNUSED(volume)
#ifdef Q_OS_LINUX
#else
#endif
}
void AlsaVolume::SetRecordMasterVolume(long volume)
{
    Q_UNUSED(volume)
#ifdef Q_OS_LINUX
#else
#endif
}

// code derived from aplay
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
//static snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK or 	SND_PCM_STREAM_CAPTURE,

QVector<Device> AlsaVolume::getDeviceList(Card &card, snd_pcm_stream_t stream)
{
    QVector<Device> devices;

    snd_ctl_t *handle;
    int err;
    QString name = "hw:" + card.indexstr;
    if ((err = snd_ctl_open(&handle, name.toLatin1(), 0)) < 0)
    {
        trace("control open " + card.device_name + " : " + snd_strerror(err));
        return devices;
    }
    snd_ctl_card_info_t *info = 0;
    snd_ctl_card_info_alloca(&info);
    snd_pcm_info_t *pcminfo = 0;
    snd_pcm_info_alloca(&pcminfo);

    if ((err = snd_ctl_card_info(handle, info)) < 0)
    {
        trace("control hardware info " + card.device_name + " : " + snd_strerror(err));
        snd_ctl_close(handle);
        return devices;
    }
    int dev = -1;
    while (1) {
        err = snd_ctl_pcm_next_device(handle, &dev);
        if (err < 0)
            trace(QString("snd_ctl_pcm_next_device ") + snd_strerror(err));
        if (dev < 0)
            break;
        snd_pcm_info_set_device(pcminfo, dev);
        snd_pcm_info_set_subdevice(pcminfo, 0);
        snd_pcm_info_set_stream(pcminfo, stream);
        if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
            if (err != -ENOENT)
                trace("control digital audio info " + card.device_name + " : " + snd_strerror(err));
            continue;
        }
        /*
mjg@mjg-vub:~/Desktop$ aplay -l
**** List of PLAYBACK Hardware Devices ****
card 0: I82801AAICH [Intel 82801AA-ICH], device 0: Intel ICH [Intel 82801AA-ICH]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
         */

        Device device;
        device.index = dev;
        device.devId = snd_pcm_info_get_id(pcminfo);
        device.devName = snd_pcm_info_get_name(pcminfo);
        device.stream = stream;

        unsigned int count = snd_pcm_info_get_subdevices_count(pcminfo);

        for (unsigned int idx = 0; idx < count; idx++)
        {
            snd_pcm_info_set_subdevice(pcminfo, idx);
            if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0)
            {
                trace("control digital audio playback info " + card.device_name + snd_strerror(err));
            }
            else
            {

                SubDevice subdev;
                subdev.index = idx;
                subdev.subdevName = snd_pcm_info_get_subdevice_name(pcminfo);
                device.subdevs.push_back(subdev);
            }
        }
        devices.push_back(device);
    }
    snd_ctl_close(handle);

    return devices;
}
//==========================================================================================================
// sample code from alsamixer
#define ARRAY_SIZE(a) (sizeof(a) / sizeof *(a))

class Control
{
public:
    snd_mixer_elem_t *elem;
    QString name;
    unsigned int flags;
#define TYPE_PVOLUME	(1u << 4)
#define TYPE_CVOLUME	(1u << 5)
#define TYPE_PSWITCH	(1u << 6)
#define TYPE_CSWITCH	(1u << 7)
#define TYPE_ENUM	(1u << 8)
#define HAS_VOLUME_0	(1u << 9)
#define HAS_VOLUME_1	(1u << 10)
#define HAS_PSWITCH_0	(1u << 11)
#define HAS_PSWITCH_1	(1u << 12)
#define HAS_CSWITCH_0	(1u << 13)
#define HAS_CSWITCH_1	(1u << 14)
#define IS_MULTICH	(1u << 15)
#define IS_ACTIVE	(1u << 16)
#define MULTICH_MASK	(0x0000f)
    snd_mixer_selem_channel_id_t volume_channels[2];
    snd_mixer_selem_channel_id_t pswitch_channels[2];
    snd_mixer_selem_channel_id_t cswitch_channels[2];
    unsigned int enum_channel_bits;
};


void create_controls(void);
void free_controls(void);

Control *controls;
unsigned int controls_count;

static const snd_mixer_selem_channel_id_t supported_channels[] = {
    SND_MIXER_SCHN_FRONT_LEFT,
    SND_MIXER_SCHN_FRONT_RIGHT,
    SND_MIXER_SCHN_REAR_LEFT,
    SND_MIXER_SCHN_REAR_RIGHT,
    SND_MIXER_SCHN_FRONT_CENTER,
    SND_MIXER_SCHN_WOOFER,
    SND_MIXER_SCHN_SIDE_LEFT,
    SND_MIXER_SCHN_SIDE_RIGHT,
};
#define LAST_SUPPORTED_CHANNEL SND_MIXER_SCHN_SIDE_RIGHT

static const snd_mixer_selem_channel_id_t control_channels[][2] = {
    { SND_MIXER_SCHN_FRONT_LEFT, SND_MIXER_SCHN_FRONT_RIGHT },
    { SND_MIXER_SCHN_REAR_LEFT, SND_MIXER_SCHN_REAR_RIGHT },
    { SND_MIXER_SCHN_FRONT_CENTER, SND_MIXER_SCHN_UNKNOWN },
    { SND_MIXER_SCHN_WOOFER, SND_MIXER_SCHN_UNKNOWN },
    { SND_MIXER_SCHN_SIDE_LEFT, SND_MIXER_SCHN_SIDE_RIGHT },
};

snd_mixer_t *mixer;
QString mixer_device_name;
snd_mixer_selem_regopt selem_regopt;
snd_mixer_selem_id_t *current_selem_id;

#define TRUE 1
#define FALSE 0


static bool has_more_than_front_capture_channels(snd_mixer_elem_t *elem)
{
    unsigned int i;

    for (i = 2; i < ARRAY_SIZE(supported_channels); ++i)
        if (snd_mixer_selem_has_capture_channel(elem, supported_channels[i]))
            return TRUE;
    return FALSE;
}

static bool has_any_control_channel(snd_mixer_elem_t *elem,
                    const snd_mixer_selem_channel_id_t channels[2],
                    int (*has_channel)(snd_mixer_elem_t *, snd_mixer_selem_channel_id_t))
{
    return has_channel(elem, channels[0]) ||
           (channels[1] != SND_MIXER_SCHN_UNKNOWN && has_channel(elem, channels[1]));
}

static bool has_merged_cswitch(snd_mixer_elem_t *elem)
{
    bool pvol, psw;
    unsigned int i;

    pvol = snd_mixer_selem_has_playback_volume(elem);
    psw = snd_mixer_selem_has_playback_switch(elem);
    if ((pvol || psw) &&
        snd_mixer_selem_has_capture_switch(elem) &&
        !snd_mixer_selem_has_capture_volume(elem)) {
        if (snd_mixer_selem_has_capture_switch_joined(elem))
            return TRUE;
        else if (((pvol && snd_mixer_selem_has_playback_volume_joined(elem)) ||
              (psw && snd_mixer_selem_has_playback_switch_joined(elem))) &&
             has_more_than_front_capture_channels(elem))
            return FALSE;
        for (i = 0; i < ARRAY_SIZE(control_channels); ++i) {
            if (has_any_control_channel(elem, control_channels[i], snd_mixer_selem_has_capture_channel) &&
                !has_any_control_channel(elem, control_channels[i], snd_mixer_selem_has_playback_channel))
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

static unsigned int get_playback_controls_count(snd_mixer_elem_t *elem)
{
    unsigned int count = 0;
    unsigned int i;
    int has_vol, has_sw;

    has_vol = snd_mixer_selem_has_playback_volume(elem);
    has_sw = snd_mixer_selem_has_playback_switch(elem);
    if (!has_vol && !has_sw)
        return 0;
    if ((!has_vol || snd_mixer_selem_has_playback_volume_joined(elem)) &&
        (!has_sw || snd_mixer_selem_has_playback_switch_joined(elem)))
        return 1;
    for (i = 0; i < ARRAY_SIZE(control_channels); ++i) {
        if (snd_mixer_selem_has_playback_channel(elem, control_channels[i][0]) ||
            (control_channels[i][1] != SND_MIXER_SCHN_UNKNOWN &&
             snd_mixer_selem_has_playback_channel(elem, control_channels[i][1])))
            ++count;
    }
    return count;
}

static unsigned int get_capture_controls_count(snd_mixer_elem_t *elem)
{
    unsigned int count = 0;
    unsigned int i;
    int has_vol, has_sw;

    has_vol = snd_mixer_selem_has_capture_volume(elem);
    has_sw = snd_mixer_selem_has_capture_switch(elem);
    if ((!has_vol && !has_sw) ||
        (/*view_mode == VIEW_MODE_ALL &&*/ has_merged_cswitch(elem)))
        return 0;
    if ((!has_vol || snd_mixer_selem_has_capture_volume_joined(elem)) &&
        (!has_sw || snd_mixer_selem_has_capture_switch_joined(elem)))
        return 1;
    for (i = 0; i < ARRAY_SIZE(control_channels); ++i) {
        if (snd_mixer_selem_has_capture_channel(elem, control_channels[i][0]) ||
            (control_channels[i][1] != SND_MIXER_SCHN_UNKNOWN &&
             snd_mixer_selem_has_capture_channel(elem, control_channels[i][1])))
            ++count;
    }
    return count;
}

static unsigned int get_controls_count_for_elem(snd_mixer_elem_t *elem)
{
    unsigned int p, c;

    if (snd_mixer_elem_get_type(elem) != SND_MIXER_ELEM_SIMPLE)
        return 0;
    if (snd_mixer_selem_is_enumerated(elem)) {
/*        switch (view_mode) {
        case VIEW_MODE_PLAYBACK:
            return snd_mixer_selem_is_enum_capture(elem) ? 0 : 1;
        case VIEW_MODE_CAPTURE:
            return snd_mixer_selem_is_enum_capture(elem) ? 1 : 0;
        case VIEW_MODE_ALL:
        default:
            return 1;
        }
        */
        return 1;
    }
    /*
    switch (view_mode) {
    case VIEW_MODE_PLAYBACK:
        return get_playback_controls_count(elem);
    case VIEW_MODE_CAPTURE:
        return get_capture_controls_count(elem);
    case VIEW_MODE_ALL:
    default:
    */
        p = get_playback_controls_count(elem);
        c = get_capture_controls_count(elem);
        return has_merged_cswitch(elem) ? p : p + c;
        /*
    }*/
}

static void create_name(Control *control)
{
    unsigned int index = snd_mixer_selem_get_index(control->elem);
    if (index > 0)
        control->name = QString(snd_mixer_selem_get_name(control->elem)) + " " + QString::number(index);
    else
        control->name = QString(snd_mixer_selem_get_name(control->elem));

    control->name.replace("IEC958", "S/PDIF");
}

static unsigned int create_controls_for_elem(snd_mixer_elem_t *elem, Control *control)
{
    unsigned int count = 0;
    unsigned int i;
    unsigned int enum_index;
    Control *front_control = NULL;
    bool has_pvol, has_psw;
    bool has_cvol, has_csw;
    bool has_channel[LAST_SUPPORTED_CHANNEL + 1];
    bool merged_cswitch;
    bool has_ch0, has_ch1;

    if (snd_mixer_elem_get_type(elem) != SND_MIXER_ELEM_SIMPLE)
        return 0;
    if (snd_mixer_selem_is_enumerated(elem)) {
        /*
        if ((view_mode == VIEW_MODE_PLAYBACK && snd_mixer_selem_is_enum_capture(elem)) ||
            (view_mode == VIEW_MODE_CAPTURE && !snd_mixer_selem_is_enum_capture(elem)))
            return 0;
            */
        control->elem = elem;
        control->flags = TYPE_ENUM;
        control->enum_channel_bits = 0;
        for (i = 0; i <= SND_MIXER_SCHN_LAST; ++i)
            if (snd_mixer_selem_get_enum_item(control->elem, static_cast<snd_mixer_selem_channel_id_t>(i), &enum_index) >= 0)
                control->enum_channel_bits |= 1 << i;
        if (snd_mixer_selem_is_active(control->elem))
            control->flags |= IS_ACTIVE;
        create_name(control);
        return 1;
    }
    has_pvol = snd_mixer_selem_has_playback_volume(elem);
    has_psw = snd_mixer_selem_has_playback_switch(elem);
    has_cvol = snd_mixer_selem_has_capture_volume(elem);
    has_csw = snd_mixer_selem_has_capture_switch(elem);
    merged_cswitch = /*view_mode == VIEW_MODE_ALL &&*/ has_merged_cswitch(elem);
    if (/*view_mode != VIEW_MODE_CAPTURE &&*/ (has_pvol || has_psw)) {
        if ((!has_pvol || snd_mixer_selem_has_playback_volume_joined(elem)) &&
            (!has_psw || snd_mixer_selem_has_playback_switch_joined(elem))) {
            control->elem = elem;
            if (has_pvol) {
                control->flags |= TYPE_PVOLUME | HAS_VOLUME_0;
                control->volume_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
            }
            if (has_psw) {
                control->flags |= TYPE_PSWITCH | HAS_PSWITCH_0;
                control->pswitch_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
            }
            if (merged_cswitch) {
                control->flags |= TYPE_CSWITCH;
                if (snd_mixer_selem_has_capture_switch_joined(elem)) {
                    control->flags |= HAS_CSWITCH_0;
                    control->cswitch_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
                } else {
                    if (snd_mixer_selem_has_capture_channel(elem, control_channels[0][0])) {
                        control->flags |= HAS_CSWITCH_0;
                        control->cswitch_channels[0] = control_channels[0][0];
                    }
                    if (control_channels[0][1] != SND_MIXER_SCHN_UNKNOWN &&
                        snd_mixer_selem_has_capture_channel(elem, control_channels[0][1])) {
                        control->flags |= HAS_CSWITCH_1;
                        control->cswitch_channels[1] = control_channels[0][1];
                    }
                }
                if ((control->flags & (HAS_CSWITCH_0 | HAS_CSWITCH_1)) == HAS_CSWITCH_1) {
                    control->flags ^= HAS_CSWITCH_0 | HAS_CSWITCH_1;
                    control->cswitch_channels[0] = control->cswitch_channels[1];
                }
            }
            if (snd_mixer_selem_is_active(control->elem))
                control->flags |= IS_ACTIVE;
            create_name(control);
            ++control;
            ++count;
        } else {
            for (i = 0; i < ARRAY_SIZE(supported_channels); ++i)
                has_channel[supported_channels[i]] =
                    snd_mixer_selem_has_playback_channel(elem, supported_channels[i]);
            for (i = 0; i < ARRAY_SIZE(control_channels); ++i) {
                has_ch0 = has_channel[control_channels[i][0]];
                has_ch1 = control_channels[i][1] != SND_MIXER_SCHN_UNKNOWN &&
                    has_channel[control_channels[i][1]];
                if (!has_ch0 && !has_ch1)
                    continue;
                control->elem = elem;
                if (has_pvol) {
                    control->flags |= TYPE_PVOLUME;
                    if (snd_mixer_selem_has_playback_volume_joined(elem)) {
                        control->flags |= HAS_VOLUME_0;
                        control->volume_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
                    } else {
                        if (has_ch0) {
                            control->flags |= HAS_VOLUME_0;
                            control->volume_channels[0] = control_channels[i][0];
                        }
                        if (has_ch1) {
                            control->flags |= HAS_VOLUME_1;
                            control->volume_channels[1] = control_channels[i][1];
                        }
                    }
                }
                if (has_psw) {
                    control->flags |= TYPE_PSWITCH;
                    if (snd_mixer_selem_has_playback_switch_joined(elem)) {
                        control->flags |= HAS_PSWITCH_0;
                        control->pswitch_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
                    } else {
                        if (has_ch0) {
                            control->flags |= HAS_PSWITCH_0;
                            control->pswitch_channels[0] = control_channels[i][0];
                        }
                        if (has_ch1) {
                            control->flags |= HAS_PSWITCH_1;
                            control->pswitch_channels[1] = control_channels[i][1];
                        }
                    }
                }
                if (merged_cswitch) {
                    control->flags |= TYPE_CSWITCH;
                    if (snd_mixer_selem_has_capture_switch_joined(elem)) {
                        control->flags |= HAS_CSWITCH_0;
                        control->cswitch_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
                    } else {
                        if (snd_mixer_selem_has_capture_channel(elem, control_channels[i][0])) {
                            control->flags |= HAS_CSWITCH_0;
                            control->cswitch_channels[0] = control_channels[i][0];
                        }
                        if (control_channels[i][1] != SND_MIXER_SCHN_UNKNOWN &&
                            snd_mixer_selem_has_capture_channel(elem, control_channels[i][1])) {
                            control->flags |= HAS_CSWITCH_1;
                            control->cswitch_channels[1] = control_channels[i][1];
                        }
                    }
                }
                if ((control->flags & (HAS_VOLUME_0 | HAS_VOLUME_1)) == HAS_VOLUME_1) {
                    control->flags ^= HAS_VOLUME_0 | HAS_VOLUME_1;
                    control->volume_channels[0] = control->volume_channels[1];
                }
                if ((control->flags & (HAS_PSWITCH_0 | HAS_PSWITCH_1)) == HAS_PSWITCH_1) {
                    control->flags ^= HAS_PSWITCH_0 | HAS_PSWITCH_1;
                    control->pswitch_channels[0] = control->pswitch_channels[1];
                }
                if ((control->flags & (HAS_CSWITCH_0 | HAS_CSWITCH_1)) == HAS_CSWITCH_1) {
                    control->flags ^= HAS_CSWITCH_0 | HAS_CSWITCH_1;
                    control->cswitch_channels[0] = control->cswitch_channels[1];
                }
                if (snd_mixer_selem_is_active(control->elem))
                    control->flags |= IS_ACTIVE;
                create_name(control);
                if (i == 0)
                    front_control = control;
                else {
                    front_control->flags |= IS_MULTICH | 0;
                    control->flags |= IS_MULTICH | i;
                }
                ++control;
                ++count;
            }
        }
    }
    if (/*view_mode != VIEW_MODE_PLAYBACK &&*/ (has_cvol || has_csw) && !merged_cswitch) {
        if ((!has_cvol || snd_mixer_selem_has_capture_volume_joined(elem)) &&
            (!has_csw || snd_mixer_selem_has_capture_switch_joined(elem))) {
            control->elem = elem;
            if (has_cvol) {
                control->flags |= TYPE_CVOLUME | HAS_VOLUME_0;
                control->volume_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
            }
            if (has_csw) {
                control->flags |= TYPE_CSWITCH | HAS_CSWITCH_0;
                control->cswitch_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
            }
            if (snd_mixer_selem_is_active(control->elem))
                control->flags |= IS_ACTIVE;
            create_name(control);
            ++control;
            ++count;
        } else {
            for (i = 0; i < ARRAY_SIZE(supported_channels); ++i)
                has_channel[supported_channels[i]] =
                    snd_mixer_selem_has_capture_channel(elem, supported_channels[i]);
            for (i = 0; i < ARRAY_SIZE(control_channels); ++i) {
                has_ch0 = has_channel[control_channels[i][0]];
                has_ch1 = control_channels[i][1] != SND_MIXER_SCHN_UNKNOWN &&
                    has_channel[control_channels[i][1]];
                if (!has_ch0 && !has_ch1)
                    continue;
                control->elem = elem;
                if (has_cvol) {
                    control->flags |= TYPE_CVOLUME;
                    if (snd_mixer_selem_has_capture_volume_joined(elem)) {
                        control->flags |= HAS_VOLUME_0;
                        control->volume_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
                    } else {
                        if (has_ch0) {
                            control->flags |= HAS_VOLUME_0;
                            control->volume_channels[0] = control_channels[i][0];
                        }
                        if (has_ch1) {
                            control->flags |= HAS_VOLUME_1;
                            control->volume_channels[1] = control_channels[i][1];
                        }
                    }
                }
                if (has_csw) {
                    control->flags |= TYPE_CSWITCH;
                    if (snd_mixer_selem_has_capture_switch_joined(elem)) {
                        control->flags |= HAS_CSWITCH_0;
                        control->cswitch_channels[0] = static_cast<snd_mixer_selem_channel_id_t>(0);
                    } else {
                        if (has_ch0) {
                            control->flags |= HAS_CSWITCH_0;
                            control->cswitch_channels[0] = control_channels[i][0];
                        }
                        if (has_ch1) {
                            control->flags |= HAS_CSWITCH_1;
                            control->cswitch_channels[1] = control_channels[i][1];
                        }
                    }
                }
                if ((control->flags & (HAS_VOLUME_0 | HAS_VOLUME_1)) == HAS_VOLUME_1) {
                    control->flags ^= HAS_VOLUME_0 | HAS_VOLUME_1;
                    control->volume_channels[0] = control->volume_channels[1];
                }
                if ((control->flags & (HAS_CSWITCH_0 | HAS_CSWITCH_1)) == HAS_CSWITCH_1) {
                    control->flags ^= HAS_CSWITCH_0 | HAS_CSWITCH_1;
                    control->cswitch_channels[0] = control->cswitch_channels[1];
                }
                if (snd_mixer_selem_is_active(control->elem))
                    control->flags |= IS_ACTIVE;
                create_name(control);
                if (i == 0)
                    front_control = control;
                else {
                    front_control->flags |= IS_MULTICH | 0;
                    control->flags |= IS_MULTICH | i;
                }
                ++control;
                ++count;
            }
        }
    }
    return count;
}
/*
static void search_for_focus_control(void)
{
    snd_mixer_elem_t *elem;
    unsigned int i;

    elem = snd_mixer_find_selem(mixer, current_selem_id);
    if (elem)
        for (i = 0; i < controls_count; ++i)
            if (controls[i].elem == elem) {
                focus_control_index = i;
                for (;;) {
                    ++i;
                    if (i >= controls_count || controls[i].elem != elem)
                        return;
                    if (controls[i].flags == current_control_flags) {
                        focus_control_index = i;
                        return;
                    }
                }
            }
    focus_control_index = 0;
}
*/
void free_controls(void)
{
//    unsigned int i;
//    for (i = 0; i < controls_count; ++i)
//        free(controls[i].name);
    delete [] controls;
    controls = NULL;
    controls_count = 0;
}

void create_mixer_object(struct snd_mixer_selem_regopt *selem_regopt)
{

    int err;

    err = snd_mixer_open(&mixer, 0);
    if (err < 0)
    {
        trace(QString("cannot open mixer ") + snd_strerror(err));
        return;
    }

    mixer_device_name = QString(selem_regopt->device);
    err = snd_mixer_selem_register(mixer, selem_regopt, NULL);
    if (err < 0)
    {
        trace(QString("cannot open mixer ") + snd_strerror(err));
        return;
    }

    //snd_mixer_set_callback(mixer, mixer_callback);

    err = snd_mixer_load(mixer);
    if (err < 0)
    {
        trace(QString("cannot open mixer controls ") + snd_strerror(err));
        return;
    }

    err = snd_mixer_selem_id_malloc(&current_selem_id);
    if (err < 0)
    {
        trace(QString("out of memory ") + snd_strerror(err));
        return;
    }
}

void create_controls(void)
{
    snd_mixer_elem_t *elem;
    Control *control;

    free_controls();

    selem_regopt.ver = 1;
    selem_regopt.abstract = SND_MIXER_SABSTRACT_NONE;
    selem_regopt.device = "default";

    create_mixer_object(&selem_regopt);
    for (elem = snd_mixer_first_elem(mixer);
         elem;
         elem = snd_mixer_elem_next(elem))
        controls_count += get_controls_count_for_elem(elem);

    if (controls_count > 0) {
        controls = new Control [controls_count];
        control = controls;
        for (elem = snd_mixer_first_elem(mixer);
             elem;
             elem = snd_mixer_elem_next(elem))
            control += create_controls_for_elem(elem, control);
        assert(control == controls + controls_count);
    }
/*
    compute_controls_layout();
    display_view_mode();

    search_for_focus_control();
    refocus_control();
*/
    free_controls();
    if (mixer)
        snd_mixer_close(mixer);
    if (current_selem_id)
        snd_mixer_selem_id_free(current_selem_id);

}
