//
// Created by Lukas on 8/18/2015.
//

#ifndef NDKTEST_ABI_INTERFACE_H
#define NDKTEST_ABI_INTERFACE_H


#include <stdbool.h>
#include <stdint.h>
#include <ucontext.h>

struct InstructionInfo;

const void*             EntryPointToCodePointer(const void *entry_point);
uint32_t                GetInstructionLength(const void *entry_point);

uint32_t                GetArgument(ucontext_t *c, unsigned int index);
void                    SetArgument(ucontext_t *c, unsigned int index, uint32_t val);
struct InstructionInfo  ExtractNextExecutedInstruction(ucontext_t *ctx);


#endif //NDKTEST_ABI_H
