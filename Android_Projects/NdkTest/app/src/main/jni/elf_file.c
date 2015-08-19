//
// Created by Lukas on 8/17/2015.
//

#include "elf_file.h"
#include "error.h"
#include "logging.h"

#include <elf.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define ElfIdentification e_ident
#define ElfObjectFileType e_type
#define ElfArchitecture e_machine
#define ElfVersion e_version
#define ElfEntryPoint e_entry
#define ElfProgramHeaderOffset e_phoff
#define ElfSectionHeaderOffset e_shoff
#define ElfFlags e_flags
#define ElfHeaderSize e_ehsize
#define ElfProgramHeaderEntrySize e_phentsize
#define ElfProgramHeaderNumber e_phnum
#define ElfSectionHeaderEntrySize e_shentsize
#define ElfSectionHeaderNumber e_shnum
#define ElfSectionNameStringTableIndex e_shstrndx

#define ELF_IDENTIFICATION_INDEX_FILE_CLASS EI_CLASS
#define ELF_IDENTIFICATION_INDEX_DATA_ENCODING EI_DATA
#define ELF_IDENTIFICATION_INDEX_FILE_VERSION EI_VERSION
#define ELF_IDENTIFICATION_INDEX_PADDING_START EI_PAD
#define ELF_IDENTIFICATION_INDEX_END EI_NIDENT


// most of this file is a C adaptation of https://android.googlesource.com/platform/art/+/android-5.1.1_r13/runtime/elf_file.cc

bool elf_file_CheckAndSet(ElfFile* elf_file, Elf32_Off offset, const char* label, byte** target)
{
    if (elf_file->container.begin + offset >= elf_file->container.end)
    {
        LOGE("Offset %d is out of range of the memory bounds for %s.", offset, label);
        return false;
    }
    *target = elf_file->container.begin + offset;
    return true;
}


Elf32_Ehdr* elf_file_GetHeader(ElfFile* self)
{
    CHECK(self->header_ != NULL);  // Header has been checked in SetMap. This is a sanity check.
    return self->header_;
}
byte* elf_file_GetProgramHeadersStart(ElfFile* self)
{
    CHECK(self->program_headers_start_ != NULL);  // Header has been set in Setup. This is a sanity check.
    return self->program_headers_start_;
}
byte* elf_file_GetSectionHeadersStart(ElfFile* self)
{
    CHECK(!self->program_header_only_);              // Only used in "full" mode.
    CHECK(self->section_headers_start_ != NULL);     // Is checked in CheckSectionsExist. Sanity check.
    return self->section_headers_start_;
}
Elf32_Phdr* elf_file_GetDynamicProgramHeader(ElfFile* self)
{
    CHECK(self->dynamic_program_header_ != NULL);  // Is checked in CheckSectionsExist. Sanity check.
    return self->dynamic_program_header_;
}
Elf32_Dyn* elf_file_GetDynamicSectionStart(ElfFile* self)
{
    CHECK(self->dynamic_section_start_ != NULL);  // Is checked in CheckSectionsExist. Sanity check.
    return self->dynamic_section_start_;
}
static bool IsSymbolSectionType(Elf32_Word section_type)
{
    return ((section_type == SHT_SYMTAB) || (section_type == SHT_DYNSYM));
}
Elf32_Sym* elf_file_GetSymbolSectionStart(ElfFile* self, Elf32_Word section_type)
{
    CHECK(IsSymbolSectionType(section_type));
    switch (section_type)
    {
        case SHT_SYMTAB:
        {
            return self->symtab_section_start_;
        }
        case SHT_DYNSYM:
        {
            return self->dynsym_section_start_;
        }
        default:
        {
            LOGF("Unknown Section Type: %d", section_type);
            return NULL;
        }
    }
}
const char* elf_file_GetStringSectionStart(ElfFile* self, Elf32_Word section_type)
{
    CHECK(IsSymbolSectionType(section_type));
    switch (section_type) {
        case SHT_SYMTAB: {
            return self->strtab_section_start_;
        }
        case SHT_DYNSYM: {
            return self->dynstr_section_start_;
        }
        default: {
            LOGF("Unknown Section Type: %d", section_type);
            return NULL;
        }
    }
}
const char* elf_file_GetString(ElfFile* self, Elf32_Word section_type, Elf32_Word i)
{
    CHECK(IsSymbolSectionType(section_type));
    if (i == 0)
    {
        return NULL;
    }
    const char* string_section_start = elf_file_GetStringSectionStart(self, section_type);
    if (string_section_start == NULL)
    {
        return NULL;
    }
    return string_section_start + i;
}

