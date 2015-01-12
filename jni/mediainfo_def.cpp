/*
 * FFDemux.h
 *
 *  Created on: Sep 13, 2014
 *      Author: amlogic
 */
 
#include <libavcodec/avcodec.h>
 
/**
 * Please refer http://android.toolib.net/reference/android/media/MediaFormat.html
 * for the details.
 * 
 * Common, Video and Audio,
 * KEY_MIME
 * KEY_MAX_INPUT_SIZE (optional, maximum size of a buffer of input data)
 * 
 * Video Decoder
 * KEY_WIDTH
 * KEY_HEIGHT
 * 
 * Audio Decoder
 * KEY_CHANNEL_COUNT
 * KEY_SAMPLE_RATE
 * KEY_IS_ADTS (optional, if decoding AAC audio content, setting this key to 1 indicates that 
 *     each audio frame is prefixed by the ADTS header.)
 * KEY_CHANNEL_MASK (optional, a mask of audio channel assignments)
 **/

const char* KEY_MIME = "mime";

/**
 * A key describing the language of the content, using either ISO 639-1
 * or 639-2/T codes.  The associated value is a string.
 */
const char* KEY_LANGUAGE = "language";

/**
 * A key describing the sample rate of an audio format.
 * The associated value is an integer
 */
const char* KEY_SAMPLE_RATE = "sample-rate";

/**
 * A key describing the number of channels in an audio format.
 * The associated value is an integer
 */
const char* KEY_CHANNEL_COUNT = "channel-count";

/**
 * A key describing the width of the content in a video format.
 * The associated value is an integer
 */
const char* KEY_WIDTH = "width";

/**
 * A key describing the height of the content in a video format.
 * The associated value is an integer
 */
const char* KEY_HEIGHT = "height";

/**
 * A key describing the maximum expected width of the content in a video
 * decoder format, in case there are resolution changes in the video content.
 * The associated value is an integer
 */
const char* KEY_MAX_WIDTH = "max-width";

/**
 * A key describing the maximum expected height of the content in a video
 * decoder format, in case there are resolution changes in the video content.
 * The associated value is an integer
 */
const char* KEY_MAX_HEIGHT = "max-height";

/** A key describing the maximum size in bytes of a buffer of data
 * described by this MediaFormat.
 * The associated value is an integer
 */
const char* KEY_MAX_INPUT_SIZE = "max-input-size";

/**
 * A key describing the bitrate in bits/sec.
 * The associated value is an integer
 */
const char* KEY_BIT_RATE = "bitrate";

/**
 * A key describing the color format of the content in a video format.
 * Constants are declared in {@link android.media.MediaCodecInfo.CodecCapabilities}.
 */
const char* KEY_COLOR_FORMAT = "color-format";

/**
 * A key describing the frame rate of a video format in frames/sec.
 * The associated value is an integer or a float.
 */
const char* KEY_FRAME_RATE = "frame-rate";

/**
 * A key describing the frequency of I frames expressed in secs
 * between I frames.
 * The associated value is an integer.
 */
const char* KEY_I_FRAME_INTERVAL = "i-frame-interval";

/**
 * @hide
 */
const char* KEY_STRIDE = "stride";
/**
 * @hide
 */
const char* KEY_SLICE_HEIGHT = "slice-height";

/**
 * Applies only when configuring a video encoder in "surface-input" mode.
 * The associated value is a long and gives the time in microseconds
 * after which the frame previously submitted to the encoder will be
 * repeated (once) if no new frame became available since.
 */
const char* KEY_REPEAT_PREVIOUS_FRAME_AFTER
    = "repeat-previous-frame-after";

/**
 * If specified when configuring a video decoder rendering to a surface,
 * causes the decoder to output "blank", i.e. black frames to the surface
 * when stopped to clear out any previously displayed contents.
 * The associated value is an integer of value 1.
 */
const char* KEY_PUSH_BLANK_BUFFERS_ON_STOP
    = "push-blank-buffers-on-shutdown";

/**
 * A key describing the duration (in microseconds) of the content.
 * The associated value is a long.
 */
