/*
 * Copyright (C) 2015 The Android Open Source Project
 * Copyright (C) 2018 Evgeniy Stenkin <stenkinevgeniy@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef EXYNOS7580_AUDIO_HW_H
#define EXYNOS7580_AUDIO_HW_H

#include <cutils/list.h>
#include <hardware/audio.h>

#include <tinyalsa/asoundlib.h>
/* TODO: remove resampler if possible when AudioFlinger supports downsampling from 48 to 8 */
#include <audio_utils/resampler.h>
#include <audio_route/audio_route.h>
#include "ril_interface.h"

//#define PREPROCESSING_ENABLED

#define SOUND_TRIGGER_HAL_LIBRARY_PATH "/system/lib/hw/sound_trigger.primary.dragon.so"

#define DEFAULT_OUTPUT_SAMPLING_RATE 48000
#define DEFAULT_INPUT_SAMPLING_RATE 48000

#define LOW_LATENCY_OUTPUT_PERIOD_SIZE 240
#define LOW_LATENCY_OUTPUT_PERIOD_COUNT 2

#define AUDIO_CAPTURE_PERIOD_DURATION_MSEC 20
#define AUDIO_CAPTURE_PERIOD_COUNT 2
#define AUDIO_CAPTURE_PERIOD_SIZE 960

#define MIXER_PATH_MAX_LENGTH 100
#define MIXER_XML_PATH "mixer_paths.xml"


#define MIN_CHANNEL_COUNT                1
#define DEFAULT_CHANNEL_COUNT            2

#define TTY_MODE_OFF    1
#define TTY_MODE_FULL   2
#define TTY_MODE_VCO    4
#define TTY_MODE_HCO    8

#define DUALMIC_CONFIG_NONE 0
#define DUALMIC_CONFIG_1 1

/* Sound devices specific to the platform
 * The DEVICE_OUT_* and DEVICE_IN_* should be mapped to these sound
 * devices to enable corresponding mixer paths
 */

enum {
    SND_DEVICE_NONE = 0,

    /* Playback devices */
    SND_DEVICE_MIN,
    SND_DEVICE_OUT_BEGIN = SND_DEVICE_MIN,
    SND_DEVICE_OUT_EARPIECE = SND_DEVICE_OUT_BEGIN,
    SND_DEVICE_OUT_SPEAKER,
    SND_DEVICE_OUT_HEADPHONES,
    SND_DEVICE_OUT_SPEAKER_AND_HEADPHONES,
    SND_DEVICE_OUT_VOICE_EARPIECE,
    SND_DEVICE_OUT_VOICE_EARPIECE_WB,
    SND_DEVICE_OUT_VOICE_SPEAKER,
    SND_DEVICE_OUT_VOICE_SPEAKER_WB,
    SND_DEVICE_OUT_VOICE_HEADPHONES,
    SND_DEVICE_OUT_VOICE_HEADPHONES_WB,
    SND_DEVICE_OUT_VOICE_BT_SCO,
    SND_DEVICE_OUT_VOICE_BT_SCO_WB,
    SND_DEVICE_OUT_HDMI,
    SND_DEVICE_OUT_SPEAKER_AND_HDMI,
    SND_DEVICE_OUT_BT_SCO,
    SND_DEVICE_OUT_END,

