/*
 * Copyright 2016 Lukas Dresel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
//
// Created by Lukas on 8/12/2015.
//

#include "instruction_illegal.h"

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
