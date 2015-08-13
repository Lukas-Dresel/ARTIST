//
// Created by Lukas on 8/12/2015.
//

#ifndef NDKTEST_ILLEGAL_INSTRUCTION_H
#define NDKTEST_ILLEGAL_INSTRUCTION_H


#include <stdint.h>


enum CONDITION
{
    ZERO_SET                = 0b0000,       // Z set
    ZERO_CLEAR              = 0b0001,       // Z clear
    CARRY_SET               = 0b0010,       // C set
    CARRY_CLEAR             = 0b0011,       // C clear
    NEGATIVE                = 0b0100,       // N set
    NEGATIVE_CLEAR          = 0b0101,       // N unset
    OVERFLOW                = 0b0110,       // O set
    OVERFLOW_CLEAR          = 0b0111,       // O clear
    UNSIGNED_HIGHER         = 0b1000,       // C set       and     Z clear
    UNSIGNED_LOWER_OR_SAME  = 0b1001,       // C clear     or      Z set
    SIGNED_GREATER_OR_EQUAL = 0b1010,       // N ==  V
    SIGNED_LESS_THAN        = 0b1011,       // N xor V
    SIGNED_GREATER_THAN     = 0b1100,       // N ==  V && Z == 0
    SIGNED_LESS_OR_EQUAL    = 0b1101,       // N xor V || Z == 1
    UNCONDITIONAL           = 0b1110,       // Always execute
    UNDEFINED_OR_UNCOND_EXT = 0b1111,       // see ARM reference
};

static int const ARM_ILLEGAL_INSTR_SCHEMATIC = 0b00000111111100000000000011110000;

static int const ARM_ILLEGAL_INSTR_CONDITION = (0b1111 << 28);
static int const ARM_ILLEGAL_INSTR_VAR_UPPER = 0b00000000000011111111111100000000;
static int const ARM_ILLEGAL_INSTR_VAR_LOWER = 0b00000000000000000000000000001111;

uint32_t make_arm_illegal_instruction(uint8_t condition, uint16_t variation)
{
    uint32_t cond = (uint32_t)((condition & 0xF) << 28);

    uint32_t variation_upper = (uint32_t)(variation & 0xFFF0);
    variation_upper <<= 4;
    variation_upper &= ARM_ILLEGAL_INSTR_VAR_UPPER;

    uint32_t variation_lower = (uint32_t)(variation & 0b1111);

    uint32_t opcode = ARM_ILLEGAL_INSTR_SCHEMATIC | cond | variation_upper | variation_lower;

    return opcode;
}

uint16_t extract_arm_illegal_instruction_variation(uint32_t opcode)
{
    uint32_t var_upper = (opcode & ARM_ILLEGAL_INSTR_VAR_UPPER) >> 4;
    uint32_t var_lower = (opcode & ARM_ILLEGAL_INSTR_VAR_LOWER);
    uint16_t result = (uint16_t)((var_upper | var_lower) & 0xFFFF);
    return result;
}
uint8_t extract_arm_illegal_instruction_condition(uint32_t opcode)
{
    return (uint8_t)(((opcode & ARM_ILLEGAL_INSTR_CONDITION) >> 28) & 0x0F);
}

static int const THUMB_ILLEGAL_INSTRUCTION_SCHEMATIC   = 0b1101111000000000;

static int const THUMB_ILLEGAL_INSTRUCTION_VARIATION   = 0b0000000011111111;

uint16_t make_thumb_illegal_instruction(uint8_t variation)
{
    uint16_t opcode = (uint16_t)THUMB_ILLEGAL_INSTRUCTION_SCHEMATIC | variation;
    return opcode;
}

uint8_t extract_thumb_illegal_instruction_variation(uint16_t opcode)
{
    uint8_t result = (uint8_t)(opcode & 0xFF);
    return result;
}

#endif //NDKTEST_ILLEGAL_INSTRUCTION_H
