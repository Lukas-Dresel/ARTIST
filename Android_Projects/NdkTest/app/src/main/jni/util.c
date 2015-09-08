#include <stdbool.h>
#include <inttypes.h>

#include "system_info.h"


#ifdef __cplusplus
extern "C"
{
#endif

uint64_t round_down(uint64_t value, uint64_t size)
{
    return value - (value % size);
}

uint64_t round_up(uint64_t value, uint64_t size)
{
    uint64_t rounded_down = round_down(value, size);
    if (rounded_down == value)
    {
        return value;
    }
    return rounded_down + size;
}

void *align_address_to_size(const void *addr, uint64_t size)
{
    return (void *) round_down((uint64_t) addr, size);
}

void *get_page_base(const void *address)
{
    return align_address_to_size(address, getSystemPageSize());
}

uint64_t get_page_offset(const void *address)
{
    return (uint64_t) (address - get_page_base(address));
}

void *get_code_base_address(const void *address)
{
    return align_address_to_size(address, 4);
}

uint64_t get_code_base_offset(const void *address)
{
    return (uint64_t) (address - align_address_to_size(address, 4));
}

const char *boolToString(bool b)
{
    return b ? "true" : "false";
}

#ifdef __cplusplus
}
#endif




