//
// Created by Lukas on 11/18/2015.
//

#ifndef NDKTEST_OAT_DUMP_H
#define NDKTEST_OAT_DUMP_H


#include <stdint.h>

void log_elf_oat_file_info(void *oat_begin, void *oat_end);

void log_oat_header_info(const struct OatHeader *hdr);

void log_oat_key_value_storage_contents(const struct OatHeader *hdr);

void log_oat_dex_file_storage_contents(const struct OatHeader *hdr);

void log_oat_dex_file_method_offsets_content(const struct OatHeader *oat_header,
                                             const struct OatClass *oat_class,
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
