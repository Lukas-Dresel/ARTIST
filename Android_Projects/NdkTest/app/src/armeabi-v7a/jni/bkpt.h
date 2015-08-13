//
// Created by Lukas on 8/12/2015.
//

#ifndef NDKTEST_BKPT_H
#define NDKTEST_BKPT_H


#include "../../../../../../../../../Users/Lukas/AppData/Local/Android/ndk/platforms/android-21/arch-mips/usr/include/stdint.h"

/*
 * static const int ARM_BKPT_SCHEMATIC = 0b 1110 0001 0010 0000 0000 0000 0111 0000;
 * static const int ARM_BKPT_OP_UPPER  = 0b 0000 0000 0000 1111 1111 1111 0000 0000;
 * static const int ARM_BKPT_OP_LOWER  = 0b 0000 0000 0000 0000 0000 0000 0000 1111;
 *
 *
 *
 */


static int const ARM_BKPT_SCHEMATIC = 0b11100001001000000000000001110000;

static int const ARM_BKPT_OP_UPPER  = 0b00000000000011111111111100000000;
static int const ARM_BKPT_OP_LOWER  = 0b00000000000000000000000000001111;

uint32_t make_arm_breakpoint(uint16_t imm16)
{
    uint32_t imm_upper = imm16;
    imm_upper <<= 4;
    imm_upper &= ARM_BKPT_OP_UPPER;

    uint32_t imm_lower = (imm16 & 0b1111);

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

static int const THUMB_BKPT_SCHEMATIC   = 0b1011111000000000;

static int const THUMB_BKPT_OPERAND     = 0b0000000011111111;

uint16_t make_thumb_breakpoint(uint8_t imm8)
{
    uint32_t opcode = THUMB_BKPT_SCHEMATIC | imm8;
    return opcode;
}

uint8_t extract_thumb_breakpoint_operand(uint16_t opcode)
{
    uint8_t result = (uint8_t)(opcode & THUMB_BKPT_OPERAND);
    return result;
}

#endif //NDKTEST_BKPT_H
