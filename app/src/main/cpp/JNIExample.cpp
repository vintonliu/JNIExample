#include <jni.h>
#include <android/log.h>

#include <string.h>
#include <unistd.h>
#include <pthread.h>

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
    LOGE("pthread id = %d", native_thread);
//    pthread_join(native_thread, NULL);
    LOGE("[%s %d] debug.", __FUNCTION__, __LINE__);
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
