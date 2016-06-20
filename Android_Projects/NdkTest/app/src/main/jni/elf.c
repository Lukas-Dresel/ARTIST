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
 
#include "elf.h"

#ifdef __cplusplus
extern "C" {
#endif

bool IsValidElfFileHeader(void* p)
{
    Elf32_Ehdr* hdr = p;

    if(hdr->e_ident[EI_MAG0] != ELFMAG0 ||
       hdr->e_ident[EI_MAG1] != ELFMAG1 ||
       hdr->e_ident[EI_MAG2] != ELFMAG2 ||
       hdr->e_ident[EI_MAG3] != ELFMAG3)
    {
        return false;
    }

    // Further checks could be added, but not necessary for our purposes yet

    return true;
}

#ifdef __cplusplus
}
#endif