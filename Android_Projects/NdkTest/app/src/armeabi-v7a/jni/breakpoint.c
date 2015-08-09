//
// Created by Lukas on 8/6/2015.
//

#include "breakpoint.h"

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

void sigill_handler(int signal, siginfo_t* sigInfo, ucontext_t* context)
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
    LOGD("\tCPSR:               "PRINT_PTR, (uintptr_t)state_info->arm_cpsr);

    /*LOGD("Flags: ");
    hexdump_aligned_primitive(&context->uc_flags, sizeof (context->uc_flags), 4, 4);
    LOGD("Link: ");
    hexdump_aligned_primitive(&context->uc_link, sizeof (context->uc_link), 4, 4);
    LOGD("Stack: ");
    hexdump_aligned_primitive(&context->uc_stack, sizeof (context->uc_stack), 4, 4);
    LOGD("MContext: ");
    hexdump_aligned_primitive(&context->uc_mcontext, sizeof (context->uc_mcontext), 4, 4);
    LOGD("Sigmask: ");
    hexdump_aligned_primitive(&context->uc_sigmask, sizeof (context->uc_sigmask), 4, 4);
    LOGD("Sigmask Padding: ");
    hexdump_aligned_primitive(&context->__padding_rt_sigset, sizeof (context->__padding_rt_sigset), 4, 4);
    LOGD("glibc sigset_t __padding: ");
    hexdump_aligned_primitive(&context->__padding, sizeof (context->__padding), 4, 4);
    LOGD("Regspace: ");
    hexdump_aligned_primitive(&context->uc_regspace, sizeof (context->uc_regspace), 4, 4);*/

    LOGD("Arg0: %x", getArg(0, context));
    LOGD("Arg1: %x", getArg(1, context));
    LOGD("Arg2: %x", getArg(2, context));
    LOGD("Arg3: %x", getArg(3, context));
    LOGD("Arg4: %x", getArg(4, context));
    LOGD("Arg5: %x", getArg(5, context));
    LOGD("Arg6: %x", getArg(6, context));
    LOGD("Arg7: %x", getArg(7, context));

    //setArg(0, (uint32_t)"50", context);
    LOGD("Overwritten: ");
    LOGD("Arg0: %x => %s", getArg(0, context), getArg(0, context));
    LOGD("Arg1: %x", getArg(1, context));
    LOGD("Arg2: %x", getArg(2, context));
    LOGD("Arg3: %x", getArg(3, context));
    LOGD("Arg4: %x", getArg(4, context));
    LOGD("Arg5: %x", getArg(5, context));
    LOGD("Arg6: %x", getArg(6, context));
    LOGD("Arg7: %x", getArg(7, context));

    short* target = (short*)getCodeBaseAddress(&atoi);

    LOGD("func before rewriting original instruction ("PRINT_PTR"):", (uintptr_t)target);
    hexdump_aligned_primitive((void*)target, 16, 16, 4);

    target[0] = (short)0x2100;
    __builtin___clear_cache((void*)target, (void*)target + 2);

    LOGD("func after rewriting original instruction ("PRINT_PTR"):", (uintptr_t)target);
    hexdump_aligned_primitive((void*)target, 16, 16, 4);

    LOGD("Returning from SIGILL-Handler.");
}

