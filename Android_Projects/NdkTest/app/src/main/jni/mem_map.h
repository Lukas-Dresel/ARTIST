//
// Created by Lukas on 8/18/2015.
//

#ifndef NDKTEST_MEM_MAP_H
#define NDKTEST_MEM_MAP_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include "typedefs.h"


typedef struct MemMap
{
    const char *name_;
    byte *begin_;  // Start of data.
    size_t size_;  // Length of data.
    void *base_begin_;  // Page-aligned base address.
    size_t base_size_;  // Length of mapping. May be changed by RemapAtEnd (ie Zygote).
    int prot_;  // Protection of the map.
    // When reuse_ is true, this is just a view of an existing mapping
    // and we do not take ownership and are not responsible for
    // unmapping.
    bool reuse_;
} MemMap;

bool mem_map_ForceClear(MemMap *self);

bool mem_map_Initialize(MemMap *self);

static bool CheckMapRequest(byte *expected_ptr, void *actual_ptr, size_t byte_count);

bool mem_map_MapFileAtAddress(MemMap *self, byte *expected_ptr, size_t byte_count,
                              int prot, int flags, int fd, off_t start, bool reuse,
                              const char *filename);

bool mem_map_Unmap(MemMap *self);

#endif //NDKTEST_MEM_MAP_H
