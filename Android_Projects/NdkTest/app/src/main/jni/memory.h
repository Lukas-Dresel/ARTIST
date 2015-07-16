#ifndef _MEMORY_H
#define _MEMORY_H

#include <jni.h>

#include <unistd.h>
#include <sys/mman.h>

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "logging.h"
#include "exceptions.h"
#include "util.h"

typedef bool (*OUTPUT_CALLBACK)             ( const void* startingAddress, uint64_t currentOffset, int numCurrentBytes, void* additionalInfo);

bool    setMemoryProtection                 ( JNIEnv* env,  void* addr, jlong numBytes, jboolean read, jboolean write, jboolean execute );

bool    iterate_byte_array_chunks           ( JNIEnv* env,  const void* addr, jlong numBytes, jlong bytesPerLine, OUTPUT_CALLBACK output, void* additionalInfo );
bool    iterate_byte_array_chunks_primitive (               const void* addr, jlong numBytes, jlong bytesPerLine, OUTPUT_CALLBACK output, void* additionalInfo );

bool    hexdump                             ( JNIEnv* env,  const void* addr, jlong numBytes, jlong bytesPerLine);
bool    hexdump_primitive                   (               const void* addr, jlong numBytes, jlong bytesPerLine);
bool    hexdump_aligned                     ( JNIEnv* env,  const void* addr, jlong numBytes, jlong bytesPerLine, jlong alignment);
bool    hexdump_aligned_primitive           (               const void* addr, jlong numBytes, jlong bytesPerLine, jlong alignment);

#endif





