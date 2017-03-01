#include "base_pch.h"

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#endif

#include "AlsaVolume.h"

AlsaVolume av;


/**
 * Reads audio devices from ALSA interface and returns count and array of
 * strings containing the devices.
 *
 * @param[out] count Number of devices found.
 * @param[out] Array of strings containing the device names.
 *
 */
AlsaVolume::AlsaVolume():
    control_values_changed(false),
    controls_changed(false)
{
}
AlsaVolume::~AlsaVolume()
{

}
// What may be the top level - one day!
QVector<Card> AlsaVolume::init()
{
    cards = getCardList();
#ifdef Q_OS_LINUX
    for(int i = 0; i < cards.size(); i++)
    {
        Card &card = cards[i];
        card.capture = getDeviceList(card, SND_PCM_STREAM_CAPTURE);
        card.playback = getDeviceList(card, SND_PCM_STREAM_PLAYBACK);
    }
#endif
    return cards;
}
// code derived from aplay
QVector<Card> AlsaVolume::getCardList()
{
    QVector<Card> cards;
#ifdef Q_OS_LINUX

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

        Card card;
        card.indexstr = QString::number(number);
        card.name = snd_ctl_card_info_get_name(info);
        card.device_name = buf;
        cards.push_back(card);
    }

#endif
    return cards;
}
bool AlsaVolume::GetInputCard(int *card)
{
    *card = currCard;
    return true;
}

bool AlsaVolume::GetOutputCard( int *card)
{
    *card = currCard;
    return true;
}

//static snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK or 	SND_PCM_STREAM_CAPTURE,

#ifdef Q_OS_LINUX
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

        trace("");
        trace(QString("PCM ") + ((stream == SND_PCM_STREAM_CAPTURE)?"capture":"playback") + " device " + device.devName);
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
                trace(QString("PCM subdevice ") + subdev.subdevName);
            }
        }
        devices.push_back(device);
    }
    snd_ctl_close(handle);
    return devices;
}
#endif
//==========================================================================================================
#ifdef Q_OS_LINUX

static int elem_callback(snd_mixer_elem_t *elem, unsigned int mask)
{
    return av.elem_callback(elem, mask);
}

int AlsaVolume::elem_callback(snd_mixer_elem_t */*elem*/, unsigned int mask)
{
    if (mask == SND_CTL_EVENT_MASK_REMOVE)
    {
        trace("elem_callback - SND_CTL_EVENT_MASK_REMOVE");
        controls_changed = true;
    } else
    {
        if (mask & SND_CTL_EVENT_MASK_VALUE)
        {
            trace("elem_callback - SND_CTL_EVENT_MASK_VALUE");
            control_values_changed = true;
        }

        if (mask & SND_CTL_EVENT_MASK_INFO)
        {
            trace("elem_callback - SND_CTL_EVENT_MASK_INFO");
            controls_changed = true;
        }
    }

    return 0;
}

static int mixer_callback(snd_mixer_t *mixer, unsigned int mask, snd_mixer_elem_t *elem)
{
    return av.mixer_callback(mixer, mask, elem);
}

int AlsaVolume::mixer_callback(snd_mixer_t */*mixer*/, unsigned int mask, snd_mixer_elem_t *elem)
{
    if (mask & SND_CTL_EVENT_MASK_ADD) {
        trace("mixer_callback - SND_CTL_EVENT_MASK_ADD");
        snd_mixer_elem_set_callback(elem, ::elem_callback);
        controls_changed = true;
    }
    return 0;
}

void AlsaVolume::timer(PxDev &dev)
{
    if (dev.handle)
        snd_mixer_handle_events(dev.handle);
}

