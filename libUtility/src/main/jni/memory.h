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
 
#ifndef _MEMORY_H
#define _MEMORY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <jni.h>

#include <unistd.h>
#include <sys/mman.h>

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef bool (*OUTPUT_CALLBACK)(const void *startingAddress, uint64_t currentOffset,
                                int numCurrentBytes, void *additionalInfo);

bool set_memory_protection(const void *addr, jlong numBytes, jboolean read, jboolean write,
                           jboolean execute);

bool iterate_byte_array_chunks(JNIEnv *env, const void *addr, jlong numBytes, jlong bytesPerLine,
                               OUTPUT_CALLBACK output, void *additionalInfo);

bool iterate_byte_array_chunks_primitive(const void *addr, jlong numBytes, jlong bytesPerLine,
                                         OUTPUT_CALLBACK output, void *additionalInfo);

void dump_process_memory_map(void);

void *allocate_memory_chunk(size_t size);

void free_memory_chunk(void *mem);

#ifdef __cplusplus
}
#endif


#endif





