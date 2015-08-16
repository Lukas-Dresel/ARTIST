//
// Created by Lukas on 8/16/2015.
//

#ifndef NDKTEST_OAT_PARSER_H
#define NDKTEST_OAT_PARSER_H

#include "oat.h"

OatHeader* parse_oat_file(void* address);

void log_oat_header_info(OatHeader* hdr);
void log_key_value_store_info(OatHeader* hdr);

#endif //NDKTEST_OAT_PARSER_H
