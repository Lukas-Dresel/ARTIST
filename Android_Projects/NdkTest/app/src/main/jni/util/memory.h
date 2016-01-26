#ifndef _MEMORY_H
#define _MEMORY_H

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

void *allocate_memory_chunk(size_t size);

void free_memory_chunk(void *mem);

#endif