    /*
     * Note: IN_BEGIN should be same as OUT_END because total number of devices
     * SND_DEVICES_MAX should not exceed MAX_RX + MAX_TX devices.
     */
    /* Capture devices */
    SND_DEVICE_IN_BEGIN = SND_DEVICE_OUT_END,
    SND_DEVICE_IN_EARPIECE_MIC  = SND_DEVICE_IN_BEGIN,
    SND_DEVICE_IN_SPEAKER_MIC,
    SND_DEVICE_IN_HEADSET_MIC,
    SND_DEVICE_IN_EARPIECE_MIC_AEC,
    SND_DEVICE_IN_SPEAKER_MIC_AEC,
    SND_DEVICE_IN_HEADSET_MIC_AEC,
    SND_DEVICE_IN_VOICE_MIC,
    SND_DEVICE_IN_VOICE_EARPIECE_MIC,
    SND_DEVICE_IN_VOICE_EARPIECE_MIC_WB,
    SND_DEVICE_IN_VOICE_SPEAKER_MIC,
    SND_DEVICE_IN_VOICE_SPEAKER_MIC_WB,
    SND_DEVICE_IN_VOICE_HEADSET_MIC,
    SND_DEVICE_IN_VOICE_HEADSET_MIC_WB,
    SND_DEVICE_IN_VOICE_BT_SCO_MIC,
    SND_DEVICE_IN_VOICE_BT_SCO_MIC_WB,
    SND_DEVICE_IN_HDMI_MIC,
    SND_DEVICE_IN_BT_SCO_MIC,
    SND_DEVICE_IN_CAMCORDER_MIC,
    SND_DEVICE_IN_VOICE_REC_HEADSET_MIC,
    SND_DEVICE_IN_VOICE_REC_MIC,
    SND_DEVICE_IN_LOOPBACK_AEC,
    SND_DEVICE_IN_END,

    SND_DEVICE_MAX = SND_DEVICE_IN_END,

};

#define MIXER_CARD 0
#define SOUND_CARD 0

/*
 * tinyAlsa library interprets period size as number of frames
 * one frame = channel_count * sizeof (pcm sample)
 * so if format = 16-bit PCM and channels = Stereo, frame size = 2 ch * 2 = 4 bytes
 * DEEP_BUFFER_OUTPUT_PERIOD_SIZE = 1024 means 1024 * 4 = 4096 bytes
 * We should take care of returning proper size when AudioFlinger queries for
 * the buffer size of an input/output stream
 */
#define PLAYBACK_PERIOD_SIZE 512
#define PLAYBACK_PERIOD_COUNT 2
#define PLAYBACK_DEFAULT_CHANNEL_COUNT 2
#define PLAYBACK_DEFAULT_SAMPLING_RATE 48000
#define PLAYBACK_START_THRESHOLD ((PLAYBACK_PERIOD_SIZE * PLAYBACK_PERIOD_COUNT) - 1)
#define PLAYBACK_STOP_THRESHOLD (PLAYBACK_PERIOD_SIZE * PLAYBACK_PERIOD_COUNT)
#define PLAYBACK_AVAILABLE_MIN 1

#define PLAYBACK_HDMI_MULTI_PERIOD_SIZE  1024
#define PLAYBACK_HDMI_MULTI_PERIOD_COUNT 4
#define PLAYBACK_HDMI_MULTI_DEFAULT_CHANNEL_COUNT 6
#define PLAYBACK_HDMI_MULTI_PERIOD_BYTES \
    (PLAYBACK_HDMI_MULTI_PERIOD_SIZE * PLAYBACK_HDMI_MULTI_DEFAULT_CHANNEL_COUNT * 2)
#define PLAYBACK_HDMI_MULTI_START_THRESHOLD 4095
#define PLAYBACK_HDMI_MULTI_STOP_THRESHOLD 4096
#define PLAYBACK_HDMI_MULTI_AVAILABLE_MIN 1

#define PLAYBACK_HDMI_DEFAULT_CHANNEL_COUNT   2

#define CAPTURE_PERIOD_SIZE 1024
#define CAPTURE_PERIOD_SIZE_LOW_LATENCY 512
#define CAPTURE_PERIOD_COUNT 2
#define CAPTURE_DEFAULT_CHANNEL_COUNT 2
#define CAPTURE_DEFAULT_SAMPLING_RATE 48000
#define CAPTURE_START_THRESHOLD 1

#define DEEP_BUFFER_OUTPUT_SAMPLING_RATE 48000
#define DEEP_BUFFER_OUTPUT_PERIOD_SIZE 1440
#define DEEP_BUFFER_OUTPUT_PERIOD_COUNT 4

#define LOW_LATENCY_CAPTURE_SAMPLE_RATE 48000
#define LOW_LATENCY_CAPTURE_PERIOD_SIZE 240
#define LOW_LATENCY_CAPTURE_USE_CASE 0



#define MAX_SUPPORTED_CHANNEL_MASKS 2

struct cras_dsp_context;

typedef int snd_device_t;

