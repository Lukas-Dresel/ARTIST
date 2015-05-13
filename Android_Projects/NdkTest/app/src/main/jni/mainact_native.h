#ifndef _MAINACT_NATIVE_H
#define _MAINACT_NATIVE_H

#include <jni.h>

#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include <stdlib.h>

#include <stdio.h>

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#include "config.h"

#include "com_example_lukas_ndktest_MainActivity.h"

#include "logging.h"
#include "memory.h"
#include "hooking.h"
#include "lib_setup.h"
#include "util.h"

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpJEnvContent
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpJEnvContent (JNIEnv * env, jobject self);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpProcessMemoryMap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpProcessMemoryMap (JNIEnv * env, jobject self);
/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpProcAlignmentMap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpProcAlignmentMap (JNIEnv * env, jobject self);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    setMemoryProtection
 * Signature: (IIZZZ)V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_setMemoryProtection (JNIEnv * env, jobject self, jlong address, jlong size, jboolean read, jboolean write, jboolean execute);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testOverwriteAtoi
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwriteAtoi (JNIEnv *, jobject);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testOverwriteOwnFunction
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwriteOwnFunction(JNIEnv *, jobject);

#endif





