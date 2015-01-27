/*
 * AVCFormat.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: amlogic
 */

#include <utils/log2.h>
#include <formats/AVCFormat.h>
#include <utils/utils.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/MediaErrors.h>

const int HDR_BUF_SIZE=1024;

namespace android {

AVCFormat::AVCFormat():
		mAVCCFound(0),
		mNALLengthSize(0),
		mAVCFormatted(0)
{
	// TODO Auto-generated constructor stub
	StreamFormats();
}

AVCFormat::~AVCFormat()
{
	// TODO Auto-generated destructor stub
}

int check_size_in_buffer(unsigned char *p, int len)
{
    unsigned int size;
    unsigned char *q = p;
    while ((q + 4) < (p + len)) {
        size = (*q << 24) | (*(q + 1) << 16) | (*(q + 2) << 8) | (*(q + 3));
        if (size & 0xff000000) {
            return 0;
        }

        if (q + size + 4 == p + len) {
            return 1;
        }

        q += size + 4;
    }
    return 0;
}

int check_size_in_buffer3(unsigned char *p, int len)
{
    unsigned int size;
    unsigned char *q = p;
    while ((q + 3) < (p + len)) {
        size = (*q << 16) | (*(q + 1) << 8) | (*(q + 2));

        if (q + size + 3 == p + len) {
            return 1;
        }

        q += size + 3;
    }
    return 0;
}

int check_size_in_buffer2(unsigned char *p, int len)
{
    unsigned int size;
    unsigned char *q = p;
    while ((q + 2) < (p + len)) {
        size = (*q << 8) | (*(q + 1));

        if (q + size + 2 == p + len) {
            return 1;
        }

        q += size + 2;
    }
    return 0;
}

int AVCFormat::formatCodecMeta(AVCodecContext  *pCodecCtx)
{
	uint8_t* in = pCodecCtx->extradata;
	int inSize = pCodecCtx->extradata_size;
    char nal_start_code[] = {0x0, 0x0, 0x0, 0x1};
    int nalsize;
    unsigned char* p = in;
    int tmpi;
    int header_len = 0;

	if (in == NULL || inSize < 10 || inSize > HDR_BUF_SIZE)
		return BAD_VALUE;

	mcodecMeta = new uint8_t[HDR_BUF_SIZE];
	if (mcodecMeta == NULL)
		return BAD_VALUE;

	if (in[0] != 1u) {
        LOGE("We only support configurationVersion 1, but this is %u.", in[0]);
        return BAD_VALUE;
	}

	if ((p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1)||
							(p[0] == 0 && p[1] == 0 && p[2] == 1 )) {
		mCodecMetaSize = inSize;
		memcpy(mcodecMeta, in, inSize);
		mAVCFormatted = 1;
	} else {
		int cnt = *(p + 5) & 0x1f; //number of sps
		// printf("number of sps :%d\n", cnt);
		p += 6;
		for (tmpi = 0; tmpi < cnt; tmpi++) {
			nalsize = (*p << 8) | (*(p + 1));
			memcpy(&(mcodecMeta[header_len]), nal_start_code, 4);
			header_len += 4;
			memcpy(&(mcodecMeta[header_len]), p + 2, nalsize);
			header_len += nalsize;
			p += (nalsize + 2);
		}

		cnt = *(p++); //Number of pps
		// printf("number of pps :%d\n", cnt);
		for (tmpi = 0; tmpi < cnt; tmpi++) {
			nalsize = (*p << 8) | (*(p + 1));
			memcpy(&(mcodecMeta[header_len]), nal_start_code, 4);
			header_len += 4;
			memcpy(&(mcodecMeta[header_len]), p + 2, nalsize);
			header_len += nalsize;
			p += (nalsize + 2);
		}
		mCodecMetaSize = header_len;
		if (header_len >= HDR_BUF_SIZE) {
			LOGE("header_len %d is larger than max length\n", header_len);
			return BAD_VALUE;
		}
	}
	mAVCCFound = 1;

	return OK;
}

int AVCFormat::getPackageSize(uint8_t* in, int inSize)
{
    int nalsize, size = inSize;
    unsigned char *src = in;

    if (mAVCFormatted)
    	return inSize;

    if (!mAVCCFound) {
        LOGE("Can not compute new payload length, have not found an AVCC "
                "header yet.");
        return 0;
    }

    if (check_size_in_buffer2(in, inSize)) {
    		int new_len = 0;

    		while ((src + 2) < (in + inSize)) {
    			nalsize = (*src << 8) | (*(src + 1));
    			src += (nalsize + 2);
    			new_len += nalsize + 4;
    		}
    		return new_len;
    }

    return inSize;
}

int AVCFormat::formatPackage(uint8_t* in, int inSize,
		uint8_t* out, int outSize)
{
    int nalsize, size = inSize;
    unsigned char *src = in;
    unsigned char *dest = out;

    if (!mAVCCFound) {
        LOGE("AVCC header has not been set.");
        return -1;
    }

    if (mAVCFormatted) {
    	memcpy(out, in, inSize);
    }

	if (check_size_in_buffer(in, size)) {
		while ((dest + 4) < (out + size)) {
			nalsize = (*src << 24) | (*(src + 1) << 16) | (*(src + 2) << 8) | (*(src + 3));
			*dest = 0;
			*(dest + 1) = 0;
			*(dest + 2) = 0;
			*(dest + 3) = 1;
			dest = dest + 4;
			src = src + 4;
			memcpy(dest, src, nalsize);
			dest += nalsize;
			src += nalsize;
		}
	} else if (check_size_in_buffer3(src, size)) {
		while ((dest + 3) < (out + size)) {
			nalsize = (*src << 16) | (*(src + 1) << 8) | (*(src + 2));
			*dest = 0;
			*(dest + 1) = 0;
			*(dest + 2) = 1;
			dest = dest + 3;
			src = src + 3;
			memcpy(dest, src, nalsize);
			dest += nalsize;
			src += nalsize;
		}
	} else if (check_size_in_buffer2(src, size)) {
		int new_len = 0;

		while ((src + 2) < (in + size)) {
			nalsize = (*src << 8) | (*(src + 1));
			*(dest + new_len) = 0;
			*(dest + new_len + 1) = 0;
			*(dest + new_len + 2) = 0;
			*(dest + new_len + 3) = 1;
			memcpy(dest + new_len + 4, src + 2, nalsize);
			src += (nalsize + 2);
			new_len += nalsize + 4;
		}
		return new_len;
	}
	return inSize;
}

size_t AVCFormat::parseNALSize(const uint8_t *data) const {
    switch (mNALLengthSize) {
        case 1:
            return *data;
        case 2:
            return U16_AT(data);
        case 3:
            return ((size_t)data[0] << 16) | U16_AT(&data[1]);
        case 4:
            return U32_AT(data);
    }

    // This cannot happen, mNALLengthSize springs to life by adding 1 to
    // a 2-bit integer.
    //CHECK(!"Invalid NAL length size.");

    return 0;
}

} /* namespace android */
