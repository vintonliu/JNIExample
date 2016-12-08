#include <jni.h>
#include <android/log.h>

#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define DEBUG
#ifdef DEBUG
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "JNIExample", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "JNIExample", __VA_ARGS__))
#else
#define LOGI(...)
#define LOGE(...)
#endif

#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

static JavaVM *gJavaVM;
static jobject gInterfaceObject, gDataObject;
const char *kInterfacePath = "org/wooyd/android/JNIExample/JNIExampleInterface";
const char *kDataPath = "org/wooyd/android/JNIExample/Data";

#ifdef __cplusplus
extern "C" {
#endif

/* video preset parameters */
typedef struct
{
    int32_t width240;   // 240 x 320
    int32_t width360;   // 360 x 480
    int32_t width480;   // 480 x 640
} VideoPresetResUnit;

typedef struct
{
    // video encoder complexity
    VideoPresetResUnit complexity;
    // video min bitrate to jump higher resolutions,
    // -1 means unable to jump next higher resolution
    VideoPresetResUnit bitrate;
    // video max framerate for difference resolutions
    VideoPresetResUnit framerate;
} VideoPresetCpuUnit;

/* Video preset on CPU performance */
typedef struct
{
    VideoPresetCpuUnit low; // video preset for low cpu performance;
    VideoPresetCpuUnit medium; // video preset for medium cpu performance;
    VideoPresetCpuUnit high; // video preset for high cpu performance;
} VideoPresetAdapter;

static VideoPresetAdapter defaultVideoPreset =
{
    // low CPU performance
    {
            // 240 // 360  // 480
            { 0,    0,      -1 }, // complexity
            { 150, -1,      -1 }, // bitrate
            { 8,   10,     -1 }  // framerate
    },
    // medium CPU performance
    {
            // 240  // 360   // 480
            { 1,    1,      -1 }, // complexity
            { 200,  -1,    -1 }, // bitrate
            { 10,   12,     -1 }  // framerate
    },
    // high CPU performance
    {
            // 240 // 360  // 480
            { 5,    5,      -1 }, // complexity
            { 200,  -1,    -1 }, // bitrate
            { 12,   15,     -1 }  // framerate
    }
};

static void callback_handler(char *s) {
    int status;
    JNIEnv *env;
    bool isAttached = false;

    status = gJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);
    if(status < 0) {
        LOGE("callback_handler: failed to get JNI environment, "
             "assuming native thread");
        status = gJavaVM->AttachCurrentThread(&env, NULL);
        if(status < 0) {
            LOGE("callback_handler: failed to attach "
                 "current thread");
            return;
        }
        isAttached = true;
    }
    /* Construct a Java string */
    jstring js = env->NewStringUTF(s);
    LOGE("[%s %d] debug.", __FUNCTION__, __LINE__);
    jclass interfaceClass = env->GetObjectClass(gInterfaceObject);
    LOGE("[%s %d] debug.", __FUNCTION__, __LINE__);
    if(!interfaceClass) {
        LOGE("callback_handler: failed to get class reference");
        if(isAttached) gJavaVM->DetachCurrentThread();
        return;
    }
    LOGE("[%s %d] debug.", __FUNCTION__, __LINE__);
    /* Find the callBack method ID */
    jmethodID method = env->GetStaticMethodID(
        interfaceClass, "callBack", "(Ljava/lang/String;)V");
    if(!method) {
        LOGE("callback_handler: failed to get method ID");
        if(isAttached) gJavaVM->DetachCurrentThread();
        return;
    }
    LOGE("[%s %d] debug.", __FUNCTION__, __LINE__);
    for (int i = 0; i < 5; i++)
    {
        env->CallStaticVoidMethod(interfaceClass, method, js);
    }
    LOGE("[%s %d] debug.", __FUNCTION__, __LINE__);

    env->DeleteLocalRef(interfaceClass);
    LOGE("[%s %d] debug.", __FUNCTION__, __LINE__);
    env->DeleteLocalRef(js);
    LOGE("[%s %d] debug.", __FUNCTION__, __LINE__);
    if ( isAttached )
        gJavaVM->DetachCurrentThread();

    LOGE("[%s %d] debug.", __FUNCTION__, __LINE__);
}

