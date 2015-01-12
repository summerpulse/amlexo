/*
 * FileSource.h
 *
 *  Created on: Sep 12, 2014
 *      Author: amlogic
 */

#ifndef FILESOURCE_H_
#define FILESOURCE_H_

#include <stdio.h>
#include <utils/Errors.h>
#include <utils/Mutex.h>
#include "DataSource.h"

namespace android {

class FileSource :public DataSource {
public:
    FileSource(const char *filename);
    FileSource(int fd, int64_t offset, int64_t length);
    virtual ~FileSource();

    virtual status_t initCheck()const;

    virtual ssize_t readAt(off64_t offset, void *data, size_t size);

    virtual int getSize(off64_t *size);

private:
    int mFd;
    int64_t mOffset;
    int64_t mLength;
    Mutex mLock;
};

};
#endif /* FILESOURCE_H_ */
