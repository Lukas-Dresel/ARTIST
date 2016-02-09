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
