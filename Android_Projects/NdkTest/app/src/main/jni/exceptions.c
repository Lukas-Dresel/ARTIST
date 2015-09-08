#include "exceptions.h"

#ifdef __cplusplus
extern "C" {
#endif

bool hasExceptionOccurred(JNIEnv *env)
{
    return (*env)->ExceptionCheck(env);
}

void throwNewJNIException(JNIEnv *env, const char *classNameNotSignature, const char *message)
{
    if (!hasExceptionOccurred(env))
    {
        jclass jClass = (*env)->FindClass(env, classNameNotSignature);
        (*env)->ThrowNew(env, jClass, message);
    }
}


#ifdef __cplusplus
}
#endif





