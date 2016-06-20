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
 
#ifndef _ELF_ANALYZER_H
#define _ELF_ANALYZER_H

#include <linux/elf.h>
#include <stdbool.h>

enum SegmentType
{
    Null                    = PT_NULL,
    Load                    = PT_LOAD,
    Dynamic                 = PT_DYNAMIC,
    Interpreter             = PT_INTERP,
    Note                    = PT_NOTE,
    ShLib_Unused            = PT_SHLIB,
    ProgramHeader           = PT_PHDR,
    ThreadLocalStorage      = PT_TLS,
    OsSpecificLow           = PT_LOOS,
    OsSpecificHigh          = PT_HIOS,
    ProcessorSpecificLow    = PT_LOPROC,
    ProcessorSpecificHigh   = PT_HIPROC,
    GCC_EHFrameHeader       = PT_GNU_EH_FRAME,
    GCC_StackExecutability  = PT_GNU_STACK
};

bool IsValidElfFileHeader(void* p);

#endif