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

#ifndef NDKTEST_ART_ENTRYPOINTS_INTERPRETER_ENTRYPOINTS_H
#define NDKTEST_ART_ENTRYPOINTS_INTERPRETER_ENTRYPOINTS_H

#include "../../../../../../../libUtility/src/main/jni/macros.h"
#include "../../../dex_internal.h"
#include "../../../stack.h"

struct Thread;
// Pointers to functions that are called by interpreter trampolines via thread-local storage.
struct PACKED(4) InterpreterEntryPoints {
    void (*pInterpreterToInterpreterBridge)(struct Thread* self, const struct CodeItem* code_item,
                                            struct ShadowFrame* shadow_frame, union JValue* result);
    void (*pInterpreterToCompiledCodeBridge)(struct Thread* self, const struct CodeItem* code_item,
                                             struct ShadowFrame* shadow_frame, union JValue* result);
};

#endif //NDKTEST_ART_ENTRYPOINTS_INTERPRETER_ENTRYPOINTS_H