const char* KEY_DURATION = "durationUs";

/**
 * A key mapping to a value of 1 if the content is AAC audio and
 * audio frames are prefixed with an ADTS header.
 * The associated value is an integer (0 or 1).
 * This key is only supported when _decoding_ content, it cannot
 * be used to configure an encoder to emit ADTS output.
 */
const char* KEY_IS_ADTS = "is-adts";

/**
 * A key describing the channel composition of audio content. This mask
 * is composed of bits drawn from channel mask definitions in {@link android.media.AudioFormat}.
 * The associated value is an integer.
 */
const char* KEY_CHANNEL_MASK = "channel-mask";

/**
 * A key describing the AAC profile to be used (AAC audio formats only).
 * Constants are declared in {@link android.media.MediaCodecInfo.CodecProfileLevel}.
 */
const char* KEY_AAC_PROFILE = "aac-profile";

/**
 * A key describing the FLAC compression level to be used (FLAC audio format only).
 * The associated value is an integer ranging from 0 (fastest, least compression)
 * to 8 (slowest, most compression).
 */
const char* KEY_FLAC_COMPRESSION_LEVEL = "flac-compression-level";

/**
 * A key for boolean AUTOSELECT behavior for the track. Tracks with AUTOSELECT=true
 * are considered when automatically selecting a track without specific user
 * choice, based on the current locale.
 * This is currently only used for subtitle tracks, when the user selected
 * 'Default' for the captioning locale.
 * The associated value is an integer, where non-0 means TRUE.  This is an optional
 * field; if not specified, AUTOSELECT defaults to TRUE.
 */
const char* KEY_IS_AUTOSELECT = "is-autoselect";

/**
 * A key for boolean DEFAULT behavior for the track. The track with DEFAULT=true is
 * selected in the absence of a specific user choice.
 * This is currently only used for subtitle tracks, when the user selected
 * 'Default' for the captioning locale.
 * The associated value is an integer, where non-0 means TRUE.  This is an optional
 * field; if not specified, DEFAULT is considered to be FALSE.
 */
const char* KEY_IS_DEFAULT = "is-default";


/**
 * A key for the FORCED field for subtitle tracks. True if it is a
 * forced subtitle track.  Forced subtitle tracks are essential for the
 * content and are shown even when the user turns off Captions.  They
 * are used for example to translate foreign/alien dialogs or signs.
 * The associated value is an integer, where non-0 means TRUE.  This is an
 * optional field; if not specified, FORCED defaults to FALSE.
 */
const char* KEY_IS_FORCED_SUBTITLE = "is-forced-subtitle";