void *native_thread_start(void *arg) {
//    sleep(1);
    callback_handler((char *) "Called from native thread");

    // need it, else would crash with sigkill(4)
    pthread_exit(0);
}

/*
 * Class:     org_wooyd_android_JNIExample_JNIExampleInterface
 * Method:    callVoid
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_wooyd_android_JNIExample_JNIExampleInterface_callVoid
  (JNIEnv *env, jclass cls) {
    pthread_t native_thread;

//    callback_handler((char *) "Called from Java thread");
    if(pthread_create(&native_thread, NULL, native_thread_start, NULL)) {
        LOGE("callVoid: failed to create a native thread");
    }
    LOGE("pthread id = %d", (int)native_thread);
}

/*
 * Class:     org_wooyd_android_JNIExample_JNIExampleInterface
 * Method:    getNewData
 * Signature: (ILjava/lang/String;)Lorg/wooyd/android/JNIExample/Data;
 */
JNIEXPORT jobject JNICALL Java_org_wooyd_android_JNIExample_JNIExampleInterface_getNewData
  (JNIEnv *env, jclass cls, jint i, jstring s) {
    jclass dataClass = env->GetObjectClass(gDataObject);
    if(!dataClass) {
        LOGE("getNewData: failed to get class reference");
        return NULL;
    }
    jmethodID dataConstructor = env->GetMethodID(
        dataClass, "<init>", "(ILjava/lang/String;)V");
    if(!dataConstructor) {
        LOGE("getNewData: failed to get method ID");
        return NULL;
    }
    jobject dataObject = env->NewObject(dataClass, dataConstructor, i, s);
    if(!dataObject) {
        LOGE("getNewData: failed to create an object");
        return NULL;
    }
    return dataObject;
}

/**
 * Class:     org_wooyd_android_JNIExample_JNIExampleInterface
 * Method:    getPresetData
 * Signature: (ILjava/lang/String;)Lorg/wooyd/android/JNIExample/VideoPresetAdapter
 */
