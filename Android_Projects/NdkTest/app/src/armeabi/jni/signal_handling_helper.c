//
// Created by Lukas on 8/14/2015.
//

#include "../../main/jni/signal_handling_helper.h"
#include "../../main/jni/logging.h"
#include "cpsr_util.h"

uint32_t get_argument(ucontext_t* c, unsigned int index)
{
    mcontext_t* state_info = &(c->uc_mcontext);
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

void set_argument(ucontext_t* c, unsigned int index, uint32_t val)
{
    mcontext_t* state_info = &(c->uc_mcontext);
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

void log_siginfo_content(siginfo_t* info)
{
    LOGD("\nSigInfo: ");
    LOGD("\tSignal number: %d", info->si_signo);
    LOGD("\tErrno: %d, Error: %s", info->si_errno, strerror(info->si_errno));
    LOGD("\tSignal Code: %d", info->si_code);
    LOGD("\tFaulting address: "PRINT_PTR"", (uintptr_t)info->si_addr);
}
void log_mcontext_content(mcontext_t* state_info)
{
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
    LOGI("\t->Thumb State:      %d", (cpsr & CPSR_FLAG_THUMB) ? 1 : 0);
    LOGI("\t->FIQ Ints disable: %d", (cpsr & CPSR_FLAG_DISABLE_FIQ_INTERRUPTS) ? 1 : 0);
    LOGI("\t->IRQ Ints disable: %d", (cpsr & CPSR_FLAG_DISABLE_IRQ_INTERRUPTS) ? 1 : 0);
    LOGI("\t->Jazelle State:    %d", (cpsr & CPSR_FLAG_JAZELLE) ? 1 : 0);
    LOGI("\t->Underflow:        %d", (cpsr & CPSR_FLAG_UNDERFLOW_SATURATION) ? 1 : 0);
    LOGI("\t->Signed Overflow:  %d", (cpsr & CPSR_FLAG_SIGNED_OVERFLOW) ? 1 : 0);
    LOGI("\t->Carry:            %d", (cpsr & CPSR_FLAG_CARRY) ? 1 : 0);
    LOGI("\t->Zero:             %d", (cpsr & CPSR_FLAG_ZERO) ? 1 : 0);
    LOGI("\t->Negative:         %d", (cpsr & CPSR_FLAG_NEGATIVE) ? 1 : 0);
}
