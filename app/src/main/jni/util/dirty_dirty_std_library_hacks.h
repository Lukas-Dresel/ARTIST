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
 
//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_DIRTY_DIRTY_STD_LIBRARY_HACKS_H
#define NDKTEST_DIRTY_DIRTY_STD_LIBRARY_HACKS_H

#include <stdint.h>

struct DIRTY_DIRTY_STD_STRING_HACK
{
    uint8_t content[32];
};
struct DIRTY_DIRTY_STD_VECTOR_HACK
{
    // This is SOOOOO NASTY, NEVER ACTUALLY USE THIS.
    // This is simply here, to emulate the contents of a std::vector in c.

    void* mem_begin;        // This should be the start-address of the memory
    void* in_use_end;       // This should point to the first byte after the used entries
    void* allocated_end;    // This should point to the first byte after the allocated memory
};

#endif //NDKTEST_DIRTY_DIRTY_STD_LIBRARY_HACKS_H
