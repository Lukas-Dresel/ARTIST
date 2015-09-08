//
// Created by Lukas on 8/21/2015.
//

#include "oat_dex_file_storage.h"
#include "memory.h"

LazyOatDexFileStorage *oat_dex_file_storage_Initialize(LazyOatInfo *oat)
{
    CHECK(oat != NULL);

    OatHeader *hdr = oat_info_GetHeader(oat);
    if (UNLIKELY(NULL == hdr))
    {
        return NULL;
    }

    void *dex_file_storage = oat_info_GetDexFileStorageOffset(oat);
    if (UNLIKELY(NULL == dex_file_storage))
    {
        return NULL;
    }

    size_t header_size = sizeof(LazyOatDexFileStorage); // header size

    // We need 1 entry more so that we can use [index, index + 1] as boundaries.
    size_t memory_locations_size =
            sizeof(void *) * (hdr->dex_file_count_ + 1);      // Memory Offset Array
    size_t dex_file_contents_size = sizeof(OatDexFileInfo) * hdr->dex_file_count_;

    size_t needed_size = header_size + memory_locations_size + dex_file_contents_size;
    LazyOatDexFileStorage *self = allocate_memory_chunk(needed_size);
    if (UNLIKELY(NULL == self))
    {
        LOGF("Allocating memory for LazyOatDexFileStorage failed.");
        return NULL;
    }
    self->oat_info = oat;
    self->dex_file_count = hdr->dex_file_count_;

    // Setup the arrays immediately after the header struct
    self->memory_locations = ((void *) self + header_size);
    self->dex_file_contents = ((void *) self->memory_locations + memory_locations_size);

    memset(self->memory_locations, 0, memory_locations_size);
    memset(self->dex_file_contents, 0, dex_file_contents_size);

    // The first dex file lies at the beginning of the dex-file storage
    self->last_initialized_memory_location_index = 0U;
    self->memory_locations[0] = dex_file_storage;

    return self;
}

OatDexFileInfo *oat_dex_file_storage_GetOatDexFileInfo(LazyOatDexFileStorage *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(self->dex_file_count > 0U);
    CHECK(index < self->dex_file_count);

    // the index has to be a lower bound to have a initialized file
    if (index < self->last_initialized_memory_location_index)
    {
        // Our OatDexFile has been located already
        return &(self->dex_file_contents[index]);
    }
    CHECK(self->dex_file_contents->memory_location == NULL)
    // Our OatDexFile hasn't been located yet, keep locating new ones.
    for (uint32_t current_index = self->last_initialized_memory_location_index;
         current_index <= index; current_index++)
    {
        byte *content_element = self->memory_locations[current_index];
        CHECK(content_element <
              self->oat_info->end); // at the start of the loop our index should not be out of range

        uint32_t oat_dex_file_location_size = *((uint32_t *) content_element);
        CHECK(oat_dex_file_location_size > 0U);
        content_element += sizeof(oat_dex_file_location_size); // Skip dex_file_location_size
        if (UNLIKELY(content_element > self->oat_info->end))
        {
            LOGF("OatDexFile %d: Found truncated oat_dex_file_location_size", current_index);
            return NULL;
        }
        OatDexFileInfo *info = &(self->dex_file_contents[current_index]);
        info->dex_file_location.length = oat_dex_file_location_size;

        char *oat_dex_file_location = (char *) content_element;
        CHECK(strlen(oat_dex_file_location) != 0);
        CHECK(strlen(oat_dex_file_location) == (oat_dex_file_location_size - 1));
        content_element += oat_dex_file_location_size;
        if (UNLIKELY(content_element > self->oat_info->end))
        {
            LOGF("OatDexFile %d: Found truncated oat_dex_file_location", current_index);
            return NULL;
        }
        info->dex_file_location.content = oat_dex_file_location;

        uint32_t oat_dex_file_checksum = *((uint32_t *) content_element);
        content_element += sizeof(oat_dex_file_checksum);
        if (UNLIKELY(content_element > self->oat_info->end))
        {
            LOGF("OatDexFile %d: Found truncated oat_dex_file_checksum", current_index);
            return NULL;
        }
        info->checksum = oat_dex_file_checksum;

        uint32_t oat_dex_file_offset = *((uint32_t *) content_element);
        content_element += sizeof(uint32_t);
        if (UNLIKELY(content_element > self->oat_info->end))
        {
            LOGF("OatDexFile %d: Found truncated oat_dex_file_offset", current_index);
            return NULL;
        }

        info->dex_file_pointer = self->oat_info->begin + oat_dex_file_offset;

        DexFileHeader *dex_header = (DexFileHeader *) info->dex_file_pointer;
        if (UNLIKELY(dex_header > self->oat_info->begin))
        {
            LOGF("OatDexFile %d: Dex File Pointer points outside the valid memory range.",
                 current_index);
            return NULL;
        }
        if (UNLIKELY((dex_header + 1) > self->oat_info->end))
        {
            LOGF("OatDexFile %d: Found truncated DexFile header", current_index);
            return NULL;
        }

        info->number_of_defined_classes = dex_header->class_defs_size_;
        info->class_definition_offsets = (uint32_t *) content_element;
        content_element += sizeof(uint32_t) * dex_header->class_defs_size_;
        if (UNLIKELY(content_element > self->oat_info->end))
        {
            LOGF("OatDexFile %d: Found truncated class_definition_offsets array", current_index);
            return NULL;
        }

        // We are all done with this one, set the memory location and references
        info->memory_location = self->memory_locations[current_index];
        info->oat_info = self->oat_info;
        info->oat_dex_file_storage = self;
        info->index = current_index;
        self->memory_locations[current_index + 1] = content_element;
    }
    return &(self->dex_file_contents[index]);
}

void *oat_dex_file_storage_GetOatDexFileLocation(LazyOatDexFileStorage *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(self->dex_file_count > 0U);
    CHECK(index < self->dex_file_count);

    // the index has to be a lower bound to have a initialized file
    if (index < self->last_initialized_memory_location_index)
    {
        // Our OatDexFile has been located already
        return self->memory_locations[index];
    }
    OatDexFileInfo *info = oat_dex_file_storage_GetOatDexFileLocation(self, index);
    if (NULL == info)
    {
        return NULL;
    }
    return info->memory_location;
}

void oat_dex_file_storage_Destroy(LazyOatDexFileStorage *self)
{
    CHECK(self != NULL);
    free_memory_chunk(self);
}