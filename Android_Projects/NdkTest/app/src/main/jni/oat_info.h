//
// Created by Lukas on 8/16/2015.
//

#ifndef NDKTEST_OAT_INFO_H
#define NDKTEST_OAT_INFO_H

#include "oat.h"
#include "dex.h"

#include <inttypes.h>
#include <sys/types.h>

typedef uint8_t byte;

// In oat.h
struct OatHeader;

// In oat_dex_file_storage.h
struct DexFileStorage;

typedef struct LazyOatInfo
{
    void *begin;
    void *end;
    struct OatHeader *oat_header;
    uint8_t *key_value_storage;
    struct DexFileStorage *dex_file_storage;
} LazyOatInfo;

LazyOatInfo *oat_info_Initialize(void *begin, void *end);

OatHeader *oat_info_GetHeader(LazyOatInfo *self);

uint8_t *oat_info_GetKeyValueStorageOffset(LazyOatInfo *self);

void *oat_info_GetDexFileStorageOffset(LazyOatInfo *self);

void log_elf_oat_file_info(void *oat_begin, void *oat_end);

void log_oat_header_info(OatHeader *header);

void log_oat_key_value_storage_contents(OatHeader *);

void log_oat_dex_file_storage_contents(OatHeader *);

void log_dex_file_header_contents(const DexFileHeader *);

void log_dex_file_class_defs_contents(const DexFileHeader *);

#endif //NDKTEST_OAT_INFO_H
