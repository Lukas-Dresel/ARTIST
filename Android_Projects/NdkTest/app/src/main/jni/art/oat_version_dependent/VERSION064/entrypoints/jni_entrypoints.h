//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_ENTRYPOINTS_JNI_ENTRYPOINTS_H
#define NDKTEST_ART_ENTRYPOINTS_JNI_ENTRYPOINTS_H

#include <jni.h>
#include "../../../../util/macros.h"

struct PACKED(4) JniEntryPoints {
// Called when the JNI method isn't registered.
void* (*pDlsymLookup)(JNIEnv* env, jobject);
};

#endif //NDKTEST_ART_ENTRYPOINTS_JNI_ENTRYPOINTS_H
