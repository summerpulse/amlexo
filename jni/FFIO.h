/*
 * FFIO.h
 *
 *  Created on: Sep 13, 2014
 *      Author: amlogic
 */

#ifndef FFIO_H_
#define FFIO_H_

extern "C"
{
#include <stdint.h>
#include <libavformat/avio.h>
}

#include "DataSource.h"
namespace android {

class FFIO {
public:
    FFIO();
    ~FFIO();

    bool init(DataSource* src);
    AVIOContext* getContext() { return mInitCheck ? mContext : NULL; }

private:
    bool mInitCheck;
    AVIOContext* mContext;
    DataSource* mSource;

    int64_t mNextReadPos;
    int32_t mWakeupHandle;

    int32_t read(uint8_t* buf, int read_size);
    int64_t seek(int64_t offset, int whence);

    static int32_t staticRead(void* thiz, uint8_t* buf, int read_size);
    static int32_t staticWrite(void* thiz, uint8_t* buf, int write_size);
    static int64_t staticSeek(void* thiz, int64_t offset, int whence);
};

};
#endif /* FFIO_H_ */
