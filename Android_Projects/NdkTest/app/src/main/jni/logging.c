//
// Created by Lukas on 8/21/2015.
//

#include <stdio.h>
#include "logging.h"
#include "util/memory.h"
#include "util/exceptions.h"
#include "util/util.h"

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