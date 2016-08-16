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
 
#include <stdio.h>
#include "logging.h"
#include "memory.h"
#include "exceptions.h"
#include "util.h"

static bool hexdump_callback(void *addr, uint64_t currentOffset, int numBytes, void *additionalInfo)
{
    unsigned char dump[3 * numBytes + 1];
    unsigned char ascii[numBytes + 1];
    unsigned char *target = (unsigned char *) (addr + currentOffset);
    for (int byteInLine = 0; byteInLine < numBytes; byteInLine++)
    {
        unsigned char c = target[byteInLine];
        sprintf(&dump[byteInLine * 3], "%02x ", c);
        if (!isprint(c))
        {
            ascii[byteInLine] = '.';
        }
        else if (c == '\r' || c == '\n' || c == '\t')
        {
            ascii[byteInLine] = '.';
        }
        else
        {
            ascii[byteInLine] = c;
        }
    }
    ascii[numBytes] = 0;
    LOGD("Hexdump: "
                 PRINT_PTR
                 " => %s | %s", (uintptr_t) target, dump, ascii);
    return true;
}

bool hexdump(JNIEnv *env, const void *addr, jlong numBytes, jlong bytesPerLine)
{
    return iterate_byte_array_chunks(env, addr, numBytes, bytesPerLine,
                                     (OUTPUT_CALLBACK) hexdump_callback, NULL);
}

bool hexdump_primitive(const void *addr, jlong numBytes, jlong bytesPerLine)
{
    return iterate_byte_array_chunks_primitive(addr, numBytes, bytesPerLine,
                                               (OUTPUT_CALLBACK) hexdump_callback, NULL);
}

bool hexdump_aligned(JNIEnv *env, const void *addr, jlong numBytes, jlong bytesPerLine,
                     jlong alignment)
{
    if (hasExceptionOccurred(env))
    {
        return false;
    }
    if (alignment <= 0)
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "Hexdump: Error alignment == 0");
        return false;
    }
    const unsigned char *aligned = (unsigned char *) align_address_to_size(addr, alignment);
    uint64_t size = numBytes + ((unsigned char *) addr - aligned);
    return iterate_byte_array_chunks(env, aligned, size, bytesPerLine,
                                     (OUTPUT_CALLBACK) hexdump_callback, NULL);
}

bool hexdump_aligned_primitive(const void *addr, jlong numBytes, jlong bytesPerLine,
                               jlong alignment)
{
    if (alignment <= 0)
    {
        return false;
    }
    const unsigned char *aligned = (unsigned char *) align_address_to_size(addr, alignment);
    uint64_t size = numBytes + ((unsigned char *) addr - aligned);
    return iterate_byte_array_chunks_primitive(aligned, size, bytesPerLine,
                                               (OUTPUT_CALLBACK) hexdump_callback, NULL);
}