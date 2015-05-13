#include "memory.h"

#ifdef __cplusplus
extern "C" {
#endif


bool setMemoryProtection (JNIEnv* env, void* startingAddress, jlong numBytes, jboolean read, jboolean write, jboolean execute)
{
    LOGD("setMemoryProtection(addr="PRINT_PTR", numBytes=%lld, read=%s, write=%s, exec=%s", (uintptr_t)startingAddress, numBytes, read ? "true" : "false", write ? "true" : "false", execute ? "true" : "false");
    int protections = 0;

    protections |=  read     ?   PROT_READ     : 0;
    protections |=  write    ?   PROT_WRITE    : 0;
    protections |=  execute  ?   PROT_EXEC     : 0;

    LOGD("Protections combined.");

    void* addr = getPageBaseAddress(startingAddress);
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

bool hexdumpUnaligned(JNIEnv* env, unsigned char* prefix, void* addr, jlong numBytes, jlong bytesPerLine)
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
    unsigned char* dump = malloc(5 * bytesPerLine + 1);

    for(int i = 0; i < numBytes; i += bytesPerLine)
    {
        for(int byteInLine = 0; byteInLine < bytesPerLine; byteInLine++)
        {
            sprintf(&dump[byteInLine * 3], "%02x ", ((unsigned char*)addr)[i + byteInLine]);
        }
        LOGD("%s: "PRINT_PTR" => %s", prefix, (uintptr_t)(addr + i), dump);
    }
    free(dump);
    return true;
}
bool hexdumpUnalignedPrimitive(unsigned char* prefix, void* addr, jlong numBytes, jlong bytesPerLine)
{
    if(addr == NULL)
    {
        return false;
    }
    if(bytesPerLine <= 0)
    {
        return false;
    }
    unsigned char* dump = malloc(5 * bytesPerLine + 1);

    for(int i = 0; i < numBytes; i += bytesPerLine)
    {
        for(int byteInLine = 0; byteInLine < bytesPerLine; byteInLine++)
        {
            sprintf(&dump[byteInLine * 3], "%02x ", ((unsigned char*)addr)[i + byteInLine]);
        }
        LOGD("%s: "PRINT_PTR" => %s", prefix, (uintptr_t)(addr + i), dump);
    }
    free(dump);
    return true;
}

static bool hexdump_callback(void* addr, uint64_t currentOffset, int numCurrentBytes, void* additionalInfo)
{
    unsigned char* dump[3 * bytesPerLine + 1];
    for(int byteInLine = 0; byteInLine < bytesPerLine; byteInLine++)
    {
        sprintf(&dump[byteInLine * 3], "%02x ", aligned[i + byteInLine]);
    }
    LOGD("Hexdump: "PRINT_PTR" => %s", prefix, (uintptr_t)addr, dump);
    return true;
}

bool hexdump(JNIEnv* env, void* addr, jlong numBytes, jlong bytesPerLine)
{
    return iterate_byte_array_chunks(env, addr, numBytes, bytesPerLine, (OUTPUT_CALLBACK)hexdumpCallback, NULL);
}
bool hexdump_primitive(void* addr, jlong numBytes, jlong bytesPerLine)
{
    return iterate_byte_array_chunks_primitive(addr, numBytes, bytesPerLine, (OUTPUT_CALLBACK)hexdumpCallback, NULL);
}

bool hexdump_aligned(JNIEnv* env, void* addr, jlong numBytes, jlong bytesPerLine, jlong alignment)
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
    void* aligned = (unsigned char*)alignAddressToSize(addr, alignment);
    uint64_t size = numBytes + ((unsigned char*)addr - aligned);
    return iterate_byte_array_chunks(env, aligned, size, bytesPerLine, (OUTPUT_CALLBACK)hexdumpCallback, NULL);
}

bool hexdump_aligned_primitive(void* addr, jlong numBytes, jlong bytesPerLine, jlong alignment)
{
    if(alignment <= 0)
    {
        return false;
    }
    void* aligned = (unsigned char*)alignAddressToSize(addr, alignment);
    uint64_t size = numBytes + ((unsigned char*)addr - aligned);
    return iterate_byte_array_chunks_primitive(aligned, size, bytesPerLine, (OUTPUT_CALLBACK)hexdumpCallback, NULL);
}



typedef bool (*) (void* startingAddress, uint64_t currentOffset, int numCurrentBytes, void* additionalInfo) OUTPUT_CALLBACK;

bool iterate_byte_array_chunks(JNIEnv* env, void* addr, jlong numBytes, jlong bytesPerLine, OUTPUT_CALLBACK output, void* additionalInfo)
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
            return false;
        }
    }
    return true;
}

bool iterate_byte_array_chunks(JNIEnv* env, void* addr, jlong numBytes, jlong bytesPerLine, OUTPUT_CALLBACK output, void* additionalInfo)
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
            return false;
        }
    }
    return true;
}

bool iterate_byte_array_chunks_primitive(void* addr, jlong numBytes, jlong bytesPerLine, OUTPUT_CALLBACK output, void* additionalInfo)
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

#ifdef __cplusplus
}
#endif





