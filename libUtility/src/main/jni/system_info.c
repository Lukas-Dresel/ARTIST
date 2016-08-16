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
#include "system_info.h"

#include <unistd.h>

#include "logging.h"
#include "util.h"


#include <stdio.h>

static long SYSTEM_PAGE_SIZE = -1;

void init_system_info()
{
    SYSTEM_PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
    if (SYSTEM_PAGE_SIZE < 1)
    {
        LOGE("Error while resolving system page size. Assuming 4k bytes. Err: %s", strerror(errno));
        SYSTEM_PAGE_SIZE = 4096;
    }
}

void destroy_system_info()
{
}

long getSystemPageSize(void)
{
    return SYSTEM_PAGE_SIZE;
}

void *get_page_base(const void *address)
{
    return align_address_to_size(address, (uint64_t)getSystemPageSize());
}

uint64_t get_page_offset(const void *address)
{
    return (uint64_t) (address - get_page_base(address));
}