int AlsaVolume::open_mixer(PxDev *dev, int card, int playback)
{
   int err = 0;
   do
   {
      snd_mixer_selem_id_t *sid;
      snd_mixer_elem_t *elem;
      char name[256];

      snd_mixer_selem_id_alloca(&sid);

      sprintf(name, "hw:%d", card);

      dev->card = card;
      dev->handle = NULL;
      dev->playback = playback;
      dev->source = -1;

      err = snd_mixer_open(&dev->handle, 0);
      if (err < 0)
      {
         break;
      }

      err = snd_mixer_attach(dev->handle, name);
      if (err < 0)
      {
         break;
      }

      err = snd_mixer_selem_register(dev->handle, NULL, NULL);
      if (err < 0)
      {
         break;
      }
      snd_mixer_set_callback(dev->handle, ::mixer_callback);

      err = snd_mixer_load(dev->handle);
      if (err < 0)
      {
         break;
      }
      for (elem = snd_mixer_first_elem(dev->handle);
           elem != NULL;
           elem = snd_mixer_elem_next(elem))
      {
         if (playback)
         {
             bool commvol = snd_mixer_selem_has_common_volume(elem);
             bool pvol = snd_mixer_selem_has_playback_volume(elem);
             bool commswitch = snd_mixer_selem_has_common_switch(elem);
             bool pswitch = snd_mixer_selem_has_playback_switch(elem);

             if (commvol || pvol || commswitch || pswitch)
            {
                 snprintf(name,
                          sizeof(name),
                          "%s:%d",
                          snd_mixer_selem_get_name(elem),
                          snd_mixer_selem_get_index(elem));

                 PxSelem selem;
                 selem.pswitch = pswitch;
                 selem.pvol = pvol;
                 selem.commswitch = commswitch;
                 selem.commvol = commvol;
                 selem.elem = elem;
                 selem.index = snd_mixer_selem_get_index(elem);
                 selem.name = name;

                 trace("Playback: commvol " + makeStr(commvol) + " pvol " + makeStr(pvol)
                       + " commswitch " + makeStr(commswitch) + " pswitch " + makeStr(pswitch) + " " + QString(name));

                 dev->selems.push_back(selem);
            }
            continue;
         }
         else
         {
             snd_mixer_selem_id_set_name(sid, "Capture");
             snd_mixer_selem_id_set_index(sid, snd_mixer_selem_get_index(elem));
             snd_mixer_elem_t *vol = snd_mixer_find_selem(dev->handle, sid);


             bool commvol = snd_mixer_selem_has_common_volume(elem);
             bool cvol = snd_mixer_selem_has_capture_volume(elem);
             bool commswitch = snd_mixer_selem_has_common_switch(elem);
             bool cswitch = snd_mixer_selem_has_capture_switch(elem);

             if (commvol || cvol || commswitch || cswitch)
             {
                 snprintf(name,
                          sizeof(name),
                          "%s:%d",
                          snd_mixer_selem_get_name(elem),
                          snd_mixer_selem_get_index(elem));

                 trace("Capture: commvol " + makeStr(commvol) + " cvol " + makeStr(cvol)
                 + " commswitch " + makeStr(commswitch) + " cswitch " + makeStr(cswitch) + " " + QString(name));

                 PxSelem selem;
                 selem.cswitch = cswitch;
                 selem.cvol = cvol;
                 selem.commswitch = commswitch;
                 selem.commvol = commvol;

                 selem.vol = vol;
                 selem.elem = elem;
                 selem.index = snd_mixer_selem_get_index(elem);
                 selem.name = name;
                 dev->selems.push_back(selem);
             }
             else if (snd_mixer_selem_is_enum_capture(elem))
             {
                int cnt = snd_mixer_selem_get_enum_items(elem);

                if (cnt < 0)
                   continue;

                for (int j = 0; j < cnt; j++) {
                   char iname[256];
                   snd_mixer_selem_get_enum_item_name(elem, j, sizeof(iname), iname);
                   snprintf(name,
                            sizeof(name),
                            "%s:%d",
                            iname,
                            snd_mixer_selem_get_index(elem));

                   trace("Enum: " + QString(name));

                   PxSelem selem;
                   selem.vol = vol;
                   selem.item = j;
                   selem.elem = elem;
                   selem.index = snd_mixer_selem_get_index(elem);
                   selem.name = name;
                   dev->selems.push_back(selem);
                }
             }
          }
      }

      if (!playback)
      {

          for (int i = 0; i < dev->selems.size(); i++)
          {
             elem = dev->selems[i].elem;
             if (snd_mixer_selem_get_capture_group(elem) >= 0)
             {
                int sw = 0;
                if (snd_mixer_selem_get_capture_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &sw) < 0)
                {
                   continue;
                }
                if (!sw)
                {
                   continue;
                }
                dev->source = i;
                break;
             }
             else if (snd_mixer_selem_is_enum_capture(elem))
             {
                unsigned int src;
                if (snd_mixer_selem_get_enum_item(elem, SND_MIXER_SCHN_FRONT_LEFT, &src) < 0)
                {
                   continue;
                }
                if (src == dev->selems[i].item)
                {
                   dev->source = i;
                   break;
                }
             }
          }

          if (dev->source == -1)
          {
             dev->source = 0;
          }
      }
      return true;

   } while (false);

   trace(QString("Error in open_mixer ") + snd_strerror(err));
    dev->selems.clear();

   if (dev->handle) {
      snd_mixer_close(dev->handle);
      dev->handle = NULL;
   }

   return false;
}



