#include "system_info.h"

static long SYSTEM_PAGE_SIZE = -1;

void init_system_info(JNIEnv* env)
{
    SYSTEM_PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
    if(SYSTEM_PAGE_SIZE < 1)
    {
        LOGE("Error while resolving system page size. Assuming 4k bytes. Err: %s", strerror(errno));
        SYSTEM_PAGE_SIZE = 4096;
    }
}
void destroy_system_info(JNIEnv* env)
{
}

long getSystemPageSize()
{
    return SYSTEM_PAGE_SIZE;
}





