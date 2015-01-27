/*
 * AVCFormat.h
 *
 *  Created on: Jan 19, 2015
 *      Author: amlogic
 */

#ifndef AVCFORMAT_H_
#define AVCFORMAT_H_
#include "StreamFormats.h"
namespace android {

class AVCFormat: public StreamFormats {
public:
	AVCFormat();
	virtual ~AVCFormat();

	virtual int formatCodecMeta(AVCodecContext  *pCodecCtx);
	virtual int getPackageSize(uint8_t* in, int inSize);
	virtual int formatPackage(uint8_t* in, int inSize,
			uint8_t* out, int outSize);

private:
	int mAVCCFound;
	int mAVCFormatted;
	size_t mNALLengthSize;
	size_t parseNALSize(const uint8_t *data) const;
};

} /* namespace android */

#endif /* AVCFORMAT_H_ */
