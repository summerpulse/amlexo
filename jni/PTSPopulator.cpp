/*
 * PTSPopulator.cpp
 *
 *  Created on: Sep 18, 2014
 *      Author: amlogic
 */
#define LOG_TAG __FILE__
#include <PTSPopulator.h>
#include <log/log.h>
#include <stdint.h>

namespace android {

const int64_t kUnknownPTS = (__INT64_C(-9223372036854775807)-1);

PTSPopulator::PTSPopulator(uint32_t streamsCount) {
    mStreamsCount = streamsCount;
    mIsPTSReliable = new bool[mStreamsCount];
    mLastPTS = new int64_t[mStreamsCount];
    for (unsigned int i = 0; i < mStreamsCount; i++) {
        mIsPTSReliable[i] = true;
        mLastPTS[i] = kUnknownPTS;
    }
}

PTSPopulator::~PTSPopulator() {
    mStreamsCount = 0;
    delete [] mIsPTSReliable;
    delete [] mLastPTS;
}

int64_t PTSPopulator::computePTS(uint32_t streamIndex,
        const int64_t pts, const int64_t dts, const bool isKeyFrame) {

    Mutex::Autolock autoLock(mLock);

    if ((mIsPTSReliable[streamIndex])
            && (pts != kUnknownPTS)
            && (mLastPTS[streamIndex] != kUnknownPTS)
            && (mLastPTS[streamIndex] == pts)) {
        mIsPTSReliable[streamIndex] = false;
        mLastPTS[streamIndex] = kUnknownPTS;
    }

    if (mIsPTSReliable[streamIndex]) {
        mLastPTS[streamIndex] = pts;
        return pts;
    } else {
        return kUnknownPTS;
    }
}

void PTSPopulator::reset() {

    Mutex::Autolock autoLock(mLock);

    for (unsigned int i = 0; i < mStreamsCount; i++) {
        mLastPTS[i] = kUnknownPTS;
    }
}

}
