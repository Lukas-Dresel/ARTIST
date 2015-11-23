//
// Created by Lukas on 8/16/2015.
//

#include "oat_info.h"

#include "logging.h"
#include "memory.h"

LazyOatInfo *oat_info_Initialize(void *begin, void *end)
{
    CHECK(end > begin);
    CHECK_RETURNNULL(begin != NULL && end != NULL);
    if (UNLIKELY(NULL == begin || NULL == end))
    {
        LOGF("Passed NULL pointer as argument. This would result in Segmentation faults.");
        LOGF("Arguments: (Begin: "
                     PRINT_PTR
                     ", End: "
                     PRINT_PTR
                     ")", (uintptr_t) begin, (uintptr_t) end);
        return NULL;
    }
    LazyOatInfo *self = allocate_memory_chunk(sizeof(LazyOatInfo));
    if (UNLIKELY(NULL == self))
    {
        LOGF("Failed to allocate memory for LazyOatInfo struct.");
        return NULL;
    }
    self->begin = begin;
    self->end = end;
    self->header = NULL;
    self->dex_file_storage_start = NULL;
    self->key_value_storage_start = NULL;
    self->dex_file_storage_info = NULL;
    return self;
}

struct OatHeader *oat_info_GetHeader(LazyOatInfo *self)
{
    CHECK(self != NULL)
    if (LIKELY(self->header != NULL))
    {
        return self->header;
    }

    void *after = self->begin + sizeof(struct OatHeader);
    if (UNLIKELY(after > self->end))
    {
        LOGF("Found Truncated Oat Header. (Base: "
                     PRINT_PTR
                     ", End: "
                     PRINT_PTR,
             (uintptr_t) self->begin, (uintptr_t) self->end);
        return NULL;
    }
    self->header = (struct OatHeader *) self->begin;
    return self->header;
}

uint8_t *oat_info_GetKeyValueStoragePointer(LazyOatInfo *self)
{
    CHECK(self != NULL);

    if (LIKELY(self->key_value_storage_start != NULL))
    {
        return self->key_value_storage_start;
    }

    struct OatHeader *hdr = oat_info_GetHeader(self);
    if (UNLIKELY(hdr == NULL))
    {
        return NULL;
    }
    CHECK(hdr->key_value_store_size_ > 0U);

    void *after_header = hdr->key_value_store_ + hdr->key_value_store_size_;
    if (UNLIKELY(after_header > self->end))
    {
        LOGF("Found truncated key-value storage. ");
        return NULL;
    }
    self->key_value_storage_start = hdr->key_value_store_;
    return self->key_value_storage_start;
}

void *oat_info_GetDexFileStoragePointer(LazyOatInfo *self)
{
    CHECK(self != NULL);
    if (LIKELY(self->dex_file_storage_start != NULL))
    {
        // Since this is already set every time except for the first run this should be likely.
        return self->dex_file_storage_start;
    }

    struct OatHeader *hdr = oat_info_GetHeader(self);
    if (UNLIKELY(hdr == NULL))
    {
        return NULL;
    }
    CHECK(hdr->key_value_store_size_ > 0U);
    void *key_value_storage = oat_info_GetKeyValueStoragePointer(self);
    if (UNLIKELY(key_value_storage == NULL))
    {
        return NULL;
    }
    void *dex_file_storage = key_value_storage + hdr->key_value_store_size_;
    self->dex_file_storage_start = dex_file_storage;
    return self->dex_file_storage_start;
}

LazyOatDexFileStorage* oat_info_GetDexFileStorageInfo(LazyOatInfo* self)
{
    CHECK(self != NULL);

    if(UNLIKELY(self->dex_file_storage_info == NULL))
    {
        self->dex_file_storage_info = oat_dex_file_storage_Initialize(self);
    }
    return self->dex_file_storage_info;
}

struct OatDexFileInfo *oat_info_GetOatDexFileByIndex(LazyOatInfo* self, uint32_t index)
{
    LazyOatDexFileStorage* s = oat_info_GetDexFileStorageInfo(self);
    return oat_dex_file_storage_GetOatDexFileInfo(s, index);
}

uint32_t oat_info_FindOatDexFileIndicesByPredicate(LazyOatInfo *self, PREDICATE p, void *additionalArgs, uint32_t* result, uint32_t maxResults)
{
    CHECK(self != NULL);
    CHECK(p != NULL);
    LazyOatDexFileStorage* s = oat_info_GetDexFileStorageInfo(self);
    uint32_t foundResults = 0;
    for(size_t i = 0; i < s->dex_file_count; i++)
    {
        struct OatDexFileInfo* info = oat_dex_file_storage_GetOatDexFileInfo(s, i);
        if(p(info, additionalArgs))
        {
            *result = i;
            result++;
            foundResults++;
            if(foundResults >= maxResults)
            {
                break;
            }
        }
    }
    return foundResults;
}

