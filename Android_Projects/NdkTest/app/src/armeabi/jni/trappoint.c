//
// Created by Lukas on 8/6/2015.
//

#include "trappoint.h"

void sigtrap_handler(int signal, siginfo_t* sigInfo, ucontext_t* context);

void init_trap_points()
{
    /*stack_t sigstk;
    if ((sigstk.ss_sp = malloc(SIGSTKSZ)) == NULL)
    {
        LOGE("Could not allocate mem for alt stack");
        return;
    }
    sigstk.ss_size = SIGSTKSZ;
    sigstk.ss_flags = 0;
    if (sigaltstack(&sigstk,NULL) < 0)
    {
        perror("sigaltstack");
    }*/

    INIT_LIST_HEAD(&installed_trap_points);

    struct sigaction sigtrap_action;

    sigtrap_action.sa_sigaction = (void*)sigtrap_handler;
    sigtrap_action.sa_flags = SA_SIGINFO;

    if(sigaction(SIGTRAP, &sigtrap_action, &old_sigtrap_action) != 0)
    {
        LOGD("Error installing SIGTRAP handler: %s", strerror(errno));
    }
}

void destroy_trap_points()
{
    if(sigaction(SIGTRAP, &old_sigtrap_action, NULL) != 0)
    {
        LOGD("Error uninstalling SIGTRAP handler: %s", strerror(errno));
    }
}

TrapPointInfo *install_trap_point(void *addr, uint32_t method, CALLBACK handler, void* additionalArgs)
{
    if(addr == NULL)
    {
        return NULL;
    }
    TrapPointInfo* trap = malloc(sizeof(TrapPointInfo));
    if(trap != NULL)
    {
        trap->handler = handler;
        trap->handler_args = additionalArgs;

        trap->target.call_addr = addr;
        trap->target.mem_addr = getCodeBaseAddress(addr);
        trap->target.thumb = (getCodeBaseOffset(addr) == 1);

        trap->instr_size = trap->target.thumb ? 2 : 4;
        trap->trapping_method = method;

        if(set_memory_protection(trap->target.mem_addr, trap->instr_size, true, true, true))
        {
            if(trap->target.thumb)
            {
                trap->thumbCode.trap_instruction = make_thumb_trap_instruction(method);

                uint16_t* target = (uint16_t*)trap->target.mem_addr;
                trap->thumbCode.preserved = *target;
                *target = trap->thumbCode.trap_instruction;
            }
            else
            {
                trap->armCode.trap_instruction = make_arm_trap_instruction(method);

                uint32_t* target = (uint32_t*)trap->target.mem_addr;
                trap->armCode.preserved = *target;
                *target = trap->armCode.trap_instruction;
            }
            __builtin___clear_cache((void*)trap->target.mem_addr, (void*)trap->target.mem_addr + trap->instr_size);

            // Installation succeeded, so insert ourself into the installed trap point list
            list_add(&trap->installed, &installed_trap_points);
            return trap;
        }
        free(trap);
    }
    return NULL;
}
void uninstall_trap_point(TrapPointInfo * trap)
{
    if(!validate_TrapPointInfo_contents(trap))
    {
        return;
    }
    if(!set_memory_protection(trap->target.mem_addr, trap->instr_size, true, true, true))
    {
        set_last_error("Cannot set memory protections to reset the trappoints target to it's original state. NOT RESETTING MEMORY TO ORIGINAL STATE.");
    }
    else
    {
        if(trap->target.thumb)
        {
            *((uint16_t*)trap->target.mem_addr) = trap->thumbCode.preserved;
        }
        else
        {
            *((uint32_t*)trap->target.mem_addr) = trap->armCode.preserved;
        }
        __builtin___clear_cache((void*)trap->target.mem_addr, (void*)trap->target.mem_addr + trap->instr_size);
    }
    list_del(&trap->installed);
    free(trap);
}

void dump_installed_trappoints_info()
{
    LOGD("Dumping information on installed trappoints.");
    TrapPointInfo* current;
    list_for_each_entry(current, &installed_trap_points, installed)
    {
        LOGD("FOUND Trappoint for "PRINT_PTR"(%s)", (uintptr_t)current->target.mem_addr, current->target.thumb ? "THUMB" : "ARM");
        LOGD("->Handler function: "PRINT_PTR, (uintptr_t)current->handler);
        LOGD("->Instruction Size: %d", current->instr_size);
        if(current->target.thumb)
        {
            LOGD("->Code Info:      ");
            LOGD("->->Breakpoint:   0x%04x", current->thumbCode.trap_instruction);
            LOGD("->->Preserved:    0x%04x", current->thumbCode.preserved);
        }
        else
        {
            LOGD("->Code Info:      ");
            LOGD("->->Breakpoint:   0x%08x", current->armCode.trap_instruction);
            LOGD("->->Preserved:    0x%08x", current->armCode.preserved);
        }
    }
}


bool validate_TrapPointInfo_contents(TrapPointInfo * trap)
{
    if (trap == NULL) {
        set_last_error("The trappoint supplied was NULL!");
        return false;
    }
    if (trap->target.call_addr == NULL || trap->target.mem_addr == NULL) {
        set_last_error(
                "A trappoint target address cannot be NULL. Either memory location or address itself was NULL.");
        return false;
    }
    if(trap->target.thumb)
    {
        if(getCodeBaseOffset(trap->target.call_addr) != 1)
        {
            set_last_error("How can the thumb mode be set if the call address doesn't have the least significant bit set?");
            return false;
        }
        if(trap->instr_size != 2)
        {
            set_last_error("If we are in thumb mode, why is the instruction size not 2?");
            return false;
        }
    }
    else
    {
        if(getCodeBaseOffset(trap->target.call_addr) != 0)
        {
            set_last_error("How can the thumb mode not be set if the call address has the least significant bit set?");
            return false;
        }
        if(trap->instr_size != 4)
        {
            set_last_error("If we are not in thumb mode, why is the instruction size not 4?");
            return false;
        }
    }
    if(list_empty(&trap->installed))
    {
        set_last_error("Why is this trappoints installed list_entry empty if its supposed to be linked into the internal list of active trappoints?");
        return false;
    }
    return true;
}