JNIEXPORT jobject JNICALL Java_org_wooyd_android_JNIExample_JNIExampleInterface_getPresetData
        (JNIEnv *env, jclass cls) {
    // Get Java Instance
    jclass presetCls= (env)->FindClass("org/wooyd/android/JNIExample/VideoPresetAdapter");
    jmethodID presetClsInitID = (env)->GetMethodID(presetCls, "<init>", "()V");
    jobject presetObject = (env)->NewObject(presetCls, presetClsInitID);
    // Get member
    jfieldID stLowCpu = (env)->GetFieldID(presetCls, "low",
                                          "Lorg/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetLowCpu;");
    jfieldID stMediumCpu = (env)->GetFieldID(presetCls, "medium",
                                          "Lorg/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetMediumCpu;");
    jfieldID stHighCpu = (env)->GetFieldID(presetCls, "high",
                                          "Lorg/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetHighCpu;");

    // low cpu
    jclass lowCpuCls= (env)->FindClass("org/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetLowCpu");
    jmethodID lowCpuClsInitID = (env)->GetMethodID(lowCpuCls, "<init>",
                                                   "(Lorg/wooyd/android/JNIExample/VideoPresetAdapter;)V");
    jobject lowCpuObject = (env)->NewObject(lowCpuCls, lowCpuClsInitID, presetObject);

    jfieldID lComplex240 = (env)->GetFieldID(lowCpuCls, "complexity_w240", "I");
    jfieldID lComplex360 = (env)->GetFieldID(lowCpuCls, "complexity_w360", "I");
    jfieldID lComplex480 = (env)->GetFieldID(lowCpuCls, "complexity_w480", "I");
    jfieldID lbitrate240 = (env)->GetFieldID(lowCpuCls, "bitrate_w240", "I");
    jfieldID lbitrate360 = (env)->GetFieldID(lowCpuCls, "bitrate_w360", "I");
    jfieldID lbitrate480 = (env)->GetFieldID(lowCpuCls, "bitrate_w480", "I");
    jfieldID lframerate240 = (env)->GetFieldID(lowCpuCls, "framerate_w240", "I");
    jfieldID lframerate360 = (env)->GetFieldID(lowCpuCls, "framerate_w360", "I");
    jfieldID lframerate480 = (env)->GetFieldID(lowCpuCls, "framerate_w480", "I");

    // medium cpu
    jclass mediumCpuCls= (env)->FindClass("org/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetMediumCpu");
    jmethodID mediumCpuClsInitID = (env)->GetMethodID(mediumCpuCls, "<init>",
                                                      "(Lorg/wooyd/android/JNIExample/VideoPresetAdapter;)V");
    jobject mediumCpuObject = (env)->NewObject(mediumCpuCls, mediumCpuClsInitID, presetObject);

    jfieldID mComplex240 = (env)->GetFieldID(mediumCpuCls, "complexity_w240", "I");
    jfieldID mComplex360 = (env)->GetFieldID(mediumCpuCls, "complexity_w360", "I");
    jfieldID mComplex480 = (env)->GetFieldID(mediumCpuCls, "complexity_w480", "I");
    jfieldID mbitrate240 = (env)->GetFieldID(mediumCpuCls, "bitrate_w240", "I");
    jfieldID mbitrate360 = (env)->GetFieldID(mediumCpuCls, "bitrate_w360", "I");
    jfieldID mbitrate480 = (env)->GetFieldID(mediumCpuCls, "bitrate_w480", "I");
    jfieldID mframerate240 = (env)->GetFieldID(mediumCpuCls, "framerate_w240", "I");
    jfieldID mframerate360 = (env)->GetFieldID(mediumCpuCls, "framerate_w360", "I");
    jfieldID mframerate480 = (env)->GetFieldID(mediumCpuCls, "framerate_w480", "I");

    // high cpu
    jclass highCpuCls= (env)->FindClass("org/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetHighCpu");
    jmethodID highCpuClsInitID = (env)->GetMethodID(highCpuCls, "<init>",
                                                    "(Lorg/wooyd/android/JNIExample/VideoPresetAdapter;)V");
    jobject highCpuObject = (env)->NewObject(highCpuCls, highCpuClsInitID, presetObject);

    jfieldID hComplex240 = (env)->GetFieldID(highCpuCls, "complexity_w240", "I");
    jfieldID hComplex360 = (env)->GetFieldID(highCpuCls, "complexity_w360", "I");
    jfieldID hComplex480 = (env)->GetFieldID(highCpuCls, "complexity_w480", "I");
    jfieldID hbitrate240 = (env)->GetFieldID(highCpuCls, "bitrate_w240", "I");
    jfieldID hbitrate360 = (env)->GetFieldID(highCpuCls, "bitrate_w360", "I");
    jfieldID hbitrate480 = (env)->GetFieldID(highCpuCls, "bitrate_w480", "I");
    jfieldID hframerate240 = (env)->GetFieldID(highCpuCls, "framerate_w240", "I");
    jfieldID hframerate360 = (env)->GetFieldID(highCpuCls, "framerate_w360", "I");
    jfieldID hframerate480 = (env)->GetFieldID(highCpuCls, "framerate_w480", "I");

    // set value
    // low cpu
    (env)->SetIntField(lowCpuObject, lComplex240, defaultVideoPreset.low.complexity.width240);
    (env)->SetIntField(lowCpuObject, lComplex360, defaultVideoPreset.low.complexity.width360);
    (env)->SetIntField(lowCpuObject, lComplex480, defaultVideoPreset.low.complexity.width480);
    (env)->SetIntField(lowCpuObject, lbitrate240, defaultVideoPreset.low.bitrate.width240);
    (env)->SetIntField(lowCpuObject, lbitrate360, defaultVideoPreset.low.bitrate.width360);
    (env)->SetIntField(lowCpuObject, lbitrate480, defaultVideoPreset.low.bitrate.width480);
    (env)->SetIntField(lowCpuObject, lframerate240, defaultVideoPreset.low.framerate.width240);
    (env)->SetIntField(lowCpuObject, lframerate360, defaultVideoPreset.low.framerate.width360);
    (env)->SetIntField(lowCpuObject, lframerate480, defaultVideoPreset.low.framerate.width480);

    // medium cpu
    (env)->SetIntField(mediumCpuObject, mComplex240, defaultVideoPreset.medium.complexity.width240);
    (env)->SetIntField(mediumCpuObject, mComplex360, defaultVideoPreset.medium.complexity.width360);
    (env)->SetIntField(mediumCpuObject, mComplex480, defaultVideoPreset.medium.complexity.width480);
    (env)->SetIntField(mediumCpuObject, mbitrate240, defaultVideoPreset.medium.bitrate.width240);
    (env)->SetIntField(mediumCpuObject, mbitrate360, defaultVideoPreset.medium.bitrate.width360);
    (env)->SetIntField(mediumCpuObject, mbitrate480, defaultVideoPreset.medium.bitrate.width480);
    (env)->SetIntField(mediumCpuObject, mframerate240, defaultVideoPreset.medium.framerate.width240);
    (env)->SetIntField(mediumCpuObject, mframerate360, defaultVideoPreset.medium.framerate.width360);
    (env)->SetIntField(mediumCpuObject, mframerate480, defaultVideoPreset.medium.framerate.width480);

    // high cpu
    (env)->SetIntField(highCpuObject, hComplex240, defaultVideoPreset.high.complexity.width240);
    (env)->SetIntField(highCpuObject, hComplex360, defaultVideoPreset.high.complexity.width360);
    (env)->SetIntField(highCpuObject, hComplex480, defaultVideoPreset.high.complexity.width480);
    (env)->SetIntField(highCpuObject, hbitrate240, defaultVideoPreset.high.bitrate.width240);
    (env)->SetIntField(highCpuObject, hbitrate360, defaultVideoPreset.high.bitrate.width360);
    (env)->SetIntField(highCpuObject, hbitrate480, defaultVideoPreset.high.bitrate.width480);
    (env)->SetIntField(highCpuObject, hframerate240, defaultVideoPreset.high.framerate.width240);
    (env)->SetIntField(highCpuObject, hframerate360, defaultVideoPreset.high.framerate.width360);
    (env)->SetIntField(highCpuObject, hframerate480, defaultVideoPreset.high.framerate.width480);

    // preset data
    (env)->SetObjectField(presetObject, stLowCpu, lowCpuObject);
    (env)->SetObjectField(presetObject, stMediumCpu, mediumCpuObject);
    (env)->SetObjectField(presetObject, stHighCpu, highCpuObject);

    return presetObject;
}

