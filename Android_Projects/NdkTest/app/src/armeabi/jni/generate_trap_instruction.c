//
// Created by Lukas on 8/14/2015.
//
#include "generate_trap_instruction.h"

#include "../../main/jni/trappoint_interface.h"
#include "../../main/jni/logging.h"
#include "bkpt.h"
#include "illegal_instruction.h"

uint16_t make_thumb_trap_instruction(uint32_t method)
{
    switch (method)
    {
        case TRAP_METHOD_SIGILL_KNOWN_ILLEGAL_INSTR:
            return make_thumb_illegal_instruction(0x0);

        case TRAP_METHOD_SIGTRAP_BKPT:
            return make_thumb_breakpoint(0x0);

        default:
            LOGE("INVALID TRAP-GENERATION METHOD(%d), defaulting to breakpoint instructions.", method);
            return make_thumb_breakpoint(0x0);
    }
}
uint32_t make_arm_trap_instruction(uint32_t method)
{
    switch (method)
    {
        case TRAP_METHOD_SIGILL_KNOWN_ILLEGAL_INSTR:
            return make_arm_illegal_instruction(UNCONDITIONAL, 0x0);

        case TRAP_METHOD_SIGTRAP_BKPT:
            return make_arm_breakpoint(0x0);

        default:
            LOGE("INVALID TRAP-GENERATION METHOD(%d), defaulting to breakpoint instructions.", method);
            return make_thumb_breakpoint(0x0);
    }
}