/* These are the supported use cases by the hardware.
 * Each usecase is mapped to a specific PCM device.
 * Refer to pcm_device_table[].
 */
typedef enum {
    USECASE_INVALID = -1,
    /* Playback usecases */
    USECASE_AUDIO_PLAYBACK_DEEP_BUFFER = 0,
    USECASE_AUDIO_PLAYBACK_LOW_LATENCY,
    USECASE_AUDIO_PLAYBACK_OFFLOAD,

    /* HFP Use case*/
    USECASE_AUDIO_HFP_SCO,
    USECASE_AUDIO_HFP_SCO_WB,

    /* Capture usecases */
    USECASE_AUDIO_CAPTURE,
    USECASE_AUDIO_CAPTURE_LOW_LATENCY,

    USECASE_VOICE_CALL,  /* Usecase setup for voice session on first subscription for DSDS/DSDA */
    USECASE_VOLTE_CALL,  /* Usecase setup for VoLTE session on first subscription */

    USECASE_INCALL_REC_UPLINK,
    USECASE_INCALL_REC_DOWNLINK,
    USECASE_INCALL_REC_UPLINK_AND_DOWNLINK,

    /* VOIP usecase*/
    USECASE_AUDIO_PLAYBACK_VOIP,
    USECASE_AUDIO_RECORD_VOIP,

    AUDIO_USECASE_MAX
} audio_usecase_t;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/*
 * tinyAlsa library interprets period size as number of frames
 * one frame = channel_count * sizeof (pcm sample)
 * so if format = 16-bit PCM and channels = Stereo, frame size = 2 ch * 2 = 4 bytes
 * DEEP_BUFFER_OUTPUT_PERIOD_SIZE = 1024 means 1024 * 4 = 4096 bytes
 * We should take care of returning proper size when AudioFlinger queries for
 * the buffer size of an input/output stream
 */

typedef enum usecase_type_t {
    PCM_PLAYBACK,
    PCM_CAPTURE,
    VOICE_CALL,
    PCM_HFP_CALL,
    USECASE_TYPE_MAX
} usecase_type_t;


struct pcm_device_profile {
    struct pcm_config config;
    int               card;
    int               device;
    int               id;
    usecase_type_t    type;
    audio_devices_t   devices;
    const char*       dsp_name;
};

struct pcm_device {
    struct listnode            stream_list_node;
    struct pcm_device_profile* pcm_profile;
    struct pcm*                pcm;
    int                        status;
    /* TODO: remove resampler if possible when AudioFlinger supports downsampling from 48 to 8 */
    struct resampler_itfe*     resampler;
    int16_t*                   res_buffer;
    size_t                     res_byte_count;
    struct cras_dsp_context*   dsp_context;
    int                        sound_trigger_handle;
};

struct stream_out {
    struct audio_stream_out     stream;
    pthread_mutex_t             lock; /* see note below on mutex acquisition order */
    pthread_mutex_t             pre_lock; /* acquire before lock to avoid DOS by playback thread */
    int pcm_device_id;
    struct pcm *pcm;
    pthread_cond_t              cond;
    struct pcm_config           config;
    struct listnode             pcm_dev_list;
    int                         standby;
    unsigned int                sample_rate;
    audio_channel_mask_t        channel_mask;
    audio_format_t              format;
    audio_devices_t             devices;
    audio_output_flags_t        flags;
    audio_usecase_t             usecase;
    /* Array of supported channel mask configurations. +1 so that the last entry is always 0 */
    audio_channel_mask_t        supported_channel_masks[MAX_SUPPORTED_CHANNEL_MASKS + 1];
    bool                        muted;
    /* total frames written, not cleared when entering standby */
    uint64_t                    written;
    audio_io_handle_t           handle;

    int                         non_blocking;

    int                         send_new_metadata;

    struct audio_device*        dev;
    void *proc_buf_out;
    size_t proc_buf_size;
};

