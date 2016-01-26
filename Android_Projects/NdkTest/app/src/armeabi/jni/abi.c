//
// Created by Lukas on 8/18/2015.
//


#include "abi.h"

#include <sys/ucontext.h>

#include "cpsr_util.h"
#include "../../main/jni/util/util.h"

bool        IsAddressThumbMode(const void *address)
{
    void* aligned = align_address_to_size(address, 4);
    uint32_t offset = (uint32_t)(address - aligned);
    return (offset == 1 || offset == 3);
}
const void* EntryPointToCodePointer(const void *address)
{
    return (const void*)((uint64_t)address & ~0x1);
}

static const uint16_t thumb_32bit_mask = 0b1111100000000000;  //  this one can be a const int.

// But these have to be constant expressions for the compiler to use them in a switch-case statement
#define THUMB_32BIT_INSTRUCTION_PATTERN1 0b1110100000000000
#define THUMB_32BIT_INSTRUCTION_PATTERN2 0b1111000000000000
#define THUMB_32BIT_INSTRUCTION_PATTERN3 0b1111100000000000

uint32_t GetInstructionLength(const void* entry_point)
{
    if(!IsAddressThumbMode(entry_point))
    {
        return 4;
    }
    uint16_t halfword = *(uint16_t*) EntryPointToCodePointer(entry_point);
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

uint32_t GetArgument(ucontext_t *c, unsigned int index)
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

void SetArgument(ucontext_t *c, unsigned int index, uint32_t val)
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

struct InstructionInfo ExtractNextExecutedInstruction(ucontext_t *ctx)
{
    struct InstructionInfo result;
    mcontext_t* stateInfo = &ctx->uc_mcontext;

    result.thumb = (stateInfo->arm_cpsr & CPSR_FLAG_THUMB) != 0;

    void* addr = (void*)stateInfo->arm_pc + ((result.thumb) ? 1 : 0);
    void* next_addr = addr + GetInstructionLength(addr);

    result.call_addr = next_addr;
    result.mem_addr = EntryPointToCodePointer(next_addr);
    return result;
}
