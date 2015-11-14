//
// Created by Lukas on 8/16/2015.
//

#ifndef NDKTEST_OAT_INFO_H
#define NDKTEST_OAT_INFO_H

#include <inttypes.h>
#include <sys/types.h>

#include "oat.h"
#include "dex.h"
#include "oat_dex_file_storage.h"
#include "util.h"
#include "typedefs.h"

// In oat.h
typedef struct OatHeader OatHeader;

// In oat_dex_file_storage.h
typedef struct LazyOatDexFileStorage LazyOatDexFileStorage;

typedef struct LazyOatInfo
{
    void *                  begin;
    void *                  end;
    struct OatHeader *      header;
    uint8_t *               key_value_storage_start;
    void *                  dex_file_storage_start;
    LazyOatDexFileStorage*  dex_file_storage_info;
} LazyOatInfo;

LazyOatInfo*            oat_info_Initialize(void *begin, void *end);
OatHeader*              oat_info_GetHeader(LazyOatInfo *self);
uint8_t*                oat_info_GetKeyValueStoragePointer(LazyOatInfo *self);
void*                   oat_info_GetDexFileStoragePointer(LazyOatInfo *self);
LazyOatDexFileStorage*  oat_info_GetDexFileStorageInfo(LazyOatInfo* self);
OatDexFileInfo*         oat_info_GetOatDexFileByIndex(LazyOatInfo* self, uint32_t index);

/**
 *  Fills the uint32_t array pointed to by result with all the indices of OatDexFiles for which
 *  predicate p returned true.
 *  In this case the predicate gets called with (ClassDef*, void* args).
 *
 *  @return the number of entries filled.
 */
uint32_t                oat_info_FindOatDexFileIndicesByPredicate(LazyOatInfo *self, PREDICATE p,
                                                                  void *additionalArgs,
                                                                  uint32_t* result, uint32_t maxResults);



void log_elf_oat_file_info(void *oat_begin, void *oat_end);

void log_oat_header_info(OatHeader *hdr);

void log_oat_key_value_storage_contents(OatHeader *hdr);

void log_oat_dex_file_storage_contents(OatHeader *hdr);

void log_oat_dex_file_method_offsets_content(const OatHeader* oat_header, const OatClass* oat_class,
                                             uint32_t method_index);

void log_oat_dex_file_class_def_contents(const uint8_t* oat_class_pointer);

void log_oat_dex_file_class_defs_contents(const OatHeader *oat_header, const DexFileHeader *hdr,
                                          const uint32_t *class_def_offsets_pointer);


void log_dex_file_header_contents(const DexFileHeader *hdr);
void log_dex_file_string_id_contents(const DexFileHeader* hdr, uint32_t string_id_index);
void log_dex_file_type_id_contents(const DexFileHeader* hdr, uint32_t type_id_index);
void log_dex_file_class_def_contents(const DexFileHeader* hdr, uint32_t class_def_index);
void log_dex_file_class_defs_contents(const DexFileHeader *hdr);

void log_dex_file_proto_id_contents(const DexFileHeader* hdr, uint32_t proto_id);
void log_dex_file_method_id_contents(const DexFileHeader *hdr, uint32_t method_index);
void log_dex_file_method_id_array_contents(const DexFileHeader *hdr);
void log_dex_file_method_id_array_contents_by_class_def_index(const DexFileHeader *hdr,
                                                              uint32_t class_def_index);

void log_dex_file_class_data_contents(const DexFileHeader* hdr, uint32_t class_def_index);

#endif //NDKTEST_OAT_INFO_H
