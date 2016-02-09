//
// Created by Lukas on 8/12/2015.
//

#include "instruction_bkpt.h"

uint32_t make_arm_breakpoint(uint16_t imm16)
{
    uint32_t imm_upper = imm16;
    imm_upper <<= 4;
    imm_upper &= ARM_BKPT_OP_UPPER;

    uint16_t imm_lower = (imm16 & 0b1111);

    uint32_t opcode = ARM_BKPT_SCHEMATIC | imm_upper | imm_lower;

    return opcode;
}

uint16_t extract_arm_breakpoint_operand(uint32_t opcode)
{
    uint32_t imm_upper = (opcode & ARM_BKPT_OP_UPPER) >> 4;
    uint32_t imm_lower = (opcode & ARM_BKPT_OP_LOWER);
    uint16_t result = (uint16_t)((imm_upper | imm_lower) & 0xFFFF);
    return result;
}

uint16_t make_thumb_breakpoint(uint8_t imm8)
{
    uint16_t opcode = (uint16_t)THUMB_BKPT_SCHEMATIC | imm8;
    return opcode;
}

uint8_t extract_thumb_breakpoint_operand(uint16_t opcode)
{
    uint8_t result = (uint8_t)(opcode & THUMB_BKPT_OPERAND);
    return result;
}
