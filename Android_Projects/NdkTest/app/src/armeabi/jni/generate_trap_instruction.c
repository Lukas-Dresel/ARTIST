//
// Created by Lukas on 8/14/2015.
//
#include "generate_trap_instruction.h"

#include "../../main/jni/hooking/trappoint_interface.h"
#include "../../main/jni/logging.h"
#include "bkpt.h"
#include "illegal_instruction.h"

uint16_t make_thumb_trap_instruction(uint32_t method)
{
    if((method & TRAP_METHOD_INSTR_KNOWN_ILLEGAL) != 0)
    {
        return make_thumb_illegal_instruction(0x0);
    }
    if((method & TRAP_METHOD_INSTR_BKPT) != 0)
    {
        return make_thumb_breakpoint(0x0);
    }
    LOGE("INVALID TRAP-GENERATION METHOD(%d).", method);
    return NULL;
}
uint32_t make_arm_trap_instruction(uint32_t method)
{
    if((method & TRAP_METHOD_INSTR_KNOWN_ILLEGAL) != 0)
    {
        return make_arm_illegal_instruction(UNCONDITIONAL, 0x0);
    }
    if((method & TRAP_METHOD_INSTR_BKPT) != 0)
    {
        return make_arm_breakpoint(0x0);
    }
    LOGE("INVALID TRAP-GENERATION METHOD(%d).", method);
    return NULL;
}
