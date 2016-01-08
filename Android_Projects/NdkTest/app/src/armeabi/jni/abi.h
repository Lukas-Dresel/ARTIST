//
// Created by Lukas on 8/18/2015.
//

#ifndef NDKTEST_ABI_H
#define NDKTEST_ABI_H

#include <stdbool.h>
#include <stdint.h>

struct InstructionInfo
{
    const void* call_addr;
    const void* mem_addr;
    bool        thumb;
};

bool IsAddressThumbMode(void *address);

#endif //NDKTEST_ABI_H
