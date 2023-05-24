/*-------------------------------------------------------------------------------

Copyright (c) 2023 Fábio Pichler

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------------------------------------------------------*/

#include "MasterVolume_Alsa.hpp"

#include <alsa/asoundlib.h>

#define MAX_CARDS 256

int mixer_init(snd_mixer_t **handle, snd_mixer_elem_t **elem);
int open_ctl(const char *name, snd_ctl_t **ctlp);
void close_all(snd_ctl_t* ctls[], int ncards);

bool MasterVolume::set(float volume)
{
    if (volume < 0.f || volume > 1.f)
        return false;

    snd_mixer_t *handle;
    snd_mixer_elem_t *elem = nullptr;
    long minv = 0, maxv = 0;

    mixer_init(&handle, &elem);
    snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);

    long value = static_cast<long>((volume * static_cast<float>(maxv - minv)) + static_cast<float>(minv));

    if (snd_mixer_selem_set_playback_volume_all(elem, value) < 0)
    {
        snd_mixer_close(handle);
        return false;
    }

    snd_mixer_close(handle);

    return true;
}

float MasterVolume::get()
{
    snd_mixer_t *handle;
    snd_mixer_elem_t *elem = nullptr;
    long minv = 0, maxv = 0, out = 0;

    mixer_init(&handle, &elem);
    snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);

    if (snd_mixer_selem_get_playback_volume(elem, snd_mixer_selem_channel_id_t::SND_MIXER_SCHN_MONO, &out) < 0)
    {
        snd_mixer_close(handle);
        return 0.f;
    }

    snd_mixer_close(handle);

    return static_cast<float>(out - minv) / static_cast<float>(maxv - minv);
}

int MasterVolume::monitor(const std::function<void()> &func, const bool &stop)
{
    snd_ctl_t *ctls[MAX_CARDS];
    int ncards = 0;
    int i, err = 0;
    int card = -1;

    while (snd_card_next(&card) >= 0 && card >= 0)
    {
        char cardname[16];

        if (ncards >= MAX_CARDS)
        {
            close_all(ctls, ncards);

            return -E2BIG;
        }

        sprintf(cardname, "hw:%d", card);
        err = open_ctl(cardname, &ctls[ncards]);

        if (err < 0)
        {
            close_all(ctls, ncards);
            return err;
        }

        ncards++;
    }

    for (;ncards > 0 && !stop;)
    {
        auto fds = new pollfd[ncards];

        for (i = 0; i < ncards; i++)
            snd_ctl_poll_descriptors(ctls[i], &fds[i], 1);

        err = poll(fds, static_cast<nfds_t>(ncards), -1);

        if (err <= 0)
            break;

        for (i = 0; i < ncards; i++)
        {
            unsigned short revents;
            snd_ctl_poll_descriptors_revents(ctls[i], &fds[i], 1, &revents);

            if (revents & POLLIN)
            {
                snd_ctl_event_t *event;
                snd_ctl_event_alloca(&event);

                if (snd_ctl_read(ctls[i], event) < 0)
                    continue;

                if (snd_ctl_event_get_type(event) != SND_CTL_EVENT_ELEM)
                    continue;

                if (snd_ctl_event_elem_get_mask(event) & SND_CTL_EVENT_MASK_VALUE)
                    func();
            }
        }
    }

    close_all(ctls, ncards);

    return 0;
}

int mixer_init(snd_mixer_t **handle, snd_mixer_elem_t **elem)
{
    if (snd_mixer_open(handle, 0) < 0)
        return 1;

    if (snd_mixer_attach(*handle, "default") < 0)
    {
        snd_mixer_close(*handle);
        return 2;
    }

    if (snd_mixer_selem_register(*handle, nullptr, nullptr) < 0)
    {
        snd_mixer_close(*handle);
        return 3;
    }

    if (snd_mixer_load(*handle) < 0)
    {
        snd_mixer_close(*handle);
        return 4;
    }

    snd_mixer_selem_id_t *sid;

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");

    if (!(*elem = snd_mixer_find_selem(*handle, sid)))
    {
        snd_mixer_close(*handle);
        return 5;
    }

    return 0;
}

int open_ctl(const char *name, snd_ctl_t **ctlp) {
    snd_ctl_t *ctl;
    int err;

    err = snd_ctl_open(&ctl, name, SND_CTL_READONLY);

    if (err < 0)
        return err;

    err = snd_ctl_subscribe_events(ctl, 1);

    if (err < 0)
    {
        snd_ctl_close(ctl);

        return err;
    }

    *ctlp = ctl;

    return 0;
}

void close_all(snd_ctl_t *ctls[], int ncards)
{
    for (ncards -= 1; ncards >= 0; --ncards)
        snd_ctl_close(ctls[ncards]);
}
