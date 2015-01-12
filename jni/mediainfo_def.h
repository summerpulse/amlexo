/*
 * FFDemux.h
 *
 *  Created on: Sep 13, 2014
 *      Author: amlogic
 */

#ifndef _MEDIAINFO_DEF_H_
#define _MEDIAINFO_DEF_H_

extern const char* KEY_MIME;

/**
 * A key describing the language of the content, using either ISO 639-1
 * or 639-2/T codes.  The associated value is a string.
 */
extern const char* KEY_LANGUAGE;

/**
 * A key describing the sample rate of an audio format.
 * The associated value is an integer
 */
extern const char* KEY_SAMPLE_RATE;

/**
 * A key describing the number of channels in an audio format.
 * The associated value is an integer
 */
extern const char* KEY_CHANNEL_COUNT;

/**
 * A key describing the width of the content in a video format.
 * The associated value is an integer
 */
extern const char* KEY_WIDTH;

/**
 * A key describing the height of the content in a video format.
 * The associated value is an integer
 */
extern const char* KEY_HEIGHT;

/**
 * A key describing the maximum expected width of the content in a video
 * decoder format, in case there are resolution changes in the video content.
 * The associated value is an integer
 */
extern const char* KEY_MAX_WIDTH;

/**
 * A key describing the maximum expected height of the content in a video
 * decoder format, in case there are resolution changes in the video content.
 * The associated value is an integer
 */
extern const char* KEY_MAX_HEIGHT;

/** A key describing the maximum size in bytes of a buffer of data
 * described by this MediaFormat.
 * The associated value is an integer
 */
extern const char* KEY_MAX_INPUT_SIZE;

/**
 * A key describing the bitrate in bits/sec.
 * The associated value is an integer
 */
extern const char* KEY_BIT_RATE;

/**
 * A key describing the color format of the content in a video format.
 * Constants are declared in {@link android.media.MediaCodecInfo.CodecCapabilities}.
 */
extern const char* KEY_COLOR_FORMAT;

/**
 * A key describing the frame rate of a video format in frames/sec.
 * The associated value is an integer or a float.
 */
extern const char* KEY_FRAME_RATE;

/**
 * A key describing the frequency of I frames expressed in secs
 * between I frames.
 * The associated value is an integer.
 */
extern const char* KEY_I_FRAME_INTERVAL;

/**
 * @hide
 */
extern const char* KEY_STRIDE;
/**
 * @hide
 */
extern const char* KEY_SLICE_HEIGHT;

/**
 * Applies only when configuring a video encoder in "surface-input" mode.
 * The associated value is a long and gives the time in microseconds
 * after which the frame previously submitted to the encoder will be
 * repeated (once) if no new frame became available since.
 */
extern const char* KEY_REPEAT_PREVIOUS_FRAME_AFTER;

/**
 * If specified when configuring a video decoder rendering to a surface,
 * causes the decoder to output "blank", i.e. black frames to the surface
 * when stopped to clear out any previously displayed contents.
 * The associated value is an integer of value 1.
 */
extern const char* KEY_PUSH_BLANK_BUFFERS_ON_STOP;

/**
 * A key describing the duration (in microseconds) of the content.
 * The associated value is a long.
 */
extern const char* KEY_DURATION;

/**
 * A key mapping to a value of 1 if the content is AAC audio and
 * audio frames are prefixed with an ADTS header.
 * The associated value is an integer (0 or 1).
 * This key is only supported when _decoding_ content, it cannot
 * be used to configure an encoder to emit ADTS output.
 */
extern const char* KEY_IS_ADTS;

/**
 * A key describing the channel composition of audio content. This mask
 * is composed of bits drawn from channel mask definitions in {@link android.media.AudioFormat}.
 * The associated value is an integer.
 */
extern const char* KEY_CHANNEL_MASK;

/**
 * A key describing the AAC profile to be used (AAC audio formats only).
 * Constants are declared in {@link android.media.MediaCodecInfo.CodecProfileLevel}.
 */
extern const char* KEY_AAC_PROFILE;

/**
 * A key describing the FLAC compression level to be used (FLAC audio format only).
 * The associated value is an integer ranging from 0 (fastest, least compression)
 * to 8 (slowest, most compression).
 */
extern const char* KEY_FLAC_COMPRESSION_LEVEL;

/**
 * A key for boolean AUTOSELECT behavior for the track. Tracks with AUTOSELECT=true
 * are considered when automatically selecting a track without specific user
 * choice, based on the current locale.
 * This is currently only used for subtitle tracks, when the user selected
 * 'Default' for the captioning locale.
 * The associated value is an integer, where non-0 means TRUE.  This is an optional
 * field; if not specified, AUTOSELECT defaults to TRUE.
 */
extern const char* KEY_IS_AUTOSELECT;

/**
 * A key for boolean DEFAULT behavior for the track. The track with DEFAULT=true is
 * selected in the absence of a specific user choice.
 * This is currently only used for subtitle tracks, when the user selected
 * 'Default' for the captioning locale.
 * The associated value is an integer, where non-0 means TRUE.  This is an optional
 * field; if not specified, DEFAULT is considered to be FALSE.
 */
extern const char* KEY_IS_DEFAULT;


/**
 * A key for the FORCED field for subtitle tracks. True if it is a
 * forced subtitle track.  Forced subtitle tracks are essential for the
 * content and are shown even when the user turns off Captions.  They
 * are used for example to translate foreign/alien dialogs or signs.
 * The associated value is an integer, where non-0 means TRUE.  This is an
 * optional field; if not specified, FORCED defaults to FALSE.
 */
extern const char* KEY_IS_FORCED_SUBTITLE;

extern const char *convertCodecIdToMimeType(AVCodecID codec_id);

#endif /* _MEDIAINFO_DEF_H_ */
