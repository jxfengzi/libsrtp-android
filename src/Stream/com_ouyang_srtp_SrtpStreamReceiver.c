#include <jni.h>
#include <stdlib.h>
#include <tiny_log.h>
#include <StreamReceiver.h>

#ifdef __cplusplus
extern "C" {
#endif


#define TAG             "stream_r_jni"

static StreamReceiver   * _receiver = NULL;
static JavaVM           * _jvm = NULL;
static jobject            _handler = NULL;
static jmethodID          _onPacket_MethodID = NULL;

/**
 * public interface SrtpStreamHandler {
 *     void onPacket(byte[] data, int size)
 * }
 */
static jmethodID getMethodID_onPacket(JNIEnv *env, jobject handler)
{
    jclass clazz;
    const char *signature = "([BI)V";

    clazz = (*env)->GetObjectClass(env, handler);
    if (clazz == NULL)
    {
        LOG_I("%s", "GetObjectClass failed");
        return NULL;
    }

    return (*env)->GetMethodID(env, clazz, "onPacket", signature);
}

typedef struct _env_t
{
    JNIEnv *env;
    bool attached;
} env_t;

static bool env_open(env_t *e)
{
    if (_jvm == NULL)
    {
        LOG_I("%s", "_jvm is NULL");
        return false;
    }

    e->env = NULL;
    e->attached = false;

    (*_jvm)->AttachCurrentThread(_jvm, &e->env, NULL);
    if (e->env == NULL)
    {
        LOG_I("%s", "AttachCurrentThread failed");
        e->attached = false;
       return false;
    }

    e->attached = true;

    return true;
}

static void env_close(env_t *e)
{
    if (e->attached)
    {
        (*_jvm)->DetachCurrentThread(_jvm);
    }
}

//static void test(JNIEnv *env)
//{
//    const char *ppp = "hello";
//    int len = 5;
//
//    jbyteArray data;
//    jint size;
//
//    data = (*env)->NewByteArray(env, len);
//    (*env)->SetByteArrayRegion(env, data, 0, len, (jbyte *) ppp);
//
//    size = len;
//
//    (*env)->CallVoidMethod(env, _handler, _onPacket_MethodID, data, size);
//}

static void packet_handler(const char *packet, int len)
{
    env_t e;

    LOG_D(TAG, "packet_handler: %d", len);

    if (_handler == NULL)
    {
        LOG_I("%s", "_handler is NULL");
        return;
    }

    if (env_open(&e))
    {
        JNIEnv *env = e.env;
        jbyteArray j_data = (*env)->NewByteArray(env, len);
        jint j_size = len;

        (*env)->SetByteArrayRegion(env, j_data, 0, len, (jbyte *) packet);
        (*env)->CallVoidMethod(env, _handler, _onPacket_MethodID, j_data, j_size);
        (*env)->ReleaseByteArrayElements(env, j_data, (*env)->GetByteArrayElements(env, j_data, NULL), 0);

        env_close(&e);
    }

    return;
}

/*
 * Class:     com_ouyang_srtp_SrtpStreamReceiver
 * Method:    initialize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL _initialize(JNIEnv * env, jobject obj)
{
    LOG_D(TAG, "_initialize");

    if (_receiver != NULL)
    {
        LOG_D(TAG, "already initialized");
        return -1;
    }

    _receiver = StreamReceiver_New();

    return 0;
}

/*
 * Class:     com_ouyang_srtp_SrtpStreamReceiver
 * Method:    destroy
 * Signature: ()I
 */
JNIEXPORT jint JNICALL _destroy(JNIEnv *env, jobject obj)
{
    LOG_D(TAG, "_destroy");

    if (_receiver == NULL)
    {
        LOG_D(TAG, "not initialized");
        return -1;
    }

    StreamReceiver_Delete(_receiver);

    return 0;
}

/*
 * Class:     com_ouyang_srtp_SrtpStreamReceiver
 * Method:    start
 * Signature: (Ljava/lang/String;ILjava/lang/String;ILcom/ouyang/srtp/SrtpStreamHandler;)I
 */
JNIEXPORT jint JNICALL _start(JNIEnv *env,
                              jobject obj,
                              jstring ip,
                              jint port,
                              jstring key,
                              jint ssrc,
                              jobject handler)
{
    const char * c_ip = NULL;
    int c_port = 0;
    const char * c_key = NULL;
    int c_ssrc = 0;

    LOG_D(TAG, "_start");

    if (_receiver == NULL)
    {
        LOG_D(TAG, "not initialized");
        return -1;
    }

    c_port = port;
    c_ssrc = ssrc;

    c_ip = (*env)->GetStringUTFChars(env, ip, NULL);
    if (c_ip == NULL)
    {
        LOG_E(TAG, "ip invalid");
        return -2;
    }

    c_key = (*env)->GetStringUTFChars(env, key, NULL);
    if (c_key == NULL)
    {
        LOG_E(TAG, "key invalid");
        return -3;
    }

    /**
     * store jvm & handler
     */
    (*env)->GetJavaVM(env, &_jvm);
    _handler = (*env)->NewGlobalRef(env, handler);
    _onPacket_MethodID = getMethodID_onPacket(env, _handler);

    if (RET_FAILED(StreamReceiver_Start(_receiver, c_ip, c_port, c_key, c_ssrc, packet_handler)))
    {
        _handler = NULL;
        return -4;
    }

    return 0;
}

/*
 * Class:     com_ouyang_srtp_SrtpStreamReceiver
 * Method:    stop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL _stop(JNIEnv * env, jobject obj)
{
    LOG_D(TAG, "_stop");

    if (_receiver == NULL)
    {
        LOG_D(TAG, "not initialized");
        return -1;
    }

    if (RET_FAILED(StreamReceiver_Stop(_receiver)))
    {
        return -2;
    }

    _jvm = NULL;
    _handler = NULL;

    return 0;
}

static const char * _theClass = "com/ouyang/srtp/SrtpStreamReceiver";

static JNINativeMethod _theMethods[] =
        {
                {"initialize", "()I", &_initialize},
                {"destroy", "()I", &_destroy},
                {"start", "(Ljava/lang/String;ILjava/lang/String;ILcom/ouyang/srtp/SrtpStreamHandler;)I", &_start},
                {"stop", "()I", &_stop},
        };

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    jclass clazz;
    int version = -1;

    LOG_I(TAG, "-> JNI_OnLoad");

    do
    {
        if ((*vm)->GetEnv(vm, (void *) &env, JNI_VERSION_1_6) == JNI_OK)
        {
            version = JNI_VERSION_1_6;
            LOG_I(TAG, "JNI 1.6");
        }
        else
        {
            if ((*vm)->GetEnv(vm, (void *) &env, JNI_VERSION_1_4) == JNI_OK)
            {
                version = JNI_VERSION_1_4;
                LOG_I(TAG, "JNI 1.4");
            }
            else
            {
                break;
            }
        }

        // Get jclass with env->FindClass.
        clazz = (*env)->FindClass(env, _theClass);
        if (clazz == NULL)
        {
            LOG_E(TAG, "Find class failed: %s", _theClass);
            version = JNI_ERR;
            break;
        }

        LOG_I(TAG, "FindClass OK: %s", _theClass);

        // Register methods with env->RegisterNatives.
        if ((*env)->RegisterNatives(env, clazz, _theMethods, sizeof(_theMethods) / sizeof(JNINativeMethod)) < 0)
        {
            LOG_E(TAG, "RegisterNatives failed.");
            version = JNI_ERR;
            break;
        }

        LOG_D(TAG, "RegisterNatives OK.");
    }
    while (0);

    return version;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    LOG_I(TAG, "%s", "JNI_OnUnload");
}


#ifdef __cplusplus
}
#endif