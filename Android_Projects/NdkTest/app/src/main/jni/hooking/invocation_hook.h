//
// Created by Lukas on 2/9/2016.
//

#ifndef NDKTEST_INVOCATION_HOOK_H
#define NDKTEST_INVOCATION_HOOK_H

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

#endif //NDKTEST_INVOCATION_HOOK_H
