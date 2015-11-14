//
// Created by Lukas on 11/14/2015.
//

#include <string.h>
#include "dex_internal.h"


bool ValidHeader(struct DexFileHeader* hdr)
{
    if(memcmp(hdr->magic_,
}