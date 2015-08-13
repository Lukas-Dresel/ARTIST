//
// Created by Lukas on 8/6/2015.
//

#include "trappoint.h"
#include "../../main/jni/error.h"

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

uint32_t getArg(unsigned int index, ucontext_t* context)
{
    mcontext_t* state_info = &(context->uc_mcontext);
    switch(index)
    {
        case 0:
            return state_info->arm_r0;
        case 1:
            return state_info->arm_r1;
        case 2:
            return state_info->arm_r2;
        case 3:
            return state_info->arm_r3;

        default:
            return *(((uint32_t*)(state_info->arm_sp)) + (index - 4));
    }
}

void setArg(unsigned int index, uint32_t val, ucontext_t* context)
{
    mcontext_t* state_info = &(context->uc_mcontext);
    switch(index)
    {
        case 0:
            state_info->arm_r0 = val;
            break;
        case 1:
            state_info->arm_r1 = val;
            break;
        case 2:
            state_info->arm_r2 = val;
            break;
        case 3:
            state_info->arm_r3 = val;
            break;

        default:
            *(((uint32_t*)(state_info->arm_sp)) + (index - 4)) = val;
            break;
    }
}

void sigtrap_handler(int signal, siginfo_t* sigInfo, ucontext_t* context)
{
    mcontext_t* state_info = &(context->uc_mcontext);
    LOGD("Inside the SIGILL handler..., signal %d, siginfo_t "PRINT_PTR", context "PRINT_PTR, signal, (uintptr_t)sigInfo, (uintptr_t)context);

    LOGD("\nSigInfo: ");
    LOGD("\tSignal number: %d", sigInfo->si_signo);
    LOGD("\tErrno: %d, Error: %s", sigInfo->si_errno, strerror(sigInfo->si_errno));
    LOGD("\tSignal Code: %d", sigInfo->si_code);
    LOGD("\tFaulting address: "PRINT_PTR"", (uintptr_t)sigInfo->si_addr);

    LOGD("\nContext: ");
    LOGD("\tTRAP-Number:        "PRINT_PTR, (uintptr_t)state_info->trap_no);
    LOGD("\tError-Code:         "PRINT_PTR, (uintptr_t)state_info->error_code);
    LOGD("\tOld Mask:           "PRINT_PTR, (uintptr_t)state_info->oldmask);
    LOGD("\tR0:                 "PRINT_PTR, (uintptr_t)state_info->arm_r0);
    LOGD("\tR1:                 "PRINT_PTR, (uintptr_t)state_info->arm_r1);
    LOGD("\tR2:                 "PRINT_PTR, (uintptr_t)state_info->arm_r2);
    LOGD("\tR3:                 "PRINT_PTR, (uintptr_t)state_info->arm_r3);
    LOGD("\tR4:                 "PRINT_PTR, (uintptr_t)state_info->arm_r4);
    LOGD("\tR5:                 "PRINT_PTR, (uintptr_t)state_info->arm_r5);
    LOGD("\tR6:                 "PRINT_PTR, (uintptr_t)state_info->arm_r6);
    LOGD("\tR7:                 "PRINT_PTR, (uintptr_t)state_info->arm_r7);
    LOGD("\tR8:                 "PRINT_PTR, (uintptr_t)state_info->arm_r8);
    LOGD("\tR9:                 "PRINT_PTR, (uintptr_t)state_info->arm_r9);
    LOGD("\tR10:                "PRINT_PTR, (uintptr_t)state_info->arm_r10);
    LOGD("\tFP:                 "PRINT_PTR, (uintptr_t)state_info->arm_fp);
    LOGD("\tIP:                 "PRINT_PTR, (uintptr_t)state_info->arm_ip);
    LOGD("\tSP:                 "PRINT_PTR, (uintptr_t)state_info->arm_sp);
    LOGD("\tLR:                 "PRINT_PTR, (uintptr_t)state_info->arm_lr);
    LOGD("\tPC:                 "PRINT_PTR, (uintptr_t)state_info->arm_pc);

    uint32_t cpsr = state_info->arm_cpsr;
    LOGD("\tCPSR:               "PRINT_PTR, (uintptr_t)cpsr);
    LOGD("\t\tThumb State:      %d", (cpsr & CPSR_FLAG_THUMB) ? 1 : 0);
    LOGD("\t\tFIQ Ints disable: %d", (cpsr & CPSR_FLAG_DISABLE_FIQ_INTERRUPTS) ? 1 : 0);
    LOGD("\t\tIRQ Ints disable: %d", (cpsr & CPSR_FLAG_DISABLE_IRQ_INTERRUPTS) ? 1 : 0);
    LOGD("\t\tJazelle State:    %d", (cpsr & CPSR_FLAG_JAZELLE) ? 1 : 0);
    LOGD("\t\tUnderflow:        %d", (cpsr & CPSR_FLAG_UNDERFLOW_SATURATION) ? 1 : 0);
    LOGD("\t\tSigned Overflow:  %d", (cpsr & CPSR_FLAG_SIGNED_OVERFLOW) ? 1 : 0);
    LOGD("\t\tCarry:            %d", (cpsr & CPSR_FLAG_CARRY) ? 1 : 0);
    LOGD("\t\tZero:             %d", (cpsr & CPSR_FLAG_ZERO) ? 1 : 0);
    LOGD("\t\tNegative:         %d", (cpsr & CPSR_FLAG_NEGATIVE) ? 1 : 0);

    LOGD("Arg0: %x", getArg(0, context));
    LOGD("Arg1: %x", getArg(1, context));
    LOGD("Arg2: %x", getArg(2, context));
    LOGD("Arg3: %x", getArg(3, context));
    LOGD("Arg4: %x", getArg(4, context));
    LOGD("Arg5: %x", getArg(5, context));
    LOGD("Arg6: %x", getArg(6, context));
    LOGD("Arg7: %x", getArg(7, context));

    void* target = (void*)state_info->arm_pc;
    LOGD("Looking for trappoint for address "PRINT_PTR" in list.", (uintptr_t)target);

    bool found = false;
    TrapPointInfo* current;
    list_for_each_entry(current, &installed_trap_points, installed)
    {
        LOGD("FOUND Trappoint for "PRINT_PTR"(%s)", (uintptr_t)current->target.mem_addr, current->target.thumb ? "THUMB" : "ARM");
        if(current->target.mem_addr == target)
        {
            found = true;
            break;
        }
    }

    if(!found)
    {
        LOGD("Could not find trappoint in list, this SIGTRAP should not originate from us. last: "PRINT_PTR"(mem_addr:"PRINT_PTR")", (uintptr_t)current, (uintptr_t)current->target.mem_addr);
        *((uint32_t*)target) = (uint32_t)-1;
        return;
    }
    LOGD("FOUND trappoint in list! Proceeding with resetting.");

    LOGD("func before rewriting original instruction ("PRINT_PTR"):", (uintptr_t)target);
    hexdump_aligned_primitive((void*)target, 16, 16, 4);

    uninstall_trap_point(current);

    LOGD("func after rewriting original instruction ("PRINT_PTR"):", (uintptr_t)target);
    hexdump_aligned_primitive((void*)target, 16, 16, 4);

    LOGD("Returning from SIGILL-Handler.");
}



