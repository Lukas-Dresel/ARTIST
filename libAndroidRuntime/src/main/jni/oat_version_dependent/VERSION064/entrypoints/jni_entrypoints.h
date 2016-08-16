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
 
//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_ENTRYPOINTS_JNI_ENTRYPOINTS_H
#define NDKTEST_ART_ENTRYPOINTS_JNI_ENTRYPOINTS_H

#include <jni.h>
#include "../../../../../../../../libUtility/src/main/jni/macros.h"

struct PACKED(4) JniEntryPoints {
// Called when the JNI method isn't registered.
void* (*pDlsymLookup)(JNIEnv* env, jobject);
};

#endif //NDKTEST_ART_ENTRYPOINTS_JNI_ENTRYPOINTS_H
