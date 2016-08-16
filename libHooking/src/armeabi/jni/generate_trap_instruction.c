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
#include <stdlib.h>
#include <utility/logging.h>

#include "../../main/jni/generate_trap_instruction.h"
#include "../../main/jni/trappoint_interface.h"
#include "../../main/jni/instruction_bkpt.h"
#include "../../main/jni/instruction_illegal.h"

uint16_t make_thumb_trap_instruction(uint32_t method)
{
    if((method & TRAP_METHOD_INSTR_KNOWN_ILLEGAL) != 0)
    {
        return make_thumb_illegal_instruction((uint8_t)rand());
    }
    if((method & TRAP_METHOD_INSTR_BKPT) != 0)
    {
        return make_thumb_breakpoint((uint8_t)rand());
    }
    LOGE("INVALID TRAP-GENERATION METHOD(%d).", method);
    return NULL;
}
uint32_t make_arm_trap_instruction(uint32_t method)
{
    if((method & TRAP_METHOD_INSTR_KNOWN_ILLEGAL) != 0)
    {
        return make_arm_illegal_instruction(UNCONDITIONAL, (uint8_t)rand());
    }
    if((method & TRAP_METHOD_INSTR_BKPT) != 0)
    {
        return make_arm_breakpoint((uint8_t)rand());
    }
    LOGE("INVALID TRAP-GENERATION METHOD(%d).", method);
    return NULL;
}
