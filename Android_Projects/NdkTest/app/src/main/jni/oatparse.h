#ifndef _OAT_PARSE_H
#define _OAT_PARSE_H

#include <inttypes.h>

#include "logging.h"

typedef struct OatFileHeader
{
    unsigned char   magic[8];
    uint32_t        checksum;
    uint32_t        instruction_set;
} OatFileHeader;


#endif