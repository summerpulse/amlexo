#include <assert.h>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <utils/log2.h>

#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/MediaErrors.h>

#include "FFDemux.h"

using namespace android;

class JavaDataSourceBridge : public DataSource {
    jmethodID mReadMethod;
    jmethodID mGetSizeMethod;
    jmethodID mCloseMethod;
    jobject   mDataSource;
    JNIEnv *mEnv;
 public:
    JavaDataSourceBridge(JNIEnv *env, jobject source) {
        mDataSource = env->NewGlobalRef(source);

        jclass datasourceclass = env->GetObjectClass(mDataSource);
        if (datasourceclass == NULL)
            LOGE("datasourceclass == NULL\n");

        mReadMethod = env->GetMethodID(datasourceclass, "readAt", "(J[BI)I");
        if (mReadMethod != NULL)
            LOGE("mReadMethod == NULL\n");

        mGetSizeMethod = env->GetMethodID(datasourceclass, "getSize", "()J");
        if (mGetSizeMethod != NULL)
            LOGE("mGetSizeMethod == NULL\n");

        mCloseMethod = env->GetMethodID(datasourceclass, "close", "()V");
        if (mCloseMethod != NULL)
            LOGE("mCloseMethod == NULL\n");
        mEnv = env;
    }

    ~JavaDataSourceBridge() {
        mEnv->CallVoidMethod(mDataSource, mCloseMethod);
        mEnv->DeleteGlobalRef(mDataSource);
    }

    virtual status_t initCheck() const {
        return OK;
    }

    virtual ssize_t readAt(off64_t offset, void* buffer, size_t size) {
        // XXX could optimize this by reusing the same array
        jbyteArray byteArrayObj = mEnv->NewByteArray(size);
        mEnv->DeleteLocalRef(mEnv->GetObjectClass(mDataSource));
        mEnv->DeleteLocalRef(mEnv->GetObjectClass(byteArrayObj));
        ssize_t numread = mEnv->CallIntMethod(mDataSource, mReadMethod, offset, byteArrayObj, size);
        mEnv->GetByteArrayRegion(byteArrayObj, 0, size, (jbyte*) buffer);
        mEnv->DeleteLocalRef(byteArrayObj);
        if (mEnv->ExceptionCheck()) {
            LOGW("Exception occurred while reading %d at %lld", size, offset);
            mEnv->ExceptionClear();
            return -1;
        }
        return numread;
    }

    virtual status_t getSize(off64_t *size) {
        if(size == NULL)
            return BAD_VALUE;

        int64_t len = mEnv->CallLongMethod(mDataSource, mGetSizeMethod);
        if (len < 0) {
            *size = UNKNOWN_ERROR;
        } else {
            *size = len;
        }
        return OK;
    }
};

int jniGetFDFromFileDescriptor(JNIEnv* env, jobject fileDescriptor) {
    jclass clazz = env->FindClass("java/io/FileDescriptor");
    if (clazz == NULL)
        return -1;
    static jfieldID fid = env->GetFieldID(clazz, "descriptor", "I");
    return env->GetIntField(fileDescriptor, fid);
}

class JFfmpegExtractor {
public:
    JFfmpegExtractor(JNIEnv *env, jobject thiz);
    void JFfmpegRlease(JNIEnv *env);

    void setDataSource (jobject source);
    void setDataSource (char* path);
    void setDataSource (int fd, jlong offset, jlong length);

    int getTrackCount ();
    status_t getTrackFormatHash (jint index, jobject* format);
    status_t getTrackFormat (JNIEnv *env, jint index, jobject* format);
    jobject selectTrack (jint index);
    void unselectTrack (jint index);
    void seekTo (jlong timeUs, jint mode);
    jboolean advance ();
    jint readSampleData (JNIEnv *env, jobject byteBuf, jint offset, size_t *sampleSize);
    int getSampleTrackIndex ();
    jlong getSampleTime ();
    jint getSampleFlags ();
    jlong getCachedDuration ();
    jboolean hasCacheReachedEndOfStream ();

private:
    jobject makeByteBufferObject(JNIEnv *env, const void *data, size_t size);
    jclass mClass;
    jweak mObject;
    FFDemux* mImpl;
};