int AlsaVolume::cleanup(px_mixer *Px)
{
   Px->info.capture.selems.clear();

   if (Px->info.capture.handle) {
      snd_mixer_close(Px->info.capture.handle);
   }

   Px->info.playback.selems.clear();

  if (Px->info.playback.handle) {
      snd_mixer_close(Px->info.playback.handle);
   }


   return false;
}
int AlsaVolume::OpenMixer_Linux_ALSA(px_mixer *Px)
{
   int card;

   if (av.GetInputCard(&card) == true) {
      if (!open_mixer(&Px->info.capture, card, false)) {
         return cleanup(Px);
      }
   }

   if (av.GetOutputCard(&card) == true) {
      if (!open_mixer(&Px->info.playback, card, true)) {
         return cleanup(Px);
      }
   }

   return true;
}

int AlsaVolume::generic_lookup(PxDev *dev, const char *generic)
{
   if (dev == NULL) {
      return -1;
   }

   for (int i = 0; i < dev->selems.size(); i++) {
      if (strncmp(dev->selems[i].name, generic, strlen(generic)) == 0) {
         return i;
      }
   }

   return -1;
}
bool AlsaVolume::has_volume_indexed(PxDev *dev, int i)
{
    snd_mixer_elem_t *elem;

    if (!dev->handle) {
       return false;
    }

    if (i < 0 || i > dev->selems.size()) {
       return false;
    }

    elem = dev->selems[i].elem;
    int s;
    if (dev->playback)
    {
       s = snd_mixer_selem_has_playback_volume(elem);
    }
    else
    {
        s = snd_mixer_selem_has_capture_volume(elem);
    }
    return s != 0;
}

PxVolume AlsaVolume::get_volume_indexed(PxDev *dev, int i)
{
   snd_mixer_elem_t *elem;
   long vol, min, max;

   if (!dev->handle) {
      return 0.0;
   }

   if (i < 0 || i > dev->selems.size()) {
      return 0.0;
   }

   elem = dev->selems[i].elem;
   if (dev->playback) {
      snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
      if (snd_mixer_selem_has_playback_channel(elem, SND_MIXER_SCHN_FRONT_LEFT)) {
         snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
         return /*(PxVolume)*/ vol * 1.0 / (max - min);
      }
   }
   else {
      snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
      if (snd_mixer_selem_has_capture_channel(elem, SND_MIXER_SCHN_FRONT_LEFT)) {
         snd_mixer_selem_get_capture_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
         return /*(PxVolume)*/ vol * 1.0 / (max - min);
      }
   }

   return 0.0;
}

PxVolume AlsaVolume::get_volume(PxDev *dev, const char *name)
{
   int i;

   if (!dev->handle) {
      return 0.0;
   }

   for (i = 0; i < dev->selems.size(); i++) {
      if (strcmp(dev->selems[i].name, name) == 0) {
         return get_volume_indexed(dev, i);
      }
   }

   return 0.0;
}

void AlsaVolume::set_volume_indexed(PxDev *dev, int i, PxVolume volume)
{
   snd_mixer_elem_t *elem;
   long vol, min, max;
   int j;

   if (!dev->handle) {
      return;
   }

   if (i < 0 || i > dev->selems.size()) {
      return;
   }

   elem = dev->selems[i].elem;
   if (dev->playback) {
      snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
      for (j = 0; j < SND_MIXER_SCHN_LAST; j++) {
         if (snd_mixer_selem_has_playback_channel(elem, static_cast<snd_mixer_selem_channel_id_t>(j))) {
            vol = volume * (max - min) + 0.5;
            snd_mixer_selem_set_playback_volume(elem, static_cast<snd_mixer_selem_channel_id_t>(j), vol);
         }
      }
   }
   else {
      snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
      for (j = 0; j < SND_MIXER_SCHN_LAST; j++) {
         if (snd_mixer_selem_has_capture_channel(elem, static_cast<snd_mixer_selem_channel_id_t>(j))) {
            vol = volume * (max - min) + 0.5;
            snd_mixer_selem_set_capture_volume(elem, static_cast<snd_mixer_selem_channel_id_t>(j), vol);
         }
      }
   }

   return;
}

