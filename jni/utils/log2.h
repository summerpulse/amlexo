/*
 * log2.h
 *
 *  Created on: Sep 18, 2014
 *      Author: amlogic
 */

#ifndef LOG2_H_
#define LOG2_H_


#include <android/log.h>

#define TAG "ExoPlayerLibJNI"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LFLG() LOGW("  %s:%d \n", __FUNCTION__, __LINE__);

#endif /* LOG2_H_ */
