#ifndef _MAINACT_NATIVE_H
#define _MAINACT_NATIVE_H

#include <jni.h>

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
 * Method:    set_memory_protection
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





