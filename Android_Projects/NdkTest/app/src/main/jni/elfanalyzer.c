#include "elfanalyzer.h"
#include "util/memory.h"

struct dump_callback_args
{
    const char* name;
};
static bool dump_callback(void* addr, uint64_t currentOffset, int numBytes, void* additionalInfo)
{
    char* name = additionalInfo;
    unsigned char dump[3 * numBytes + 1];
    unsigned char* target = (unsigned char*)(addr + currentOffset);
    for(int byteInLine = 0; byteInLine < numBytes; byteInLine++)
    {
        sprintf(&dump[byteInLine * 3], "%02x ", target[byteInLine]);
    }
    LOGD("%s: "PRINT_PTR" => %s", name, (uintptr_t)target, dump);
    return true;
}

static bool dumpElf64Bytes(JNIEnv* env, const unsigned char* name, const void* addr, uint64_t numBytes)
{
    struct dump_callback_args args;
    args.name = name;
    return iterate_byte_array_chunks(env, addr, numBytes, 16, (OUTPUT_CALLBACK)dump_callback, &args);
}
static bool dumpElf64Word(JNIEnv* env, const unsigned char* name, const void* addr)
{
    struct dump_callback_args args;
    args.name = name;
    return iterate_byte_array_chunks(env, addr, sizeof(Elf64_Word), sizeof(Elf64_Word), (OUTPUT_CALLBACK)dump_callback, &args);
}
static bool dumpElf64Half(JNIEnv* env, const unsigned char* name, const void* addr)
{
    struct dump_callback_args args;
    args.name = name;
    return iterate_byte_array_chunks(env, addr, sizeof(Elf64_Half), sizeof(Elf64_Half), (OUTPUT_CALLBACK)dump_callback, &args);
}
static bool dumpElf64Addr(JNIEnv* env, const unsigned char* name, const void* addr)
{
    struct dump_callback_args args;
    args.name = name;
    return iterate_byte_array_chunks(env, addr, sizeof(Elf64_Addr), sizeof(Elf64_Addr), (OUTPUT_CALLBACK)dump_callback, &args);
}
static bool dumpElf64Offs(JNIEnv* env, const unsigned char* name, const void* addr)
{
    struct dump_callback_args args;
    args.name = name;
    return iterate_byte_array_chunks(env, addr, sizeof(Elf64_Off), sizeof(Elf64_Off), (OUTPUT_CALLBACK)dump_callback, &args);
}
static bool dump64BitFileHeader(JNIEnv* env, Elf64_Ehdr* header)
{
    dumpElf64Bytes(env, "Identification", &header->e_ident, EI_NIDENT);

    dumpElf64Half(env, "Type", &header->e_type);
    dumpElf64Half(env, "Machine", &header->e_machine);
    dumpElf64Word(env, "Version", &header->e_version);
    dumpElf64Addr(env, "Entrypoint", &header->e_entry);
    dumpElf64Offs(env, "Program Header Table Offset", &header->e_phoff);
    dumpElf64Offs(env, "Section Header Table Offset", &header->e_shoff);
    dumpElf64Word(env, "Flags", &header->e_flags);
    dumpElf64Half(env, "Elf Header Size", &header->e_ehsize);
    dumpElf64Half(env, "Program Header Table Entry Size: ", &header->e_phentsize);
    dumpElf64Half(env, "Program Header Table Number of Entries: ", &header->e_phnum);
    dumpElf64Half(env, "Section Header Table Entry Size: ", &header->e_phentsize);
    dumpElf64Half(env, "Section Header Table Number of Entries: ", &header->e_phnum);
}
void dumpElfInfo(JNIEnv* env, const void* headerAddress)
{
    //if(((unsigned char*)headerAddress)[4] == 1)
}

void dump64BitElfInfo(JNIEnv* env, void* hdr)
{
    LOGI("Dumping elf header at "PRINT_PTR, (uintptr_t)hdr);
    Elf64_Ehdr* fileHeader = (Elf64_Ehdr*)hdr;
}
void dump32BitElfInfo(JNIEnv* env, void* header)
{

}
