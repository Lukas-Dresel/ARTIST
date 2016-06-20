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
// Created by Lukas on 1/31/2016.
//

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "function_hook.h"
#include "trappoint_interface.h"

/*
struct FunctionHook
{
    uint32_t trap_method;
    void* trappoint_entry;
    void* trappoint_exit;
    struct Handler* handler;
};

void exit_handler(void* addr, ucontext_t* ctx, void* arg);
void entry_handler(void *addr, ucontext_t *ctx, void *arg);

void entry_handler(void *addr, ucontext_t *ctx, void *arg)
{
    struct FunctionHook* self = (struct FunctionHook*)arg;
    trappoint_Disable((TrapPointInfo *) self->trappoint_entry);

    if(self->handler->OnEntryHandler != NULL)
    {
        // Call the handler for the function entry point.
        self->handler->OnEntryHandler(addr, ctx, self->handler->entry_args);
    }

    /* Recursive calls of functions can't be monitored by this mechanism. This is because with this
     * method the trappoint on the function entry is only reset once the function returned to its
     * caller. In the case of a recursive function call this function the OnEntry handler is invoked
     * the first time the function is called and the OnExit handler only when the highest instance
     * of the function returns. So it does match a call with its corresponding exit but only once.
     * Sadly i didn't find a way to do this reliably.
     *
     * Because of this structure this deallocation should be fine.
     *//*
    if(self->trappoint_exit != NULL)
    {
        trappoint_Uninstall(self->trappoint_exit); // Uninstall and free previous exit trappoint
    }

    void* ret_addr = ExtractReturnAddress(ctx);
    struct TrapPointInfo* trap = trappoint_Install(ret_addr, self->trap_method, exit_handler,
                                                   (void *) self);
    if(trap == NULL)
    {
        LOGF("Could not install return address trappoint. THIS IS BAD!.");
        LOGF("Last error: %s", get_last_error());
    }
    self->trappoint_exit = trap;
}*//*
void*FunctionHook_Install(void *entry_point, struct Handler *handler, uint32_t trapping_method)
{

}*/

#ifdef __cplusplus
}
#endif

