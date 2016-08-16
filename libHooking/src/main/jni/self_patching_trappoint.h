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
// Created by Lukas on 8/19/2015.
//

#ifndef NDKTEST_SELF_PATCHING_TRAPPOINT_H
#define NDKTEST_SELF_PATCHING_TRAPPOINT_H

#include "hooking_common.h"
#include <stdbool.h>

// This is primarily designed to be an internal interface to provide a more general
// breakpoint interface. While accessible for outside use, there is no focus on ease-of-use.
// If that is desired write your own bridge for the functionality you are looking for.
// For reference look at breakpoint.[ch] and invocation_hook.[ch]

// This is intentionally chosen to fit the signature of the abi_interface.h functions ;)
typedef void* (*PATCH_ADDRESS_EXTRACTOR)(ucontext_t *ctx);

void    init_self_patching_trappoints();
void    destroy_self_patching_trappoints();

void*   SelfPatchingTrappoint_Install   (void *address, uint32_t trap_method,
                                         HOOKCALLBACK PostTrigger_Handler, void* PostTrigger_Arg,
                                         HOOKCALLBACK PrePatch_Handler,    void* PrePatch_Arg,
                                         PATCH_ADDRESS_EXTRACTOR patch_at);
bool    SelfPatchingTrappoint_Enable    (void *p);
bool    SelfPatchingTrappoint_Disable   (void *p);
void    SelfPatchingTrappoint_Uninstall (void *p);





#endif //NDKTEST_SELF_PATCHING_TRAPPOINT_H
