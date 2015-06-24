#include "mainact_native.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*

JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

*/

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

int dummyFunc(int a, int b, int c, int d, int e, int f)
{
    LOGD("%d", a+b+c+d+e+f);
    return a+b+c+d+e+f;
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testOverwriteOwnFunction
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwriteOwnFunction (JNIEnv * env, jobject this)
{
    unsigned char * addr = (void*)&dummyFunc;

    setMemoryProtection(env, addr, 1, true, true, true);
    char previousValue = *addr;

    LOGI("Calling dummyFunc("PRINT_PTR")(10, 20, 30, 40, 50, 60) before: %d", (uintptr_t)&dummyFunc, dummyFunc(10, 20, 30, 40, 50, 60));

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 16, 4);

    LOGI("First Byte of address: " PRINT_PTR " => %02x before write.", (uintptr_t)addr, *addr);
    *addr = 0xFF;
    LOGI("First Byte of address: " PRINT_PTR " => %02x after write.", (uintptr_t)addr, *addr);

    LOGI("Hexdump after: ");
    hexdump_aligned(env, addr, 16, 16, 4);
    __builtin___clear_cache(addr, addr + 1);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    LOGI("Calling dummyFunc("PRINT_PTR")(10, 20, 30, 40, 50, 60) after: %d", (uintptr_t)&dummyFunc, dummyFunc(10, 20, 30, 40, 50, 60));

    LOGI("Restoring ...");
    *addr = previousValue;
    LOGI("First Byte of address: " PRINT_PTR " => %02x after restore.", (uintptr_t)addr, *addr);
    return;
}

/*
 *  AtoI Disassembled
 *  00 21       -   0000 0000 0010 0001     -   00000 00000 100 001 b   -   LSL     0   r4  r1      =>  LSL     r1, r4, #0          r1 = r4;
 *  0a 22       -   0000 1010 0010 0010     -   00001 01000 100 010 b   -   LSR     8   r4  r2      =>  LSR     r2, r4, #8          r2 = r4 >> 8;
 *  00 f0       -   0000 0000 1111 0000     -   00000 00011 110 000 b   -   LSL     3   r6  r0      =>  LSL     r0, r6, #3          r0 = r6 << 3;
 *  b8 ba       -
 *
 *
 *
 *
 *
 *
 */


/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testOverwriteAtoi
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwriteAtoi (JNIEnv * env, jobject this)
{
    unsigned char * addr = (unsigned char *)&atoi;

    setMemoryProtection(env, addr, 1, true, true, true);
    char previousValue = *addr;

    LOGI("Calling atoi("PRINT_PTR") before: %d", (uintptr_t)&atoi, atoi("10"));

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 16, 4);

    *addr = 0x41;

    LOGI("Hexdump after: ");
    hexdump_aligned(env, addr, 16, 16, 4);
    __builtin___clear_cache((void*)addr, (void*)addr + 1);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    LOGI("Calling atoi("PRINT_PTR") after: %d", (uintptr_t)&atoi, atoi("10"));

    LOGI("Restoring ...");
    *addr = previousValue;
    LOGI("First Byte of address: " PRINT_PTR " => %02x after restore.", (uintptr_t)addr, *addr);
    return;
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

    char* str = "10";
    LOGI("Atoi Argument: "PRINT_PTR" => \"%s\"("PRINT_PTR")", (uintptr_t)&str, str, (uintptr_t)str);

    LOGI("Before Hooking: atoi(\"10\") = %d", atoi(str));

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 16, 4);

    memcpy(addr, &val, 2);
    __builtin___clear_cache((void*)addr, (void*)addr + 1);

    LOGI("Hexdump after: ");
    hexdump_aligned(env, addr, 16, 16, 4);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    LOGI("Calling atoi("PRINT_PTR") after: %d", (uintptr_t)&atoi, atoi("10"));

    LOGI("Restoring ...");
    *addr = previousValue;
    LOGI("First short of address: " PRINT_PTR " => %hx after restore.", (uintptr_t)addr, *addr);
    return;
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testBreakpoint
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testBreakpoint (JNIEnv * env, jobject this)
{
    unsigned short * addr = (unsigned short *)getCodeBaseAddress((void*)&dummyFunc);

    unsigned short val = 0xde01;

    setMemoryProtection(env, addr, 1, true, true, true);
    char previousValue = *addr;

    LOGI("Calling dummyFunc("PRINT_PTR")(10, 20, 30, 40, 50, 60) before: %d", (uintptr_t)&dummyFunc, dummyFunc(10, 20, 30, 40, 50, 60));

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 16, 4);

    memcpy(addr, &val, 2);
    __builtin___clear_cache((void*)addr, (void*)addr + 1);
    LOGI("First Byte of address: " PRINT_PTR " => %02x after write.", (uintptr_t)addr, *addr);

    LOGI("Hexdump after: ");
    hexdump_aligned(env, addr, 16, 16, 4);
    __builtin___clear_cache(addr, addr + 1);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    LOGI("Calling dummyFunc("PRINT_PTR")(10, 20, 30, 40, 50, 60) after: %d", (uintptr_t)&dummyFunc, dummyFunc(10, 20, 30, 40, 50, 60));

    LOGI("Restoring ...");
    *addr = previousValue;
    LOGI("First Byte of address: " PRINT_PTR " => %02x after restore.", (uintptr_t)addr, *addr);
    return;
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testOverwriteJNIEnvFunc
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwriteJNIEnvFunc (JNIEnv * env, jobject this)
{
    unsigned char * addr = (unsigned char *)(*env)->NewBooleanArray;

    setMemoryProtection(env, addr, 1, true, true, true);
    char previousValue = *addr;

    LOGI("We attempt to overwrite JNI Function NewBooleanArray ("PRINT_PTR").", (uintptr_t)addr);

    LOGI("Calling NewBooleanArray("PRINT_PTR") before: "PRINT_PTR, (uintptr_t)(*env)->NewBooleanArray, (uintptr_t)(*env)->NewBooleanArray(env, 10));

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 16, 4);

    *addr = 0x41;

    LOGI("Hexdump after: ");
    hexdump_aligned(env, addr, 16, 16, 4);

    __builtin___clear_cache((void*)addr, (void*)addr + 1);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    LOGI("Calling NewBooleanArray("PRINT_PTR") after: "PRINT_PTR, (uintptr_t)(*env)->NewBooleanArray, (uintptr_t)(*env)->NewBooleanArray(env, 10));

    *addr = previousValue;

    return;
}


