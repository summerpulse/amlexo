/*
 * StreamFormats.h
 *
 *  Created on: Jan 19, 2015
 *      Author: amlogic
 */

#ifndef STREAMFORMATS_H_
#define STREAMFORMATS_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/Errors.h>
extern "C"
{
#include <stdint.h>
#include <libavformat/version.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
}

namespace android {

class StreamFormats {
public:
	StreamFormats();
	virtual ~StreamFormats();
	virtual int formatCodecMeta(AVCodecContext  *pCodecCtx);
	virtual int getPackageSize(uint8_t* in, int inSize);
	virtual int formatPackage(uint8_t* in, int inSize,
			uint8_t* out, int outSize);

	int mCodecMetaSize;
	uint8_t* mcodecMeta;
};

} /* namespace android */

#endif /* STREAMFORMATS_H_ */
