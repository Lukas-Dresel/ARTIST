//
// Created by Lukas on 2/9/2016.
//

#include "breakpoint.h"
#include "self_patching_trappoint.h"
#include "../abi/abi_interface.h"

// Internally this does nothing but forward to a self-patching trappoint
// which patches at the next instruction to be executed

void*   Breakpoint_Install      (void *address, uint32_t trap_method, HOOKCALLBACK handler, void *arg)
{
    return SelfPatchingTrappoint_Install(address, trap_method, handler, arg, NULL, NULL, ExtractNextExecutedInstructionPointer);
}
bool    Breakpoint_Enable       (void *p)
{
    SelfPatchingTrappoint_Enable(p);
}
bool    Breakpoint_Disable      (void *p)
{
    SelfPatchingTrappoint_Disable(p);
}
void    Breakpoint_Uninstall    (void *p)
{
    SelfPatchingTrappoint_Uninstall(p);
}
