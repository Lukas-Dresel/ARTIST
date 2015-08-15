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

static int const THUMB_ILLEGAL_INSTRUCTION_SCHEMATIC   = 0b1101111000000000;
static int const THUMB_ILLEGAL_INSTRUCTION_VARIATION   = 0b0000000011111111;



uint32_t make_arm_illegal_instruction(uint8_t condition, uint16_t variation);
uint16_t extract_arm_illegal_instruction_variation(uint32_t opcode);
uint8_t extract_arm_illegal_instruction_condition(uint32_t opcode);

uint16_t make_thumb_illegal_instruction(uint8_t variation);
uint8_t extract_thumb_illegal_instruction_variation(uint16_t opcode);

#endif //NDKTEST_ILLEGAL_INSTRUCTION_H
