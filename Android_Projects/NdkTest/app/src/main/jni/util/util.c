/*
 * Copyright 2016 Lukas Dresel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <stdbool.h>
#include <inttypes.h>



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

const char *boolToString(bool b)
{
    return b ? "true" : "false";
}

#ifdef __cplusplus
}
#endif




