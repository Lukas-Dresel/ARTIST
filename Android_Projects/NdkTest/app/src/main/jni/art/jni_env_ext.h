//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_JNI_ENV_EXT_H_H
#define NDKTEST_ART_JNI_ENV_EXT_H_H

#include <inttypes.h>
#include <jni.h>
#include "../util/macros.h"
#include "thread.h"
#include "indirect_reference_table.h"
#include "reference_table.h"

struct JNIEnvExt
{
    JNIEnv base;  // Used to be an inherited struct

    struct Thread* const self;
    void* const vm; // Used to be JavaVMExt*

    // Cookie used when using the local indirect reference table.
    uint32_t local_ref_cookie;

    // JNI local references.
    struct IndirectReferenceTable locals;

    // Stack of cookies corresponding to PushLocalFrame/PopLocalFrame calls.
    // TODO: to avoid leaks (and bugs), we need to clear this vector on entry (or return)
    // to a native method.
    struct DIRTY_DIRTY_STD_VECTOR_HACK stacked_local_ref_cookies;

    // Frequently-accessed fields cached from JavaVM.
    bool check_jni;

    // How many nested "critical" JNI calls are we in?
    int critical;

    // Entered JNI monitors, for bulk exit on thread detach.
    struct ReferenceTable monitors;

    // Used by -Xcheck:jni.
    const struct JNINativeInterface* unchecked_functions;

};

#endif //NDKTEST_ART_JNI_ENV_EXT_H_H
