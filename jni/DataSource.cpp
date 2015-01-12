/*
 * DataSource.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: amlogic
 */

#include "DataSource.h"
#include <utils/log2.h>
#include <sys/stat.h>
#include <utils/Errors.h>

namespace android {

DataSource::DataSource() {
	// TODO Auto-generated constructor stub

}

DataSource::~DataSource() {
	// TODO Auto-generated destructor stub
}


bool DataSource::getUInt16(off64_t offset, uint16_t *x) {
    *x = 0;

    uint8_t byte[2];
    if (readAt(offset, byte, 2) != 2) {
        return false;
    }

    *x = (byte[0] << 8) | byte[1];

    return true;
}

bool DataSource::getUInt24(off64_t offset, uint32_t *x) {
    *x = 0;

    uint8_t byte[3];
    if (readAt(offset, byte, 3) != 3) {
        return false;
    }

    *x = (byte[0] << 16) | (byte[1] << 8) | byte[2];

    return true;
}

bool DataSource::getUInt32(off64_t offset, uint32_t *x) {
    *x = 0;

    uint32_t tmp;
    if (readAt(offset, &tmp, 4) != 4) {
        return false;
    }

    *x = ntohl(tmp);

    return true;
}

bool DataSource::getUInt64(off64_t offset, uint64_t *x) {
    *x = 0;

    uint64_t tmp;
    if (readAt(offset, &tmp, 8) != 8) {
        return false;
    }

    *x = tmp;

    return true;
}

status_t DataSource::getSize(off64_t *size) {
    *size = 0;

    return UNKNOWN_ERROR;
}

}