JFfmpegExtractor::JFfmpegExtractor(JNIEnv *env, jobject thiz)
      :mClass(NULL),
      mObject(NULL)
{
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL)
        return;

    mClass = (jclass)env->NewGlobalRef(clazz);
    mObject = env->NewWeakGlobalRef(thiz);

    mImpl = new FFDemux();
}

void JFfmpegExtractor::JFfmpegRlease(JNIEnv *env)
{
	env->DeleteWeakGlobalRef(mObject);
    mObject = NULL;
    env->DeleteGlobalRef(mClass);
    mClass = NULL;
}

void JFfmpegExtractor::setDataSource(jobject source)
{
    
}

void JFfmpegExtractor::setDataSource (char*  path)
{
    if (mImpl)
        mImpl->setDataSource((char*)path);
}

void JFfmpegExtractor::setDataSource (int fd, jlong offset, jlong length)
{
    if (mImpl)
        mImpl->setDataSource(fd, offset, length);
}

int JFfmpegExtractor::getTrackCount ()
{
    if (mImpl)
        return mImpl->countTracks();
    else
    	return 0;
}

jobject JFfmpegExtractor::makeByteBufferObject(
        JNIEnv *env, const void *data, size_t size) {
    jbyteArray byteArrayObj = env->NewByteArray(size);
    env->SetByteArrayRegion(byteArrayObj, 0, size, (const jbyte *)data);

    jclass clazz = env->FindClass("java/nio/ByteBuffer");
    CHECK(clazz != NULL);

    jmethodID byteBufWrapID =
        env->GetStaticMethodID(
        		clazz, "wrap", "([B)Ljava/nio/ByteBuffer;");
    CHECK(byteBufWrapID != NULL);

    jobject byteBufObj = env->CallStaticObjectMethod(
            clazz, byteBufWrapID, byteArrayObj);

    env->DeleteLocalRef(byteArrayObj); byteArrayObj = NULL;

    return byteBufObj;
}

status_t JFfmpegExtractor::getTrackFormat (JNIEnv *env, jint index, jobject* format)
{
    FFDemux::TrackInfo* pinfo;
    jobject jmediaformat = NULL;
    jstring mime;
    jclass MediaFormatClazz;

    if (mImpl)
        mImpl->getTrackFormat(index, &pinfo);

    MediaFormatClazz = env->FindClass("android/media/MediaFormat");

    if (MediaFormatClazz == NULL) {
        return -EINVAL;
    }

    mime = env->NewStringUTF(pinfo->mime.c_str());
    if (pinfo->mTrackType == 0) {
        jmethodID CreateMFormatID =
        		env->GetStaticMethodID(
                    MediaFormatClazz,
                    "createVideoFormat",
                    "(Ljava/lang/String;II)Landroid/media/MediaFormat;");

        if (CreateMFormatID == NULL)
            return -EINVAL;
        jmediaformat = env->CallStaticObjectMethod(
							MediaFormatClazz, CreateMFormatID, mime,
							pinfo->video.width, pinfo->video.height);
    } else if (pinfo->mTrackType == 1) {
    	jstring name;
    	jobject val;
    	jmethodID SetPropID;
        jmethodID CreateMFormatID =
        		env->GetStaticMethodID(
                    MediaFormatClazz,
                    "createAudioFormat",
                    "(Ljava/lang/String;II)Landroid/media/MediaFormat;");

        if (CreateMFormatID == NULL)
            return -EINVAL;

        jmediaformat = env->CallStaticObjectMethod(
                    MediaFormatClazz, CreateMFormatID, mime,
                    pinfo->audio.sampleRate, pinfo->audio.channels);

        /* set csd */
        SetPropID = env->GetMethodID(MediaFormatClazz,
                    "setByteBuffer", "(Ljava/lang/String;Ljava/nio/ByteBuffer;)V");
        if (SetPropID && pinfo->mExtraData) {
        	LOGE(" pass csd \n");
			name = env->NewStringUTF("csd-0");
			val = makeByteBufferObject(env, pinfo->mExtraData,
					pinfo->mExtraSize);
			env->CallVoidMethod(jmediaformat,SetPropID, name, val);
			if (name) {
				env->DeleteLocalRef(name);
				name = NULL;
			}
			if (val) {
				env->DeleteLocalRef(val);
				val = NULL;
			}
        }
    }
    env->DeleteLocalRef(mime); mime = NULL;

    *format = jmediaformat;
    return 0;
}

