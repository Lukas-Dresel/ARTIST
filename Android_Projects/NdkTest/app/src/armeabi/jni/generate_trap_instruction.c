//
// Created by Lukas on 8/14/2015.
//
#include <stdlib.h>
#include "generate_trap_instruction.h"

#include "../../main/jni/hooking/trappoint_interface.h"
#include "../../main/jni/logging.h"
#include "instruction_bkpt.h"
#include "instruction_illegal.h"

uint16_t make_thumb_trap_instruction(uint32_t method)
{
    if((method & TRAP_METHOD_INSTR_KNOWN_ILLEGAL) != 0)
    {
        return make_thumb_illegal_instruction((uint8_t)rand());
    }
    if((method & TRAP_METHOD_INSTR_BKPT) != 0)
    {
        return make_thumb_breakpoint((uint8_t)rand());
    }
    LOGE("INVALID TRAP-GENERATION METHOD(%d).", method);
    return NULL;
}
uint32_t make_arm_trap_instruction(uint32_t method)
{
    if((method & TRAP_METHOD_INSTR_KNOWN_ILLEGAL) != 0)
    {
        return make_arm_illegal_instruction(UNCONDITIONAL, (uint8_t)rand());
    }
    if((method & TRAP_METHOD_INSTR_BKPT) != 0)
    {
        return make_arm_breakpoint((uint8_t)rand());
    }
    LOGE("INVALID TRAP-GENERATION METHOD(%d).", method);
    return NULL;
}