static InlineFunctionHook* android_log_print_hook;
int _hook__android_log_print(int prio, const char *tag, const char *fmt, ...)
{
    disable_inline_function_hook(android_log_print_hook);

    __android_log_print(ANDROID_LOG_INFO, "jni_mainact_native", "__android_log_print called with format string \"%s\"", fmt);

    va_list varargs;
    int ret;

    va_start(varargs, fmt);
    ret = __android_log_vprint(prio, tag, fmt, varargs);
    va_end(varargs);

    enable_inline_function_hook(android_log_print_hook);

    return ret;
}

static InlineFunctionHook* atoi_hook;
int hook_atoi(const char* str)
{
    LOGI("We are inside atoi right now! o.O");
    LOGI("Args: %s", str);
    disable_inline_function_hook(atoi_hook);

    LOGD("Dumping current stack frame around "PRINT_PTR":", (uintptr_t)&str);
    hexdump_aligned_primitive((void*)&str, 64, 4, 4);

    errno = 0;
    int realResult = atoi(str);
    int savedErrno = errno;
    if(realResult == 0)
    {
        LOGI("Possible error parsing \"%s\" to int. Error: %s", str, strerror(errno));
    }
    enable_inline_function_hook(atoi_hook);
    errno = savedErrno;
    return 42;
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testHookingAtoi
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testHookingAtoi (JNIEnv * env, jobject this)
{
    LOGI("Argument addresses: &env("PRINT_PTR"), &this("PRINT_PTR")", (uintptr_t)&env, (uintptr_t)&this);
    LOGI("Hook installation starting ...");
    atoi_hook = install_inline_function_hook(env, "atoi", &atoi, "hook_atoi", &hook_atoi);
    if(atoi_hook == NULL)
    {
        return;
    }
    LOGI("Hook installation done.");
    print_full_inline_function_hook_info(env);

    char* str = "10";
    LOGI("Address of parsestring: \"%s\"("PRINT_PTR")", str, (uintptr_t)&str);

    LOGI("Before Hooking: atoi(\"10\") = %d", atoi(str));
    if(!enable_inline_function_hook(atoi_hook))
    {
        return;
    }
    LOGI("Enabled Hook.");
    print_full_inline_function_hook_info(env);

    LOGI("After Hooking: atoi(\"10\") = %d", atoi(str));
    if(!disable_inline_function_hook(atoi_hook))
    {
        return;
    }
    LOGI("Disabled Hook.");
    print_full_inline_function_hook_info(env);
    if(!uninstall_inline_function_hook(atoi_hook))
    {
        return;
    }
    LOGD("Dumping current stack frame around "PRINT_PTR":", (uintptr_t)&env);
    hexdump_aligned_primitive((void*)&env - 16, 64, 4, 4);

    LOGI("Done hooking atoi!");
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testHookingAndroidLogPrint
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testHookingAndroidLogPrint (JNIEnv * env, jobject this)
{
    LOGI("Hook installation starting ...");

    android_log_print_hook = install_inline_function_hook(env, "__android_log_print", &__android_log_print, "_hook__android_log_print", &_hook__android_log_print);
    RETURN_ON_EXCEPTION(env,,);

    LOGI("Hook installation done.");

    enable_inline_function_hook(android_log_print_hook);

    RETURN_ON_EXCEPTION(env,,);

    LOGI("Hululululululululululululululu. %s", "(abcd)");

    disable_inline_function_hook(android_log_print_hook);
    RETURN_ON_EXCEPTION(env,,);

    LOGI("Disabled hook for __android_log_print.");

    uninstall_inline_function_hook(android_log_print_hook);
    RETURN_ON_EXCEPTION(env,,);
}




/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpJEnvContent
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpJEnvContent (JNIEnv * env, jobject this)
{
    LOGI("%-32s "PRINT_PTR, "printf:", (uintptr_t)printf);

    LOGI("%-32s "PRINT_PTR, "reserved0:", (uintptr_t)(*env)->reserved0);
    LOGI("%-32s "PRINT_PTR, "reserved1:", (uintptr_t)(*env)->reserved1);
    LOGI("%-32s "PRINT_PTR, "reserved2:", (uintptr_t)(*env)->reserved2);
    LOGI("%-32s "PRINT_PTR, "reserved3:", (uintptr_t)(*env)->reserved3);

    LOGI("%-32s "PRINT_PTR, "GetVersion:", (uintptr_t)(*env)->GetVersion);

    LOGI("%-32s "PRINT_PTR, "DefineClass:", (uintptr_t)(*env)->DefineClass);
    LOGI("%-32s "PRINT_PTR, "FindClass:", (uintptr_t)(*env)->FindClass);

    LOGI("%-32s "PRINT_PTR, "FromReflectedMethod:", (uintptr_t)(*env)->FromReflectedMethod);
    LOGI("%-32s "PRINT_PTR, "FromReflectedField:", (uintptr_t)(*env)->FromReflectedField);
    LOGI("%-32s "PRINT_PTR, "ToReflectedMethod:", (uintptr_t)(*env)->ToReflectedMethod);
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
/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpProcAlignmentMap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpProcAlignmentMap (JNIEnv * env, jobject this)
{
    char buff[3200];

    FILE* fp = fopen("/proc/cpu/alignment", "r");
    while( fgets (buff, sizeof(buff), fp) != NULL )
    {
        LOGI("AlignmentMap: %s", buff);
    }
    if(feof(fp))
    {
        LOGI("AlignmentMap: %s", "#################### END OF ALIGNMENT MAP ###################");
    }
    if(ferror(fp))
    {
        LOGI("AlignmentMap: %s", "#################### ERROR READING /proc/cpu/alignment ###################");
    }
    fclose(fp);
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testLibDLCapabilities
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testLibDLCapabilities(JNIEnv *env, jobject this)
{
    void* libHandle = dlopen("libc.so", RTLD_LAZY);
    LOGI("[DLOpen]: Found libc to be at "PRINT_PTR, (uintptr_t)libHandle);
    if(libHandle == NULL)
    {
        LOGE("Could not load libc! Error: %s", strerror(errno));
        throwNewJNIException(env, "java/lang/NullPointerException", "Could not load libc!");
    }
    void* addrOfAtoi = dlsym(libHandle, "atoi");
    LOGI("[DLSym]: Found atoi to be at "PRINT_PTR, (uintptr_t)addrOfAtoi);

    LOGD("Dumping memory around atoi");
    hexdump_aligned(env, addrOfAtoi, 32, 8, 4);

    dlclose(libHandle);
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    setMemoryProtection
 * Signature: (IIZZZ)V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_setMemoryProtection (JNIEnv * env, jobject this, jlong address, jlong size, jboolean read, jboolean write, jboolean execute)
{
    setMemoryProtection(env, (void*)address, size, read, write, execute);
}

#ifdef __cplusplus
}
#endif