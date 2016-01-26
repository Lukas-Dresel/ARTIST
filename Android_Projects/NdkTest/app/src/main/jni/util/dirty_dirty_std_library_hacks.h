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