void AlsaVolume::set_volume(PxDev *dev, const char *name, PxVolume volume)
{
   int i;

   if (!dev->handle) {
      return;
   }

   for (i = 0; i < dev->selems.size(); i++) {
      if (strcmp(dev->selems[i].name, name) == 0) {
         set_volume_indexed(dev, i, volume);
         break;
      }
   }

   return;
}
bool AlsaVolume::has_switch_indexed(PxDev *dev, int i)
{
    if (!dev->handle) {
       return false;
    }

    if (i < 0 || i > dev->selems.size()) {
       return false;
    }

    snd_mixer_elem_t *elem = dev->selems[i].elem;
    int s = 0;
    if (dev->playback)
    {
        s = snd_mixer_selem_has_playback_switch(elem);
    }
    else
    {
        s = snd_mixer_selem_has_capture_switch(elem);
    }

    return s != 0;
}

bool AlsaVolume::get_switch_indexed(PxDev *dev, int i)
{
    if (!dev->handle) {
       return false;
    }

    if (i < 0 || i > dev->selems.size()) {
       return false;
    }

    snd_mixer_elem_t *elem = dev->selems[i].elem;
    int s = 0;
    int err = 0;
    if (dev->playback)
    {
        err = snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &s);
    }
    else
    {
        err = snd_mixer_selem_get_capture_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &s);
    }

    if (err < 0)
    {
        trace(QString("set switch failed: ") + snd_strerror(err));
    }

    return s != 0;
}

void AlsaVolume::set_switch_indexed(PxDev *dev, int i, bool set)
{
    if (!dev->handle) {
       return;
    }

    if (i < 0 || i > dev->selems.size()) {
       return;
    }

    snd_mixer_elem_t *elem = dev->selems[i].elem;

    int err = 0;
    if (dev->playback)
    {
        err = snd_mixer_selem_set_playback_switch_all(elem, set);
    }
    else
    {
        err = snd_mixer_selem_set_capture_switch_all(elem, set);
    }

    if (err < 0)
    {
        trace(QString("set switch failed: ") + snd_strerror(err));
    }
    return;

}

void AlsaVolume::close_mixer(px_mixer *Px)
{
   cleanup(Px);

   return;
}

int AlsaVolume::get_num_mixers(px_mixer * /*Px*/)
{
   //PxInfo *info = (PxInfo *)Px->info;

   return 1;
}

const char *AlsaVolume::get_mixer_name(px_mixer */*Px*/, int /*i*/)
{
   //PxInfo *info = (PxInfo *)Px->info;

   return "ALSA";
}

/*
|| Master volume
*/

PxVolume AlsaVolume::get_master_volume(px_mixer *Px)
{
   PxDev *dev = &Px->info.playback;

   return get_volume_indexed(dev, generic_lookup(dev, "Master"));
}

void AlsaVolume::set_master_volume(px_mixer *Px, PxVolume volume)
{
    PxDev *dev = &Px->info.playback;

   set_volume_indexed(dev, generic_lookup(dev, "Master"), volume);

   /* Ensure pending events are handled...otherwise, they build up */
   if (dev->handle)
      snd_mixer_handle_events(dev->handle);

   return;
}

/*
|| Main output volume
*/

int AlsaVolume::supports_pcm_output_volume(px_mixer *Px)
{
    PxDev *dev = &Px->info.playback;

   return generic_lookup(dev, "PCM") != -1;
}

PxVolume AlsaVolume::get_pcm_output_volume(px_mixer *Px)
{
    PxDev *dev = &Px->info.playback;

   return get_volume_indexed(dev, generic_lookup(dev, "PCM"));
}

void AlsaVolume::set_pcm_output_volume(px_mixer *Px, PxVolume volume)
{
    PxDev *dev = &Px->info.playback;

   set_volume_indexed(dev, generic_lookup(dev, "PCM"), volume);

   /* Ensure pending events are handled...otherwise, they build up */
   if (dev->handle)
      snd_mixer_handle_events(dev->handle);

   return;
}

/*
|| All output volumes
*/

int AlsaVolume::get_num_output_volumes(px_mixer *Px)
{

   if (Px->info.playback.handle) {
      return Px->info.playback.selems.size();
   }

   return 0;
}

const char *AlsaVolume::get_output_volume_name(px_mixer *Px, int i)
{

   if (Px->info.playback.handle) {
      if (i >= 0 && i < Px->info.playback.selems.size()) {
         return Px->info.playback.selems[i].name;
      }
   }

   return NULL;
}

PxVolume AlsaVolume::get_output_volume(px_mixer *Px, int i)
{
   return get_volume_indexed(&Px->info.playback, i);
}