void sigsegv_handler(int signal, siginfo_t* sigInfo, ucontext_t* context)
{
    mcontext_t *state_info = &(context->uc_mcontext);
    LOGD("Inside the SIGSEGV handler..., signal %d, siginfo_t "PRINT_PTR", context "PRINT_PTR, signal, (uintptr_t) sigInfo, (uintptr_t) context);

    LOGD("\nSigInfo: ");
    LOGD("\tSignal number: %d", sigInfo->si_signo);
    LOGD("\tErrno: %d, Error: %s", sigInfo->si_errno, strerror(sigInfo->si_errno));
    LOGD("\tSignal Code: %d", sigInfo->si_code);

    LOGD("\tFaulting address: "PRINT_PTR, (uintptr_t) sigInfo->si_addr);

    LOGD("\nContext: ");
    LOGD("\tTRAP-Number:        "PRINT_PTR, (uintptr_t) state_info->trap_no);
    LOGD("\tError-Code:         "PRINT_PTR, (uintptr_t) state_info->error_code);
    LOGD("\tOld Mask:           "PRINT_PTR, (uintptr_t) state_info->oldmask);
    LOGD("\tR0:                 "PRINT_PTR, (uintptr_t) state_info->arm_r0);
    LOGD("\tR1:                 "PRINT_PTR, (uintptr_t) state_info->arm_r1);
    LOGD("\tR2:                 "PRINT_PTR, (uintptr_t) state_info->arm_r2);
    LOGD("\tR3:                 "PRINT_PTR, (uintptr_t) state_info->arm_r3);
    LOGD("\tR4:                 "PRINT_PTR, (uintptr_t) state_info->arm_r4);
    LOGD("\tR5:                 "PRINT_PTR, (uintptr_t) state_info->arm_r5);
    LOGD("\tR6:                 "PRINT_PTR, (uintptr_t) state_info->arm_r6);
    LOGD("\tR7:                 "PRINT_PTR, (uintptr_t) state_info->arm_r7);
    LOGD("\tR8:                 "PRINT_PTR, (uintptr_t) state_info->arm_r8);
    LOGD("\tR9:                 "PRINT_PTR, (uintptr_t) state_info->arm_r9);
    LOGD("\tR10:                "PRINT_PTR, (uintptr_t) state_info->arm_r10);
    LOGD("\tFP:                 "PRINT_PTR, (uintptr_t) state_info->arm_fp);
    LOGD("\tIP:                 "PRINT_PTR, (uintptr_t) state_info->arm_ip);
    LOGD("\tSP:                 "PRINT_PTR, (uintptr_t) state_info->arm_sp);
    LOGD("\tLR:                 "PRINT_PTR, (uintptr_t) state_info->arm_lr);
    LOGD("\tPC:                 "PRINT_PTR, (uintptr_t) state_info->arm_pc);
    LOGD("\tCPSR:               "PRINT_PTR, (uintptr_t) state_info->arm_cpsr);

    /*LOGD("Flags: ");
    hexdump_aligned_primitive(&context->uc_flags, sizeof (context->uc_flags), 4, 4);
    LOGD("Link: ");
    hexdump_aligned_primitive(&context->uc_link, sizeof (context->uc_link), 4, 4);
    LOGD("Stack: ");
    hexdump_aligned_primitive(&context->uc_stack, sizeof (context->uc_stack), 4, 4);
    LOGD("MContext: ");
    hexdump_aligned_primitive(&context->uc_mcontext, sizeof (context->uc_mcontext), 4, 4);
    LOGD("Sigmask: ");
    hexdump_aligned_primitive(&context->uc_sigmask, sizeof (context->uc_sigmask), 4, 4);
    LOGD("Sigmask Padding: ");
    hexdump_aligned_primitive(&context->__padding_rt_sigset, sizeof (context->__padding_rt_sigset), 4, 4);
    LOGD("glibc sigset_t __padding: ");
    hexdump_aligned_primitive(&context->__padding, sizeof (context->__padding), 4, 4);
    LOGD("Regspace: ");
    hexdump_aligned_primitive(&context->uc_regspace, sizeof (context->uc_regspace), 4, 4);*/

    LOGD("Arg0: %x", getArg(0, context));
    LOGD("Arg1: %x", getArg(1, context));
    LOGD("Arg2: %x", getArg(2, context));
    LOGD("Arg3: %x", getArg(3, context));
    LOGD("Arg4: %x", getArg(4, context));
    LOGD("Arg5: %x", getArg(5, context));
    LOGD("Arg6: %x", getArg(6, context));
    LOGD("Arg7: %x", getArg(7, context));
}

static struct sigaction old_sigill_action;
static struct sigaction old_sigsegv_action;
void init_breakpoints()
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

    struct sigaction sigill_action;

    sigill_action.sa_sigaction = (void*)sigill_handler;
    sigill_action.sa_flags = SA_SIGINFO;

    if(sigaction(SIGTRAP, &sigill_action, &old_sigill_action) != 0)
    {
        LOGD("Error installing SIGTRAP handler: %s", strerror(errno));
    }

    /*struct sigaction segv_action;

    segv_action.sa_sigaction = (void*)sigsegv_handler;
    segv_action.sa_flags = SA_SIGINFO;

    if(sigaction(SIGSEGV, &segv_action, &old_sigsegv_action) != 0)
    {
        LOGD("Error installing SIGTRAP handler: %s", strerror(errno));
    }*/
}

void run_breakpoint_test(JNIEnv* env)
{
    unsigned short * addr = (unsigned short *)getCodeBaseAddress((void*)&atoi);

    unsigned short val = 0xde01;

    setMemoryProtection(env, addr, 4, true, true, true);
    unsigned short previousValue = *addr;

    LOGI("Calling atoi("PRINT_PTR") before: %d", (uintptr_t)addr, atoi("10"));

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    volatile bool run = false;

    int i = 0;
    while(run)
    {
        i++;
    }

    memcpy(addr, &val, 2);
    __builtin___clear_cache((void*)addr, (void*)addr + 4);

    LOGI("Hexdump after: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    errno = 0;
    int result = atoi("10");
    LOGI("Calling atoi("PRINT_PTR") after: %d [Error: %s]", (uintptr_t)&atoi, result, strerror(errno));

    LOGI("Restoring ...");
    *addr = previousValue;
    LOGI("First short of address: " PRINT_PTR " => %hx after restore.", (uintptr_t)addr, *addr);
    return;
}

void destroy_breakpoints()
{
    if(sigaction(SIGILL, &old_sigill_action, NULL) != 0)
    {
        LOGD("Error uninstalling SIGTRAP handler: %s", strerror(errno));
    }
    /*if(sigaction(SIGSEGV, &old_sigsegv_action, NULL) != 0)
    {
        LOGD("Error uninstalling SIGTRAP handler: %s", strerror(errno));
    }*/
}