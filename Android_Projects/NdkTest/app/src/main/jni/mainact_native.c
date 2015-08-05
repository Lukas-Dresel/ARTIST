#include "mainact_native.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
    LOGD("Inside the SIGTRAP handler..., signal %d, siginfo_t "PRINT_PTR", context "PRINT_PTR, signal, (uintptr_t)sigInfo, (uintptr_t)context);

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

    LOGD("Arg0: %lx", getArg(0, context));
    LOGD("Arg1: %lx", getArg(1, context));
    LOGD("Arg2: %lx", getArg(2, context));
    LOGD("Arg3: %lx", getArg(3, context));
    LOGD("Arg4: %lx", getArg(4, context));
    LOGD("Arg5: %lx", getArg(5, context));
    LOGD("Arg6: %lx", getArg(6, context));
    LOGD("Arg7: %lx", getArg(7, context));

    setArg(0, 666, context);
    LOGD("Overwritten: ");
    LOGD("Arg0: %lx", getArg(0, context));
    LOGD("Arg1: %lx", getArg(1, context));
    LOGD("Arg2: %lx", getArg(2, context));
    LOGD("Arg3: %lx", getArg(3, context));
    LOGD("Arg4: %lx", getArg(4, context));
    LOGD("Arg5: %lx", getArg(5, context));
    LOGD("Arg6: %lx", getArg(6, context));
    LOGD("Arg7: %lx", getArg(7, context));

    *((short*)state_info->arm_pc) = 0xb598;
    LOGD("func after rewriting original instruction: ");
    hexdump_aligned_primitive((void*)state_info->arm_pc, 16, 16, 4);
}

jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGI("Loading library...");

    struct sigaction old_action;
    struct sigaction trap_action;

    trap_action.sa_sigaction = sigtrap_handler;
    trap_action.sa_flags = SA_SIGINFO;


    if(sigaction(SIGTRAP, &trap_action, &old_action) != 0)
    {
        LOGD("Error installing SIGTRAP handler: %s", strerror(errno));
    }

    init();

    return JNI_VERSION_1_6;
}
void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    LOGI("Unloading library.");
    destroy();
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testBreakpointAtoi
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testBreakpointAtoi (JNIEnv * env, jobject this)
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
    while(!run)
    {
        i++;
    }

    memcpy(addr, &val, 2);
    __builtin___clear_cache((void*)addr, (void*)addr + 1);

    LOGI("Hexdump after: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    LOGI("Calling atoi("PRINT_PTR") after: %d", (uintptr_t)&atoi, atoi("10"));

    LOGI("Restoring ...");
    *addr = previousValue;
    LOGI("First short of address: " PRINT_PTR " => %hx after restore.", (uintptr_t)addr, *addr);
    return;
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpProcessMemoryMap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpProcessMemoryMap (JNIEnv * env, jobject this)
{
    char buff[3200];

    FILE* fp = fopen("/proc/self/maps", "r");
    while( fgets (buff, sizeof(buff), fp) != NULL )
    {
        LOGI("MemoryMap: %s", buff);
    }
    if(feof(fp))
    {
        LOGI("MemoryMap: %s", "#################### END OF MEMORY MAP ###################");
    }
    if(ferror(fp))
    {
        LOGI("MemoryMap: %s", "#################### ERROR READING /proc/self/maps ###################");
    }
    fclose(fp);
}

#ifdef __cplusplus
}
#endif