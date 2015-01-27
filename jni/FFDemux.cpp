/*
 * FFDemux.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: amlogic
 */

#include <utils/log2.h>
#include "FFDemux.h"
#include "mediainfo_def.h"
#include "PTSPopulator.h"
#include "utils/log2.h"
#include "formats/AVCFormat.h"

#include <stdint.h>

namespace android {

const int SEEK_TO_PREVIOUS_SYNC = 0;
const int SEEK_TO_NEXT_SYNC = 1;
const int SEEK_TO_CLOSEST_SYNC = 2;
const int kMicroSecPerSec = 1000000;
const int64_t kUnknownPTS = __INT64_C(-9223372036854775807)-1;

char const* FFDemux::MediaMimeItem::getstr()
{
    if (type == MD_STRING) {
        return str.c_str();
    }

    return NULL;
}

int FFDemux::MediaMimeItem::setstr(const char* val)
{
    str = val;
    type = MD_STRING;
    return 0;
}

FFDemux::MediaMimeItem::~MediaMimeItem()
{

}

FFDemux::FFDemux()
        :src(NULL),
        mFormatContext(NULL),
        videoIndex(-1),
        audioIndex(-1),
        subtitleIndex(-1),
        videoStreamIndex(-1),
        audioStreamIndex(-1),
        subtitleStreamIndex(-1),
        mStatus(0),
        currentTrack(-1),
        mSampleTimeUs(0),
        mPTSPopulator(NULL),
        mStartTimeUs(0)
{
    // TODO Auto-generated constructor stub

}

FFDemux::~FFDemux() 
{
	Mutex::Autolock autoLock(mLock);
    // TODO Auto-generated destructor stub

    avformat_free_context(mFormatContext);
    avformat_close_input(&mFormatContext);
    if (src)
        delete src;
    mTracks.clear();
}

AVFormatContext* FFDemux::openAVFormatContext(FFIO *fio)
{
    if (fio == NULL) {
        LOGE("Bad input parameter. \n");
        return NULL;
    }
    AVFormatContext* context = avformat_alloc_context();
    if (context == NULL) {
        LOGE("Failed to allocate AVFormatContext.");
        return NULL;
    }
    context->pb = fio->getContext();
    int res = avformat_open_input(
            &context,
            NULL,  // need to pass a filename
            NULL,  // probe the container format.
            NULL);  // no special parameters
    if (res < 0) {
        LOGE("Failed to open the input stream.");
        avformat_free_context(context);
        return NULL;
    }
    res = avformat_find_stream_info(context, NULL);
    if (res < 0) {
        LOGE("Failed to find stream information.");
        avformat_close_input(&context);
        return NULL;
    }
    return context;
}

StreamFormats* FFDemux::selectFormat(enum AVCodecID codecid)
{
	if (codecid == AV_CODEC_ID_H264)
		return new AVCFormat();
	else if (codecid == AV_CODEC_ID_AAC)
		return new StreamFormats();
	return new StreamFormats();
}

status_t FFDemux::post_init()
{
    status_t ret = -1;
    const char* str;
    MediaMimeItem mmt;
    AVStream *st;
    AVCodecContext* codec;
    TrackInfo trackinfo;

    mFormatContext = openAVFormatContext(&ffio);
    if (mFormatContext == NULL) {
        LOGE("Failed to open FFmpeg context.");
        goto exit;
    }

    mTracks.clear();
    ret = av_find_best_stream(mFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0) {
        LOGW("Could not find %s stream in input file\n",
                    av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
    } else {
    	videoIndex = mTracks.size();
        mTracks.push_back(trackinfo);
        mTracks[videoIndex].mTrackType = 0;
        mTracks[videoIndex].mTrackFlags = 0;
        mTracks[videoIndex].mTrackIndex = ret;
    }
    ret = av_find_best_stream(mFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (ret < 0) {
    	LOGW("Could not find %s stream in input file\n",
                    av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
    } else {
    	audioIndex = mTracks.size();
    	mTracks.push_back(trackinfo);
        mTracks[audioIndex].mTrackType = 1;
        mTracks[audioIndex].mTrackFlags = 0;
        mTracks[audioIndex].mTrackIndex = ret;
    }
    ret = av_find_best_stream(mFormatContext, AVMEDIA_TYPE_SUBTITLE, -1, -1, NULL, 0);
    if (ret < 0) {
        LOGW("Could not find %s stream in input file\n",
                    av_get_media_type_string(AVMEDIA_TYPE_SUBTITLE));
    } else {
    	subtitleIndex = mTracks.size();
    	mTracks.push_back(trackinfo);
        mTracks[subtitleIndex].mTrackType = 2;
        mTracks[subtitleIndex].mTrackFlags = 0;
        mTracks[subtitleIndex].mTrackIndex = ret;
    }

    if (videoIndex == -1 && audioIndex == -1) {
    	LOGE("What a f**king stream, no audio/video\n");
    	goto exit;
    }

    /* set mime for video. */
    if (videoIndex >= 0) {
		st = mFormatContext->streams[mTracks[videoIndex].mTrackIndex];
		codec = st->codec;
		str = convertCodecIdToMimeType(st->codec->codec_id);
		if (str == NULL) {
			LOGV("Doesn't support video type %x\n", st->codec->codec_id);
			goto exit;
		} else
			LOGV("Video: %s:width:%d, height:%d found.\n",
						str, st->codec->width, st->codec->height);
		mTracks[videoIndex].mSeleted = 0;
		mTracks[videoIndex].mTrackType = 0;
		mTracks[videoIndex].mExtraPushed = 0;
		mTracks[videoIndex].mime = str;
		mTracks[videoIndex].video.width = st->codec->width;
		mTracks[videoIndex].video.height = st->codec->height;
		mTracks[videoIndex].mPacketQueue.clear();
		mTracks[videoIndex].mFormat = selectFormat(codec->codec_id);
		mTracks[videoIndex].mFormat->formatCodecMeta(codec);
		mTracks[videoIndex].mExtraData = mTracks[videoIndex].mFormat->mcodecMeta;
		mTracks[videoIndex].mExtraSize = mTracks[videoIndex].mFormat->mCodecMetaSize;
    }

    /* set mime for audio. */
    if (audioIndex >= 0) {
		st = mFormatContext->streams[mTracks[audioIndex].mTrackIndex];
		codec = st->codec;
		str = convertCodecIdToMimeType(st->codec->codec_id);
		if (str == NULL) {
			LOGV("Doesn't support audio type %x\n", st->codec->codec_id);
			goto exit;
		} else
			LOGV("Audio: %s, rate:%d, channels:%d found.\n", str,
					st->codec->sample_rate,
					st->codec->channels);
		mTracks[audioIndex].mSeleted = 0;
		mTracks[audioIndex].mTrackType = 1;
		mTracks[audioIndex].mExtraPushed = 0;
		mTracks[audioIndex].mime = str;
		mTracks[audioIndex].audio.sampleRate = st->codec->sample_rate;
		mTracks[audioIndex].audio.channels = st->codec->channels;
		mTracks[audioIndex].mPacketQueue.clear();
		mTracks[audioIndex].mFormat = selectFormat(codec->codec_id);
		mTracks[audioIndex].mFormat->formatCodecMeta(codec);
		mTracks[audioIndex].mExtraData = mTracks[audioIndex].mFormat->mcodecMeta;
		mTracks[audioIndex].mExtraSize = mTracks[audioIndex].mFormat->mCodecMetaSize;

    }

    /* set parameter for subtitle. */
    if (subtitleIndex >= 0) {
		st = mFormatContext->streams[mTracks[subtitleIndex].mTrackIndex];
		codec = st->codec;
		str = convertCodecIdToMimeType(st->codec->codec_id);
		if (str == NULL) {
			LOGV("Doesn't support subtitle type %x\n", st->codec->codec_id);
			goto exit;
		} else
			LOGV("Subtitle: %s found.\n", str);
		mTracks[subtitleIndex].mSeleted = 0;
		mTracks[subtitleIndex].mExtraPushed = 2;
		mTracks[subtitleIndex].mTrackType = 2;
		mTracks[subtitleIndex].mime = str;
		mTracks[subtitleIndex].subtitle.lang = "eng";
		mTracks[subtitleIndex].mPacketQueue.clear();
		mTracks[subtitleIndex].mFormat = selectFormat(codec->codec_id);
		mTracks[subtitleIndex].mFormat->formatCodecMeta(codec);
		mTracks[subtitleIndex].mExtraData = mTracks[subtitleIndex].mFormat->mcodecMeta;
		mTracks[subtitleIndex].mExtraSize = mTracks[subtitleIndex].mFormat->mCodecMetaSize;
    }

    mPTSPopulator = new PTSPopulator(mTracks.size());
    if (static_cast<int64_t>(AV_NOPTS_VALUE) !=  mFormatContext->start_time)
    	mStartTimeUs = mFormatContext->start_time;

    ret = 0;
exit:
    return ret;
}

status_t FFDemux::setDataSource(const char *path)
{
    status_t ret = -1;
    Mutex::Autolock autoLock(mLock);

    av_register_all();
    src = new FileSource(path);
    if (src != NULL) {
        ffio.init(src);
        ret = post_init();
    } else
        LOGE(" init source error.\n");

    return ret;
}

status_t FFDemux::setDataSource(int fd, off64_t offset, off64_t size)
{
	Mutex::Autolock autoLock(mLock);
	status_t ret = -1;
    av_register_all();
	src = new FileSource(fd, offset, size);
	if (src != NULL) {
		ffio.init(src);
		ret = post_init();
	} else
		LOGE(" init source error.\n");
	return post_init();
}

status_t FFDemux::setDataSource(DataSource* datasource)
{
	Mutex::Autolock autoLock(mLock);

    return -1;
}

size_t FFDemux::countTracks()
{
	Mutex::Autolock autoLock(mLock);
    return  mTracks.size();
}

status_t FFDemux::getTrackFormat(size_t index, TrackInfo** trackinfo)
{
	Mutex::Autolock autoLock(mLock);
    if (index < 0 || index >= mTracks.size())
        return -1;
    *trackinfo = &mTracks[index];
    return 0;
}

status_t FFDemux::selectTrack(size_t index)
{
    int i;
    Mutex::Autolock autoLock(mLock);

    if (index < 0 || index >= mTracks.size())
        return -1;

    mTracks[index].mSeleted = 1;
    if (index == videoIndex)
    	videoStreamIndex = mTracks[index].mTrackIndex;
    if (index == audioIndex)
    	audioStreamIndex = mTracks[index].mTrackIndex;
    if (index == subtitleIndex)
    	subtitleStreamIndex = mTracks[index].mTrackIndex;
    return 0;
}

status_t FFDemux::unselectTrack(size_t index)
{
    int i;
    Mutex::Autolock autoLock(mLock);

    if (index < 0 || index >= mTracks.size())
        return -1;

    mTracks[index].mSeleted = 0;
    if (index == videoIndex)
    	videoStreamIndex = -1;
    if (index == audioIndex)
    	audioStreamIndex = -1;
    if (index == subtitleIndex)
    	subtitleStreamIndex = -1;
    return 0;
}

status_t FFDemux::seekTo(int64_t timeUs, int mode)
{
    int seekFlag = 0;
    switch (mode) {
        case SEEK_TO_PREVIOUS_SYNC:
            seekFlag |= AVSEEK_FLAG_BACKWARD;
            break;
        case SEEK_TO_NEXT_SYNC:
            break;
        case SEEK_TO_CLOSEST_SYNC:
            seekFlag |= AVSEEK_FLAG_ANY;
            break;
        default:
        	LOGE("Unknown seek mode.");
            break;
    }

    mPTSPopulator->reset();
    {
    	Mutex::Autolock autoLock(mLock);
        int64_t seekPosition = convertTimeBaseToMicroSec(timeUs);
        if (av_seek_frame(mFormatContext, -1 /* default stream */,
                seekPosition, seekFlag) < 0) {
        	LOGE("Failed to seek to %lld", seekPosition);
            return -1;
        }
    }
    currentTrack = -1;
    while (currentTrack == -1) {
    	advance();
    }
    return 0;
}

int64_t FFDemux::convertTimeBaseToMicroSec(int64_t time)
{
    return time * kMicroSecPerSec / AV_TIME_BASE;
}

int64_t FFDemux::convertMicroSecToTimeBase(int64_t time)
{
    return time * AV_TIME_BASE / kMicroSecPerSec;
}

int64_t FFDemux::convertStreamTimeToUs(AVStream *st, int64_t timeInStreamTime)
{
    return timeInStreamTime * AV_TIME_BASE * st->time_base.num / st->time_base.den;
}

status_t FFDemux::advance()
{
    int i;
    int ret = 0;
    int search_end = 0;
    AVPacket *packet = new AVPacket();

    Mutex::Autolock autoLock(mLock);
    do {
    	mStatus = av_read_frame(mFormatContext, packet);
        if (mStatus < 0 || avio_feof(mFormatContext->pb)) {
        	currentTrack = -1;
        	LOGE("[ERROR]av_read_frame:%s.\n", av_err2str(mStatus));
        	delete packet;
            return mStatus;
        }
        if (packet->size == 0) {
        	av_free_packet(packet);
        	continue;
        }

        if (packet->stream_index == videoStreamIndex) {
        	av_dup_packet(packet);
        	mTracks[videoIndex].mPacketQueue.push_back(packet);
        } else if(packet->stream_index == audioStreamIndex) {
        	av_dup_packet(packet);
        	mTracks[audioIndex].mPacketQueue.push_back(packet);
        } else if(packet->stream_index == subtitleStreamIndex) {
        	av_dup_packet(packet);
        	mTracks[subtitleIndex].mPacketQueue.push_back(packet);
        } else {
        	av_free_packet(packet);
        	continue;
        }
        break;
    } while (true);
    return 0;
}

int FFDemux::select_current_track()
{
	/* return the extra data first. */
	if (videoStreamIndex >= 0 && mTracks[videoIndex].mExtraPushed == 0)
		return videoIndex;
	else if (audioStreamIndex >= 0 && mTracks[audioIndex].mExtraPushed == 0)
		return audioIndex;
	else if (subtitleStreamIndex >= 0 && mTracks[subtitleIndex].mExtraPushed == 0)
		return subtitleIndex;

	/* just select a non empty buffer to pop. */
	if (videoStreamIndex >= 0 && mTracks[videoIndex].mPacketQueue.size() > 0)
		return videoIndex;
	else if (audioStreamIndex >= 0 && mTracks[audioIndex].mPacketQueue.size() > 0)
		return audioIndex;
	else if (subtitleStreamIndex >= 0 && mTracks[subtitleIndex].mPacketQueue.size() > 0)
		return subtitleIndex;
	return -1;
}

status_t FFDemux::readSampleData(char* buf, size_t& len, int offset)
{
	Mutex::Autolock autoLock(mLock);
	if (offset > len) {
		len = 0;
		LOGE("[X]Offset:%d exeeding:%d.\n", offset, len);
		return -1;
	}
	if (mStatus < 0) {
		len = 0;
		return -1;
	}

	currentTrack = select_current_track();
	if (currentTrack < 0 || currentTrack > 2) {
		len = 0;
		return 0;
	}

	if (mTracks[currentTrack].mExtraPushed) {
		StreamFormats* formater;
		AVPacket *packet;
		int newPacketLen;
		int destLen;

		if (mTracks[currentTrack].mPacketQueue.size() == 0) {
			len = 0;
			return 0;
		}

		packet = mTracks[currentTrack].mPacketQueue[0];
		mTracks[currentTrack].mPacketQueue.erase(mTracks[currentTrack].mPacketQueue.begin());
		formater = mTracks[currentTrack].mFormat;
		if (packet == NULL) {
			len = 0;
			return 0;
		}

		newPacketLen = formater->getPackageSize(packet->data, packet->size);
		destLen = (len - offset) > newPacketLen ?newPacketLen : (len - offset);
		formater->formatPackage((uint8_t*)packet->data, packet->size,
						(uint8_t*)(buf + offset), destLen);
		len = destLen;
        if ((packet->stream_index == videoStreamIndex) ||
        		(packet->stream_index == audioStreamIndex) ||
        		(packet->stream_index == subtitleStreamIndex)) {
            const bool isKeyFrame = (packet->flags & AV_PKT_FLAG_KEY) != 0;
            AVStream *st = mFormatContext->streams[packet->stream_index];
            const int64_t ptsFromFFmpeg =
                    (packet->pts == static_cast<int64_t>(AV_NOPTS_VALUE))
                    ? kUnknownPTS : convertStreamTimeToUs(st, packet->pts);
            const int64_t dtsFromFFmpeg =
                    (packet->dts == static_cast<int64_t>(AV_NOPTS_VALUE))
                    ? kUnknownPTS : convertStreamTimeToUs(st, packet->dts);
            const int64_t predictedPTSInUs = mPTSPopulator->computePTS(
            		packet->stream_index, ptsFromFFmpeg, dtsFromFFmpeg, isKeyFrame);
            const int64_t normalizedPTSInUs = (predictedPTSInUs == kUnknownPTS)?
                    dtsFromFFmpeg - mStartTimeUs : ((predictedPTSInUs - mStartTimeUs < 0
                    && predictedPTSInUs - mStartTimeUs > -10) ? 0 : predictedPTSInUs - mStartTimeUs); // starttime may exceed pts a little in some ugly streams.

			mSampleTimeUs = normalizedPTSInUs;
        }
        av_free_packet(packet);
        delete packet;
	} else {
		uint8_t* extra = mTracks[0].mFormat->mcodecMeta;
		int extraLen = mTracks[0].mFormat->mCodecMetaSize;
		if (extraLen > 0) {
			int destLen = (len - offset) >extraLen ?
					extraLen : (len - offset);
			memcpy((buf + offset), extra, destLen);
			len = destLen;
		} else {
			len = 0;
		}
		mTracks[currentTrack].mExtraPushed = 1;
	}
    return 0;
}

status_t FFDemux::getSampleTrackIndex(size_t *trackIndex)
{
	while (currentTrack < 0)
		advance();
    *trackIndex = currentTrack;
    return 0;
}

status_t FFDemux::getSampleTime(int64_t *sampleTimeUs)
{
    *sampleTimeUs = mSampleTimeUs;
    return 0;
}

bool FFDemux::getCachedDuration(int64_t *durationUs, bool *eos)
{
	Mutex::Autolock autoLock(mLock);
    return true;
}

}
