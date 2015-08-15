//
// Created by Lukas on 8/15/2015.
//

#include "trappoint_signal_handling.h"

#include <stdbool.h>

#include "trappoint.h"

void sigill_handler(int signal, siginfo_t* sigInfo, ucontext_t* context)
{
    mcontext_t* state_info = &(context->uc_mcontext);
}
void sigtrap_handler(int signal, siginfo_t* sigInfo, ucontext_t* context)
{
    mcontext_t* state_info = &(context->uc_mcontext);

    LOGD("Inside the SIGTRAP handler..., signal %d, siginfo_t "PRINT_PTR", context "PRINT_PTR, signal, (uintptr_t)sigInfo, (uintptr_t)context);

    //log_siginfo_content(sigInfo);
    //log_mcontext_content(state_info);

    void* target = (void*)state_info->arm_pc;
    LOGD("Looking for trappoint for address "PRINT_PTR" in list.", (uintptr_t)target);

    bool found = false;
    TrapPointInfo* current;
    list_for_each_entry(current, &installed_trap_points, installed)
    {
        if(current->target.mem_addr == target)
        {
            LOGI("FOUND Trappoint for "PRINT_PTR"(%s)", (uintptr_t)current->target.mem_addr, current->target.thumb ? "THUMB" : "ARM");
            if(current->handler != NULL)
            {
                LOGI("Executing registered handler at "PRINT_PTR" with arguments("PRINT_PTR", "PRINT_PTR", "PRINT_PTR")",
                        (uintptr_t)(current->target.mem_addr), (uintptr_t)(context), (uintptr_t)(current->handler_args));
                current->handler(current->target.mem_addr, context, current->handler_args);
            }
            else
            {
                LOGI("No trappoint handler was registered, so it wasn't executed.");
            }
            found = true;
        }
    }

    if(!found)
    {
        LOGD("Could not find trappoint in list, this SIGTRAP should not originate from us.");
        *((uint32_t*)target) = (uint32_t)-1;
        return;
    }
    LOGD("FOUND trappoint in list! Proceeding with resetting.");

    uninstall_trap_point(current);

    LOGD("Returning from SIGILL-Handler.");
}