//
// Created by Lukas on 8/14/2015.
//

#ifndef NDKTEST_GENERATE_TRAP_INSTRUCTION_H
#define NDKTEST_GENERATE_TRAP_INSTRUCTION_H

#include <stdint.h>

uint16_t make_thumb_trap_instruction(uint32_t method);
uint32_t make_arm_trap_instruction(uint32_t method);

#endif //NDKTEST_GENERATE_TRAP_INSTRUCTION_H
