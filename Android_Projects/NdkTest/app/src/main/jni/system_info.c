#include <stdio.h>
#include "system_info.h"

static long SYSTEM_PAGE_SIZE = -1;

void init_system_info(JNIEnv *env)
{
    SYSTEM_PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
    if (SYSTEM_PAGE_SIZE < 1)
    {
        LOGE("Error while resolving system page size. Assuming 4k bytes. Err: %s", strerror(errno));
        SYSTEM_PAGE_SIZE = 4096;
    }
}

void destroy_system_info(JNIEnv *env)
{
}

long getSystemPageSize(void)
{
    return SYSTEM_PAGE_SIZE;
}

void dump_process_memory_map(void)
{
    char buff[3200];

    FILE *fp = fopen("/proc/self/maps", "r");
    while (fgets(buff, sizeof(buff), fp) != NULL)
    {
        LOGI("MemoryMap: %s", buff);
    }
    if (feof(fp))
    {
        LOGI("MemoryMap: %s", "#################### END OF MEMORY MAP ###################");
    }
    if (ferror(fp))
    {
        LOGI("MemoryMap: %s",
             "#################### ERROR READING /proc/self/maps ###################");
    }
    fclose(fp);
}





