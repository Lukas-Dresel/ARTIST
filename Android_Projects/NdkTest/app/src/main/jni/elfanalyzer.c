#include "elfanalyzer.h"


/*void dumpElf64Bytes(bool is64Bit, const unsigned char* name, void* addr, uint64_t numBytes)
{
    hexdumpUnalignedPrimitive(name, addr, numBytes, 16);
}
void dumpElf64Word(bool is64Bit, const unsigned char* name, void* addr)
{
    hexdumpUnalignedPrimitive(name, addr, sizeof(Elf64_Word), sizeof(Elf64_Word));
}
void dumpElf64Half(bool is64Bit, const unsigned char* name, void* addr)
{
    hexdumpUnalignedPrimitive(name, addr, sizeof(Elf64_Half), sizeof(Elf64_Half));
}
void dumpElf64Addr(bool is64Bit, const unsigned char* name, void* addr)
{
    hexdumpUnalignedPrimitive(name, addr, sizeof(Elf64_Addr), sizeof(Elf64_Addr));
}
void dumpElf64Offs(bool is64Bit, const unsigned char* name, void* addr)
{
    hexdumpUnalignedPrimitive(name, addr, sizeof(Elf64_Off), sizeof(Elf64_Off));
}
void dump64BitFileHeader(bool is64Bit, Elf64_Ehdr* header)
{
    dumpElf64Bytes("Identification", &header->e_ident, EI_NIDENT);

    dumpElf64Half("Type", &header->e_type);
    dumpElf64Half("Machine", &header->e_machine);
    dumpElf64Word("Version", &header->e_version);
    dumpElf64Addr("Entrypoint", &header->e_entry);
    dumpElf64Offs("Program Header Table Offset", &header->e_phoff);
    dumpElf64Offs("Section Header Table Offset", &header->e_shoff);
    dumpElf64Word("Flags", &header->e_flags);
    dumpElf64Half("Elf Header Size", &header->e_ehsize);
    dumpElf64Half("Program Header Table Entry Size: ", &header->e_phentsize);
    dumpElf64Half("Program Header Table Number of Entries: ", &header->e_phnum);
    dumpElf64Half("Section Header Table Entry Size: ", &header->e_phentsize);
    dumpElf64Half("Section Header Table Number of Entries: ", &header->e_phnum);

}
void dumpElfInfo(JNIEnv* env, void* headerAddress)
{
    if(((unsigned char*)headerAddress)[4] == 1)
}

void dump64BitElfInfo(JNIEnv* env, void* hdr)
{
    LOGI("Dumping elf header at "PRINT_PTR, (uintptr_t)hdr);
    Elf64_Ehdr* fileHeader = (Elf64_Ehdr*)hdr;
}
void dump32BitElfInfo(JNIEnv* env, void* header)
{

}*/
