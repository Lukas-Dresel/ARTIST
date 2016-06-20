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
// Created by Lukas on 3/11/2016.
//

#ifndef NDKTEST_MIRROR_HACKS_H
#define NDKTEST_MIRROR_HACKS_H

#include <stdint.h>

struct MirrorHackStringContent
{
    char unknown[12];
    uint16_t chars[0];
};
struct MirrorHackString
{
    void* some_ref;
    void* null;
    struct MirrorHackStringContent* str_content;
    uint32_t str_len;
    uint32_t hash;
};


#endif //NDKTEST_MIRROR_HACKS_H
