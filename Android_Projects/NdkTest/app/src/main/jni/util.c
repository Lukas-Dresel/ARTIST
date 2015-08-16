#include <stdbool.h>
#include <inttypes.h>

#include "system_info.h"


#ifdef __cplusplus
extern "C"
{
#endif

uint64_t alignToSize(uint64_t value, uint64_t size)
{
    return value - (value % size);
}

void *alignAddressToSize(const void *addr, uint64_t size)
{
    return (void *) alignToSize((uint64_t) addr, size);
}

 void *getPageBaseAddress(const void *address)
 {
    return alignAddressToSize(address, getSystemPageSize());
}

uint64_t getPageBaseOffset(const void *address)
{
    return (uint64_t) (address - getPageBaseAddress(address));
}

void *getCodeBaseAddress(const void *address)
{
    return alignAddressToSize(address, 4);
}

uint64_t getCodeBaseOffset(const void *address)
{
    return (uint64_t) (address - alignAddressToSize(address, 4));
}

const char *boolToString(bool b)
{
    return b ? "true" : "false";
}

#ifdef __cplusplus
}
#endif




