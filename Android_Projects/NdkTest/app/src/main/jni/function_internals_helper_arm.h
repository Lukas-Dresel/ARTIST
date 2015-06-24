#ifndef _FUNCTION_INTERNALS_HELPER_ARM_H
#define _FUNCTION_INTERNALS_HELPER_H

#include <jni.h>
#include <inttypes.h>
#include <sys/mman.h>

#include "exceptions.h"
#include "system_info.h"
#include "util.h"

static inline bool isFunctionProperlyCodeAligned(void* address)
{
    uint64_t offset = getCodeBaseOffset(address);
    return offset == 1 || offset == 0;
}
static inline bool isFunctionThumbMode(void* address)
{
    uint64_t offset = getCodeBaseOffset(address);
    return offset == 1;
}
static inline bool isFunctionArmMode(void* address)
{
    uint64_t offset = getCodeBaseOffset(address);
    return offset == 0;
}

static inline bool setMemProtectFull(JNIEnv* env, void* address, jlong num)
{
    void* addrAligned = getPageBaseAddress(address);
    jlong size = num + (address - addrAligned);

    if(mprotect(addrAligned, size, PROT_READ|PROT_WRITE|PROT_EXEC) == -1)
    {
        LOGE("Failed to change protections of %lld bytes from address " PRINT_PTR ", error: %s", size, (uintptr_t)addrAligned, strerror(errno));
        throwNewJNIException(env, "java/lang/RuntimeException", "Failed to set memory protections.");
        return false;
    }
    return true;
}
static inline bool setMemProtectFullPrimitive(void* address, jlong num)
{
    void* addrAligned = getPageBaseAddress(address);
    jlong size = num + (address - addrAligned);

    if(mprotect(addrAligned, size, PROT_READ|PROT_WRITE|PROT_EXEC) == -1)
    {
        return false;
    }
    return true;
}

#endif