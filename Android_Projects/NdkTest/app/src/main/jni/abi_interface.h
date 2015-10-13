//
// Created by Lukas on 8/18/2015.
//

#ifndef NDKTEST_ABI_INTERFACE_H
#define NDKTEST_ABI_INTERFACE_H


#include <stdbool.h>
#include <stdint.h>
#include <ucontext.h>

struct InstructionInfo;
typedef struct InstructionInfo InstructionInfo;

void* entry_point_to_code_pointer(void* address);
uint32_t get_instruction_length(void* address);

uint32_t get_argument(ucontext_t *c, unsigned int index);
void set_argument(ucontext_t *c, unsigned int index, uint32_t val);
InstructionInfo extract_next_executed_instruction(ucontext_t *ctx);


#endif //NDKTEST_ABI_H
