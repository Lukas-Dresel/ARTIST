//
// Created by Lukas on 11/14/2015.
//

#ifndef NDKTEST_ANDROID_UTF_H
#define NDKTEST_ANDROID_UTF_H

/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Header file created to match android_utf.c

#include <stddef.h>
#include <stdint.h>

uint16_t        GetTrailingUtf16Char(uint32_t maybe_pair);
uint16_t        GetLeadingUtf16Char(uint32_t maybe_pair);
uint32_t        GetUtf16FromUtf8(const char **utf8_data_in);
int             CompareModifiedUtf8ToModifiedUtf8AsUtf16CodePointValues(const char *utf8_1,
                                                                   const char *utf8_2);
size_t          CountModifiedUtf8Chars(const char *utf8);
void            ConvertModifiedUtf8ToUtf16(uint16_t *utf16_data_out, const char *utf8_data_in);
void            ConvertUtf16ToModifiedUtf8(char *utf8_out, const uint16_t *utf16_in, size_t char_count);
int32_t         ComputeUtf16Hash(const uint16_t *chars, size_t char_count);
size_t          ComputeModifiedUtf8Hash(const char *chars);
int             CompareModifiedUtf8ToUtf16AsCodePointValues(const char *utf8, const uint16_t *utf16,
                                                size_t utf16_length);
size_t CountUtf8Bytes(const uint16_t *chars, size_t char_count);
#endif //NDKTEST_ANDROID_UTF_H
