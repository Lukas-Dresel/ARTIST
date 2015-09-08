#ifndef _UTIL_H
#define _UTIL_H

#include <stdbool.h>
#include <inttypes.h>

#define COUNT_32BIT_LEADING_ZEROS(x) __builtin_clz(x)
#define COUNT_64BIT_LEADING_ZEROS(x) __builtin_clzll(x)

#define COUNT_32BIT_TAILING_ZEROS(x) __builtin_ctz(x)
#define COUNT_64BIT_TAILING_ZEROS(x) __builtin_ctzll(x)


uint64_t round_down(uint64_t value, uint64_t size);

uint64_t round_up(uint64_t value, uint64_t size);

void *align_address_to_size(const void *addr, uint64_t size);

void *get_page_base(const void *address);

uint64_t get_page_offset(const void *address);

void *get_code_base_address(const void *address);

uint64_t get_code_base_offset(const void *address);

const char *boolToString(bool b);

#endif





