/*
 * DataSource.h
 *
 *  Created on: Sep 16, 2014
 *      Author: amlogic
 */

#ifndef DATASOURCE_H_
#define DATASOURCE_H_

#include <stdio.h>
#include <utils/Errors.h>
#include <utils/Mutex.h>
namespace android {

class DataSource {
public:
	DataSource();
	virtual ~DataSource();

    virtual status_t initCheck() const = 0;

    virtual ssize_t readAt(off64_t offset, void *data, size_t size) = 0;

    // Convenience methods:
    bool getUInt16(off64_t offset, uint16_t *x);
    bool getUInt24(off64_t offset, uint32_t *x); // 3 byte int, returned as a 32-bit int
    bool getUInt32(off64_t offset, uint32_t *x);
    bool getUInt64(off64_t offset, uint64_t *x);

    // May return ERROR_UNSUPPORTED.
    virtual status_t getSize(off64_t *size);
    
};

};
#endif /* DATASOURCE_H_ */
