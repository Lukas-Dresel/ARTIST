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
struct OatHeader;

// In oat_dex_file_storage.h
typedef struct LazyOatDexFileStorage LazyOatDexFileStorage;

typedef struct LazyOatInfo
{
    void *begin;
    void *end;
    struct OatHeader *header;
    uint8_t *key_value_storage_start;
    void *dex_file_storage_start;
    LazyOatDexFileStorage *dex_file_storage_info;
} LazyOatInfo;

LazyOatInfo *oat_info_Initialize(void *begin, void *end);

struct OatHeader *oat_info_GetHeader(LazyOatInfo *self);

uint8_t *oat_info_GetKeyValueStoragePointer(LazyOatInfo *self);

void *oat_info_GetDexFileStoragePointer(LazyOatInfo *self);

LazyOatDexFileStorage *oat_info_GetDexFileStorageInfo(LazyOatInfo *self);

struct OatDexFileInfo * oat_info_GetOatDexFileByIndex(LazyOatInfo *self, uint32_t index);

/**
 *  Fills the uint32_t array pointed to by result with all the indices of OatDexFiles for which
 *  predicate p returned true.
 *  In this case the predicate gets called with (DexClassDef*, void* args).
 *
 *  @return the number of entries filled.
 */
uint32_t oat_info_FindOatDexFileIndicesByPredicate(LazyOatInfo *self, PREDICATE p,
                                                   void *additionalArgs,
                                                   uint32_t *result, uint32_t maxResults);

#endif //NDKTEST_OAT_INFO_H