jobject JFfmpegExtractor::selectTrack (jint index)
{
    if (mImpl)
        mImpl->selectTrack(index);
    return 0;
}

void JFfmpegExtractor::unselectTrack (jint index)
{
    if (mImpl)
        mImpl->unselectTrack(index);
}

void JFfmpegExtractor::seekTo (jlong timeUs, jint mode)
{
    if (mImpl)
        mImpl->seekTo(timeUs, mode);
}

jboolean JFfmpegExtractor::advance ()
{
    if (mImpl)
        return mImpl->advance();
    else
        return false;
}

jint JFfmpegExtractor::readSampleData (JNIEnv *env, jobject byteBuf, jint offset,  size_t *sampleSize)
{
    status_t err;
    void *dst = env->GetDirectBufferAddress(byteBuf);
    jlong dstSize;
    jbyteArray byteArray = NULL;
    size_t size = 0;

    if (dst == NULL) {
        jclass byteBufClass = env->FindClass("java/nio/ByteBuffer");
        CHECK(byteBufClass != NULL);

        jmethodID arrayID =
            env->GetMethodID(byteBufClass, "array", "()[B");
        CHECK(arrayID != NULL);

        byteArray =
            (jbyteArray)env->CallObjectMethod(byteBuf, arrayID);

        if (byteArray == NULL) {
            return INVALID_OPERATION;
        }

        jboolean isCopy;
        dst = env->GetByteArrayElements(byteArray, &isCopy);

        dstSize = env->GetArrayLength(byteArray);
    } else {
        dstSize = env->GetDirectBufferCapacity(byteBuf);
    }

    if (dstSize < offset) {
        if (byteArray != NULL) {
            env->ReleaseByteArrayElements(byteArray, (jbyte *)dst, 0);
        }

        return -ERANGE;
    }

    size = dstSize - offset;

    err = mImpl->readSampleData((char *)dst, size, offset);

    if (byteArray != NULL) {
        env->ReleaseByteArrayElements(byteArray, (jbyte *)dst, 0);
    }

    *sampleSize = size;

    if (err < 0) {
        return err;
    }

    return OK;
}

int JFfmpegExtractor::getSampleTrackIndex ()
{
    size_t trackIndex = -1;
    if (mImpl)
        mImpl->getSampleTrackIndex(&trackIndex);

    return trackIndex;
}

jlong JFfmpegExtractor::getSampleTime ()
{
    int64_t sampleTimeUs = 0;
    if (mImpl)
        return mImpl->getSampleTime(&sampleTimeUs);
    else
        return 0;
}

jint JFfmpegExtractor::getSampleFlags ()
{
    return 0;
}

jlong JFfmpegExtractor::getCachedDuration ()
{
#if 0
    if (mImpl)
        return mImpl->getCachedDuration();
    else
#endif
        return 0;
}

jboolean JFfmpegExtractor::hasCacheReachedEndOfStream ()
{

        return false;
}

static jfieldID gcontext;

static void setMediaExtractor(
        JNIEnv *env, jobject thiz, JFfmpegExtractor* extractor)
{
    JFfmpegExtractor* old;
    
    old = (JFfmpegExtractor *)env->GetIntField(thiz, gcontext);

    if (old != NULL)
        delete old;
    if (extractor)
        env->SetIntField(thiz, gcontext, (int)extractor);

}

static JFfmpegExtractor* getMediaExtractor(JNIEnv *env, jobject thiz) {
    if (gcontext == NULL) {
        LOGE(" uninitialized class.\n");
        return NULL;
    } else
        return (JFfmpegExtractor *)env->GetIntField(thiz, gcontext);
}