const char *MEDIA_MIMETYPE_IMAGE_JPEG = "image/jpeg";
const char *MEDIA_MIMETYPE_VIDEO_VPX = "video/x-vnd.on2.vp8";
const char *MEDIA_MIMETYPE_VIDEO_VP8 = "video/x-vnd.on2.vp8";
const char *MEDIA_MIMETYPE_VIDEO_VP9 = "video/x-vnd.on2.vp9";
const char *MEDIA_MIMETYPE_VIDEO_AVC = "video/avc";
const char *MEDIA_MIMETYPE_VIDEO_MPEG4 = "video/mp4v-es";
const char *MEDIA_MIMETYPE_VIDEO_H263 = "video/3gpp";
const char *MEDIA_MIMETYPE_VIDEO_MPEG2 = "video/mpeg2";
const char *MEDIA_MIMETYPE_VIDEO_RM = "video/rm";
const char *MEDIA_MIMETYPE_VIDEO_RAW = "video/raw";
const char *MEDIA_MIMETYPE_VIDEO_WMV1 = "video/wmv1";
const char *MEDIA_MIMETYPE_VIDEO_WMV2 = "video/wmv2";
const char *MEDIA_MIMETYPE_VIDEO_WMV3 = "video/wmv3";
const char *MEDIA_MIMETYPE_AUDIO_AMR_NB = "audio/3gpp";
const char *MEDIA_MIMETYPE_AUDIO_AMR_WB = "audio/amr-wb";
const char *MEDIA_MIMETYPE_AUDIO_MPEG = "audio/mpeg";
const char *MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_I = "audio/mpeg-L1";
const char *MEDIA_MIMETYPE_AUDIO_MPEG_LAYER_II = "audio/mpeg-L2";
const char *MEDIA_MIMETYPE_AUDIO_AAC = "audio/mp4a-latm";
const char *MEDIA_MIMETYPE_AUDIO_QCELP = "audio/qcelp";
const char *MEDIA_MIMETYPE_AUDIO_VORBIS = "audio/vorbis";
const char *MEDIA_MIMETYPE_AUDIO_G711_ALAW = "audio/g711-alaw";
const char *MEDIA_MIMETYPE_AUDIO_G711_MLAW = "audio/g711-mlaw";
const char *MEDIA_MIMETYPE_AUDIO_RAW = "audio/raw";
const char *MEDIA_MIMETYPE_AUDIO_ADPCM_IMA = "audio/adpcm-ima";
const char *MEDIA_MIMETYPE_AUDIO_ADPCM_MS = "audio/adpcm-ms";
const char *MEDIA_MIMETYPE_AUDIO_FLAC = "audio/flac";
const char *MEDIA_MIMETYPE_AUDIO_AAC_ADTS = "audio/aac-adts";
const char *MEDIA_MIMETYPE_AUDIO_ALAC = "audio/alac";
const char *MEDIA_MIMETYPE_AUDIO_AAC_ADIF = "audio/aac-adif";
const char *MEDIA_MIMETYPE_AUDIO_AAC_LATM = "audio/aac-latm";
const char *MEDIA_MIMETYPE_AUDIO_ADTS_PROFILE = "audio/adts";
const char *MEDIA_MIMETYPE_AUDIO_WMA = "audio/wma";
const char *MEDIA_MIMETYPE_AUDIO_WMAPRO = "audio/wmapro";
const char *MEDIA_MIMETYPE_AUDIO_DTSHD  = "audio/dtshd";
const char *MEDIA_MIMETYPE_AUDIO_MSGSM = "audio/gsm";
const char *MEDIA_MIMETYPE_AUDIO_APE = "audio/ape";
const char *MEDIA_MIMETYPE_AUDIO_TRUEHD = "audio/truehd";
const char *MEDIA_MIMETYPE_AUDIO_FFMPEG = "audio/ffmpeg";
const char *MEDIA_MIMETYPE_CONTAINER_MPEG4 = "video/mp4";
const char *MEDIA_MIMETYPE_CONTAINER_WAV = "audio/x-wav";
const char *MEDIA_MIMETYPE_CONTAINER_AIFF = "audio/x-aiff";
const char *MEDIA_MIMETYPE_CONTAINER_OGG = "application/ogg";
const char *MEDIA_MIMETYPE_CONTAINER_MATROSKA = "video/x-matroska";
const char *MEDIA_MIMETYPE_CONTAINER_MPEG2TS = "video/mp2ts";
const char *MEDIA_MIMETYPE_CONTAINER_AVI = "video/avi";
const char *MEDIA_MIMETYPE_CONTAINER_MPEG2PS = "video/mp2p";
const char *MEDIA_MIMETYPE_CONTAINER_WVM = "video/wvm";
const char * MEDIA_MIMETYPE_CONTAINER_PR= "application/vnd.ms-playready";
const char *MEDIA_MIMETYPE_TEXT_3GPP = "text/3gpp-tt";
const char *MEDIA_MIMETYPE_TEXT_SUBRIP = "application/x-subrip";
const char *MEDIA_MIMETYPE_AUDIO_AC3 = "audio/ac3";
const char *MEDIA_MIMETYPE_AUDIO_EC3 = "audio/eac3";
const char *MEDIA_MIMETYPE_CONTAINER_DDP = "audio/ddp";
const char *MEDIA_MIMETYPE_VIDEO_MJPEG = "video/mjpeg";
const char *MEDIA_MIMETYPE_VIDEO_MSMPEG4 = "video/x-msmpeg";
const char *MEDIA_MIMETYPE_VIDEO_SORENSON_SPARK = "video/x-sorenson-spark";
const char *MEDIA_MIMETYPE_VIDEO_WMV = "video/x-ms-wmv";
const char *MEDIA_MIMETYPE_VIDEO_VC1 = "video/vc1";
const char *MEDIA_MIMETYPE_VIDEO_VP6 = "video/x-vnd.on2.vp6";
const char *MEDIA_MIMETYPE_VIDEO_VP6F = "video/x-vnd.on2.vp6f";
const char *MEDIA_MIMETYPE_VIDEO_VP6A = "video/x-vnd.on2.vp6a";
const char *MEDIA_MIMETYPE_VIDEO_HEVC = "video/hevc";

