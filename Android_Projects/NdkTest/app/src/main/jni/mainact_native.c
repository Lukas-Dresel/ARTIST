#include <jni.h>
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
#include "memory_map_lookup.h"
#include "util/list.h"
#include "statistics.h"
#include "hooking/invocation_hook.h"
#include "art/mirror_hacks.h"
#include "art/android_utf.h"
#include "art_resolution.h"
#include "art/oat_version_dependent/VERSION045/thread.h"
#include "art/oat_version_dependent/VERSION045/entrypoints/quick_entrypoints.h"
#include "hooking/breakpoint.h"
#include "thread_lookup.h"

#ifdef __cplusplus
extern "C"
{
#endif


static bool setupBootOat(struct OatFile *oat_file)
{
    /*
     * void *elf_begin = (void *) 0x706a8000;
     * void *elf_oat_begin = elf_begin + 0x1000; // The oat section usually starts in the next page
     * void *elf_oat_end = (void *) 0x7368d000;
     */

    struct MemoryMapView * view = CreateMemoryMapView();
    if(view == NULL)
    {
        return false;
    }
    struct MemoryMappedFile *boot_oat = findFileByPath(view, "/data/dalvik-cache/arm/system@framework@boot.oat");
    if (boot_oat == NULL)
    {
        LOGD("Unable to find file \"/data/dalvik-cache/arm/system@framework@boot.oat\"");
        DestroyMemoryMapView(view);
        return false;
    }
    void *elf_start;
    void *elf_oat_start;
    void *elf_oat_end;
    if (!extractElfOatPointersFromFile(boot_oat, &elf_start, &elf_oat_start, &elf_oat_end))
    {
        LOGD("boot.oat doesn't appear to be an Oat file ... ???");
        DestroyMemoryMapView(view);
        return false;
    }
    DestroyMemoryMapView(view);
    return oat_Setup(oat_file, elf_oat_start, elf_oat_end);
}

static bool findFunction(struct OatFile* oat, struct OatDexFile* oat_dex, struct OatClass* class, struct OatMethod* method,
                         char* dex_path, char* class_name, char* method_name, char* method_proto, bool direct) {

    if (!oat_FindDexFile(oat, oat_dex, dex_path))
    {
        return false;
    }
    LOGD("Found OatDexFile %s", dex_path);

    if (!oat_FindClassInDex(oat_dex, class, class_name))
    {
        return false;
    }
    LOGD("Found OatClass %s", class_name);

    if (direct) {
        if (!oat_FindDirectMethod(class, method, method_name, method_proto))
        {
            return false;
        }
    }
    else
    {
        if (!oat_FindVirtualMethod(class, method, method_name, method_proto))
        {
            return false;
        }
    }
    LOGD("Found %s OatMethod %s [%s]", (direct) ? "direct" : "virtual", method_name, method_proto);
    return true;
}
static void dumpMappedOatFileStatistics()
{
    struct MemoryMapView * view = CreateMemoryMapView();
    if(view != NULL)
    {
        struct MemoryMappedFile* cur_file;
        LOGD("Loaded oat files: ");
        list_for_each_entry(cur_file, &view->list_files, view_list_files_entry)
        {
            if(FileIsElfOatFilePredicate(view, cur_file, NULL))
            {
                void* elf_start;
                void* elf_oat_start;
                void* elf_oat_end;
                if(!extractElfOatPointersFromFile(cur_file, &elf_start, &elf_oat_start, &elf_oat_end))
                {
                    continue;
                }
                struct OatFile oat;
                if(!oat_Setup(&oat, elf_oat_start, elf_oat_end))
                {
                    continue;
                }
                LOGD("Oat file compilation statistics for: %s", cur_file->path->path);
                stats_logNumCompiledMethodsInOatFile(&oat);
            }
        }
        DestroyMemoryMapView(view);
    }
}

JNIEXPORT void JNICALL Java_com_example_lukas_ndktest_MainActivity_testtest(
        JNIEnv *env, jobject instance, jstring a, jint b, jstring c, jstring d, jint e)
{
    LOGI("a: "PRINT_PTR, (uintptr_t)a);
    LOGI("b: "PRINT_PTR, (uintptr_t)b);
    LOGI("c: "PRINT_PTR, (uintptr_t)c);
    LOGI("d: "PRINT_PTR, (uintptr_t)d);
    LOGI("e: "PRINT_PTR, (uintptr_t)e);
}

static void loadLibrary_OnEntry(void *addr, ucontext_t *ctx, void *additionalArg)
{
    JavaVM* javaVM = additionalArg;

    LOGI("Called loadLibrary.");

    JNIEnv* env = NULL;
    if((*javaVM)->GetEnv(javaVM, (void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return;
    }
    unsigned char * arg0 = (void*)GetArgument(ctx, 0);
    unsigned char * arg1 = (void*)GetArgument(ctx, 1);
    struct MirrorHackString* loaded_library_name = (struct MirrorHackString*)arg1;
    char utf8[loaded_library_name->str_len + 10];
    ConvertUtf16ToModifiedUtf8(&utf8[0], loaded_library_name->str_content->chars, loaded_library_name->str_len);
    LOGI("Tried to load library: %s", utf8);
}
static void loadLibrary_OnExit(void *addr, ucontext_t *ctx, void *additionalArg)
{
    JavaVM* javaVM = additionalArg;
    LOGI("Exiting loadLibrary.");
    dumpMappedOatFileStatistics();
}

static bool hookSystemLoadLibrary(JavaVM* javaVM)
{
    struct OatFile boot_oat;
    if(!setupBootOat(&boot_oat))
    {
        LOGE("Could not locate boot.oat file.");
        return false;
    }
    struct OatDexFile oat_dex;
    struct OatClass java_lang_System;
    if(!oat_FindClass(&boot_oat, &oat_dex, &java_lang_System, "Ljava/lang/System;"))
    {
        LOGE("Could not find java.lang.System class.");
        return false;
    }
    struct OatMethod loadLibrary;
    if(!oat_FindMethod(&java_lang_System, &loadLibrary, "loadLibrary", "(Ljava/lang/String;)V"))
    {
        LOGE("Could not find method loadLibrary.");
        return false;
    }
    if(!oat_HasQuickCompiledCode(&loadLibrary))
    {
        LOGE("Method loadLibrary is interpreted, not compiled.");
        return false;
    }
    void* impl = oat_GetQuickCompiledEntryPoint(&loadLibrary);
    if(InvocationHook_Install(impl, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL,
                           loadLibrary_OnEntry, javaVM, loadLibrary_OnExit, javaVM) == NULL)
    {
        LOGE("Could not install loadLibrary invocation_hook");
        return false;
    }
    LOGI("Successfully hooked java.lang.System.loadLibrary(String).");
    return true;
}
static bool hookFunction(const char* class_name, const char* method_name, const char* proto,
                         HOOKCALLBACK onEntry, void* onEntryArgs,
                         HOOKCALLBACK onExit, void* onExitArgs)
{
    CHECK_RETURNFALSE(class_name != NULL);
    CHECK_RETURNFALSE(method_name != NULL);
    CHECK_RETURNFALSE(proto != NULL);


    struct ArtMethodContext method;

    LOGD("Looking up function %s", method_name);
    if(!android_FindLoadedMethod(&method, class_name, method_name, proto))
    {
        LOGE("Could not find [%s] :: %s %s ", class_name, method_name, proto);
        return false;
    }



    const struct DexHeader* dex = method.clazz.oat_dex.data.dex_file_pointer;
    uint16_t class_def_index = GetIndexForClassDef(dex, method.clazz.oat_class.dex_class.class_def);

    log_dex_file_class_def_contents(dex, class_def_index);
    log_oat_dex_file_class_def_contents(method.clazz.oat_class.oat_class_data.backing_memory_address);

    uint32_t ignored;
    stats_logNumCompiledMethodsInOatClass(&method.clazz.oat_class, &ignored, &ignored);



    LOGD("Checking for quick code.");
    if(!oat_HasQuickCompiledCode(&method.oat_method))
    {
        LOGE("Method %s :: %s %s is interpreted, not compiled.", class_name, method_name, proto);
        return false;
    }
    LOGD("Found Quick code, hooking next.");
    void* impl = oat_GetQuickCompiledEntryPoint(&method.oat_method);
    CHECK_RETURNFALSE(impl != NULL);

    if(InvocationHook_Install(impl, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL,
                              onEntry, onEntryArgs, onExit, onExitArgs) == NULL)
    {
        LOGE("Could not install invocation_hook for method %s :: %s %s", class_name, method_name, proto);
        return false;
    }
    LOGI("Successfully hooked %s :: %s %s (String).", class_name, method_name, proto);
    return true;
}
static bool breakOnFunction(const char* class_name, const char* method_name, const char* proto,
                         HOOKCALLBACK onEntry, void* onEntryArgs)
{
    CHECK_RETURNFALSE(class_name != NULL);
    CHECK_RETURNFALSE(method_name != NULL);
    CHECK_RETURNFALSE(proto != NULL);


    struct ArtMethodContext method;

    LOGD("Looking up function %s", method_name);
    if(!android_FindLoadedMethod(&method, class_name, method_name, proto))
    {
        LOGE("Could not find [%s] :: %s %s ", class_name, method_name, proto);
        return false;
    }



    const struct DexHeader* dex = method.clazz.oat_dex.data.dex_file_pointer;
    uint16_t class_def_index = GetIndexForClassDef(dex, method.clazz.oat_class.dex_class.class_def);

    log_dex_file_class_def_contents(dex, class_def_index);
    log_oat_dex_file_class_def_contents(method.clazz.oat_class.oat_class_data.backing_memory_address);

    uint32_t ignored;
    stats_logNumCompiledMethodsInOatClass(&method.clazz.oat_class, &ignored, &ignored);



    LOGD("Checking for quick code.");
    if(!oat_HasQuickCompiledCode(&method.oat_method))
    {
        LOGE("Method %s :: %s %s is interpreted, not compiled.", class_name, method_name, proto);
        return false;
    }
    LOGD("Found Quick code, hooking next.");
    void* impl = oat_GetQuickCompiledEntryPoint(&method.oat_method);
    CHECK_RETURNFALSE(impl != NULL);

    if(Breakpoint_Install(impl, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL,
                              onEntry, onEntryArgs) == NULL)
    {
        LOGE("Could not install breakpoint for method %s :: %s %s", class_name, method_name, proto);
        return false;
    }
    LOGI("Successfully breaking on %s :: %s %s (String).", class_name, method_name, proto);
    return true;
}
void default_OnEntry(void *addr, ucontext_t *ctx, char* func_name)
{
}
void default_OnExit(void *addr, ucontext_t *ctx, char* func_name)
{

}
struct SuspendMonitor
{
    void* pTestSuspendAddress;
    void* hook;
    uint32_t calls;
};
static void monitorTestSuspend_OnEntry(void* addr, ucontext_t* ctx, void* arg)
{
    struct SuspendMonitor* monitor = arg;
    monitor->calls++;
    return;
}
static void monitorTestSuspend_OnExit(void* addr, ucontext_t* ctx, void* arg)
{
    return;
}
static installTestSuspendMonitor_OnEntry(void* addr, ucontext_t* ctx, void* arg)
{
    struct SuspendMonitor* monitor = arg;
    monitor->hook = InvocationHook_Install(monitor->pTestSuspendAddress, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL,
                                           monitorTestSuspend_OnEntry, arg,
                                           monitorTestSuspend_OnExit, arg);
    monitor->calls = 0;
    if(monitor->hook == NULL)
    {
        LOGE("Could not install pTestSuspend monitoring hook.");
    }
}
static installTestSuspendMonitor_OnExit(void* addr, ucontext_t* ctx, void* arg)
{
    struct SuspendMonitor* monitor = arg;
    if(monitor->hook != NULL)
    {
        InvocationHook_Uninstall(monitor->hook);
        monitor->hook = NULL;
    }
    LOGI("pTestSuspend was called %d times.", monitor->calls);
}
static void monitorTestSuspendInFunction(JNIEnv* env, const char* class_name, const char* method_name,
                                         const char* method_proto)
{
    struct Thread* thread = GetCurrentThreadObjectPointer(env);
    if(thread == NULL)
    {
        LOGE("Could not find current thread structure.");
        return;
    }
    struct QuickEntryPoints* quick_ep = &thread->tlsPtr_.quick_entrypoints;
    void (*suspendCheck)() = quick_ep->pTestSuspend;

    struct SuspendMonitor* monitor = allocate_memory_chunk(sizeof(struct SuspendMonitor));
    if(monitor == NULL)
    {
        LOGE("Could not allocate memory.");
        return;
    }
    hookFunction(class_name, method_name, method_proto, installTestSuspendMonitor_OnEntry, monitor,
                     installTestSuspendMonitor_OnExit, monitor);
}

static void doPTestSuspendHook(JNIEnv* env)
{
    struct Thread* thread = GetCurrentThreadObjectPointer(env);
    if(thread == NULL)
    {
        LOGE("Could not find current thread structure.");
        return;
    }
    struct QuickEntryPoints* quick_ep = &thread->tlsPtr_.quick_entrypoints;
    void (*suspendCheck)() = quick_ep->pTestSuspend;

    if(!InvocationHook_Install(suspendCheck, TRAP_METHOD_INSTR_KNOWN_ILLEGAL | TRAP_METHOD_SIG_ILL,
                               (HOOKCALLBACK)default_OnEntry, "pTestSuspend",
                               (HOOKCALLBACK)default_OnExit, "pTestSuspend"))
    {
        LOGE("Could not hook pTestSuspend.");
    }
    else
    {
        LOGI("Successfully hooked pTestSupend.");
    }
}

void evaluation_applicability(JavaVM* vm)
{
    if (!hookSystemLoadLibrary(vm))
    {
        LOGE("Could not hook java.lang.System.loadLibrary(String).");
    }
    dumpMappedOatFileStatistics();
}
void evaluation_performance(JavaVM* vm)
{
    if (!breakOnFunction("Lcom/android/cm3/MethodAtom;", "execute", "()I",
                         (HOOKCALLBACK) default_OnEntry, "com.android.cm3.MethodAtom.execute"))
    {
        LOGE("Could not break on com.android.cm3.MethodAtom.execute.");
    }
}
jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{

    LOGW("#####################################################################");
    LOGW("Loading library ndktest ...");
    LOGW("#####################################################################");
    init();

    evaluation_performance(vm);
    //evaluation_applicability(vm);

    JNIEnv* env = NULL;
    if((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) == JNI_OK)
    {
        LOGD("Found thread object at"PRINT_PTR, (uintptr_t)GetCurrentThreadObjectPointer(env));
    }

    return JNI_VERSION_1_6;
}

void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    LOGI("#####################################################################");
    LOGI("Unloading library ndktest ...");
    LOGI("#####################################################################");
    dumpMappedOatFileStatistics();
    destroy();
}


void handler_change_NewStringUTF_arg(void *trap_addr, ucontext_t *context, void *args)
{
    LOGI("Inside the trappoint handler...");
    LOGI("Previously Arg1: %x", (uint32_t)GetArgument(context, 1));
    SetArgument(context, 1, (uint32_t) "HALA HALA HALA!");
    LOGI("After overwriting Arg1: %x", (uint32_t)GetArgument(context, 1));
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

    trappoint_Install(func, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL,
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
    if(!oat_FindDexFile(&oat, &core_libart_jar, "/system/framework/core-libart.jar"))
    {
        return;
    }
    if(!oat_FindClassInDex(&core_libart_jar, &java_lang_Integer, "Ljava/lang/Integer;"))
    {
        return;
    }
    uint16_t class_def_index = GetIndexForClassDef(core_libart_jar.data.dex_file_pointer, java_lang_Integer.dex_class.class_def);
    if(!oat_FindDirectMethod(&java_lang_Integer, &int_bitcount, "bitCount", "(I)I"))
    {
        return;
    }
    if(oat_HasQuickCompiledCode(&int_bitcount))
    {
        void* target_addr = oat_GetQuickCompiledEntryPoint(&int_bitcount);
        trappoint_Install(target_addr, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL,
                          handler_change_bitcount_arg, (void *) 65525);
        LOGI("Installed trappoint for java.lang.Integer.bitcount()");
    }
}

JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_dumpQuickEntryPointsInfo(JNIEnv *env, jobject instance)
{
    void *current_thread_pointer = GetCurrentThreadObjectPointer(env);
    LOGD("The current_thread_pointer lies at "PRINT_PTR, (uintptr_t) current_thread_pointer);
}
void handler_suspend(void *trap_addr, ucontext_t *context, void *args)
{
    LOGD("Handling %s at "PRINT_PTR, (char*)args, (uintptr_t)trap_addr);
}
JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_testHookingThreadEntryPoints(JNIEnv *env, jobject instance)
{
    doPTestSuspendHook(env);

    void *current_thread_pointer = GetCurrentThreadObjectPointer(env);
    LOGD("The current_thread_pointer lies at "PRINT_PTR, (uintptr_t) current_thread_pointer);
    struct Thread* thread = (struct Thread*)current_thread_pointer;

    struct QuickEntryPoints* quick_ep = &thread->tlsPtr_.quick_entrypoints;
    trappoint_Install(quick_ep->pTestSuspend, TRAP_METHOD_INSTR_KNOWN_ILLEGAL | TRAP_METHOD_SIG_ILL,
                      handler_suspend, "pTestSuspend");
    trappoint_Install(quick_ep->pInvokeDirectTrampolineWithAccessCheck,
                      TRAP_METHOD_INSTR_KNOWN_ILLEGAL | TRAP_METHOD_SIG_ILL, handler_suspend, "pInvokeDirectTrampolineWithAccessCheck");
    trappoint_Install(quick_ep->pInvokeVirtualTrampolineWithAccessCheck,
                      TRAP_METHOD_INSTR_KNOWN_ILLEGAL | TRAP_METHOD_SIG_ILL, handler_suspend, "pInvokeVirtualTrampolineWithAccessCheck");
    trappoint_Install(quick_ep->pInvokeStaticTrampolineWithAccessCheck,
                      TRAP_METHOD_INSTR_KNOWN_ILLEGAL | TRAP_METHOD_SIG_ILL, handler_suspend, "pInvokeStaticTrampolineWithAccessCheck");
    trappoint_Install(quick_ep->pInvokeSuperTrampolineWithAccessCheck,
                      TRAP_METHOD_INSTR_KNOWN_ILLEGAL | TRAP_METHOD_SIG_ILL, handler_suspend, "pInvokeSuperTrampolineWithAccessCheck");
}

JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_testHookingDexLoadClass(JNIEnv *env, jobject instance) {

    struct OatFile oat;
    struct OatDexFile core_libart_jar;
    struct OatClass dalvik_system_DexFile;
    struct OatMethod loadClass;

    if (!setupBootOat(&oat))
    {
        return;
    }
    if(!findFunction(&oat, &core_libart_jar, &dalvik_system_DexFile, &loadClass,
                 "/system/framework/core-libart.jar",
                 "Ldalvik/system/DexFile;",
                 "loadClass", "(Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/lang/Class;", false))
    {
        return;
    }

    if(!oat_HasQuickCompiledCode(&loadClass))
    {
        LOGD("No compiled code was found for this method.");
    }
    else
    {
        void* entrypoint = oat_GetQuickCompiledEntryPoint(&loadClass);
        LOGD("This method has its entrypoint at "PRINT_PTR, (uintptr_t)entrypoint);
    }
    uint16_t class_def_index = GetIndexForClassDef(core_libart_jar.data.dex_file_pointer,
                                                   dalvik_system_DexFile.dex_class.class_def);

    log_dex_file_class_def_contents(core_libart_jar.data.dex_file_pointer, class_def_index);
    log_oat_dex_file_class_def_contents(dalvik_system_DexFile.oat_class_data.backing_memory_address);
}



JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_testHookingInterpretedFunction(JNIEnv *env, jobject instance) {

    struct OatFile oat;
    struct OatDexFile core_libart_jar;
    struct OatClass dalvik_system_BaseDexClassLoader;
    struct OatMethod findLibrary;

    if (!setupBootOat(&oat))
    {
        return;
    }
    LOGD("Setup our oat file!");
    if (!oat_FindDexFile(&oat, &core_libart_jar, "/system/framework/core-libart.jar")) {
        return;
    }
    LOGD("Found OatDexFile /system/framework/core-libart.jar");
    if (!oat_FindClassInDex(&core_libart_jar, &dalvik_system_BaseDexClassLoader,
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
        CHECK(size_diff == 0); // Or'ing with a value where higher values are set should never change the encoding size.

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
    if(!oat_FindClassInDex(&core_libart_jar, &dalvik_system_DexFile, "Ldalvik/system/DexFile;"))
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

    void* next_instruction_pointer = ExtractNextExecutedInstructionPointer(context);
    LOGD("SingleStep-Handler: Next instruction assumed to be: "PRINT_PTR, (uintptr_t)next_instruction_pointer);
    if(next_instruction_pointer > start && next_instruction_pointer < end)
    {
        LOGD("Attempting to install next trappoint in single step chain. ");
        trappoint_Install((void *) next_instruction_pointer, 0, handler_step_function, arg);
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
    //dump_process_memory_map();


    char* looking_for = "/data/dalvik-cache/arm/system@framework@boot.oat";
    struct MemoryMapView * view = CreateMemoryMapView();
    if(view == NULL)
    {
        return;
    }

    struct MemoryMappedFile * boot_oat = findFileByPath(view, looking_for);
    if(boot_oat == NULL)
    {
        LOGD("Unable to find oat file %s", looking_for);
        return;
    }
    logFileContents(boot_oat);
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
JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_dumpSystemLoadLibraryState(JNIEnv *env, jobject instance)
{
    void* elf_start = (void*)0x706a8000;
    void* oat_start = elf_start + 0x1000;

    void* elf_end = (void*)0x7368d000;


    struct OatFile oat;
    struct OatDexFile oat_dex;
    struct OatClass oat_class;
    struct OatMethod oat_method;
    if(!oat_Setup(&oat, oat_start, elf_end))
    {
        LOGF("Could not parse the oat file.");
        return;
    }
    if(!oat_FindClass(&oat, &oat_dex, &oat_class, "Ljava/lang/System;"))
    {
        LOGF("Could not find class java.lang.System");
        return;
    }
    if(!oat_FindDirectMethod(&oat_class, &oat_method, "loadLibrary", "(Ljava/lang/String;)V"))
    {
        LOGF("Could not find function loadLibrary");
        return;
    }
    const struct DexHeader* dex_hdr = oat_dex.data.dex_file_pointer;
    log_oat_dex_file_class_def_contents(oat_class.oat_class_data.backing_memory_address);
    log_dex_file_class_def_contents(dex_hdr, GetIndexForClassDef(dex_hdr, oat_class.dex_class.class_def));
}


JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_dumpLibArtInterpretedFunctionsInNonAbstractClasses(
        JNIEnv *env, jobject instance)
{
    struct OatFile mainOat;
    if(!setupBootOat(&mainOat))
    {
        LOGF("Could not parse system/framework/boot.oat.");
        return;
    }

    struct OatDexFile current_oat_dex;
    for(uint32_t dex_file_index = 0; dex_file_index < NumDexFiles(mainOat.header); dex_file_index++)
    {
        if(!oat_GetOatDexFile(&mainOat, &current_oat_dex, dex_file_index))
        {
            // Since they are all sequentially stored if one fails all should fail.
            LOGF("Error parsing oat dex file #%d", dex_file_index);
            return;
        }
        LOGD("OatDexFile #%d: %s", current_oat_dex.index, current_oat_dex.data.location_string.content);
        struct OatClass clazz;
        for(uint32_t i = 0; i < dex_NumberOfClassDefs(current_oat_dex.data.dex_file_pointer); i++)
        {
            if(!oat_GetClass(&current_oat_dex, &clazz, i))
            {
                // Here as well, if one's broken all of them should be.
                LOGF("Error parsing oat class def data for class #%d in oat_dex_file #%d.", i, dex_file_index);
                return;
            }
            if( (clazz.oat_class_data.class_type == kOatClassAllCompiled) ||
                (clazz.dex_class.class_def->access_flags_ & kAccAbstract != 0) ||
                (clazz.dex_class.class_def->access_flags_ & kAccInterface != 0) )
            {
                continue;
            }
            const char* class_name = GetClassDefNameByIndex(clazz.oat_dex_file->data.dex_file_pointer, (uint16_t)i);
            /*if(strchr(class_name, '$') != NULL)
            {
                continue;
            }*/
            LOGD("Class [%d]: %s => %s", i, class_name, GetOatClassTypeRepresentation(clazz.oat_class_data.class_type));
        }
    }

}



JNIEXPORT jclass JNICALL
Java_dalvik_system_DexFile_loadClass(JNIEnv *env, jobject instance, jstring name_, jobject loader)
{
    const char *name = (*env)->GetStringUTFChars(env, name_, 0);

    LOGD("Tried to load class: %s", name);

    (*env)->ReleaseStringUTFChars(env, name_, name);
    return NULL;
}
JNIEXPORT jclass JNICALL
Java_dalvik_system_BaseDexClassLoader_findLibrary(JNIEnv *env, jobject instance, jstring name_)
{
    const char *name = (*env)->GetStringUTFChars(env, name_, 0);

    LOGD("Tried to find library: %s", name);

    (*env)->ReleaseStringUTFChars(env, name_, name);
    return NULL;
}

JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_registerNativeHookForFindLibrary(JNIEnv *env,
                                                                           jobject instance)
{
    jclass clazz = (*env)->FindClass(env, "dalvik/system/BaseDexClassLoader");
    JNINativeMethod hook = {
            .fnPtr = (void*)&Java_dalvik_system_DexFile_loadClass,
            .name = "loadClass",
            .signature = "(Ljava/lang/String;)Ljava/lang/Class;"
    };
    (*env)->RegisterNatives(env, clazz, &hook, 1);
}
JNIEXPORT void JNICALL
Java_com_example_lukas_ndktest_MainActivity_registerNativeHookForDexFileLoadClass(JNIEnv *env,
                                                                           jobject instance)
{
    jclass clazz = (*env)->FindClass(env, "dalvik/system/DexFile");
    JNINativeMethod hook = {
            .fnPtr = (void*)&Java_dalvik_system_DexFile_loadClass,
            .name = "loadClass",
            .signature = "(Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/lang/Class;"
    };
    (*env)->RegisterNatives(env, clazz, &hook, 1);
}



#ifdef __cplusplus
}
#endif