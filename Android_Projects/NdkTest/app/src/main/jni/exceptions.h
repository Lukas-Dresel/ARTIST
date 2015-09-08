#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include <jni.h>
#include <stdbool.h>
#include "logging.h"

bool hasExceptionOccurred(JNIEnv *env);

void throwNewJNIException(JNIEnv *env, const char *classNameNotSignature, const char *message);

#define RETURN_ON_EXCEPTION(env, cleanup, defReturn) \
    do { \
        if(hasExceptionOccurred((env))) { \
        do { \
            cleanup \
           } while (0); \
        return defReturn; \
        } } while(0);


#endif