const char *MEDIA_MIMETYPE_AUDIO_DTS = "audio/dtshd";
const char *MEDIA_MIMETYPE_AUDIO_MP1 = "audio/mp1";
const char *MEDIA_MIMETYPE_AUDIO_MP2 = "audio/mp2";

const char *MEDIA_MIMETYPE_TEXT_TTML = "application/ttml+xml";

const char *MEDIA_MIMETYPE_CONTAINER_ASF = "video/x-ms-asf";
const char *MEDIA_MIMETYPE_CONTAINER_FLV = "video/x-flv";

struct KeyMap {
    const char *mime;
    AVCodecID key;
};
static const KeyMap kKeyMap[] = {
    { MEDIA_MIMETYPE_VIDEO_AVC, AV_CODEC_ID_H264 },
    { MEDIA_MIMETYPE_VIDEO_HEVC, AV_CODEC_ID_HEVC},
    { MEDIA_MIMETYPE_VIDEO_SORENSON_SPARK, AV_CODEC_ID_FLV1 },
    { MEDIA_MIMETYPE_VIDEO_H263, AV_CODEC_ID_H263 },
    { MEDIA_MIMETYPE_VIDEO_AVC, AV_CODEC_ID_H264 },
    { MEDIA_MIMETYPE_VIDEO_MJPEG, AV_CODEC_ID_MJPEG },
    // Setting MPEG2 mime type for MPEG1 video intentionally because
    // All standards-compliant MPEG-2 Video decoders are fully capable of
    // playing back MPEG-1 Video streams conforming to the CPB.
    { MEDIA_MIMETYPE_VIDEO_MPEG2, AV_CODEC_ID_MPEG1VIDEO },
    { MEDIA_MIMETYPE_VIDEO_MPEG2, AV_CODEC_ID_MPEG2VIDEO },
    { MEDIA_MIMETYPE_VIDEO_MPEG4, AV_CODEC_ID_MPEG4 },
    { MEDIA_MIMETYPE_VIDEO_MSMPEG4, AV_CODEC_ID_MSMPEG4V3 },
    { MEDIA_MIMETYPE_VIDEO_VC1, AV_CODEC_ID_VC1 },
    { MEDIA_MIMETYPE_VIDEO_VP6, AV_CODEC_ID_VP6 },
    { MEDIA_MIMETYPE_VIDEO_VP6A, AV_CODEC_ID_VP6A },
    { MEDIA_MIMETYPE_VIDEO_VP6F, AV_CODEC_ID_VP6F },
    { MEDIA_MIMETYPE_VIDEO_VPX, AV_CODEC_ID_VP8 },
    { MEDIA_MIMETYPE_VIDEO_VP9, AV_CODEC_ID_VP9 },
    { MEDIA_MIMETYPE_VIDEO_WMV, AV_CODEC_ID_WMV3 /* WMV9 */ },
	{ MEDIA_MIMETYPE_VIDEO_RM, AV_CODEC_ID_RV40 },
	{ MEDIA_MIMETYPE_VIDEO_WMV2, AV_CODEC_ID_WMV2 },
	{ MEDIA_MIMETYPE_VIDEO_WMV1, AV_CODEC_ID_WMV1 },
    { MEDIA_MIMETYPE_AUDIO_AAC, AV_CODEC_ID_AAC },
    { MEDIA_MIMETYPE_AUDIO_AC3, AV_CODEC_ID_AC3 },
    { MEDIA_MIMETYPE_AUDIO_EC3, AV_CODEC_ID_EAC3 },
    // TODO: check if AMR works fine once decoder supports it.
    { MEDIA_MIMETYPE_AUDIO_AMR_NB, AV_CODEC_ID_AMR_NB },
    { MEDIA_MIMETYPE_AUDIO_AMR_WB, AV_CODEC_ID_AMR_WB },
    { MEDIA_MIMETYPE_AUDIO_DTS, AV_CODEC_ID_DTS },
    { MEDIA_MIMETYPE_AUDIO_G711_ALAW, AV_CODEC_ID_PCM_ALAW },
    { MEDIA_MIMETYPE_AUDIO_G711_MLAW, AV_CODEC_ID_PCM_MULAW },
    { MEDIA_MIMETYPE_AUDIO_MP1, AV_CODEC_ID_MP1 },
    { MEDIA_MIMETYPE_AUDIO_MP2, AV_CODEC_ID_MP2 },
    { MEDIA_MIMETYPE_AUDIO_MPEG, AV_CODEC_ID_MP3 },
    // TODO: add more PCM codecs including A/MuLAW.
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_S16LE },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_S16BE },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_U16LE },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_U16BE },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_S8 },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_U8 },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_S24LE },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_S24BE },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_U24LE },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_U24BE },
    { MEDIA_MIMETYPE_AUDIO_RAW, AV_CODEC_ID_PCM_BLURAY },
    { MEDIA_MIMETYPE_AUDIO_VORBIS, AV_CODEC_ID_VORBIS },
    { MEDIA_MIMETYPE_AUDIO_WMA, AV_CODEC_ID_WMAV1 },
    { MEDIA_MIMETYPE_AUDIO_WMA, AV_CODEC_ID_WMAV2 },
    { MEDIA_MIMETYPE_AUDIO_WMA, AV_CODEC_ID_WMAVOICE },
    { MEDIA_MIMETYPE_AUDIO_WMAPRO, AV_CODEC_ID_WMAPRO },
    { MEDIA_MIMETYPE_AUDIO_WMA, AV_CODEC_ID_WMALOSSLESS },
    { MEDIA_MIMETYPE_AUDIO_APE, AV_CODEC_ID_APE},
    { MEDIA_MIMETYPE_AUDIO_TRUEHD, AV_CODEC_ID_TRUEHD},        
    { MEDIA_MIMETYPE_TEXT_3GPP, AV_CODEC_ID_MOV_TEXT },
    { MEDIA_MIMETYPE_AUDIO_FFMPEG, AV_CODEC_ID_COOK },
    { MEDIA_MIMETYPE_AUDIO_FFMPEG, AV_CODEC_ID_FLAC },
    { MEDIA_MIMETYPE_AUDIO_FFMPEG, AV_CODEC_ID_ADPCM_CT},
    { MEDIA_MIMETYPE_AUDIO_FFMPEG, AV_CODEC_ID_WAVPACK},
    { MEDIA_MIMETYPE_AUDIO_FFMPEG, AV_CODEC_ID_TTA},
    { MEDIA_MIMETYPE_AUDIO_FFMPEG, AV_CODEC_ID_DSD_LSBF},
    { MEDIA_MIMETYPE_AUDIO_FFMPEG, AV_CODEC_ID_DSD_MSBF},
    { MEDIA_MIMETYPE_AUDIO_FFMPEG, AV_CODEC_ID_DSD_LSBF_PLANAR},
    { MEDIA_MIMETYPE_AUDIO_FFMPEG, AV_CODEC_ID_DSD_MSBF_PLANAR},
};

#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
static const size_t kNumEntries = NELEM(kKeyMap);

const char *convertCodecIdToMimeType(AVCodecID codec_id)
{
    for (size_t i = 0; i < kNumEntries; ++i) {
        if (kKeyMap[i].key == codec_id) {
            return kKeyMap[i].mime;
        }
    }

    return NULL;
}