void AlsaVolume::set_output_volume(px_mixer *Px, int i, PxVolume volume)
{
   set_volume_indexed(&Px->info.playback, i, volume);

   return;
}

/*
|| Input source
*/

int AlsaVolume::get_num_input_sources(px_mixer *Px)
{
   if (Px->info.capture.handle) {
      return Px->info.capture.selems.size();
   }

   return 0;
}

const char *AlsaVolume::get_input_source_name(px_mixer *Px, int i)
{
   if (Px->info.capture.handle) {
      if (i >= 0 && i < Px->info.capture.selems.size()) {
         return Px->info.capture.selems[i].name;
      }
   }

   return NULL;
}

int AlsaVolume::get_current_input_source(px_mixer *Px)
{
   if (!Px->info.capture.handle) {
      return -1;
   }

   return Px->info.capture.source;
}

void AlsaVolume::set_current_input_source(px_mixer *Px, int i)
{
   snd_mixer_elem_t *elem;

   if (!Px->info.capture.handle) {
      return;
   }

   if (i < 0 || i >= Px->info.capture.selems.size()) {
      return;
   }

   elem = Px->info.capture.selems[i].elem;
   if (snd_mixer_selem_get_capture_group(elem) >= 0) {
      snd_mixer_selem_set_capture_switch_all(elem, true);
   }
   else if (snd_mixer_selem_is_enum_capture(elem)) {
      int j;
      for (j = 0; j < SND_MIXER_SCHN_LAST; j++) {
         snd_mixer_selem_set_enum_item(elem, static_cast<snd_mixer_selem_channel_id_t>(j), Px->info.capture.selems[i].item);
      }
   }

   Px->info.capture.source = i;

   /* Ensure pending events are handled...otherwise, they build up */
   snd_mixer_handle_events(Px->info.capture.handle);

   set_input_volume(Px, get_input_volume(Px));

   return;
}

/*
|| Input volume
*/

PxVolume AlsaVolume::get_input_volume(px_mixer *Px)
{
   snd_mixer_elem_t *elem;
   long min;
   long max;
   long vol;
   PxVolume volume = 0.0;

   if (Px->info.capture.source < 0 || Px->info.capture.selems.size() < 1) {
      return volume;
   }

   elem = Px->info.capture.selems[Px->info.capture.source].vol;
   if (!elem) {
      return volume;
   }

   snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
   if (snd_mixer_selem_has_capture_channel(elem, SND_MIXER_SCHN_FRONT_LEFT)) {
      snd_mixer_selem_get_capture_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
      volume = ( vol * 1.0) / (max - min);
   }

   return volume;
}

void AlsaVolume::set_input_volume(px_mixer *Px, PxVolume volume)
{
   snd_mixer_elem_t *elem;
   long min;
   long max;
   long vol;

   if (Px->info.capture.source < 0 || Px->info.capture.selems.size() < 1) {
      return;
   }

   elem = Px->info.capture.selems[Px->info.capture.source].vol;
   if (!elem) {
      return;
   }

   snd_mixer_selem_get_capture_volume_range(elem, &min, &max);
   vol = volume * (max - min) + 0.5;
   snd_mixer_selem_set_capture_volume_all(elem, vol);

   if (snd_mixer_selem_has_capture_switch(elem)) {
      snd_mixer_selem_set_capture_switch_all(elem, vol > 0 ? true : false);
   }

   /* Ensure pending events are handled...otherwise, they build up */
   snd_mixer_handle_events(Px->info.capture.handle);

   return;
}
/*
        get_func = snd_mixer_selem_get_playback_switch;
        set_func = snd_mixer_selem_set_playback_switch;
        channel_ids[0] = control->pswitch_channels[0];
        channel_ids[1] = control->pswitch_channels[1];
    } else {
        switch_1_mask = HAS_CSWITCH_1;
        get_func = snd_mixer_selem_get_capture_switch;
        set_func = snd_mixer_selem_set_capture_switch;
        channel_ids[0] = control->cswitch_channels[0];
        channel_ids[1] = control->cswitch_channels[1];


int snd_mixer_selem_set_playback_switch_all(snd_mixer_elem_t *elem, int value);
int snd_mixer_selem_set_capture_switch_all(snd_mixer_elem_t *elem, int value);

int snd_mixer_selem_get_playback_switch(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, int *value);
int snd_mixer_selem_get_capture_switch(snd_mixer_elem_t *elem, snd_mixer_selem_channel_id_t channel, int *value);

 */
#endif
