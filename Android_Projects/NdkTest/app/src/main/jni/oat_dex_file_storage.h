//
// Created by Lukas on 8/21/2015.
//

#ifndef NDKTEST_LAZY_OAT_DEX_FILE_STORAGE_H
#define NDKTEST_LAZY_OAT_DEX_FILE_STORAGE_H

#include <sys/types.h>
#include "oat.h"
#include "typedefs.h"
#include "oat_info.h"

struct LazyOatInfo;
struct OatDexFileInfo;
struct LazyOatDexFileStorage
{
    // The Oat Info structure this is the dex_file_storage_info of.
    struct LazyOatInfo *oat_info;

    // The number of dex files. This is equivalent to OatHeader->dex_files_count_
    size_t dex_file_count;

    // The DexFile Storage needs to be walked sequentially due to the
    // String Length encoding of the dex file encoding.
    // Therefore it suffices to check for the last located entry
    // and initialize all entries between that one and the currently
    // requested one.
    //
    // This means that all the OatDexFile's up to the last located
    // one have already been located as well
    //
    uint32_t last_initialized_memory_location_index;

    // The memory_locations hold pointers into the oat file for every
    // dex file already located. A value of NULL indicates an unlocated
    // file. This can be used for consistency checks in combination
    // with the last_initialized_memory_location_index field
    //
    // the memory of the OatDexFile at index ranges from
    // memory_locations[index] (inclusive) to
    // memory_locations[index + 1] (exclusive)
    void **memory_locations;

    // For every located OatDexFile this struct contains the relevant information.
    // This could have been made to be lazy, but because the variable length encoding
    // makes an almost complete parsing necessary anyway this is done immediately
    struct OatDexFileInfo *dex_file_contents;
};

struct LazyOatDexFileStorage*  oat_dex_file_storage_Initialize(struct LazyOatInfo *oat);
struct OatDexFileInfo*  oat_dex_file_storage_GetOatDexFileInfo(struct LazyOatDexFileStorage *self, uint32_t index);
void*                   oat_dex_file_storage_GetOatDexFileLocation(struct LazyOatDexFileStorage *self, uint32_t index);
void                    oat_dex_file_storage_Destroy(struct LazyOatDexFileStorage *self);

#endif //NDKTEST_LAZY_OAT_DEX_FILE_STORAGE_H
