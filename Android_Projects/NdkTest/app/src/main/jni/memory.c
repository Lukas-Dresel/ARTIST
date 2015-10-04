#include "memory.h"
#include "logging.h"
#include "util.h"

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

bool set_memory_protection(void *startingAddress, jlong numBytes, jboolean read, jboolean write,
                           jboolean execute)
{
    LOGD("set_memory_protection(addr="
                 PRINT_PTR
                 ", numBytes=%lld, read=%s, write=%s, exec=%s", (uintptr_t) startingAddress,
         numBytes, read ? "true" : "false", write ? "true" : "false", execute ? "true" : "false");
    int protections = 0;

    protections |= read ? PROT_READ : 0;
    protections |= write ? PROT_WRITE : 0;
    protections |= execute ? PROT_EXEC : 0;

    LOGD("Protections combined.");

    void *addr = get_page_base(startingAddress);
    jlong size = numBytes + (startingAddress - addr);

    LOGD("Attempting to change memory permissions to %x...", protections);
    if (mprotect(addr, size, protections) == -1)
    {
        LOGE("Failed to change protections of %lld bytes from address "
                     PRINT_PTR
                     ", errno: %d", size, (uintptr_t) addr, errno);
        return false;
    }
    LOGD("Success! Changed protections of %lld bytes from address "
                 PRINT_PTR
                 " to full.", size, (uintptr_t) addr);
    return true;
}
void *allocate_memory_chunk(size_t size)
{
    CHECK(size > 0);
    //TODO change to own memory implementation to avoid being fooled by usermode code
    void *result = malloc(size);
    if (UNLIKELY(result == NULL))
    {
        LOGF("malloc of size %zd failed: %s", size, strerror(errno));
    }
    return result;
}

void free_memory_chunk(void *mem)
{
    CHECK_NE(mem, NULL);
    free(mem);
}

#ifdef __cplusplus
}
#endif