struct stream_in {
    struct audio_stream_in              stream;
    pthread_mutex_t                     lock; /* see note below on mutex acquisition order */
    pthread_mutex_t                     pre_lock; /* acquire before lock to avoid DOS by
                                                     capture thread */
    struct pcm_config                   config;
    struct pcm *pcm;
    int 				pcm_device_id;
    struct listnode                     pcm_dev_list;
    int                                 standby;
    audio_source_t                      source;
    audio_devices_t                     devices;
    uint32_t                            main_channels;
    audio_usecase_t                     usecase;
    usecase_type_t                      usecase_type;
    bool                                enable_aec;
    audio_input_flags_t                 input_flags;

    /* TODO: remove resampler if possible when AudioFlinger supports downsampling from 48 to 8 */
    unsigned int                        requested_rate;
    struct resampler_itfe*              resampler;
    struct resampler_buffer_provider    buf_provider;
    int16_t 				*buffer;
    size_t 				buffer_size;
    size_t 				frames_in;
    int                                 read_status;
    int16_t*                            read_buf;
    size_t                              read_buf_size;
    size_t                              read_buf_frames;

    void *proc_buf_out;
    size_t proc_buf_size;
    int64_t                             frames_read; // total frames read, not cleared when


#ifdef PREPROCESSING_ENABLED
    void *proc_buf_in;
    size_t proc_buf_frames;

    int num_preprocessors;
    struct effect_info_s preprocessors[MAX_PREPROCESSORS];

    bool aux_channels_changed;
    uint32_t aux_channels;
#endif

    struct audio_device*                dev;
};

struct audio_usecase {
    struct listnode         adev_list_node;
    audio_usecase_t         id;
    usecase_type_t          type;
    audio_devices_t         devices;
    snd_device_t            out_snd_device;
    snd_device_t            in_snd_device;
    struct audio_stream*    stream;
    struct listnode         mixer_list;
};


struct audio_device {
    struct audio_hw_device  device;
    pthread_mutex_t         lock; /* see note below on mutex acquisition order */
    struct listnode         mixer_list;
    struct mixer 	    *mixer;
    struct audio_route      *audio_route;
    audio_mode_t            mode;
    struct stream_in*       active_input;
    struct stream_out*      primary_output;
    struct stream_out* 	    current_call_output;
    int                     in_call;
    float                   voice_volume;
    bool                    mic_mute;
    bool                    fix;
    bool                    voice_two_mic;
    bool                    voice_wb;
    int                     tty_mode;
    bool                    bluetooth_nrec;
    bool 		    bluetooth_wbs;
    bool                    screen_off;
    bool 		    fm_mode;
    bool                    sco_act;
    int*                    snd_dev_ref_cnt;
    struct listnode         usecase_list;
    bool                    speaker_lr_swap;
    unsigned int            cur_hdmi_channels;
    int                     dualmic_config;
    bool                    ns_in_voice_rec;

    void*                   sound_trigger_lib;
    int                     (*sound_trigger_open_for_streaming)();
    size_t                  (*sound_trigger_read_samples)(int, void*, size_t);
    int                     (*sound_trigger_close_for_streaming)(int);

    int                     dummybuf_thread_timeout;
    int                     dummybuf_thread_cancel;
    int                     dummybuf_thread_active;
    audio_devices_t         dummybuf_thread_devices;
    /* RIL */
    struct 	            ril_handle ril;

    /* Call audio */
    struct pcm              *pcm_voice_rx;
    struct pcm              *pcm_voice_tx;

    /* SCO audio */
    struct pcm 		    *pcm_sco_rx;
    struct pcm 		    *pcm_sco_tx;


    /* FM audio */
    struct pcm 		    *pcm_fm_rx;
    struct pcm 	            *pcm_fm_tx;

    pthread_mutex_t         dummybuf_thread_lock;
    pthread_t               dummybuf_thread;

    pthread_mutex_t         lock_inputs; /* see note below on mutex acquisition order */
};

/*
 * NOTE: when multiple mutexes have to be acquired, always take the
 * lock_inputs, stream_in, stream_out, and then audio_device.
 * stream_in mutex must always be before stream_out mutex
 * if both have to be taken (see get_echo_reference(), put_echo_reference()...)
 * dummybuf_thread mutex is not related to the other mutexes with respect to order.
 * lock_inputs must be held in order to either close the input stream, or prevent closure.
 */

#endif // EXYNOS7580_AUDIO_HW_H