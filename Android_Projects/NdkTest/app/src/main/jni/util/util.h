#ifndef _UTIL_H
#define _UTIL_H

#include <stdbool.h>
#include <inttypes.h>

typedef bool (*PREDICATE)(void* current, void* additionalArgs);

uint64_t round_down(uint64_t value, uint64_t size);

uint64_t round_up(uint64_t value, uint64_t size);

void *align_address_to_size(const void *addr, uint64_t size);

void *get_page_base(const void *address);

uint64_t get_page_offset(const void *address);

const char *boolToString(bool b);

#endif





