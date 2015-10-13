//
// Created by Lukas on 8/18/2015.
//


#include "abi.h"

#include <sys/ucontext.h>

#include "../../main/jni/abi_interface.h"
#include "cpsr_util.h"
#include "../../main/jni/util.h"

bool is_address_thumb_mode(void* address)
{
    void* aligned = align_address_to_size(address, 4);
    uint32_t offset = (uint32_t)(address - aligned);
    return (offset == 1 || offset == 3);
}
void* entry_point_to_code_pointer(void* address)
{
    return (void*)((uint64_t)address & ~0x1);
}

static const uint16_t thumb_32bit_mask = 0b1111100000000000;  //  this one can be a const int.

// But these have to be constant expressions for the compiler to use them in a switch-case statement
#define THUMB_32BIT_INSTRUCTION_PATTERN1 0b1110100000000000
#define THUMB_32BIT_INSTRUCTION_PATTERN2 0b1111000000000000
#define THUMB_32BIT_INSTRUCTION_PATTERN3 0b1111100000000000

uint32_t get_instruction_length(void* address)
{
    if(!is_address_thumb_mode(address))
    {
        return 4;
    }
    uint16_t halfword = *(uint16_t*)entry_point_to_code_pointer(address);
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

uint32_t get_argument(ucontext_t* c, unsigned int index)
{
    mcontext_t* state_info = &(c->uc_mcontext);
    switch(index)
    {
        case 0:
            return state_info->arm_r0;
        case 1:
            return state_info->arm_r1;
        case 2:
            return state_info->arm_r2;
        case 3:
            return state_info->arm_r3;

        default:
            return *(((uint32_t*)(state_info->arm_sp)) + (index - 4));
    }
}

void set_argument(ucontext_t* c, unsigned int index, uint32_t val)
{
    mcontext_t* state_info = &(c->uc_mcontext);
    switch(index)
    {
        case 0:
            state_info->arm_r0 = val;
            break;
        case 1:
            state_info->arm_r1 = val;
            break;
        case 2:
            state_info->arm_r2 = val;
            break;
        case 3:
            state_info->arm_r3 = val;
            break;

        default:
            *(((uint32_t*)(state_info->arm_sp)) + (index - 4)) = val;
            break;
    }
}

InstructionInfo extract_next_executed_instruction(ucontext_t* ctx)
{
    InstructionInfo result;
    mcontext_t* stateInfo = &ctx->uc_mcontext;

    result.thumb = (stateInfo->arm_cpsr & CPSR_FLAG_THUMB) != 0;

    void* addr = (void*)stateInfo->arm_pc + ((result.thumb) ? 1 : 0);
    void* next_addr = addr + get_instruction_length(addr);

    result.call_addr = next_addr;
    result.mem_addr = entry_point_to_code_pointer(next_addr);
    return result;
}
