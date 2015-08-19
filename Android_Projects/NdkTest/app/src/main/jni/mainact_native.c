#include "mainact_native.h"

#include "debug_util.h"
#include "trappoint_interface.h"
#include "signal_handling_helper.h"
#include "lib_setup.h"
#include "logging.h"
#include "util.h"
#include "memory.h"
#include "system_info.h"
#include "oat_parser.h"

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


void handler_change_NewStringUTF_arg(void* trap_addr, ucontext_t* context, void* args)
{
    LOGI("Inside the trappoint handler...");
    LOGI("Previously Arg1: %x", get_argument(context, 1));
    set_argument(context, 1, (uint32_t)"HALA HALA HALA!");
    LOGI("After overwriting Arg1: %x", get_argument(context, 1));
}
void handler_hello_world(void* trap_addr, ucontext_t* context, void* args)
{
    LOGD("Inside the trappoint handler...");
    LOGD("Hello, World!");
}

void run_trap_point_test(JNIEnv *env)
{
    void* func = (*env)->NewStringUTF;
    void* addr = (unsigned short *) get_code_base_address(func);

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    dump_installed_trappoints_info();

    LOGI("Installing hook for FindClass("PRINT_PTR")", (uintptr_t)func);

    install_trappoint(func, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL,
                      &handler_change_NewStringUTF_arg, NULL);

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

JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwritingJavaCode(JNIEnv *env, jobject instance)
{
    waitForDebugger();

    dump_process_memory_map();
    void*       elf_oat_base = (void*)0x7740b000;
    hexdump()
    void*       elf_oat_end  = (void*)0xAFFFFFFF;
    log_elf_oat_file_info(elf_oat_base, elf_oat_end);
    uint32_t    offset_calling_func_return = 0x5d5f34 + 1/*Thumb*/;
    uint32_t    interesting_offset = 0x5d5f80;

    LOGD("Installing higher function trappoint at "PRINT_PTR, (uintptr_t)elf_oat_base + offset_calling_func_return);
    install_trappoint(elf_oat_base + offset_calling_func_return,
                      TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL, &handler_hello_world,
                      NULL);

    LOGD("Trappoint installed, let's see if it worked.");
}


/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testBreakpointAtoi
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testBreakpointAtoi (JNIEnv * env, jobject instance)
{
    waitForDebugger();

    run_trap_point_test(env);
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpProcessMemoryMap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpProcessMemoryMap (JNIEnv * env, jobject this)
{
    dump_process_memory_map();
}

#ifdef __cplusplus
}
#endif