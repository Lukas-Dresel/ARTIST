/*
 * Copyright 2016 Lukas Dresel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
//
// Created by Lukas on 11/18/2015.
//

#ifndef NDKTEST_OAT_DUMP_H
#define NDKTEST_OAT_DUMP_H


#include <stdint.h>
#include "oat_internal.h"

void log_elf_oat_file_info(void *oat_begin, void *oat_end);

void log_oat_header_info(const struct OatHeader *hdr);

void log_oat_key_value_storage_contents(const struct OatHeader *hdr);

void log_oat_dex_file_storage_contents(const struct OatHeader *hdr);

void log_oat_dex_file_method_offsets_content(const struct OatHeader *oat_header,
                                             const struct OatClassData *oat_class,
                                             uint32_t method_index);

void log_oat_dex_file_class_def_contents(const uint8_t *oat_class_pointer);

void log_oat_dex_file_class_defs_contents(const struct OatHeader *oat_header,
                                          const struct DexHeader *hdr,
                                          const uint32_t *class_def_offsets_pointer);


void log_dex_file_header_contents(const struct DexHeader *hdr);

void log_dex_file_string_id_contents(const struct DexHeader *hdr, uint32_t string_id_index);

void log_dex_file_type_id_contents(const struct DexHeader *hdr, uint32_t type_id_index);

void log_dex_file_class_def_contents(const struct DexHeader *hdr, uint16_t class_def_index);

void log_dex_file_class_defs_contents(const struct DexHeader *hdr);

void log_dex_file_proto_id_contents(const struct DexHeader *hdr, uint32_t proto_id);

void log_dex_file_method_id_contents(const struct DexHeader *hdr, uint32_t method_index);

void log_dex_file_method_id_array_contents(const struct DexHeader *hdr);

void log_dex_file_method_id_array_contents_by_class_def_index(const struct DexHeader *hdr,
                                                              uint16_t class_def_index);

void log_dex_file_class_data_contents(const struct DexHeader *hdr, uint16_t class_def_index);

#endif //NDKTEST_OAT_DUMP_H
