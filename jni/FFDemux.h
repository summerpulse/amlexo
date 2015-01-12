/*
 * FFDemux.h
 *
 *  Created on: Sep 13, 2014
 *      Author: amlogic
 */

#ifndef FFDEMUX_H_
#define FFDEMUX_H_

extern "C"
{
#include <stdint.h>
#include <libavformat/version.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
}

#include <vector>
#include <string>
#include "FileSource.h"
#include "FFIO.h"

#include <utils/Errors.h>
#include <utils/String8.h>
#include <utils/List.h>

namespace android {

class FFDemux {
public:
    enum TrackFlags {
        kIsVorbis       = 1,
    };
    class MediaMimeItem {
    public:
        enum MediaItemType {
            MD_U32,
            MD_U64,
            MD_STRING
        };
        enum MediaItemType type;
        inline int32_t get32() {return data.val32;};
        inline int32_t get64() {return data.val64;};
        inline void set32(int32_t val) {data.val32 = val;};
        inline void set64(int32_t val) {data.val64 = val;};
        char const* getstr();
        int setstr(const char* val);
        ~MediaMimeItem();
    private:
        union {
            int32_t val32;
            int64_t val64;
        } data;
        std::string str;
    };

    struct VideoParam {
        int width;
        int height;
    };
    struct AudioParam {
        int sampleRate;
        int channels;
    };
    struct SubtitleParam {
        std::string lang;
    };
    struct TrackInfo {
        int mTrackIndex;
        int mTrackType; /* 0 video. 1 audio. 2 subtitle. */
        int mSeleted;
        uint32_t mTrackFlags;  // bitmask of "TrackFlags"
        std::string mime;
        struct VideoParam video;
        struct AudioParam audio;
        struct SubtitleParam subtitle;
    };

    FFDemux();
    virtual ~FFDemux();

    status_t setDataSource(const char *path);
    status_t setDataSource(int fd, off64_t offset, off64_t size);
    status_t setDataSource(DataSource* datasource);

    size_t countTracks();
    status_t getTrackFormat(size_t index, TrackInfo** trackinfo);

    status_t selectTrack(size_t index);
    status_t unselectTrack(size_t index);

    status_t seekTo(int64_t timeUs, int mode);

    status_t advance();
    status_t readSampleData(char* buf, size_t& len, int offset);
    status_t getSampleTrackIndex(size_t *trackIndex);
    status_t getSampleTime(int64_t *sampleTimeUs);
    bool getCachedDuration(int64_t *durationUs, bool *eos);

protected:
    DataSource* src;
    FFIO ffio;
    status_t post_init();

private:
    std::vector<TrackInfo> mTracks;

    AVFormatContext *mFormatContext;
    AVPacket mPkt;
    int currentTrack;
    int videoIndex;
    int audioIndex;
    int subtitleIndex;
    int64_t mSampleTimeUs;

    AVFormatContext* openAVFormatContext(FFIO *fio);
    Mutex mLock;
    int mStatus;
};

}
#endif /* FFDEMUX_H_ */
