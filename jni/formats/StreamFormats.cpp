/*
 * StreamFormats.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: amlogic
 */
#include "formats/StreamFormats.h"
#include <utils/log2.h>
#include <stdlib.h>

namespace android {

StreamFormats::StreamFormats()
	:mcodecMeta(NULL),
	 mCodecMetaSize(0)

{
	// TODO Auto-generated constructor stub

}

StreamFormats::~StreamFormats()
{
	// TODO Auto-generated destructor stub
	if (mcodecMeta)
		delete mcodecMeta;
	mcodecMeta = NULL;
}

int StreamFormats::formatCodecMeta(AVCodecContext  *pCodecCtx)
{
	if (pCodecCtx->extradata == NULL)
		return BAD_VALUE;
	mcodecMeta = new uint8_t[pCodecCtx->extradata_size];
	if (mcodecMeta == NULL)
		return BAD_VALUE;

	mCodecMetaSize = pCodecCtx->extradata_size;
	memcpy(mcodecMeta, pCodecCtx->extradata, pCodecCtx->extradata_size);
	return OK;
}

int StreamFormats::getPackageSize(uint8_t* in, int inSize)
{
	return inSize;
}

int StreamFormats::formatPackage(uint8_t* in, int inSize,
		uint8_t* out, int outSize)
{
	memcpy(out, in, inSize);
	return OK;
}

} /* namespace android */