/**
 * Class:     org_wooyd_android_JNIExample_JNIExampleInterface
 * Method:    getPresetData
 * Signature: (ILjava/lang/String;)Lorg/wooyd/android/JNIExample/VideoPresetAdapter
 */
JNIEXPORT jint JNICALL Java_org_wooyd_android_JNIExample_JNIExampleInterface_getVideoPresetData
        (JNIEnv *env, jclass cls, jobject presetObj) {
    jclass presetCls = (env)->GetObjectClass(presetObj);
    if ( !presetCls ) {
        return -1;
    }

    jfieldID stLowCpu = (env)->GetFieldID(presetCls, "low",
                                          "Lorg/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetLowCpu;");
    jfieldID stMediumCpu = (env)->GetFieldID(presetCls, "medium",
                                             "Lorg/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetMediumCpu;");
    jfieldID stHighCpu = (env)->GetFieldID(presetCls, "high",
                                           "Lorg/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetHighCpu;");

    jobject lowCpuObj = (jobject)(env)->GetObjectField(presetObj, stLowCpu);
    jclass lowCpuCls = (env)->GetObjectClass(lowCpuObj);
    jfieldID lComplex240 = (env)->GetFieldID(lowCpuCls, "complexity_w240", "I");
    jfieldID lComplex360 = (env)->GetFieldID(lowCpuCls, "complexity_w360", "I");
    jfieldID lComplex480 = (env)->GetFieldID(lowCpuCls, "complexity_w480", "I");
    jfieldID lbitrate240 = (env)->GetFieldID(lowCpuCls, "bitrate_w240", "I");
    jfieldID lbitrate360 = (env)->GetFieldID(lowCpuCls, "bitrate_w360", "I");
    jfieldID lbitrate480 = (env)->GetFieldID(lowCpuCls, "bitrate_w480", "I");
    jfieldID lframerate240 = (env)->GetFieldID(lowCpuCls, "framerate_w240", "I");
    jfieldID lframerate360 = (env)->GetFieldID(lowCpuCls, "framerate_w360", "I");
    jfieldID lframerate480 = (env)->GetFieldID(lowCpuCls, "framerate_w480", "I");

    jobject mediumCpuObj = (jobject) (env)->GetObjectField(presetObj, stMediumCpu);
    jclass mediumCpuCls = (env)->GetObjectClass(mediumCpuObj);
    jfieldID mComplex240 = (env)->GetFieldID(mediumCpuCls, "complexity_w240", "I");
    jfieldID mComplex360 = (env)->GetFieldID(mediumCpuCls, "complexity_w360", "I");
    jfieldID mComplex480 = (env)->GetFieldID(mediumCpuCls, "complexity_w480", "I");
    jfieldID mbitrate240 = (env)->GetFieldID(mediumCpuCls, "bitrate_w240", "I");
    jfieldID mbitrate360 = (env)->GetFieldID(mediumCpuCls, "bitrate_w360", "I");
    jfieldID mbitrate480 = (env)->GetFieldID(mediumCpuCls, "bitrate_w480", "I");
    jfieldID mframerate240 = (env)->GetFieldID(mediumCpuCls, "framerate_w240", "I");
    jfieldID mframerate360 = (env)->GetFieldID(mediumCpuCls, "framerate_w360", "I");
    jfieldID mframerate480 = (env)->GetFieldID(mediumCpuCls, "framerate_w480", "I");

    jobject highCpuObj = (jobject) (env)->GetObjectField(presetObj, stHighCpu);
    jclass highCpuCls = (env)->GetObjectClass(highCpuObj);
    jfieldID hComplex240 = (env)->GetFieldID(highCpuCls, "complexity_w240", "I");
    jfieldID hComplex360 = (env)->GetFieldID(highCpuCls, "complexity_w360", "I");
    jfieldID hComplex480 = (env)->GetFieldID(highCpuCls, "complexity_w480", "I");
    jfieldID hbitrate240 = (env)->GetFieldID(highCpuCls, "bitrate_w240", "I");
    jfieldID hbitrate360 = (env)->GetFieldID(highCpuCls, "bitrate_w360", "I");
    jfieldID hbitrate480 = (env)->GetFieldID(highCpuCls, "bitrate_w480", "I");
    jfieldID hframerate240 = (env)->GetFieldID(highCpuCls, "framerate_w240", "I");
    jfieldID hframerate360 = (env)->GetFieldID(highCpuCls, "framerate_w360", "I");
    jfieldID hframerate480 = (env)->GetFieldID(highCpuCls, "framerate_w480", "I");

    // set value
    // low cpu
    (env)->SetIntField(lowCpuObj, lComplex240, defaultVideoPreset.low.complexity.width240);
    (env)->SetIntField(lowCpuObj, lComplex360, defaultVideoPreset.low.complexity.width360);
    (env)->SetIntField(lowCpuObj, lComplex480, defaultVideoPreset.low.complexity.width480);
    (env)->SetIntField(lowCpuObj, lbitrate240, defaultVideoPreset.low.bitrate.width240);
    (env)->SetIntField(lowCpuObj, lbitrate360, defaultVideoPreset.low.bitrate.width360);
    (env)->SetIntField(lowCpuObj, lbitrate480, defaultVideoPreset.low.bitrate.width480);
    (env)->SetIntField(lowCpuObj, lframerate240, defaultVideoPreset.low.framerate.width240);
    (env)->SetIntField(lowCpuObj, lframerate360, defaultVideoPreset.low.framerate.width360);
    (env)->SetIntField(lowCpuObj, lframerate480, defaultVideoPreset.low.framerate.width480);

    // medium cpu
    (env)->SetIntField(mediumCpuObj, mComplex240, defaultVideoPreset.medium.complexity.width240);
    (env)->SetIntField(mediumCpuObj, mComplex360, defaultVideoPreset.medium.complexity.width360);
    (env)->SetIntField(mediumCpuObj, mComplex480, defaultVideoPreset.medium.complexity.width480);
    (env)->SetIntField(mediumCpuObj, mbitrate240, defaultVideoPreset.medium.bitrate.width240);
    (env)->SetIntField(mediumCpuObj, mbitrate360, defaultVideoPreset.medium.bitrate.width360);
    (env)->SetIntField(mediumCpuObj, mbitrate480, defaultVideoPreset.medium.bitrate.width480);
    (env)->SetIntField(mediumCpuObj, mframerate240, defaultVideoPreset.medium.framerate.width240);
    (env)->SetIntField(mediumCpuObj, mframerate360, defaultVideoPreset.medium.framerate.width360);
    (env)->SetIntField(mediumCpuObj, mframerate480, defaultVideoPreset.medium.framerate.width480);

    // high cpu
    (env)->SetIntField(highCpuObj, hComplex240, defaultVideoPreset.high.complexity.width240);
    (env)->SetIntField(highCpuObj, hComplex360, defaultVideoPreset.high.complexity.width360);
    (env)->SetIntField(highCpuObj, hComplex480, defaultVideoPreset.high.complexity.width480);
    (env)->SetIntField(highCpuObj, hbitrate240, defaultVideoPreset.high.bitrate.width240);
    (env)->SetIntField(highCpuObj, hbitrate360, defaultVideoPreset.high.bitrate.width360);
    (env)->SetIntField(highCpuObj, hbitrate480, defaultVideoPreset.high.bitrate.width480);
    (env)->SetIntField(highCpuObj, hframerate240, defaultVideoPreset.high.framerate.width240);
    (env)->SetIntField(highCpuObj, hframerate360, defaultVideoPreset.high.framerate.width360);
    (env)->SetIntField(highCpuObj, hframerate480, defaultVideoPreset.high.framerate.width480);

    return 0;
}

