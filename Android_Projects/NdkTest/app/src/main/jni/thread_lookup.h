//
// Created by Lukas on 4/13/2016.
//

#ifndef NDKTEST_THREAD_LOOKUP_H
#define NDKTEST_THREAD_LOOKUP_H

#include <jni.h>

struct Thread* GetCurrentThreadObjectPointer(JNIEnv* env);

#endif //NDKTEST_THREAD_LOOKUP_H
