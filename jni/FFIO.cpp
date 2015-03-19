/*
 * FFIO.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: amlogic
 */

#include <FFIO.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avio.h>
}

#include "utils/log2.h"
#define TAG "FFIO"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)


static uint32_t kAdapterBufferSize = 32768;

namespace android {

FFIO::FFIO()
    : mInitCheck(false),
      mContext(NULL),
      mNextReadPos(0),
      mSource(NULL)
{
    
}

FFIO::~FFIO()
{
    if (mInitCheck && NULL != mContext->buffer) {
        av_free(mContext->buffer);
    }
    av_free(mContext);
}

bool FFIO::init(DataSource* src)
{
    if (NULL == src) {
        LOGE("Input source should not be NULL.");
        return false;
    }

    // Already inited?  If so, this is an error.
    if (mInitCheck) {
        LOGW("Adapter is already initialized.");
        return false;
    }

    uint32_t targetSize = kAdapterBufferSize + FF_INPUT_BUFFER_PADDING_SIZE;
    // Need to use ffmpeg's buffer allocation/free routines for this memory - it
    // may re-allocate the buffer during play.
    uint8_t *buffer = static_cast<uint8_t *>(av_mallocz(targetSize));
    if (NULL != buffer) {
        mContext = avio_alloc_context(
                buffer,
                static_cast<int>(kAdapterBufferSize),
                0,  // write_flag = false.
                static_cast<void *>(this),
                staticRead,
                staticWrite,
                staticSeek);
        if (mContext != NULL) {
            mSource = src;
            mInitCheck = true;
        } else {
            LOGE("Failed to initialize AVIOContext.");
        }
    } else {
        LOGE("Failed to allocate %u bytes for ByteIOAdapter.", targetSize);
    }

    if (!mInitCheck) {
        av_free(buffer);
        buffer = NULL;
        mSource = NULL;
    }

    return mInitCheck;
}


int32_t FFIO::read(uint8_t* buf, int read_size)
{
    if (!mInitCheck || NULL == buf) {
        return -1;
    }
    if (!read_size) {
        return 0;
    }

    if (NULL == mSource) {
        LOGE("mSource == NULL?\n");
        return -1;
    }

    ssize_t result = 0;
    uint64_t pos = mNextReadPos;

    result = mSource->readAt(pos, buf, read_size);
    if (result > 0) {
        mNextReadPos += result;
    }

    return static_cast<int>(result);
}

int64_t FFIO::seek(int64_t offset, int whence)
{
    if (!mInitCheck) {
        return -1;
    }

    int64_t target = -1;
    int64_t size = 0;
    bool sizeSupported = (OK == mSource->getSize(&size));

    switch(whence) {
    case SEEK_SET:
        target = offset;
        break;
    case SEEK_CUR:
        target = mNextReadPos + offset;
        break;
    case SEEK_END:
        if (sizeSupported) {
            target = size + offset;
        }
        break;
    case AVSEEK_SIZE:
        return size;
    default:
        LOGE("Invalid seek whence (%d) in ByteIOAdapter::Seek", whence);
    }

    if ((target < 0) || (target > size)) {
        LOGW("Invalid seek request to %lld (size: %lld).", target, size);
        return -1;
    }

    mNextReadPos = target;

    return target;
}

int32_t FFIO::staticRead(void* thiz, uint8_t* buf, int read_size)
{
    if (thiz == NULL) {
        LOGE("thiz == NULL?\n");
        return -1;
    }
    return static_cast<FFIO *>(thiz)->read(buf, read_size);
}

int32_t FFIO::staticWrite(void* thiz, uint8_t* buf, int write_size)
{
    LOGE("Why do you want to write?");
    return -1;
}

int64_t FFIO::staticSeek(void* thiz, int64_t offset, int whence)
{
    if (thiz == NULL) {
        LOGE("thiz == NULL?");
        return -1;
    }
    return static_cast<FFIO *>(thiz)->seek(offset, whence);
}

}
