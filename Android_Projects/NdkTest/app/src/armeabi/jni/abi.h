//
// Created by Lukas on 8/18/2015.
//

#ifndef NDKTEST_ABI_H
#define NDKTEST_ABI_H

#include <stdbool.h>

struct InstructionInfo
{
    void*   call_addr;
    void*   mem_addr;
    bool    thumb;
};
typedef struct InstructionInfo InstructionInfo;

#endif //NDKTEST_ABI_H
