#include "elf.h"

#ifdef __cplusplus
extern "C" {
#endif

bool IsValidElfFileHeader(void* p)
{
    Elf32_Ehdr* hdr = p;

    if(hdr->e_ident[EI_MAG0] != ELFMAG0 ||
       hdr->e_ident[EI_MAG1] != ELFMAG1 ||
       hdr->e_ident[EI_MAG2] != ELFMAG2 ||
       hdr->e_ident[EI_MAG3] != ELFMAG3)
    {
        return false;
    }

    // Further checks could be added, but not necessary for our purposes yet

    return true;
}

#ifdef __cplusplus
}
#endif