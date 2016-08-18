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
// Created by Lukas on 8/18/2015.
//


#include <sys/ucontext.h>
#include <utility/util.h>
#include "../../main/jni/abi.h"

#include "../../main/jni/cpsr_util.h"

bool        IsAddressThumbMode(const void *address)
{
    void* aligned = align_address_to_size(address, 4);
    uint32_t offset = (uint32_t)(address - aligned);
    return (offset == 1 || offset == 3);
}
void* InstructionPointerToCodePointer(const void *instruction_pointer)
{
    return (void*)((uint64_t) instruction_pointer & ~0x1);
}

static const uint16_t thumb_32bit_mask = 0b1111100000000000;  //  this one can be a const int.

// But these have to be constant expressions for the compiler to use them in a switch-case statement
#define THUMB_32BIT_INSTRUCTION_PATTERN1 0b1110100000000000
#define THUMB_32BIT_INSTRUCTION_PATTERN2 0b1111000000000000
#define THUMB_32BIT_INSTRUCTION_PATTERN3 0b1111100000000000

uint32_t GetInstructionLengthAtInstructionPointer(const void *instruction_pointer)
{
    if(!IsAddressThumbMode(instruction_pointer))
    {
        return 4;
    }
    uint16_t halfword = *(uint16_t*) InstructionPointerToCodePointer(instruction_pointer);
    uint16_t masked = halfword & thumb_32bit_mask;
    switch(masked)
    {
        case THUMB_32BIT_INSTRUCTION_PATTERN1:
        case THUMB_32BIT_INSTRUCTION_PATTERN2:
        case THUMB_32BIT_INSTRUCTION_PATTERN3:
            return 4;
        default:
            return 2;
    }
}

uint64_t GetArgument(ucontext_t *c, unsigned int index)
{
    mcontext_t* state_info = &(c->uc_mcontext);
    switch(index)
    {
        case 0:
            return (uint64_t)state_info->arm_r0;
        case 1:
            return (uint64_t)state_info->arm_r1;
        case 2:
            return (uint64_t)state_info->arm_r2;
        case 3:
            return (uint64_t)state_info->arm_r3;

        default:
            return (uint64_t)*(((uint32_t*)(state_info->arm_sp)) + (index - 4));
    }
}

void SetArgument(ucontext_t *c, unsigned int index, uint64_t val)
{
    mcontext_t* state_info = &(c->uc_mcontext);
    switch(index)
    {
        case 0:
            state_info->arm_r0 = (uint32_t)val;
            break;
        case 1:
            state_info->arm_r1 = (uint32_t)val;
            break;
        case 2:
            state_info->arm_r2 = (uint32_t)val;
            break;
        case 3:
            state_info->arm_r3 = (uint32_t)val;
            break;

        default:
            *(((uint32_t*)(state_info->arm_sp)) + (index - 4)) = (uint32_t)val;
            break;
    }
}

void* ExtractNextExecutedInstructionPointer(ucontext_t *ctx)
{
    mcontext_t* stateInfo = &ctx->uc_mcontext;

    bool thumb = (stateInfo->arm_cpsr & CPSR_FLAG_THUMB) != 0;

    void* addr = (void*)stateInfo->arm_pc + (thumb ? 1 : 0);
    void* next_addr = addr + GetInstructionLengthAtInstructionPointer(addr);

    return next_addr;
}
void* ExtractReturnAddress(ucontext_t *ctx)
{
    mcontext_t* stateInfo = &ctx->uc_mcontext;
    void* addr = (void*)stateInfo->arm_lr;
    return addr;
}
