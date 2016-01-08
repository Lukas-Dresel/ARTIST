#include "mainact_native.h"

#include "debug_util.h"
#include "trappoint_interface.h"
#include "signal_handling_helper.h"
#include "lib_setup.h"
#include "logging.h"
#include "memory.h"
#include "system_info.h"
#include "abi_interface.h"
#include "../../armeabi/jni/abi.h"
#include "oat.h"


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


void handler_change_NewStringUTF_arg(void *trap_addr, ucontext_t *context, void *args)
{
    LOGI("Inside the trappoint handler...");
    LOGI("Previously Arg1: %x", GetArgument(context, 1));
    SetArgument(context, 1, (uint32_t) "HALA HALA HALA!");
    LOGI("After overwriting Arg1: %x", GetArgument(context, 1));
}

void handler_hello_world(void *trap_addr, ucontext_t *context, void *args)
{
    LOGD("Inside the trappoint handler...");
    LOGD("Hello, World!");
}

void run_trap_point_test(JNIEnv *env)
{
    void *func = (*env)->NewStringUTF;
    void *addr = (unsigned short *)((uint64_t)func & ~0x1);

    LOGI("Hexdump before: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    dump_installed_trappoints_info();

    LOGI("Installing hook for FindClass("
                 PRINT_PTR
                 ")", (uintptr_t) func);

    install_trappoint(func, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL,
                      &handler_change_NewStringUTF_arg, NULL);

    dump_installed_trappoints_info();

    LOGI("Hexdump after trappoint installation: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    char *str = "HOLO HOLO HOLO!";
    LOGI("Calling find class with args ("
                 PRINT_PTR
                 ", "
                 PRINT_PTR
                 ")", (uintptr_t) env, (uintptr_t) str);

    jstring jstr = (*env)->NewStringUTF(env, str);

    dump_installed_trappoints_info();

    LOGI("Hexdump after execution: ");
    hexdump_aligned(env, addr, 16, 8, 8);

    const char *resultingString = (*env)->GetStringUTFChars(env, jstr, NULL);
    LOGD("The string contains the value \"%s\".", resultingString);
    (*env)->ReleaseStringUTFChars(env, jstr, resultingString);

    return;
}


JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwritingJavaCode(
        JNIEnv *env, jobject instance)
{
    void* elf_begin = (void*)0x711ec000;
    void* elf_oat_begin = elf_begin + 0x1000; // The oat section usually starts in the next page
    void *elf_oat_end = (void*)0xFFFFFF00;

    struct OatFile oat;
    struct OatDexFile oat_dex;

    if(!oat_Setup(&oat, elf_oat_begin, elf_oat_end))
    {
        return;
    }

    if(!oat_FindDexFile(&oat, &oat_dex, "asdf"))
    {

    }

}

struct Step_Handler_Args
{
    void* func_start;
    uint32_t func_size;
};
static struct Step_Handler_Args step_handler_args;
void handler_step_function(void *trap_addr, ucontext_t *context, void *args)
{
    struct Step_Handler_Args* arg = (struct Step_Handler_Args*)args;

    void* start = arg->func_start;
    void* end = arg->func_start + arg->func_size;

    struct InstructionInfo next_instr = ExtractNextExecutedInstruction(context);
    LOGD("SingleStep-Handler: Next instruction assumed to be: "PRINT_PTR, (uintptr_t)next_instr.call_addr);
    if(next_instr.call_addr > start && next_instr.call_addr < end)
    {
        LOGD("Attempting to install next trappoint in single step chain. ");
        install_trappoint(next_instr.call_addr, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL, handler_step_function, arg);
    }
    else
    {
        LOGD("Stopping singlestepping, as adress is out of range.");
    }
}

JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testSingleStep(
        JNIEnv *env, jobject instance)
{

}

JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_tryNukeDexContent(
        JNIEnv *env, jobject instance)
{

}



/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    testBreakpointAtoi
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testBreakpointAtoi(JNIEnv *env,
                                                                                      jobject instance)
{
    waitForDebugger();

    run_trap_point_test(env);
}

/*
 * Class:     com_example_lukas_ndktest_MainActivity
 * Method:    dumpProcessMemoryMap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_dumpProcessMemoryMap(JNIEnv *env,
                                                                                        jobject this)
{
    dump_process_memory_map();
}

#ifdef __cplusplus
}
#endif