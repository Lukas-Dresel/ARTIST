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
// Created by Lukas on 9/25/2015.
//

#include <stdbool.h>
#include "bit_vector_util.h"
#include "../logging.h"
#include "../util/bit_utils.h"

// Mostly adopted from AOSP source code.
// platform/art/+/master/runtime/base/bit_vector.cc

static const uint32_t kWordBytes = sizeof(uint32_t);
static const uint32_t kWordBits = 8 * sizeof(uint32_t);

static uint32_t WordIndex(uint32_t idx)
{
    return idx >> 5;
}
// A bit mask to extract the bit for the given index.
static uint32_t BitMask(uint32_t idx)
{
    return (uint32_t)(1 << (idx & 0x1f));
}

#if defined(__clang__) && defined(__ARM_64BIT_STATE)
// b/19180814 When POPCOUNT is inlined, boot up failed on arm64 devices.
__attribute__((optnone))
#endif
uint32_t bit_vector_NumSetBits(const uint32_t* data, uint32_t end)
{
    CHECK(data != NULL);

    uint32_t word_end = WordIndex(end);
    uint32_t partial_word_bits = end & 0x1f;

    uint32_t count = 0u;
    for(uint32_t word = 0u; word < word_end; word++)
    {
        count += COUNT_32BIT_SET_BITS(data[word]);
    }
    if(partial_word_bits != 0u)
    {
        count += COUNT_32BIT_SET_BITS(data[word_end] & ~(0xffffffffu << partial_word_bits));
    }
    return count;

}
bool bit_vector_IsBitSet(const uint32_t* data, uint32_t bit_index)
{
    CHECK(data != NULL);

    return (data[WordIndex(bit_index)] & BitMask(bit_index)) != 0;
}