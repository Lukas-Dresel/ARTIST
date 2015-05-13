#ifndef _UTIL_H
#define _UTIL_H

#include <stdbool.h>
#include <inttypes.h>

#include "system_info.h"

static inline uint64_t alignToSize(uint64_t value, uint64_t size)
{
    return value - (value % size);
}
static inline void* alignAddressToSize(void* addr, uint64_t size)
{
    return (void*)alignToSize((uint64_t)addr, size);
}
static inline void* getPageBaseAddress(void* address)
{
    return alignAddressToSize(address, getSystemPageSize());
}
static inline uint64_t getPageBaseOffset(void* address)
{
    return (uint64_t)(address - getPageBaseAddress(address));
}
static inline void* getCodeBaseAddress(void* address)
{
    return alignAddressToSize(address, 4);
}
static inline uint64_t getCodeBaseOffset(void* address)
{
    return (uint64_t)(address - alignAddressToSize(address, 4));
}

static inline const char* boolToString(bool b)
{
    return b ? "true" : "false";
}

#endif