/**
 * Method: setPresetData
 */
JNIEXPORT jint JNICALL Java_org_wooyd_android_JNIExample_JNIExampleInterface_setPresetData
        (JNIEnv *env, jclass cls, jobject presetObj) {
    jclass presetCls = (env)->GetObjectClass(presetObj);
    if ( !presetCls ) {
        return 0;
    }

    jfieldID stLowCpu = (env)->GetFieldID(presetCls, "low",
                                          "Lorg/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetLowCpu;");
//    jfieldID stMediumCpu = (env)->GetFieldID(presetCls, "medium",
//                                             "Lorg/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetMediumCpu;");
//    jfieldID stHighCpu = (env)->GetFieldID(presetCls, "high",
//                                           "Lorg/wooyd/android/JNIExample/VideoPresetAdapter$VideoPresetHighCpu;");

    jobject lowCpuObj = (jobject)(env)->GetObjectField(presetObj, stLowCpu);
    jclass lowCpuCls = (env)->GetObjectClass(lowCpuObj);
    jfieldID lComplex240 = (env)->GetFieldID(lowCpuCls, "complexity_w240", "I");
    jfieldID lComplex360 = (env)->GetFieldID(lowCpuCls, "complexity_w360", "I");
    jfieldID lComplex480 = (env)->GetFieldID(lowCpuCls, "complexity_w480", "I");
//    jfieldID lbitrate240 = (env)->GetFieldID(lowCpuCls, "bitrate_w240", "I");
//    jfieldID lbitrate360 = (env)->GetFieldID(lowCpuCls, "bitrate_w360", "I");
//    jfieldID lbitrate480 = (env)->GetFieldID(lowCpuCls, "bitrate_w480", "I");
//    jfieldID lframerate240 = (env)->GetFieldID(lowCpuCls, "framerate_w240", "I");
//    jfieldID lframerate360 = (env)->GetFieldID(lowCpuCls, "framerate_w360", "I");
//    jfieldID lframerate480 = (env)->GetFieldID(lowCpuCls, "framerate_w480", "I");

    defaultVideoPreset.low.complexity.width240 =
            (env)->GetIntField(lowCpuObj, lComplex240);
    defaultVideoPreset.low.complexity.width360 =
            (env)->GetIntField(lowCpuObj, lComplex360);
    defaultVideoPreset.low.complexity.width480 =
            (env)->GetIntField(lowCpuObj, lComplex480);
//    defaultVideoPreset.low.bitrate.width240 =
//            (env)->GetIntField(lowCpuObj, lbitrate240);
//    defaultVideoPreset.low.bitrate.width360 =
//            (env)->GetIntField(lowCpuObj, lbitrate360);
//    defaultVideoPreset.low.bitrate.width480 =
//            (env)->GetIntField(lowCpuObj, lbitrate480);
//    defaultVideoPreset.low.framerate.width240 =
//            (env)->GetIntField(lowCpuObj, lframerate240);
//    defaultVideoPreset.low.framerate.width360 =
//            (env)->GetIntField(lowCpuObj, lframerate360);
//    defaultVideoPreset.low.framerate.width480 =
//            (env)->GetIntField(lowCpuObj, lframerate480);

//    jobject mediumCpuObj = (jobject) (env)->GetObjectField(presetObj, stMediumCpu);
//    jclass mediumCpuCls = (env)->GetObjectClass(mediumCpuObj);
//    jfieldID mComplex240 = (env)->GetFieldID(mediumCpuCls, "complexity_w240", "I");
//    jfieldID mComplex360 = (env)->GetFieldID(mediumCpuCls, "complexity_w360", "I");
//    jfieldID mComplex480 = (env)->GetFieldID(mediumCpuCls, "complexity_w480", "I");
//    jfieldID mbitrate240 = (env)->GetFieldID(mediumCpuCls, "bitrate_w240", "I");
//    jfieldID mbitrate360 = (env)->GetFieldID(mediumCpuCls, "bitrate_w360", "I");
//    jfieldID mbitrate480 = (env)->GetFieldID(mediumCpuCls, "bitrate_w480", "I");
//    jfieldID mframerate240 = (env)->GetFieldID(mediumCpuCls, "framerate_w240", "I");
//    jfieldID mframerate360 = (env)->GetFieldID(mediumCpuCls, "framerate_w360", "I");
//    jfieldID mframerate480 = (env)->GetFieldID(mediumCpuCls, "framerate_w480", "I");
//
//    jobject highCpuObj = (jobject) (env)->GetObjectField(presetObj, stHighCpu);
//    jclass highCpuCls = (env)->GetObjectClass(highCpuObj);
//    jfieldID hComplex240 = (env)->GetFieldID(highCpuCls, "complexity_w240", "I");
//    jfieldID hComplex360 = (env)->GetFieldID(highCpuCls, "complexity_w360", "I");
//    jfieldID hComplex480 = (env)->GetFieldID(highCpuCls, "complexity_w480", "I");
//    jfieldID hbitrate240 = (env)->GetFieldID(highCpuCls, "bitrate_w240", "I");
//    jfieldID hbitrate360 = (env)->GetFieldID(highCpuCls, "bitrate_w360", "I");
//    jfieldID hbitrate480 = (env)->GetFieldID(highCpuCls, "bitrate_w480", "I");
//    jfieldID hframerate240 = (env)->GetFieldID(highCpuCls, "framerate_w240", "I");
//    jfieldID hframerate360 = (env)->GetFieldID(highCpuCls, "framerate_w360", "I");
//    jfieldID hframerate480 = (env)->GetFieldID(highCpuCls, "framerate_w480", "I");

    return 1;
}

