//
// Created by Lukas on 8/18/2015.
//


#include "abi.h"

#include <sys/ucontext.h>

#include "../../main/jni/abi_interface.h"
#include "cpsr_util.h"

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
    InstructionInfo info;
    mcontext_t* stateInfo = &ctx->uc_mcontext;
    if((stateInfo->arm_cpsr & CPSR_FLAG_THUMB) != 0)
    {
        info.mem_addr = (void*)stateInfo->arm_pc + 2;
        info.call_addr = info.mem_addr + 1;
        info.thumb = true;
        return info;
    }
    else
    {
        info.mem_addr = (void*)stateInfo->arm_pc + 4;
        info.call_addr = info.mem_addr;
        info.thumb = false;
        return info;
    }
}
