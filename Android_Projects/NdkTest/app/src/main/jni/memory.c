#include "memory.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <ctype.h>

bool iterate_byte_array_chunks(JNIEnv* env, const void* addr, jlong numBytes, jlong bytesPerLine, OUTPUT_CALLBACK output, void* additionalInfo)
{
    if(hasExceptionOccurred(env))
    {
        return false;
    }
    if(addr == NULL)
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "Hexdump: Error addr == NULL");
        return false;
    }
    if(bytesPerLine <= 0)
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "Hexdump: Error bytesPerLine <= 0");
        return false;
    }
    for(uint64_t i = 0; i < numBytes; i += bytesPerLine)
    {
        if(!output(addr, i, bytesPerLine, additionalInfo))
        {
            throwNewJNIException(env, "java/lang/RuntimeException", "Hexdump: Error: The callback returned false.");
            return false;
        }
    }
    return true;
}

bool iterate_byte_array_chunks_primitive(const void* addr, jlong numBytes, jlong bytesPerLine, OUTPUT_CALLBACK output, void* additionalInfo)
{
    if(addr == NULL)
    {
        return false;
    }
    if(bytesPerLine <= 0)
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
    LOGD("set_memory_protection(addr="PRINT_PTR", numBytes=%lld, read=%s, write=%s, exec=%s", (uintptr_t)startingAddress, numBytes, read ? "true" : "false", write ? "true" : "false", execute ? "true" : "false");
    int protections = 0;

    protections |=  read     ?   PROT_READ     : 0;
    protections |=  write    ?   PROT_WRITE    : 0;
    protections |=  execute  ?   PROT_EXEC     : 0;

    LOGD("Protections combined.");

    void* addr = get_page_base(startingAddress);
    jlong size = numBytes + (startingAddress - addr);

    LOGD("Attempting to change memory permissions to %x...", protections);
    if(mprotect(addr, size, protections) == -1)
    {
        LOGE("Failed to change protections of %lld bytes from address " PRINT_PTR ", errno: %d", size, (uintptr_t)addr, errno);
        return false;
    }
    LOGD("Success! Changed protections of %lld bytes from address " PRINT_PTR " to full.", size, (uintptr_t)addr);
    return true;
}

static bool hexdump_callback(void* addr, uint64_t currentOffset, int numBytes, void* additionalInfo)
{
    unsigned char dump[3 * numBytes + 1];
    unsigned char ascii[numBytes + 1];
    unsigned char* target = (unsigned char*)(addr + currentOffset);
    for(int byteInLine = 0; byteInLine < numBytes; byteInLine++)
    {
        char c = target[byteInLine];
        sprintf(&dump[byteInLine * 3], "%02x ", c);
        if(!isprint(c))
        {
            ascii[byteInLine] = '.';
        }
        if(c == '\r' || c == '\n' || c == '\t')
        {
            ascii[byteInLine] = '.';
        }
        ascii[byteInLine] = c;
    }
    LOGD("Hexdump: "PRINT_PTR" => %s | ", (uintptr_t)target, dump);
    return true;
}

bool hexdump(JNIEnv* env, const void* addr, jlong numBytes, jlong bytesPerLine)
{
    return iterate_byte_array_chunks(env, addr, numBytes, bytesPerLine, (OUTPUT_CALLBACK)hexdump_callback, NULL);
}

bool hexdump_primitive(const void* addr, jlong numBytes, jlong bytesPerLine)
{
    return iterate_byte_array_chunks_primitive(addr, numBytes, bytesPerLine, (OUTPUT_CALLBACK)hexdump_callback, NULL);
}

bool hexdump_aligned(JNIEnv* env, const void* addr, jlong numBytes, jlong bytesPerLine, jlong alignment)
{
    if(hasExceptionOccurred(env))
    {
        return false;
    }
    if(alignment <= 0)
    {
        throwNewJNIException(env, "java/lang/RuntimeException", "Hexdump: Error alignment == 0");
        return false;
    }
    const unsigned char* aligned = (unsigned char*) align_address_to_size(addr, alignment);
    uint64_t size = numBytes + ((unsigned char*)addr - aligned);
    return iterate_byte_array_chunks(env, aligned, size, bytesPerLine, (OUTPUT_CALLBACK)hexdump_callback, NULL);
}

bool hexdump_aligned_primitive(const void* addr, jlong numBytes, jlong bytesPerLine, jlong alignment)
{
    if(alignment <= 0)
    {
        return false;
    }
    const unsigned char* aligned = (unsigned char*) align_address_to_size(addr, alignment);
    uint64_t size = numBytes + ((unsigned char*)addr - aligned);
    return iterate_byte_array_chunks_primitive(aligned, size, bytesPerLine, (OUTPUT_CALLBACK)hexdump_callback, NULL);
}

#ifdef __cplusplus
}
#endif





