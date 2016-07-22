/*
 * Copyright 2016 Lukas Dresel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef _MAINACT_NATIVE_H
#define _MAINACT_NATIVE_H

#include <jni.h>

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpJEnvContent
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpJEnvContent(JNIEnv *env,
                                                                                   jobject self);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpProcessMemoryMap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpProcessMemoryMap(JNIEnv *env,
                                                                                        jobject self);
/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpProcAlignmentMap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpProcAlignmentMap(JNIEnv *env,
                                                                                        jobject self);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    set_memory_protection
 * Signature: (IIZZZ)V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_setMemoryProtection(JNIEnv *env,
                                                                                       jobject self,
                                                                                       jlong address,
                                                                                       jlong size,
                                                                                       jboolean read,
                                                                                       jboolean write,
                                                                                       jboolean execute);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testOverwriteAtoi
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwriteAtoi(JNIEnv *,
                                                                                     jobject);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpQuickEntryPointsInfo
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpQuickEntryPointsInfo(JNIEnv* , jobject);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testHookingThreadEntryPoints
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_testHookingThreadEntryPoints(JNIEnv *env, jobject instance);

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testOverwriteOwnFunction
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwriteOwnFunction(
        JNIEnv *, jobject);

JNIEXPORT void JNICALL
        Java_com_example_lukas_ndktest_MainActivity_testHookingAOTCompiledFunction(JNIEnv *env,
                                                                            jobject instance);

JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testSingleStep(
        JNIEnv *env, jobject instance);

#endif





