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
jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGI("Loading library...");
    init();

    return JNI_VERSION_1_6;
}
void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    LOGI("Unloading library.");
    destroy();
}

int dummyFunc(int a, int b)
{
    LOGD("A: %d, B: %d", a, b);
    int result = (a + b) * 2;
    LOGD("(A + B) * 2 = %d", result);
    return result;
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

    LOGI("Calling dummyFunc("PRINT_PTR")(10, 100) before: %d", (uintptr_t)&dummyFunc, dummyFunc(10, 100));

    LOGI("Hexdump before: ");
    hexdump_aligned(env, "Hexdump", addr, 16, 8);

    LOGI("First Byte of address: " PRINT_PTR " => %02x before write.", (uintptr_t)addr, *addr);
    *addr = 0xFF;
    LOGI("First Byte of address: " PRINT_PTR " => %02x after write.", (uintptr_t)addr, *addr);

    LOGI("Hexdump after: ");
    hexdump_aligned(env, "Hexdump", addr, 16, 8);
    __builtin___clear_cache(addr, addr + 1);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    LOGI("Calling dummyFunc("PRINT_PTR")(10, 100) after: %d", (uintptr_t)&dummyFunc, dummyFunc(10, 100));

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
    hexdump_aligned(env, "Hexdump", addr, 16, 8);

    *addr = 0x41;

    LOGI("Hexdump after: ");
    hexdump_aligned(env, "Hexdump", addr, 16, 8);
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

    LOGI("Calling atoi("PRINT_PTR") before: %d", (uintptr_t)addr, atoi("10"));

    LOGI("Hexdump before: ");
    hexdump_aligned(env, "Hexdump", addr, 16, 8);

    memcpy(addr, &val, 2);
    __builtin___clear_cache((void*)addr, (void*)addr + 1);

    LOGI("Hexdump after: ");
    hexdump_aligned(env, "Hexdump", addr, 16, 8);

    LOGI("We expect to fail here because opcodes are invalid, if this doesn't raise a SEGFAULT we fucked up.");
    LOGI("Calling atoi("PRINT_PTR") after: %d", (uintptr_t)&atoi, atoi("10"));

    LOGI("Restoring ...");
    *addr = previousValue;
    LOGI("First short of address: " PRINT_PTR " => %hx after restore.", (uintptr_t)addr, *addr);
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
    hexdump_aligned(env, "Hexdump", addr, 16, 8);

    *addr = 0x41;

    LOGI("Hexdump after: ");
    hexdump_aligned(env, "Hexdump", addr, 16, 8);

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
int hook_atoi(char* str)
{
    LOGI("We are inside atoi right now! o.O Args: %s", str);
    disable_inline_function_hook(atoi_hook);

    hexdump_aligned_primitive("Hexdump", (void*)&str, 32, 8);

    errno = 0;
    int realResult = atoi(str);
    int savedErrno = errno;
    if(realResult == 0)
    {
        LOGI("Possible error parsing \"%s\" to int. Error: %s", str, strerror(errno));
    }
    enable_inline_function_hook(atoi_hook);
    errno = savedErrno;
    return realResult * 10;
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testHookingAtoi
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testHookingAtoi (JNIEnv * env, jobject this)
{
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