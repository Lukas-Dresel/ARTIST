#include "mainact_native.h"

#include "lib_setup.h"
#include "logging.h"
#include "system_info.h"
#include "../../armeabi/jni/abi.h"
#include "hooking/trappoint_interface.h"
#include "art/oat.h"
#include "abi/abi_interface.h"
#include "art/oat_dump.h"
#include "art/leb128.h"
#include "art/modifiers.h"
#include "util/memory.h"


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
}

JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_testHookingInterpretedFunction(JNIEnv *env, jobject instance) {
    void *elf_begin = (void *) 0x706a8000;
    void *elf_oat_begin = elf_begin + 0x1000; // The oat section usually starts in the next page
    void *elf_oat_end = (void *) 0x7368d000;

    LOGI("/data/dalvik-cache/arm/system@framework@boot.oat");
    hexdump_primitive((const void *) elf_oat_begin, 0x10, 0x10);
    hexdump_primitive((const void *) elf_oat_end, 0x10, 0x10);

    struct OatFile oat;
    struct OatDexFile core_libart_jar;
    struct OatClass dalvik_system_BaseDexClassLoader;
    struct OatMethod findLibrary;

    if (!oat_Setup(&oat, elf_oat_begin, elf_oat_end)) {
        return;
    }
    LOGD("Setup our oat file!");
    if (!oat_FindDexFile(&oat, &core_libart_jar, "/system/framework/core-libart.jar")) {
        return;
    }
    LOGD("Found OatDexFile /system/framework/core-libart.jar");
    if (!oat_FindClass(&core_libart_jar, &dalvik_system_BaseDexClassLoader,
                       "Ldalvik/system/BaseDexClassLoader;")) {
        return;
    }
    LOGD("Found OatClass dalvik.system.BaseDexClassLoader");


    uint16_t class_def_index = GetIndexForClassDef(core_libart_jar.data.dex_file_pointer,
                                                   dalvik_system_BaseDexClassLoader.dex_class.class_def);

    log_dex_file_class_def_contents(core_libart_jar.data.dex_file_pointer, class_def_index);
    log_oat_dex_file_class_def_contents(
            dalvik_system_BaseDexClassLoader.oat_class_data.backing_memory_address);

    if (!oat_FindVirtualMethod(&dalvik_system_BaseDexClassLoader, &findLibrary, "findLibrary",
                               "(Ljava/lang/String;)Ljava/lang/String;")) {
        return;
    }
    LOGD("Found OatMethod findLibrary");


    struct DecodedMethod *decoded = &findLibrary.dex_method.decoded_method_data;
    LOGD("Method contents before overwrite: ");
    LOGD("Method contents ["
                 PRINT_PTR
                 " (Size: %d)]", (uintptr_t) decoded->backing_memory_address,
         decoded->backing_memory_size);
    LOGD("Method Id index difference: %x", decoded->method_idx_diff);
    LOGD("Access Flags:               %x", decoded->access_flags);
    LOGD("Code Offset:                %x", decoded->code_off);


    void *data = decoded->backing_memory_address;

    if (!set_memory_protection(data, decoded->backing_memory_size, true, true, false))
    {
        LOGF("Could not change the memory protections of the encoded method to allow for writing.");
        return;
    }
    DecodeUnsignedLeb128((const uint8_t**)&data); // Skip the Method id index as it stays the same.

    uint32_t old_access_flags = decoded->access_flags;
    uint32_t new_access_flags = decoded->access_flags | kAccNative;
    size_t old_flag_size = UnsignedLeb128Size(old_access_flags);
    size_t new_flag_size = UnsignedLeb128Size(new_access_flags);

    int size_diff = new_flag_size - old_flag_size;
    LOGD("Size difference: %d", size_diff);

    if(new_flag_size > old_flag_size)
    {
        CHECK(size_diff == 1); // Since kAccNative is 0x80 the difference can't be more than 1

        LOGD("Since the access flags value is too low, its encoding changes by setting this method to native.");
        LOGD("We have to modify the code_offset value to compensate for this so the overall size of the EncodedMethod stays the same.");
        uint32_t code_offset_size = UnsignedLeb128Size(decoded->code_off);
        if(code_offset_size <= 1)
        {
            // TODO
            // If our code offset has an encoding of only 1 byte we are shit out of luck,
            // because we simply have no room for modifications
            //
            // Possible Workarounds:
            //
            // Actually do enlarge the file to hold our modifications
            // (infeasible propably, too many changes necessary, offsets break etc.)
            //
            // Try modifying the next function as well with a simple redirection trampoline to give us more space to work with.
            // (Doesn't really solve the problem as this e.g. only works if our method is not the last on in the array.)
            LOGF("Simply nothing we can do this method cannot be hooked this way. It's code offset is too small to allow for any changes.");
        }
        else
        {
            LOGD("code_offset modification is possible, the code_offset is large enough.");
            uint32_t uleb128_sized_values[] = {0, 1 << 0, 1 << 7, 1 << 14, 1 << 21, 1 << 28};

            EncodeUnsignedLeb128(data, new_access_flags);
            LOGD("Overwrote access flags with value %x", new_access_flags);
            // Overwrite the code offset with a value 1 smaller to keep the size constant afterwards.
            EncodeUnsignedLeb128(data + new_flag_size, uleb128_sized_values[code_offset_size - 1]);
            LOGD("Overwrote code offset with value %x", uleb128_sized_values[code_offset_size - 1]);
        }
    }
    else
    {
        CHECK(size_diff == 0); // Oring with a value where higher values are set should never change the encoding size.

        LOGD("The access flags were set high enough no tricky code_offset manipulation necessary.");
        // No further write are necessary since we overwrite with a value of the exact same encoding size.
        EncodeUnsignedLeb128(data, new_access_flags);
        LOGD("Overwrote access flags with value %x", new_access_flags);
    }

    if (!oat_FindVirtualMethod(&dalvik_system_BaseDexClassLoader, &findLibrary, "findLibrary", "(Ljava/lang/String;)Ljava/lang/String;")) {
        return;
    }
    LOGD("Re-parsed the findLibrary method.");


    decoded = &findLibrary.dex_method.decoded_method_data;
    LOGD("Method contents after overwrite: ");
    LOGD("Method contents ["PRINT_PTR" (Size: %d)]", (uintptr_t) decoded->backing_memory_address,
         decoded->backing_memory_size);
    LOGD("Method Id index difference: %x", decoded->method_idx_diff);
    LOGD("Access Flags:               %x", decoded->access_flags);
    LOGD("Code Offset:                %x", decoded->code_off);

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