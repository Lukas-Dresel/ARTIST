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

#include "invocation_hook.h"
#include "../abi/abi_interface.h"

void*   InvocationHook_Install   (void *address, uint32_t trap_method,
                                  HOOKCALLBACK OnEntry,  void* OnEntry_Arg,
                                  HOOKCALLBACK OnReturn, void* OnReturn_Arg)
{
    return SelfPatchingTrappoint_Install(address, trap_method, OnEntry, OnEntry_Arg, OnReturn, OnReturn_Arg, ExtractReturnAddress);
}
bool    InvocationHook_Enable    (void *p)
{
    return SelfPatchingTrappoint_Enable(p);
}
bool    InvocationHook_Disable   (void *p)
{
    return SelfPatchingTrappoint_Disable(p);
}
void    InvocationHook_Uninstall (void *p)
{
    return SelfPatchingTrappoint_Uninstall(p);
}
