/*
 * PTSPopulator.h
 *
 *  Created on: Sep 18, 2014
 *      Author: amlogic
 */

#ifndef PTSPOPULATOR_H_
#define PTSPOPULATOR_H_

#include <stdlib.h>
#include <utils/Mutex.h>
#include <utils/Errors.h>

extern const int64_t kUnknownPTS;

namespace android {

class PTSPopulator {
public:
	PTSPopulator(uint32_t streamsCount);
	virtual ~PTSPopulator();
    virtual int64_t computePTS(uint32_t streamIndex,
            const int64_t pts, const int64_t dts, const bool isKeyFrame);
    virtual void reset();
private:
    Mutex mLock;
    uint32_t mStreamsCount;
    bool *mIsPTSReliable;
    int64_t *mLastPTS;
};

}

#endif /* PTSPOPULATOR_H_ */
