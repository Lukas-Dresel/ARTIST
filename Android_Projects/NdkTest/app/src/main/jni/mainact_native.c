#include "mainact_native.h"

#include "debug_util.h"
#include "trappoint_interface.h"
#include "signal_handling_helper.h"
#include "lib_setup.h"
#include "logging.h"
#include "util.h"
#include "memory.h"

#ifdef __cplusplus
extern "C"
{
#endif

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


void trap_point_test_handler(void* trap_addr, ucontext_t* context, void* args)
{
    LOGI("Inside the trappoint handler...");
    LOGI("Previously Arg1: %x", get_argument(context, 1));
    set_argument(context, 1, (uint32_t)"HALA HALA HALA!");
    LOGI("After overwriting Arg1: %x", get_argument(context, 1));
}

void run_trap_point_test(JNIEnv *env)
{
    void* func = (*env)->NewStringUTF;
    void* addr = (unsigned short *)getCodeBaseAddress(func);

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    waitForDebugger();

    dump_installed_trappoints_info();

    LOGI("Installing hook for FindClass("PRINT_PTR")", (uintptr_t)func);

    install_trap_point(func, TRAP_METHOD_SIGILL_KNOWN_ILLEGAL_INSTR, &trap_point_test_handler, NULL);

    dump_installed_trappoints_info();

    LOGI("Hexdump after trappoint installation: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    char* str = "HOLO HOLO HOLO!";
    LOGI("Calling find class with args ("PRINT_PTR", "PRINT_PTR")", (uintptr_t)env, (uintptr_t)str);

    jstring jstr = (*env)->NewStringUTF(env, str);

    dump_installed_trappoints_info();

    LOGI("Hexdump after execution: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    const char* resultingString = (*env)->GetStringUTFChars(env, jstr, NULL);
    LOGD("The string contains the value \"%s\".", resultingString);
    (*env)->ReleaseStringUTFChars(env, jstr, resultingString);

    return;
}




/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testBreakpointAtoi
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testBreakpointAtoi (JNIEnv * env, jobject this)
{
    run_trap_point_test(env);
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