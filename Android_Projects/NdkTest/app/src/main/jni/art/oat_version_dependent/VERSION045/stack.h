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
// Created by Lukas on 1/30/2016.
//

#ifndef NDKTEST_STACK_H
#define NDKTEST_STACK_H

#include <stdint.h>

#include "../../../util/macros.h"

// ShadowFrame has 3 possible layouts:
//  - portable - a unified array of VRegs and references. Precise references need GC maps.
//  - interpreter - separate VRegs and reference arrays. References are in the reference array.
//  - JNI - just VRegs, but where every VReg holds a reference.
struct ShadowFrame {
    // Link to previous shadow frame or NULL.
    struct ShadowFrame* link_;
    void* method_; // mirror::ArtMethod* method_;
    uint32_t dex_pc_;
    uint32_t vregs_[0];

};

struct ManagedStack;
struct PACKED(4) ManagedStack
{
    struct ManagedStack* link_;
    struct ShadowFrame* top_shadow_frame_;
    void* top_quick_frame_; // StackReference<mirror::ArtMethod>* top_quick_frame_;
    uintptr_t top_quick_frame_pc_;
};


#endif //NDKTEST_STACK_H