Elf32_Word* elf_file_GetHashSectionStart(ElfFile* self)
{
    return hash_section_start_;
}
Elf32_Word elf_file_GetHashBucketNum(ElfFile* self)
{
    return elf_file_GetHashSectionStart(self)[0];
}
Elf32_Word elf_file_GetHashChainNum(ElfFile* self)
{
    return elf_file_GetHashSectionStart(self)[1];
}
Elf32_Word elf_file_GetHashBucket(ElfFile* self, size_t i, bool* ok)
{
    if (i >= elf_file_GetHashBucketNum(self))
    {
        *ok = false;
        return 0;
    }
    *ok = true;
    // 0 is nbucket, 1 is nchain
    return elf_file_GetHashSectionStart(self)[2 + i];
}
Elf32_Word elf_file_GetHashChain(ElfFile* self, size_t i, bool* ok)
{
    if (i >= elf_file_GetHashBucketNum(self))
    {
        *ok = false;
        return 0;
    }
    *ok = true;
    // 0 is nbucket, 1 is nchain, & chains are after buckets
    return elf_file_GetHashSectionStart(self)[2 + elf_file_GetHashBucketNum(self) + i];
}


Elf32_Word elf_file_GetProgramHeaderNum(ElfFile* self)
{
    elf_file_GetHeader(self)->e_phnum;
    return elf_file_GetHeader(self)->e_phnum;
}
Elf32_Phdr* elf_file_GetProgramHeader(ElfFile* self, Elf32_Word i)
{
    CHECK_LT(i, elf_file_GetProgramHeaderNum(self));  // Sanity check for caller.
    byte* program_header = elf_file_GetProgramHeadersStart(self) + (i * elf_file_GetHeader(self)->e_phentsize);
    if ((void*)program_header >= self->container.end)
    {
        return NULL;  // Failure condition.
    }
    return (Elf32_Phdr*)(program_header);
}
Elf32_Phdr* elf_file_FindProgamHeaderByType(ElfFile* self, Elf32_Word type)
{
    for (Elf32_Word i = 0; i < elf_file_GetProgramHeaderNum(self); i++)
    {
        Elf32_Phdr* program_header = elf_file_GetProgramHeader(self, i);
        if (program_header->p_type == type)
        {
            return program_header;
        }
    }
    return NULL;
}
Elf32_Word elf_file_GetSectionHeaderNum(ElfFile* self)
{
    return elf_file_GetHeader(self)->e_shnum;
}
Elf32_Shdr* elf_file_GetSectionHeader(ElfFile* self, Elf32_Word i)
{
    // Can only access arbitrary sections when we have the whole file, not just program header.
    // Even if we Load(), it doesn't bring in all the sections.
    CHECK(!self->program_header_only_);

    if (i >= elf_file_GetSectionHeaderNum(self))
    {
        return NULL;  // Failure condition.
    }
    byte* section_header = elf_file_GetSectionHeadersStart(self) + (i * elf_file_GetHeader(self)->e_shentsize);
    if (section_header >= self->container.end)
    {
        return NULL;  // Failure condition.
    }
    return (Elf32_Shdr*)(section_header);
}
Elf32_Shdr* elf_file_FindSectionByType(ElfFile* self, Elf32_Word type)
{
    // Can only access arbitrary sections when we have the whole file, not just program header.
    // We could change this to switch on known types if they were detected during loading.
    CHECK(!self->program_header_only_);
    for (Elf32_Word i = 0; i < elf_file_GetSectionHeaderNum(self); i++)
    {
        Elf32_Shdr* section_header = elf_file_GetSectionHeader(self, i);
        if (section_header->sh_type == type)
        {
            return section_header;
        }
    }
    return NULL;
}
Elf32_Shdr* elf_file_FindSectionByName(ElfFile* self, const char* name)
{
    CHECK(!self->program_header_only_);
    Elf32_Shdr* shstrtab_sec = elf_file_GetSectionNameStringSection(self);
    if (shstrtab_sec == NULL)
    {
        return NULL;
    }
    for (uint32_t i = 0; i < elf_file_GetSectionHeaderNum(self); i++)
    {
        Elf32_Shdr* shdr = elf_file_GetSectionHeader(self, i);
        if (shdr == NULL)
        {
            return NULL;
        }
        const char* sec_name = elf_file_GetString(self, shstrtab_sec, shdr->sh_name);
        if (sec_name == NULL)
        {
            continue;
        }
        if (strcmp(name, sec_name) == 0)
        {
            return shdr;
        }
    }
    return NULL;
}
// from bionic
static unsigned elfhash(const char *_name)
{
    const unsigned char *name = (const unsigned char *) _name;
    unsigned h = 0, g;
    while (*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }
    return h;
}
Elf32_Shdr* elf_file_GetSectionNameStringSection(ElfFile* self)
{
    return elf_file_GetSectionHeader(self, elf_file_GetHeader(self)->e_shstrndx);
}