int jniThrowException(JNIEnv *env, const char* className, const char* msg) {
    if (env && className && env->ExceptionCheck()) {
        /* TODO: consider creating the new exception with this as "cause" */
        jclass clazz = env->FindClass(className);
        if (clazz == NULL)
            return -1;
        env->ThrowNew(clazz, msg);
    }

    return 0;
}

static void FFmpegExtractor_setDataSourceJDataSource (JNIEnv *env, jobject object,
        jobject source)
{
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    }
    extractor->setDataSource(source);
}

static void FFmpegExtractor_setDataSourceUrl (JNIEnv *env, jobject object,
        jstring path, jbyteArray keys, jbyteArray values)
{
    if (path == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }
    const char* c_path = env->GetStringUTFChars(path, NULL);
    if (c_path == NULL) {
        return;
    }

    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    }
    extractor->setDataSource((char*)c_path);

    env->ReleaseStringUTFChars(path, c_path);
    path = NULL;
}

static void FFmpegExtractor_setDataSourceFd (JNIEnv *env, jobject object,
        jobject fd, jlong offset, jlong length)
{
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    }
    extractor->setDataSource(jniGetFDFromFileDescriptor(env, object),
    		offset, length);
    
}

static void FFmpegExtractor_release (JNIEnv *env, jobject object)
{
    setMediaExtractor(env, object, NULL);
}

static int FFmpegExtractor_getTrackCount (JNIEnv *env, jobject object)
{
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    }
    return extractor->getTrackCount();
}

static jobject FFmpegExtractor_getTrackFormatNative (JNIEnv *env, jobject object, jint index)
{
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    }

    jobject format;
    status_t err = extractor->getTrackFormat(env, index, &format);
    return format;
}

static jobject FFmpegExtractor_selectTrack (JNIEnv *env, jobject object, jint index)
{
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else 
        extractor->selectTrack(index);
    return 0;
}

static void FFmpegExtractor_unselectTrack (JNIEnv *env, jobject object, jint index)
{
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else
        extractor->unselectTrack(index);
}

static void FFmpegExtractor_seekTo (JNIEnv *env, jobject object, jlong timeUs, jint mode)
{
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else
        extractor->seekTo(timeUs, mode);
}

static jboolean FFmpegExtractor_advance (JNIEnv *env, jobject object)
{
    bool ret = false;
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else
         ret = extractor->advance();

    return ret;
}

static jint FFmpegExtractor_readSampleData (JNIEnv *env, jobject object,
        jobject byteBuf, jint offset)
{
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    size_t sampleSize;
    status_t err;

    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }

    err = extractor->readSampleData(env, byteBuf, offset, &sampleSize);

    if (err == ERROR_END_OF_STREAM) {
        return -1;
    } else if (err != OK) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return -1;
    }

    return sampleSize;
}

static jint FFmpegExtractor_getSampleTrackIndex (JNIEnv *env, jobject object)
{
    int ret = -1;
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else
        ret = extractor->getSampleTrackIndex();
    return ret;
}

static jlong FFmpegExtractor_getSampleTime (JNIEnv *env, jobject object)
{
    int64_t ret = -1;
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else
        ret = extractor->getSampleTime();
    return ret;
}

static jint FFmpegExtractor_getSampleFlags (JNIEnv *env, jobject object)
{
    int ret = -1;
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else
        ret = extractor->getSampleFlags();
    return ret;
}

static jlong FFmpegExtractor_getCachedDuration (JNIEnv *env, jobject object)
{
    int64_t ret = -1;
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else
        ret = extractor->getCachedDuration();
    return ret;
}