void run_trap_point_test(JNIEnv *env)
{
    unsigned short * addr = (unsigned short *)getCodeBaseAddress((void*)&tolower);

    unsigned short val = 0xde01;

    set_memory_protection(addr, 4, true, true, true);
    unsigned short previousValue = *addr;

    char c = 'A';

    LOGI("Calling ["PRINT_PTR"]tolower(%c) before: %d", (uintptr_t)addr, c, tolower(c));

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    waitForDebugger();

    install_trap_point(&tolower, NULL);

    LOGI("Hexdump after: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    errno = 0;
    int result = tolower(c);
    LOGI("Calling ["PRINT_PTR"]tolower(%c) after: %d", (uintptr_t)addr, c, result);

    LOGI("Restoring ...");
    *addr = previousValue;
    LOGI("First short of address: " PRINT_PTR " => %hx after restore.", (uintptr_t)addr, *addr);
    return;
}

TrapPointInfo *install_trap_point(void *addr, CALLBACK handler)
{
    if(addr == NULL)
    {
        return NULL;
    }
    TrapPointInfo* trap = malloc(sizeof(TrapPointInfo));
    if(trap != NULL)
    {
        trap->handler = handler;

        trap->target.call_addr = addr;
        trap->target.mem_addr = getCodeBaseAddress(addr);
        trap->target.thumb = (getCodeBaseOffset(addr) == 1);

        trap->instr_size = trap->target.thumb ? 2 : 4;

        if(set_memory_protection(trap->target.mem_addr, trap->instr_size, true, true, true))
        {
            if(trap->target.thumb)
            {
                trap->thumbCode.bkpt = make_thumb_breakpoint(0x0);

                uint16_t* target = (uint16_t*)trap->target.mem_addr;
                trap->thumbCode.preserved = *target;
                *target = trap->thumbCode.bkpt;
            }
            else
            {
                trap->armCode.bkpt = make_arm_breakpoint(0x0);

                uint32_t* target = (uint32_t*)trap->target.mem_addr;
                trap->armCode.preserved = *target;
                *target = trap->armCode.bkpt;
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

bool validate_TrapPointInfo_contents(TrapPointInfo * trap)
{
    if(trap == NULL)
    {
        set_last_error("The trappoint supplied was NULL!");
        return false;
    }
    if(trap->target.call_addr == NULL || trap->target.mem_addr == NULL)
    {
        set_last_error("A trappoint target address cannot be NULL. Either memory location or address itself was NULL.");
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
