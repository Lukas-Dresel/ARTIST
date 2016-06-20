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
// Created by Lukas on 8/23/2015.
//

#ifndef NDKTEST_LEB128_H
#define NDKTEST_LEB128_H

#include <stdint.h>

// Reads an unsigned LEB128 value, updating the given pointer to point
// just past the end of the read value. This function tolerates
// non-zero high-order bits in the fifth encoded byte.
uint32_t DecodeUnsignedLeb128(const uint8_t **data);

// Reads an unsigned LEB128 + 1 value. updating the given pointer to point
// just past the end of the read value. This function tolerates
// non-zero high-order bits in the fifth encoded byte.
// It is possible for this function to return -1.
int32_t DecodeUnsignedLeb128P1(const uint8_t **data);

// Reads a signed LEB128 value, updating the given pointer to point
// just past the end of the read value. This function tolerates
// non-zero high-order bits in the fifth encoded byte.
int32_t DecodeSignedLeb128(const uint8_t **data);

// Returns the number of bytes needed to encode the value in unsigned LEB128.
uint32_t UnsignedLeb128Size(uint32_t data);

// Returns the number of bytes needed to encode the value in unsigned LEB128.
uint32_t SignedLeb128Size(int32_t data);

uint8_t *EncodeUnsignedLeb128(uint8_t *dest, uint32_t value);

uint8_t *EncodeSignedLeb128(uint8_t *dest, int32_t value);

#endif //NDKTEST_LEB128_H
