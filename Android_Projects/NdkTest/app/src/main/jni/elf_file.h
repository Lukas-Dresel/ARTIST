//
// Created by Lukas on 8/17/2015.
//

#ifndef NDKTEST_ELF_FILE_H
#define NDKTEST_ELF_FILE_H

#include <linux/elf.h>
#include <sys/types.h>
#include <stdbool.h>

typedef uint8_t byte;

typedef struct MemoryRegion
{
    void*      begin;
    void*      end;
    uint32_t   size;
} MemoryRegion;

typedef struct ElfFile
{
    MemoryRegion    container;

    bool            program_header_only_;

    Elf32_Ehdr*     header_;
    byte*           program_headers_start_;
    byte*           section_headers_start_;

    Elf32_Phdr*     dynamic_program_header_;
    Elf32_Dyn*      dynamic_section_start_;
    Elf32_Sym*      symtab_section_start_;
    Elf32_Sym*      dynsym_section_start_;
    char*           strtab_section_start_;
    char*           dynstr_section_start_;
    Elf32_Word*     hash_section_start_;

} ElfFile;

bool            elf_file_SetupFromMemory(ElfFile* elf_file, void* begin, void* end, bool program_header_only);

Elf32_Ehdr*     elf_file_GetHeader(ElfFile* self);

byte*           elf_file_GetProgramHeadersStart(ElfFile* self);
byte*           elf_file_GetSectionHeadersStart(ElfFile* self);
Elf32_Phdr*     elf_file_GetDynamicProgramHeader(ElfFile* self);
Elf32_Dyn*      elf_file_GetDynamicSectionStart(ElfFile* self);
Elf32_Sym*      elf_file_GetSymbolSectionStart(ElfFile* self, Elf32_Word section_type);
const char*     elf_file_GetStringSectionStart(ElfFile* self, Elf32_Word section_type);
const char*     elf_file_GetString(ElfFile* self, Elf32_Word section_type, Elf32_Word i);

Elf32_Word*     elf_file_GetHashSectionStart(ElfFile* self);
Elf32_Word      elf_file_GetHashBucketNum(ElfFile* self);
Elf32_Word      elf_file_GetHashChainNum(ElfFile* self);
Elf32_Word      elf_file_GetHashBucket(ElfFile* self, size_t i, bool* ok);
Elf32_Word      elf_file_GetHashChain(ElfFile* self, size_t i, bool* ok);

Elf32_Word      elf_file_GetProgramHeaderNum(ElfFile* self);
Elf32_Phdr*     elf_file_GetProgramHeader(ElfFile* self, Elf32_Word i);
Elf32_Phdr*     elf_file_FindProgamHeaderByType(ElfFile* self, Elf32_Word type);

Elf32_Word      elf_file_GetSectionHeaderNum(ElfFile* self);
Elf32_Shdr*     elf_file_GetSectionHeader(ElfFile* self, Elf32_Word i);
Elf32_Shdr*     elf_file_FindSectionByType(ElfFile* self, Elf32_Word type);
Elf32_Shdr*     elf_file_FindSectionByName(ElfFile* self, const char* name);
Elf32_Shdr*     elf_file_GetSectionNameStringSection(ElfFile* self);

Elf32_Word      elf_file_GetSymbolNum(ElfFile* self, Elf32_Shdr* section_header);
Elf32_Sym*      elf_file_GetSymbol(ElfFile* self, Elf32_Word section_type, Elf32_Word i);


#endif //NDKTEST_ELF_FILE_H
