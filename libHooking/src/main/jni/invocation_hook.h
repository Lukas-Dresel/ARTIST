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
// Created by Lukas on 2/9/2016.
//

#ifndef ARTIST_INVOCATION_HOOK_H
#define ARTIST_INVOCATION_HOOK_H

// This implements a top-level function hook. This means that it tracks a functions top-most call.
// In practice this means that if a function is called from somewhere inside itself again
// (more general recursion) these calls will not be monitored.

#include <stdint.h>
#include <stdbool.h>
#include "hooking_common.h"
#include "self_patching_trappoint.h"

void*   InvocationHook_Install   (void *address, uint32_t trap_method,
                                  HOOKCALLBACK OnEntry,  void* OnEntry_Arg,
                                  HOOKCALLBACK OnReturn, void* OnReturn_Arg);
bool    InvocationHook_Enable    (void *p);
bool    InvocationHook_Disable   (void *p);
void    InvocationHook_Uninstall (void *p);

#endif //ARTIST_INVOCATION_HOOK_H
