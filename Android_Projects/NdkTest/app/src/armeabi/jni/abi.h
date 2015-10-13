//
// Created by Lukas on 8/18/2015.
//

#ifndef NDKTEST_ABI_H
#define NDKTEST_ABI_H

#include <stdbool.h>
#include <stdint.h>

struct InstructionInfo
{
    void*   call_addr;
    void*   mem_addr;
    bool    thumb;
};
typedef struct InstructionInfo InstructionInfo;

bool is_address_thumb_mode(void* address);

uint32_t get_instruction_length(void* address);

#endif //NDKTEST_ABI_H
