LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

APP_BUILD_SCRIPT= Application.mk
LOCAL_MODULE    := ExoPlayerLibJNI
LOCAL_SRC_FILES := ExoPlayerLibJNI.cpp \
			FileSource.cpp \
			FFIO.cpp \
			FFDemux.cpp \
			DataSource.cpp \
			PTSPopulator.cpp \
			mediainfo_def.cpp \
			utils/utils.cpp \
			formats/StreamFormats.cpp \
			formats/AVCFormat.cpp


PRIVATE_LIBDIR=$(LOCAL_PATH)/android-libs

PRIVATE_INCLUDE=$(LOCAL_PATH)/android-headers-kk

LOCAL_CFLAGS += -I$(LOCAL_PATH)/dist/include \
			-I$(LOCAL_PATH)/utils  \
			-D__STDC_FORMAT_MACROS \
			-D__STDC_CONSTANT_MACROS \
			-DHAVE_PTHREADS \
			-Wno-multichar \
			-I$(PRIVATE_INCLUDE)/system/core/include/ \
			-I$(PRIVATE_INCLUDE)/hardware/libhardware/include/ \
			-I$(PRIVATE_INCLUDE)/frameworks/native/include/ \
			-I$(PRIVATE_INCLUDE)/frameworks/av/include/ \


LOCAL_LDLIBS := $(LOCAL_PATH)/dist/lib/libavformat-56.so \
			 $(LOCAL_PATH)/dist/lib/libavutil-54.so \
			 $(LOCAL_PATH)/dist/lib/libavcodec-56.so \
			 -landroid -lc -lm -ldl -llog -lEGL -lGLESv2 \
			 -L$(PRIVATE_LIBDIR) \
			 -lgcc -lstagefright -lmedia -lutils -lbinder -lcutils -lui

MYJNI=$(APP_PROJECT_PATH)/jni
MYLIB=$(APP_PROJECT_PATH)/libs/$(TARGET_ARCH_ABI)

include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := avformat-56
LOCAL_SRC_FILES := dist/lib/libavformat-56.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avutil-54
LOCAL_SRC_FILES := dist/lib/libavutil-54.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avcodec-56
LOCAL_SRC_FILES := dist/lib/libavcodec-56.so
include $(PREBUILT_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := avfilter-5
#LOCAL_SRC_FILES := dist/lib/libavfilter-5.so
#include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := rtmp-1
LOCAL_SRC_FILES := dist/lib/librtmp-1.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swresample-1
LOCAL_SRC_FILES := dist/lib/libswresample-1.so
include $(PREBUILT_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := avdevice-56
#LOCAL_SRC_FILES := dist/lib/libavdevice-56.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := crypto.so
#LOCAL_SRC_FILES := dist/lib/libcrypto.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := ssl
#LOCAL_SRC_FILES := dist/lib/libssl.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := swscale-3
#LOCAL_SRC_FILES := dist/lib/libswscale-3.so
#include $(PREBUILT_SHARED_LIBRARY)

