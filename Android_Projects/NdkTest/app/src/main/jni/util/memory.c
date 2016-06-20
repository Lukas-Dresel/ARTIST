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
 
#include "memory.h"
#include "util.h"
#include "exceptions.h"
#include "../logging.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

bool iterate_byte_array_chunks(JNIEnv *env, const void *addr, jlong numBytes, jlong bytesPerLine,
                               OUTPUT_CALLBACK output, void *additionalInfo)
{
    if (hasExceptionOccurred(env))
    {
        return false;
    }
    if (addr == NULL)
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "Hexdump: Error addr == NULL");
        return false;
    }
    if (bytesPerLine <= 0)
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "Hexdump: Error bytesPerLine <= 0");
        return false;
    }
    for (uint64_t i = 0; i < numBytes; i += bytesPerLine)
    {
        if (!output(addr, i, bytesPerLine, additionalInfo))
        {
            throwNewJNIException(env, "java/lang/RuntimeException",
                                 "Hexdump: Error: The callback returned false.");
            return false;
        }
    }
    return true;
}

bool iterate_byte_array_chunks_primitive(const void *addr, jlong numBytes, jlong bytesPerLine,
                                         OUTPUT_CALLBACK output, void *additionalInfo)
{
    if (addr == NULL)
    {
        return false;
    }
    if (bytesPerLine <= 0)
    {
        return false;
    }
    for (uint64_t i = 0; i < numBytes; i += bytesPerLine)
    {
        if (!output(addr, i, bytesPerLine, additionalInfo))
        {
            return false;
        }
    }
    return true;
}

bool set_memory_protection(const void *startingAddress, jlong numBytes, jboolean read, jboolean write,
                           jboolean execute)
{
    /*LOGD("set_memory_protection(addr="
                 PRINT_PTR
                 ", numBytes=%lld, read=%s, write=%s, exec=%s", (uintptr_t) startingAddress,
         numBytes, read ? "true" : "false", write ? "true" : "false", execute ? "true" : "false");*/
    int protections = 0;

    protections |= read ? PROT_READ : 0;
    protections |= write ? PROT_WRITE : 0;
    protections |= execute ? PROT_EXEC : 0;

    void *addr = get_page_base(startingAddress);
    jlong size = numBytes + (startingAddress - addr);

    /*LOGV("Attempting to change memory permissions to %x...", protections);*/
    if (mprotect(addr, size, protections) == -1)
    {
        LOGE("Failed to change protections of %lld bytes from address "
                     PRINT_PTR
                     ", errno: %d", size, (uintptr_t) addr, errno);
        return false;
    }
    /*LOGV("Success! Changed protections of %lld bytes from address "
                 PRINT_PTR
                 " to full.", size, (uintptr_t) addr);*/
    return true;
}
void *allocate_memory_chunk(size_t size)
{
    CHECK(size > 0);
    //TODO change to own memory implementation to avoid being fooled by usermode code
    void *result = malloc(size);
    if (UNLIKELY(result == NULL))
    {
        set_last_error("Allocation of memory failed.");
        LOGF("malloc of size %zd failed: %s", size, strerror(errno));
    }
    LOGV("Allocation of %d bytes memory returned: "PRINT_PTR, size, result);
    return result;
}

void free_memory_chunk(void *mem)
{
    CHECK_NE(mem, NULL);
    LOGV("Memory being freed: "PRINT_PTR, mem);
    free(mem);
}

#ifdef __cplusplus
}
#endif



