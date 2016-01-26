#include "mainact_native.h"

#include "lib_setup.h"
#include "logging.h"
#include "system_info.h"
#include "../../armeabi/jni/abi.h"
#include "hooking/trappoint_interface.h"
#include "art/oat.h"
#include "abi/abi_interface.h"
#include "art/oat_dump.h"

#include "art/oat_version_dependent/VERSION045/thread.h"


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

void handler_change_bitcount_arg(void *trap_addr, ucontext_t *context, void *args)
{
    LOGI("Inside the trappoint handler...");
    LOGI("Previously Arg1: %x", GetArgument(context, 1));
    SetArgument(context, 1, (uint32_t)args);
    LOGI("After overwriting Arg1: %x", GetArgument(context, 1));
}
JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testHookingAOTCompiledFunction(
        JNIEnv *env, jobject instance)
{
    void* elf_begin = (void*)0x706a8000;
    void* elf_oat_begin = elf_begin + 0x1000; // The oat section usually starts in the next page
    void *elf_oat_end = (void*)0x7368d000;

    LOGI("/data/dalvik-cache/arm/system@framework@boot.oat");
    hexdump_primitive((const void*)elf_oat_begin, 0x10, 0x10);
    hexdump_primitive((const void*)elf_oat_end, 0x10, 0x10);

    struct OatFile      oat;
    struct OatDexFile   core_libart_jar;
    struct OatClass     java_lang_Integer;
    struct OatMethod    int_bitcount;

    if(!oat_Setup(&oat, elf_oat_begin, elf_oat_end))
    {
        return;
    }
    LOGD("Setup our oat file!");
    if(!oat_FindDexFile(&oat, &core_libart_jar, "/system/framework/core-libart.jar"))
    {
        return;
    }
    LOGD("Found OatDexFile /system/framework/core-libart.jar");
    if(!oat_FindClass(&core_libart_jar, &java_lang_Integer, "Ljava/lang/Integer;"))
    {
        return;
    }
    LOGD("Found OatClass java.lang.Integer");
    uint16_t class_def_index = GetIndexForClassDef(core_libart_jar.data.dex_file_pointer, java_lang_Integer.dex_class.class_def);
    if(!oat_FindDirectMethod(&java_lang_Integer, &int_bitcount, "bitCount", "(I)I"))
    {
        return;
    }
    LOGD("Found OatMethod bitcount");
    if(oat_HasQuickCompiledCode(&int_bitcount))
    {
        void* target_addr = oat_GetQuickCompiledEntryPoint(&int_bitcount);
        install_trappoint(target_addr, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL, handler_change_bitcount_arg, (void*)65525);
        LOGD("bitcounts Quick Code Entrypoint:     "PRINT_PTR, (uintptr_t)oat_GetQuickCompiledEntryPoint(&int_bitcount));
        LOGD("bitcounts Quick Code Memory Address: "PRINT_PTR, (uintptr_t)oat_GetQuickCompiledMemoryPointer(&int_bitcount));
    }
}

void* GetCurrentThreadObjectPointer()
{
    void* table;
    asm("mov %0, r9" : "=r" (table));
    return table;
}
JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_dumpQuickEntryPointsInfo(JNIEnv *env, jobject instance)
{
    void *current_thread_pointer = GetCurrentThreadObjectPointer();
    LOGD("The current_thread_pointer lies at "PRINT_PTR, (uintptr_t) current_thread_pointer);

    Thread* thread_info = current_thread_pointer;
    LOGD("The TestSuspend entry contains ["PRINT_PTR"]:"PRINT_PTR, (uintptr_t)&thread_info->tlsPtr_.quick_entrypoints.pTestSuspend, (uintptr_t)thread_info->tlsPtr_.quick_entrypoints.pTestSuspend);
    LOGD("The offset of pTestSuspend to the current_thread structure is %lu", (void*)&thread_info->tlsPtr_.quick_entrypoints.pTestSuspend - current_thread_pointer);
}

JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_testHookingInterpretedFunction(JNIEnv *env, jobject instance)
{
    void* elf_begin = (void*)0x706a8000;
    void* elf_oat_begin = elf_begin + 0x1000; // The oat section usually starts in the next page
    void *elf_oat_end = (void*)0x7368d000;

    LOGI("/data/dalvik-cache/arm/system@framework@boot.oat");
    hexdump_primitive((const void*)elf_oat_begin, 0x10, 0x10);
    hexdump_primitive((const void*)elf_oat_end, 0x10, 0x10);

    struct OatFile      oat;
    struct OatDexFile   core_libart_jar;
    struct OatClass     dalvik_system_BaseDexClassLoader;
    struct OatMethod    findLibrary;

    if(!oat_Setup(&oat, elf_oat_begin, elf_oat_end))
    {
        return;
    }
    LOGD("Setup our oat file!");
    if(!oat_FindDexFile(&oat, &core_libart_jar, "/system/framework/core-libart.jar"))
    {
        return;
    }
    LOGD("Found OatDexFile /system/framework/core-libart.jar");
    if(!oat_FindClass(&core_libart_jar, &dalvik_system_BaseDexClassLoader, "Ldalvik/system/BaseDexClassLoader;"))
    {
        return;
    }
    LOGD("Found OatClass dalvik.system.BaseDexClassLoader");


    uint16_t class_def_index = GetIndexForClassDef(core_libart_jar.data.dex_file_pointer, dalvik_system_BaseDexClassLoader.dex_class.class_def);

    log_dex_file_class_def_contents(core_libart_jar.data.dex_file_pointer, class_def_index);
    log_oat_dex_file_class_def_contents(dalvik_system_BaseDexClassLoader.oat_class_data.backing_memory_address);

    if(!oat_FindVirtualMethod(&dalvik_system_BaseDexClassLoader, &findLibrary, "findLibrary", "(Ljava/lang/String;)Ljava/lang/String;"))
    {
        return;
    }
    LOGD("Found OatMethod findLibrary");
    log_dex_file_method_id_contents(core_libart_jar.data.dex_file_pointer, GetIndexForMethodID(core_libart_jar.data.dex_file_pointer, findLibrary.dex_method.method_id));
    log_oat_dex_file_method_offsets_content(oat.header, &dalvik_system_BaseDexClassLoader.oat_class_data, findLibrary.dex_method.class_method_idx);
}
/*JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_testHookingInterpretedFunction(JNIEnv *env, jobject instance)
{
    void* elf_begin = (void*)0x706a8000;
    void* elf_oat_begin = elf_begin + 0x1000; // The oat section usually starts in the next page
    void *elf_oat_end = (void*)0x7368d000;

    LOGI("/data/dalvik-cache/arm/system@framework@boot.oat");
    hexdump_primitive((const void*)elf_oat_begin, 0x10, 0x10);
    hexdump_primitive((const void*)elf_oat_end, 0x10, 0x10);

    struct OatFile      oat;
    struct OatDexFile   core_libart_jar;
    struct OatClass     dalvik_system_DexFile;
    struct OatMethod    defineClassNative;

    if(!oat_Setup(&oat, elf_oat_begin, elf_oat_end))
    {
        return;
    }
    LOGD("Setup our oat file!");
    if(!oat_FindDexFile(&oat, &core_libart_jar, "/system/framework/core-libart.jar"))
    {
        return;
    }
    LOGD("Found OatDexFile /system/framework/core-libart.jar");
    if(!oat_FindClass(&core_libart_jar, &dalvik_system_DexFile, "Ldalvik/system/DexFile;"))
    {
        return;
    }
    LOGD("Found OatClass dalvik.system.DexFile");


    uint16_t class_def_index = GetIndexForClassDef(core_libart_jar.data.dex_file_pointer, dalvik_system_DexFile.dex_class.class_def);

    log_dex_file_class_def_contents(core_libart_jar.data.dex_file_pointer, class_def_index);

    if(!oat_FindVirtualMethod(&dalvik_system_DexFile, &defineClassNative, "defineClassNative", "(Ljava/lang/String;)Ljava/lang/String;"))
    {
        return;
    }
    LOGD("Found OatMethod findLibrary");
    log_dex_file_method_id_contents(core_libart_jar.data.dex_file_pointer, GetIndexForMethodID(core_libart_jar.data.dex_file_pointer, defineClassNative.dex_method.method_id));
    log_oat_dex_file_method_offsets_content(oat.header, &dalvik_system_DexFile.oat_class_data, defineClassNative.dex_method.class_method_idx);
}*/

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
        install_trappoint((void*)next_instr.call_addr, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL, handler_step_function, arg);
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



JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_dumpMainOatInternals(JNIEnv *env, jobject instance)
{
    void* elf_start = (void*)0x706a8000;
    void* oat_start = elf_start + 0x1000;

    void* elf_end = (void*)0x7368d000;
    /*
     * /data/dalvik-cache/arm/system@framework@boot.oat
     */
    LOGI("/data/dalvik-cache/arm/system@framework@boot.oat");
    hexdump_primitive((const void*)elf_start, 0x10, 0x10);
    hexdump_primitive((const void*)oat_start, 0x10, 0x10);

    log_elf_oat_file_info(oat_start, elf_end);
}
#ifdef __cplusplus
}
#endif