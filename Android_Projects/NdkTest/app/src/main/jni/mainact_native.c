#include "mainact_native.h"

#include "debug_util.h"
#include "trappoint_interface.h"
#include "signal_handling_helper.h"
#include "lib_setup.h"
#include "logging.h"
#include "util.h"
#include "memory.h"
#include "system_info.h"
#include "oat_info.h"
#include "dex_class_data.h"
#include "oat_class.h"


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
    LOGI("Previously Arg1: %x", get_argument(context, 1));
    set_argument(context, 1, (uint32_t) "HALA HALA HALA!");
    LOGI("After overwriting Arg1: %x", get_argument(context, 1));
}

void handler_hello_world(void *trap_addr, ucontext_t *context, void *args)
{
    LOGD("Inside the trappoint handler...");
    LOGD("Hello, World!");
}

void run_trap_point_test(JNIEnv *env)
{
    void *func = (*env)->NewStringUTF;
    void *addr = (unsigned short *) get_code_base_address(func);

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

bool dexFileNamePredicate_StrStr(OatDexFileInfo* current, void* args)
{
    return strstr(current->dex_file_location.content, args) != NULL;
}
bool dexFileClassPredicate_strstr(const DexFileHeader *hdr, ClassDef *c, void *args)
{
    TypeId t = dex_file_GetTypeDescriptorByIndex(hdr, c->class_idx_);
    const char* s = dex_file_GetStringDataByIndex(hdr, t.descriptor_idx_).content;
    return strstr(s, args) != NULL;
}
bool dexFileClassPredicate_strcmp(const DexFileHeader* hdr, ClassDef* c, void* args)
{
    TypeId t = dex_file_GetTypeDescriptorByIndex(hdr, c->class_idx_);
    const char* s = dex_file_GetStringDataByIndex(hdr, t.descriptor_idx_).content;
    return strcmp(s, args) == 0;
}
bool dexFileMethodDescriptorPredicate_RecursiveClassMethods(DexFileHeader* hdr, MethodId* m, void* args)
{
    uint32_t class_index = (uint32_t)args;
    ClassDef c = dex_file_GetClassDefinitionByIndex(hdr, class_index);
    if(m->class_idx_ == c.class_idx_)
    {
        return true;
    }
    if(c.superclass_idx_ == kDexNoIndex)
    {
        return false;
    }
    return dexFileMethodDescriptorPredicate_RecursiveClassMethods(hdr, m, (void*)c.superclass_idx_);
}

struct Handler_Arg_Info
{
    uint32_t argument_index;
    uint32_t new_value;
};
static struct Handler_Arg_Info set_arg_handler_args;
void handler_set_arg(void *trap_addr, ucontext_t *context, void *args)
{
    struct Handler_Arg_Info* arg = (struct Handler_Arg_Info*)args;
    LOGD("Attempting to set argument #%d to new value %d", arg->argument_index, arg->new_value);
    LOGD("Arg #%d before overwrite: %d", arg->argument_index, get_argument(context, arg->argument_index));
    set_argument(context, arg->argument_index, arg->new_value);
    LOGD("Arg #%d after overwrite: %d", arg->argument_index, get_argument(context, arg->argument_index));
}

JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testOverwritingJavaCode(
        JNIEnv *env, jobject instance)
{
    //waitForDebugger();

    /*
    const char *lib2 = "/data/dalvik-cache/arm/data@app@com.example.lukas.ndktest-2@base.apk@classes.dex";
    const char *lib3 = "/data/dalvik-cache/arm/data@app@com.example.lukas.ndktest-3@base.apk@classes.dex";

    dump_process_memory_map();

    void *lib_handle = dlopen(lib2, RTLD_NOW);
    if (NULL == lib_handle)
    {
        LOGW("Failed finding first lib path, trying alternative.");
        lib_handle = dlopen(lib3, RTLD_NOW);
    }
    if (NULL == lib_handle)
    {
        LOGF("Dlopen failed: %s", strerror(errno));
        return;
    }
     */

    // For Samsung S4
    // void* lib_handle = dlopen("/system/framework/arm/boot.oat", RTLD_NOW);

    //void* lib_handle = dlopen("/data/dalvik-cache/arm/system@framework@boot.oat", RTLD_NOW);
    //void *elf_oat_begin = dlsym(lib_handle, "oatdata");
    void* elf_oat_begin = (void*)(0x711ec000 + 0x1000);
    if (NULL == elf_oat_begin)
    {
        LOGF("Dlsym(\"oatdata\") failed: %s", strerror(errno));
        return;
    }
    void *elf_oat_end = (void*)0xFFFFFF00;
    if (NULL == elf_oat_begin)
    {
        LOGF("Dlsym(\"oatlastword\") failed: %s", strerror(errno));
        return;
    }
    elf_oat_end += sizeof(uint32_t);

    LazyOatInfo* oat_info = oat_info_Initialize(elf_oat_begin, elf_oat_end);
    uint32_t oat_dex_file_index;
    if(!oat_info_FindOatDexFileIndicesByPredicate(oat_info, (PREDICATE)dexFileNamePredicate_StrStr, "libart", &oat_dex_file_index, 1))
    {
        LOGF("Could not find dex file libart in boot.oat in memory.");
        return;
    }
    LOGD("Index of libart dex file: %d", oat_dex_file_index);
    OatDexFileInfo* oat_dex_file = oat_info_GetOatDexFileByIndex(oat_info, oat_dex_file_index);
    char oat_dex_file_location[oat_dex_file->dex_file_location.length + 1];
    strncpy(oat_dex_file_location, oat_dex_file->dex_file_location.content, oat_dex_file->dex_file_location.length);
    oat_dex_file_location[oat_dex_file->dex_file_location.length] = 0;
    LOGD("Resolved oat_dex_file location: %s", oat_dex_file_location);

    //log_oat_dex_file_storage_contents(oat_info_GetHeader(oat_info));

    uint32_t current_class_def_index;
    DexFileHeader* dex_hdr = oat_dex_file->dex_file_pointer;
    if(!dex_file_FindClassDefinitionIndicesByPredicate(dex_hdr, dexFileClassPredicate_strcmp,
                                                       "Ljava/lang/Integer;", &current_class_def_index,
                                                       1))
    {
        LOGF("Could not resolve class index for class in libart dex file.");
        return;
    }

    ClassDef dex_class_def = dex_file_GetClassDefinitionByIndex(dex_hdr, current_class_def_index);
    OatClass oat_class_def;
    oat_class_Extract(&oat_class_def, oat_info, oat_dex_file_index, current_class_def_index);

    uint8_t* dex_class_data_pointer = (void*)dex_hdr + dex_class_def.class_data_off_;
    DexClassData* class_data = dex_class_data_Initialize(dex_class_data_pointer);
    if(class_data == NULL)
    {
        return;
    }

    uint32_t interesting_method_index = 3;

    log_dex_file_method_id_contents(dex_hdr, dex_class_data_GetMethodIdIndex_DirectMethod(class_data, interesting_method_index));
    log_oat_dex_file_method_offsets_content(oat_info->header, &oat_class_def, interesting_method_index);
    void* method_code = oat_class_GetMethodCodePointer(oat_info->header, &oat_class_def, interesting_method_index);

    set_arg_handler_args.argument_index = 1;
    set_arg_handler_args.new_value = 63;
    LOGD("&overwrite_arg: "PRINT_PTR, (uintptr_t)&set_arg_handler_args);
    LOGD("Please overwrite arg #%d with %d", set_arg_handler_args.argument_index, set_arg_handler_args.new_value);
    LOGD("Installing Integer.bitcount trappoint at "PRINT_PTR, (uintptr_t)method_code);
    install_trappoint(method_code, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL,
                      &handler_set_arg, (void*)&set_arg_handler_args);

    LOGD("Trappoint installed, let's see if it worked.");
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