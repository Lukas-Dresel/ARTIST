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

#include "breakpoint.h"

#include <hostsystem/abi.h>
#include "self_patching_trappoint.h"

// Internally this does nothing but forward to a self-patching trappoint
// which patches at the next instruction to be executed

void*   Breakpoint_Install      (void *address, uint32_t trap_method, HOOKCALLBACK handler, void *arg)
{
    return SelfPatchingTrappoint_Install(address, trap_method, handler, arg, NULL, NULL, ExtractNextExecutedInstructionPointer);
}
bool    Breakpoint_Enable       (void *p)
{
    return SelfPatchingTrappoint_Enable(p);
}
bool    Breakpoint_Disable      (void *p)
{
    return SelfPatchingTrappoint_Disable(p);
}
void    Breakpoint_Uninstall    (void *p)
{
    SelfPatchingTrappoint_Uninstall(p);
}