Elf32_Word elf_file_GetSymbolNum(ElfFile* self, Elf32_Shdr* section_header)
{
    if (UNLIKELY(!IsSymbolSectionType(section_header->sh_type)))
    {
        LOGF( "Section header type %d is not a symbol section type.", section_header->sh_type);
    }
    CHECK_NE(0U, section_header->sh_entsize);
    return section_header->sh_size / section_header->sh_entsize;
}

Elf32_Sym* elf_file_GetSymbol(ElfFile* self, Elf32_Word section_type, Elf32_Word i)
{
    Elf32_Sym* sym_start = elf_file_GetSymbolSectionStart(self, section_type);
    if (sym_start == NULL)
    {
        return NULL;
    }
    return sym_start + i;
}
const byte* elf_file_FindDynamicSymbolAddress(const char* symbol_name)
{
    // Check that we have a hash section.
    if (GetHashSectionStart() == NULL)
    {
        return NULL;  // Failure condition.
    }
    const Elf32_Sym* sym = FindDynamicSymbol(symbol_name);
    if (sym != NULL)
    {
    {
        // TODO: we need to change this to calculate base_address_ in ::Open,
        // otherwise it will be wrongly 0 if ::Load has not yet been called.
        return base_address_ + sym->st_value;
    }
    else
    {
        return NULL;
    }
}

static bool elf_file_SetupHeader(ElfFile* elf_file)
{
    if((elf_file->container.begin + sizeof(Elf32_Ehdr)) >= elf_file->container.end)
    {
        LOGE("Not enough space left in the memory region for the ELF-Header.");
        return false;
    }

    // Code inspired by AOSP platform/art https://android.googlesource.com/platform/art/+/android-5.1.1_r13/runtime/elf_file.cc

    Elf32_Ehdr* hdr = (Elf32_Ehdr*)elf_file->container.begin;
    if((ELFMAG0 != hdr->e_ident[EI_MAG0]) ||
       (ELFMAG1 != hdr->e_ident[EI_MAG1]) ||
       (ELFMAG2 != hdr->e_ident[EI_MAG2]) ||
       (ELFMAG3 != hdr->e_ident[EI_MAG3]))
    {
        LOGE("Invalid elf magic value. Expected %d %d %d %d, found %d %d %d %d",
             ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,
             hdr->e_ident[ELFMAG0], hdr->e_ident[ELFMAG1], hdr->e_ident[ELFMAG2], hdr->e_ident[ELFMAG3]);
        return false;
    }

    if(ELFCLASS32 != hdr->e_ident[EI_CLASS])
    {
        LOGE("Invalid ELF file class for android elf files in ELF identification. Expected %d, found %d", ELFCLASS32, hdr->e_ident[EI_CLASS]);
        return false;
    }

    if (ELFDATA2LSB != hdr->e_ident[EI_DATA])
    {
        LOGE("Invalid data encoding value in ELF identification. Expected %d, found %d", ELFDATA2LSB, hdr->e_ident[EI_DATA]);
        return false;
    }
    if (EV_CURRENT != hdr->e_ident[EI_VERSION])
    {
        LOGE("Invalid ELF file version in ELF identification. Expected %d, found %d", EV_CURRENT, hdr->e_ident[EI_VERSION]);
        return false;
    }
    if (ET_DYN != hdr->e_type)
    {
        LOGE("Invalid ELF file type. Expected %d, found %d", ET_DYN, hdr->e_type);
        return false;
    }
    if (EV_CURRENT != hdr->e_version)
    {
        LOGE("Invalid ELF file version. Expected %d, found %d", EV_CURRENT, hdr->e_version);
        return false;
    }
    if (0 != hdr->e_entry)
    {
        LOGE("Unexpected Entry point value. Expected %d, found %d", 0, hdr->e_entry);
        return false;
    }
    if (0 == hdr->e_phoff)
    {
        LOGE("Expected non-zero value for the ProgramHeaderTable offset");
        return false;
    }
    if (0 == hdr->e_shoff)
    {
        LOGE("Expected non-zero value for the SectionHeaderTable offset");
        return false;
    }
    if (0 == hdr->e_ehsize)
    {
        LOGE("Expected non-zero value for the ELF header size");
        return false;
    }
    if (0 == hdr->e_phentsize)
    {
        LOGE("Expected non-zero value for the ProgramHeaderTable entry size");
        return false;
    }
    if (0 == hdr->e_phnum)
    {
        LOGE("Expected non-zero value for the ProgramHeaderTable entry count");
        return false;
    }
    if (0 == hdr->e_shentsize)
    {
        LOGE("Expected non-zero value for the SectionHeaderTable entry size");
        return false;
    }
    if (0 == hdr->e_shnum)
    {
        LOGE("Expected non-zero value for the SectionHeaderTable entry count");
        return false;
    }
    if (0 == hdr->e_shstrndx)
    {
        LOGE("Expected non-zero value for the SectionNameStringTable SectionHeader index");
        return false;
    }
    if (hdr->e_shstrndx >= hdr->e_shnum)
    {
        LOGE("Invalid value for the SectionNameStringTable SectionHeader index. Expected value < %d, found %d", hdr->e_shnum, hdr->e_shstrndx);
        return false;
    }
    //TODO what to do with this?
    if (!elf_file->program_header_only_)
    {
        if (hdr->e_phoff >= elf_file->container.size)
        {
            LOGE("ProgramHeaderTable offset was out of bounds. Expected value < &zd, found %d", elf_file->container.size, hdr->e_phoff);
            return false;
        }
        if (hdr->e_shoff >= elf_file->container.size)
        {
            LOGE("SectionHeaderTable offset was out of the specified memory area. Expected value < &zd, found %d", elf_file->container.size, hdr->e_shoff);
            return false;
        }
    }
    elf_file->header_ = hdr;
    return true;
}

bool elf_file_SetupFromMemory(ElfFile* elf_file, void* begin, void* end, bool program_header_only)
{
    CHECK(begin < end); // Sanity Check, you can't have end before start

    elf_file->container.begin = begin;
    elf_file->container.end = end;
    elf_file->container.size = end - begin;

    elf_file->program_header_only_ = program_header_only;

    if(!elf_file_SetupHeader(elf_file))
    {
        return false;
    }
    if(!elf_file_CheckAndSet(elf_file, elf_file->header_->e_phoff, "program headers", &elf_file->program_headers_start_))
    {
        return false;
    }
    if(!elf_file_CheckAndSet(elf_file, elf_file->header_->e_shoff, "section headers", &elf_file->section_headers_start_))
    {
        return false;
    }

    return true;
}

