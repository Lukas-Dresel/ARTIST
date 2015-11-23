//
// Created by Lukas on 8/16/2015.
//


#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include "logging.h"

static bool DecodeOatDexFile(struct DecodedOatDexFile* result, const struct OatHeader* hdr, void** start_p, void* end)
{
    void* data = *start_p;
    CHECK(data < end); // at the start of the loop our index should not be out of range

    uint32_t oat_dex_file_location_size = *((uint32_t *)data);
    CHECK(oat_dex_file_location_size > 0U);
    data += sizeof(oat_dex_file_location_size); // Skip dex_file_location_size
    if (UNLIKELY(data > end))
    {
        LOGF("Found truncated oat_dex_file_location_size.");
        return NULL;
    }

    char *oat_dex_file_location = (char *)data;
    CHECK(strlen(oat_dex_file_location) != 0);
    data += oat_dex_file_location_size;
    if (UNLIKELY(data > end))
    {
        LOGF("Found truncated oat_dex_file_location");
        return NULL;
    }

    uint32_t oat_dex_file_checksum = *((uint32_t *)data);
    data += sizeof(oat_dex_file_checksum);
    if (UNLIKELY(data > end))
    {
        LOGF("Found truncated oat_dex_file_checksum");
        return NULL;
    }

    uint32_t dex_file_offset = *((uint32_t *)data);
    data += sizeof(uint32_t);
    if (UNLIKELY(data > end))
    {
        LOGF("Found truncated dex_file_offset");
        return NULL;
    }

    struct DexHeader *dex_header = (struct DexHeader *)((void*)hdr + dex_file_offset);
    if (UNLIKELY(dex_header > end))
    {
        LOGF("Dex File Pointer points outside the valid memory range.");
        return NULL;
    }
    if (UNLIKELY((dex_header + 1) > end))
    {
        LOGF("Found truncated DexFile header");
        return NULL;
    }
    if(UNLIKELY((void*)dex_header + dex_header->file_size_ > end))
    {
        LOGF("Found truncated DexFile");
        return NULL;
    }

    uint32_t num_classes = dex_header->class_defs_size_;
    uint32_t* class_def_offsets = (uint32_t*)data;

    data += sizeof(uint32_t) * dex_header->class_defs_size_;
    if (UNLIKELY(data > end))
    {
        LOGF("Found truncated class_definition_offsets array");
        return NULL;
    }

    // We are all done with this one, set the memory location and references
    result->backing_memory_address = *start_p;
    result->backing_memory_size = data - *start_p;

    result->location_string.length = oat_dex_file_location_size;
    result->location_string.content = oat_dex_file_location;
    result->checksum = oat_dex_file_checksum;
    result->dex_file_offset = dex_file_offset;

    result->number_of_defined_classes = num_classes;
    result->class_definition_offsets = class_def_offsets;
}

bool oat_Setup(struct OatFile* result, void *mem_begin, void *mem_end)
{
    CHECK_RETURNFALSE(mem_end > mem_begin);
    CHECK_RETURNFALSE(mem_begin != NULL && mem_end != NULL);

    result->begin = mem_begin;
    result->end = mem_end;
    result->header = (struct OatHeader*) mem_begin;
    result->key_value_storage_start = &result->header->key_value_store_[0];
    result->dex_file_storage_start = result->key_value_storage_start + result->header->key_value_store_size_;
    return true;
}




