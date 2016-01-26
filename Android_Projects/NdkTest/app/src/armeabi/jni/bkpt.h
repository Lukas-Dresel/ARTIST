//
// Created by Lukas on 8/12/2015.
//

#ifndef NDKTEST_BKPT_H
#define NDKTEST_BKPT_H


#include <stdint.h>

static int const ARM_BKPT_SCHEMATIC     = 0b11100001001000000000000001110000;
static int const ARM_BKPT_OP_UPPER      = 0b00000000000011111111111100000000;
static int const ARM_BKPT_OP_LOWER      = 0b00000000000000000000000000001111;

static int const THUMB_BKPT_SCHEMATIC   = 0b1011111000000000;
static int const THUMB_BKPT_OPERAND     = 0b0000000011111111;


uint32_t make_arm_breakpoint(uint16_t imm16);
uint16_t extract_arm_breakpoint_operand(uint32_t opcode);

uint16_t make_thumb_breakpoint(uint8_t imm8);
uint8_t extract_thumb_breakpoint_operand(uint16_t opcode);

#endif //NDKTEST_BKPT_H
