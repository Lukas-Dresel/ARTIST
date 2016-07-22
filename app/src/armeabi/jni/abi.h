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
// Created by Lukas on 8/18/2015.
//

#ifndef NDKTEST_ABI_H
#define NDKTEST_ABI_H

#include <stdbool.h>
#include <stdint.h>

struct InstructionInfo
{
    const void* call_addr;
    const void* mem_addr;
    bool        thumb;
};

bool IsAddressThumbMode(const void *address);

#endif //NDKTEST_ABI_H
