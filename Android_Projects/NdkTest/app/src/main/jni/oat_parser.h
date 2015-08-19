//
// Created by Lukas on 8/16/2015.
//

#ifndef NDKTEST_OAT_PARSER_H
#define NDKTEST_OAT_PARSER_H

#include "oat.h"

void        log_elf_oat_file_info(void* address, void* end);

void        log_oat_header_info(OatHeader* hdr);
void        log_key_value_store_info(OatHeader* hdr);

#endif //NDKTEST_OAT_PARSER_H
