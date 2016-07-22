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
// Created by Lukas on 8/14/2015.
//

#ifndef NDKTEST_GENERATE_TRAP_INSTRUCTION_H
#define NDKTEST_GENERATE_TRAP_INSTRUCTION_H

#include <stdint.h>


uint16_t make_thumb_trap_instruction(uint32_t method);
uint32_t make_arm_trap_instruction(uint32_t method);

#endif //NDKTEST_GENERATE_TRAP_INSTRUCTION_H
