/*
 * FileSource.cpp
 *
 *  Created on: Sep 12, 2014
 *      Author: amlogic
 */
#define LOG_TAG __FILE__
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log/log.h>

#include <FileSource.h>
#include <assert.h>
#include <stdint.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avio.h>
#undef CodecType
}


namespace android {

FileSource::FileSource(const char *filename)
        : mFd(-1),
        mOffset(0),
        mLength(-1)
{
    mFd = open(filename, O_LARGEFILE | O_RDONLY);

    if (mFd >= 0) {
        mLength = lseek64(mFd, 0, SEEK_END);
    } else {
        ALOGE("Failed to open file '%s'. (%s)", filename, strerror(errno));
    }
}

FileSource::FileSource(int fd, int64_t offset, int64_t length)
        : mFd(fd),
        mOffset(offset),
        mLength(length)
{
    if (mOffset <= 0)
        ALOGE(" invalid offset:%lld\n", mOffset);
    if (length <= 0)
        ALOGE(" invalid length:%lld\n", length);
    if (length < mOffset)
        ALOGE(" length should greater than offset.\n");
}

FileSource::~FileSource()
{
    if (mFd >= 0) {
        close(mFd);
        mFd = -1;
    }
}

status_t FileSource::initCheck() const
{
    return mFd >= 0 ? OK : NO_INIT;
}

ssize_t FileSource::readAt(off64_t offset, void *data, size_t size)
{
    if (mFd < 0) {
        return NO_INIT;
    }

    Mutex::Autolock autoLock(mLock);

    if (mLength >= 0) {
        if (offset >= mLength) {
            return 0;  // read beyond EOF.
        }
        int64_t numAvailable = mLength - offset;
        if ((int64_t)size > numAvailable) {
            size = numAvailable;
        }
    }

    off64_t result = lseek64(mFd, offset + mOffset, SEEK_SET);
    if (result == -1) {
        ALOGE("seek to %lld failed", offset + mOffset);
        return UNKNOWN_ERROR;
    }

    return ::read(mFd, data, size);
}

int FileSource::getSize(off64_t *size)
{
    Mutex::Autolock autoLock(mLock);

    if (mFd < 0) {
        return NO_INIT;
    }

    *size = mLength;

    return OK;
}

}
