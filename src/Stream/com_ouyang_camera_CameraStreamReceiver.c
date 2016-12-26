#include <jni.h>
#include <stdlib.h>
#include <tiny_log.h>

#ifdef __cplusplus
extern "C" {
#endif


#define TAG             "stream_jni"

/*
 * Class:     com_ouyang_camera_CameraStreamReceiver
 * Method:    initialize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL _initialize(JNIEnv * env, jobject obj)
{
    LOG_D(TAG, "_initialize");

    return 0;
}

/*
 * Class:     com_ouyang_camera_CameraStreamReceiver
 * Method:    destroy
 * Signature: ()I
 */
JNIEXPORT jint JNICALL _destroy(JNIEnv *env, jobject obj)
{
    LOG_D(TAG, "_destroy");

    return 0;
}

/*
 * Class:     com_ouyang_camera_CameraStreamReceiver
 * Method:    start
 * Signature: (Ljava/lang/String;ILjava/lang/String;ILcom/ouyang/camera/StreamHandler;)I
 */
JNIEXPORT jint JNICALL _start(JNIEnv *env,
                              jobject obj,
                              jstring ip,
                              jint port,
                              jstring key,
                              jint ssrc,
                              jobject handler)
{
    LOG_D(TAG, "_start");

    return 0;
}

/*
 * Class:     com_ouyang_camera_CameraStreamReceiver
 * Method:    stop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL _stop(JNIEnv * env, jobject obj)
{
    LOG_D(TAG, "_stop");

    return 0;
}

static const char * _theClass = "com/ouyang/camera/CameraStreamReceiver";

static JNINativeMethod _theMethods[] =
        {
                {"initialize", "()I", &_initialize},
                {"destroy", "()I", &_destroy},
                {"start", "(Ljava/lang/String;ILjava/lang/String;ILcom/ouyang/camera/StreamHandler;)I", &_start},
                {"stop", "()I", &_stop},
        };

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv * env = NULL;
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