/*
 * Class:     org_wooyd_android_JNIExample_JNIExampleInterface
 * Method:    getDataString
 * Signature: (Lorg/wooyd/android/JNIExample/Data;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_wooyd_android_JNIExample_JNIExampleInterface_getDataString
  (JNIEnv *env, jclass cls, jobject dataObject) {
    jclass dataClass = env->GetObjectClass(gDataObject);
    if(!dataClass) {
        LOGE("getDataString: failed to get class reference");
        return NULL;
    }
    jfieldID dataStringField = env->GetFieldID(
        dataClass, "s", "Ljava/lang/String;");
    if(!dataStringField) {
        LOGE("getDataString: failed to get field ID");
        return NULL;
    }
    jstring dataStringValue = (jstring) env->GetObjectField(
        dataObject, dataStringField);
    return dataStringValue;
}

void initClassHelper(JNIEnv *env, const char *path, jobject *objptr) {
    jclass cls = env->FindClass(path);
    if(!cls) {
        LOGE("initClassHelper: failed to get %s class reference", path);
        return;
    }
    jmethodID constr = env->GetMethodID(cls, "<init>", "()V");
    if(!constr) {
        LOGE("initClassHelper: failed to get %s constructor", path);
        return;
    }
    jobject obj = env->NewObject(cls, constr);
    if(!obj) {
        LOGE("initClassHelper: failed to create a %s object", path);
        return;
    }
    (*objptr) = env->NewGlobalRef(obj);

}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    gJavaVM = vm;
    LOGI("JNI_OnLoad called");
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("Failed to get the environment using GetEnv()");
        return -1;
    }

    initClassHelper(env, kInterfacePath, &gInterfaceObject);
    initClassHelper(env, kDataPath, &gDataObject);

    jclass interfaceClass = env->FindClass(kInterfacePath);

    JNINativeMethod methods[] = {
        {
            "callVoid",
            "()V",
            (void *) Java_org_wooyd_android_JNIExample_JNIExampleInterface_callVoid
        },
        {
            "getNewData",
            "(ILjava/lang/String;)Lorg/wooyd/android/JNIExample/Data;",
            (void *) Java_org_wooyd_android_JNIExample_JNIExampleInterface_getNewData
        },
        {
            "getDataString",
            "(Lorg/wooyd/android/JNIExample/Data;)Ljava/lang/String;",
            (void *) Java_org_wooyd_android_JNIExample_JNIExampleInterface_getDataString
        },
        {
            "getPresetData",
            "()Lorg/wooyd/android/JNIExample/VideoPresetAdapter;",
            (void*) Java_org_wooyd_android_JNIExample_JNIExampleInterface_getPresetData
        },
        {
            "getVideoPresetData",
            "(Lorg/wooyd/android/JNIExample/VideoPresetAdapter;)I",
            (void*) Java_org_wooyd_android_JNIExample_JNIExampleInterface_getVideoPresetData
        },
        {
            "setPresetData",
            "(Lorg/wooyd/android/JNIExample/VideoPresetAdapter;)I",
            (void*) Java_org_wooyd_android_JNIExample_JNIExampleInterface_setPresetData
        }
    };

    if (env->RegisterNatives(interfaceClass, methods, NELEM(methods)) != JNI_OK) {
        LOGE("Failed to register native methods");
        return -1;
    }

    return JNI_VERSION_1_4;
}
#ifdef __cplusplus
}
#endif