static jboolean FFmpegExtractor_hasCacheReachedEndOfStream (JNIEnv *env, jobject object)
{
    bool ret = false;
    JFfmpegExtractor* extractor = getMediaExtractor(env, object);
    if (extractor == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else
        ret = extractor->hasCacheReachedEndOfStream();
    return ret;
}

static void FFmpegExtractor_native_init (JNIEnv *env, jobject object)
{
    jclass clazz = env->FindClass("com/google/android/exoplayer/FFExtractor");
    if (clazz == NULL)
        return;

    gcontext = env->GetFieldID(clazz, "mNativeContext", "I");
    if (gcontext == NULL)
        LOGE(" fail to register class FFmpegExtractor...\n");
}

static void FFmpegExtractor_native_setup (JNIEnv *env, jobject object)
{
    JFfmpegExtractor* ffdemux = new JFfmpegExtractor(env, object);
    setMediaExtractor(env, object, ffdemux);
}

static void FFmpegExtractor_native_finalize (JNIEnv *env, jobject object)
{
	JFfmpegExtractor* extractor = getMediaExtractor(env, object);
	extractor->JFfmpegRlease(env);
    FFmpegExtractor_release(env, object);
}

/** basic methods is:
 *      setDataSource
 *      getTrackCount
 *      getTrackFormat
 *      selectTrack
 *      readSampleData
 *      getSampleTrackIndex
 *      getSampleTime
 *      advance
 **/

static JNINativeMethod gMethods[] = {
#if 0
    { "setDataSource_native", "(Lcom/google/android/exoplayer/upstream/DataSource;)V",
        (void *)FFmpegExtractor_setDataSourceJDataSource },
#endif
    { "setDataSource_native", "(Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;)V",
        (void *)FFmpegExtractor_setDataSourceUrl },
    { "setDataSource_native", "(Ljava/io/FileDescriptor;JJ)V", (void *)FFmpegExtractor_setDataSourceFd },
    { "release_native", "()V", (void *)FFmpegExtractor_release },
    { "getTrackCount_native", "()I", (void *)FFmpegExtractor_getTrackCount },
    { "getTrackFormatNative_native", "(I)Landroid/media/MediaFormat;", (void *)FFmpegExtractor_getTrackFormatNative },
    { "selectTrack_native", "(I)V", (void *)FFmpegExtractor_selectTrack },
    { "unselectTrack_native", "(I)V", (void *)FFmpegExtractor_unselectTrack},
    { "seekTo_native", "(JI)V", (void *)FFmpegExtractor_seekTo },
    { "advance_native", "()Z", (void *)FFmpegExtractor_advance },
    { "readSampleData_native", "(Ljava/nio/ByteBuffer;I)I", (void *)FFmpegExtractor_readSampleData },
    { "getSampleTrackIndex_native", "()I", (void *)FFmpegExtractor_getSampleTrackIndex },
    { "getSampleTime_native", "()J", (void *)FFmpegExtractor_getSampleTime },
    { "getSampleFlags_native", "()I", (void *)FFmpegExtractor_getSampleFlags },
    { "getCachedDuration_native", "()J", (void *)FFmpegExtractor_getCachedDuration },
    { "hasCacheReachedEndOfStream_native", "()Z", (void *)FFmpegExtractor_hasCacheReachedEndOfStream },
    { "native_init", "()V", (void *)FFmpegExtractor_native_init },
    { "native_setup", "()V", (void *)FFmpegExtractor_native_setup },
    { "native_finalize", "()V", (void *)FFmpegExtractor_native_finalize }
};

//LOGE("\t\t\t kasin __ %d __\n", __LINE__);
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;
    jclass clazz = NULL;
    jint jversion = JNI_VERSION_1_1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed\n");
    } else {
        jversion = JNI_VERSION_1_4;
        clazz = env->FindClass("com/google/android/exoplayer/FFExtractor");
        if (clazz == NULL) {
            LOGE("ERROR: FindClass com/google/android/exoplayer/FFExtractor.\n");
            return -1;
        }
        result = env->RegisterNatives(clazz,
                    gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
    }

    if (result < 0)
        LOGE("ERROR: Register Native Method fail.\n");
    return jversion;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;
    jclass clazz = NULL;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed\n");
    } else {
        clazz = env->FindClass("com/google/android/exoplayer/FFExtractor");
        if (clazz != NULL)
            env->UnregisterNatives(clazz);
    }
}
