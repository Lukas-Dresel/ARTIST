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

bool    setMemoryProtection ( JNIEnv* env, void* startingAddress, jlong numBytes, jboolean read, jboolean write, jboolean execute );

bool    hexdumpAligned      ( JNIEnv* env, unsigned char* prefix, void* addr, jlong numBytes, jlong bytesPerLine );

bool    hexdumpAlignedPrimitive(unsigned char* prefix, void* addr, jlong numBytes, jlong bytesPerLine);

bool    hexdumpUnaligned      ( JNIEnv* env, unsigned char* prefix, void* addr, jlong numBytes, jlong bytesPerLine );

bool    hexdumpUnalignedPrimitive(unsigned char* prefix, void* addr, jlong numBytes, jlong bytesPerLine);

#endif





