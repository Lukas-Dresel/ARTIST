#ifndef _UTIL_H
#define _UTIL_H

#include <stdbool.h>
#include <inttypes.h>

uint64_t alignToSize(uint64_t value, uint64_t size);
void* alignAddressToSize(const void* addr, uint64_t size);
void* getPageBaseAddress(const void* address);
uint64_t getPageBaseOffset(const void* address);
void* getCodeBaseAddress(const void* address);
uint64_t getCodeBaseOffset(const void* address);

const char* boolToString(bool b);

#endif





