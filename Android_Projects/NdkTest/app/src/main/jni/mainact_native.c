#include "mainact_native.h"

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