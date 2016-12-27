#include <jni.h>
#include <tiny_log.h>
#include <StreamSender.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TAG             "stream_r_jni"

static StreamSender   * _sender = NULL;

/*
 * Class:     com_ouyang_camera_CameraStreamSender
 * Method:    initialize
 * Signature: (Ljava/lang/String;ILjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL _initialize(JNIEnv *env, jobject obj, jstring ip, jint port, jstring key, jint ssrc)
{
    const char * c_ip = NULL;
    int c_port = 0;
    const char * c_key = NULL;
    int c_ssrc = 0;

    LOG_D(TAG, "_initialize");

    c_port = port;
    c_ssrc = ssrc;

    c_ip = (*env)->GetStringUTFChars(env, ip, NULL);
    if (c_ip == NULL)
    {
        LOG_E(TAG, "ip invalid");
        return -1;
    }

    c_key = (*env)->GetStringUTFChars(env, key, NULL);
    if (c_key == NULL)
    {
        LOG_E(TAG, "key invalid");
        return -2;
    }

    return StreamSender_Initialize(_sender, c_ip, c_port, c_key, c_ssrc);
}

/*
 * Class:     com_ouyang_camera_CameraStreamSender
 * Method:    destroy
 * Signature: ()I
 */
JNIEXPORT jint JNICALL _destroy(JNIEnv *env, jobject obj)
{
    StreamSender_Finalize(_sender);

    return 0;
}

/*
 * Class:     com_ouyang_camera_CameraStreamSender
 * Method:    Sendto
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL _sendto(JNIEnv *env, jobject obj, jbyteArray buf, jint len)
{
    jbyte* buffer = (*env)->GetByteArrayElements(env, buf, 0);
    char* c_buf = (char *)buffer;
    int c_len = len;

    return  StreamSender_Sendto(_sender, c_buf, (size_t) c_len);
}

static const char * _theClass = "com/ouyang/camera/CameraStreamSender";

static JNINativeMethod _theMethods[] =
        {
                {"initialize", "(Ljava/lang/String;ILjava/lang/String;I)I", &_initialize},
                {"destroy", "()I", &_destroy},
                {"sendto", "([BI)I", &_sendto},
        };

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    jclass clazz;
    int version = -1;

    LOG_I(TAG, "%s", "JNI_OnLoad");

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

        _sender = StreamSender_New();
    }
    while (0);

    return version;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    LOG_I(TAG, "%s", "JNI_OnUnload");

    StreamSender_Delete(_sender);
}

#ifdef __cplusplus
}
#endif