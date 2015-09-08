//
// Created by Lukas on 8/16/2015.
//

#include "oat_info.h"

#include "error.h"
#include "logging.h"
#include "mem_map.h"
#include "memory.h"
#include "debug_util.h"

LazyOatInfo *oat_info_Initialize(void *begin, void *end)
{
    CHECK(end > begin);
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
    self->oat_header = NULL;
    self->dex_file_storage = NULL;
    self->key_value_storage = NULL;
    return self;
}

OatHeader *oat_info_GetHeader(LazyOatInfo *self)
{
    CHECK(self != NULL)
    if (LIKELY(self->oat_header != NULL))
    {
        return self->oat_header;
    }

    void *after = self->begin + sizeof(OatHeader);
    if (UNLIKELY(after > self->end))
    {
        LOGF("Found Truncated Oat Header. (Base: "
                     PRINT_PTR
                     ", End: "
                     PRINT_PTR,
             (uintptr_t) self->begin, (uintptr_t) self->end);
        return NULL;
    }
    self->oat_header = (OatHeader *) self->begin;
    return self->oat_header;
}

uint8_t *oat_info_GetKeyValueStorageOffset(LazyOatInfo *self)
{
    CHECK(self != NULL);

    if (LIKELY(self->key_value_storage != NULL))
    {
        return self->key_value_storage;
    }

    OatHeader *hdr = oat_info_GetHeader(self);
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
    self->key_value_storage = hdr->key_value_store_;
    return self->key_value_storage;
}

void *oat_info_GetDexFileStorageOffset(LazyOatInfo *self)
{
    CHECK(self != NULL);
    if (LIKELY(self->dex_file_storage !=
               NULL)) // Since this is set every time but the first one this should be likely.
    {
        return self->dex_file_storage;
    }

    OatHeader *hdr = oat_info_GetHeader(self);
    if (UNLIKELY(hdr == NULL))
    {
        return NULL;
    }
    CHECK(hdr->key_value_store_size_ > 0U);
    void *key_value_storage = oat_info_GetKeyValueStorageOffset(self);
    if (UNLIKELY(key_value_storage == NULL))
    {
        return NULL;
    }
    void *dex_file_storage = key_value_storage + hdr->key_value_store_size_;
    self->dex_file_storage = dex_file_storage;
    return self->dex_file_storage;
}

void log_elf_oat_file_info(void *oat_begin, void *oat_end)
{
    CHECK(oat_begin < oat_end);
    OatHeader *header = (OatHeader *) oat_begin;
    log_oat_header_info(header);
    return;
}

void log_oat_header_info(OatHeader *hdr)
{
    LOGD("Oat Header Information:");

    LOGD("Magic:");
    hexdump_primitive(hdr->magic_, sizeof(hdr->magic_), sizeof(hdr->magic_));
    LOGD("Version:");
    hexdump_primitive(hdr->version_, sizeof(hdr->version_), sizeof(hdr->version_));
    LOGD("Checksum:                                      %08x", hdr->adler32_checksum_);

    clear_last_error();
    LOGD("InstructionSet:                                %d (%s)", hdr->instruction_set_,
         repr_InstructionSet(hdr->instruction_set_));
    if (error_occurred())
    {
        LOGE("Found invalid instruction set in Oat Header at "
                     PRINT_PTR, (uintptr_t) hdr);
    }
    LOGD("InstructionSetFeatures:                        %x", hdr->instruction_set_features_);
    if ((hdr->instruction_set_ & kHwDiv) == kHwDiv)
    {
        LOGD("->Hardware Division Support (%x)", kHwDiv);
    }
    if ((hdr->instruction_set_ & kHwLpae) == kHwLpae)
    {
        LOGD("->Large Physical Address Extension (%x)", kHwLpae);
    }
    LOGD("Dex File Count:                                %d", hdr->dex_file_count_);
    LOGD("Executable Offset:                             %x", hdr->executable_offset_);

    LOGD("Interp2Interp Bridge Offset:                   %x",
         hdr->interpreter_to_interpreter_bridge_offset_);
    LOGD("Interp2Compiled Bridge Offset:                 %x",
         hdr->interpreter_to_compiled_code_bridge_offset_);
    LOGD("JNI DLSym lookup offset:                       %x", hdr->jni_dlsym_lookup_offset_);

    LOGD("Portable IMT Conflict Trampoline Offset:       %x",
         hdr->portable_imt_conflict_trampoline_offset_);
    LOGD("Portable Resolution Trampoline Offset:         %x",
         hdr->portable_resolution_trampoline_offset_);
    LOGD("Portable to Interpreter Bridge Offset:         %x",
         hdr->portable_to_interpreter_bridge_offset_);

    LOGD("Quick Generic JNI Trampoline Offset:           %x",
         hdr->quick_generic_jni_trampoline_offset_);
    LOGD("Quick IMT Conflict Trampoline Offset:          %x",
         hdr->quick_imt_conflict_trampoline_offset_);
    LOGD("Quick Resolution Trampoline Offset:            %x",
         hdr->quick_resolution_trampoline_offset_);
    LOGD("Quick to Interpreter Bridge Offset:            %x",
         hdr->quick_to_interpreter_bridge_offset_);

    LOGD("Image Patch Delta:                             %x", hdr->image_patch_delta_);
    LOGD("Image File Location Oat Checksum:              %x",
         hdr->image_file_location_oat_checksum_);
    LOGD("Image File Location Oat Data Begin:            %x",
         hdr->image_file_location_oat_data_begin_);

    LOGD("Key-Value Store Size:                          %d", hdr->key_value_store_size_);

    log_oat_key_value_storage_contents(hdr);
    log_oat_dex_file_storage_contents(hdr);
}

void log_oat_key_value_storage_contents(OatHeader *hdr)
{
    size_t index = 0;
    const char *key;
    const char *value;
    LOGD("Key-Value Store: ");
    while (oat_header_GetStoreKeyValuePairByIndex(hdr, index, &key, &value))
    {
        LOGD("%s = %s", key, value);
        index++;
    }
    return;
}

void log_oat_dex_file_class_defs_contents(const OatHeader *, const DexFileHeader *, const uint32_t *);
void log_oat_dex_file_storage_contents(OatHeader *hdr)
{
    byte *ptr = (byte *) hdr->key_value_store_;
    ptr += hdr->key_value_store_size_;
    LOGD("Dex-File Storage (%d dex files):", hdr->dex_file_count_);
    hexdump_primitive(ptr, 128, 16);
    for (size_t dex_file_index = 0; dex_file_index < hdr->dex_file_count_; dex_file_index++)
    {
        LOGD("Dex-File #%zd:", dex_file_index);

        uint32_t dex_file_location_string_length = *((uint32_t *) ptr);
        LOGD("dex_file_location_string_length = %u", dex_file_location_string_length);
        ptr += sizeof(dex_file_location_string_length);
        hexdump_primitive(ptr, 64, 16);

        char *dex_file_location = (char *) ptr;
        char copy[dex_file_location_string_length + 1];
        copy[dex_file_location_string_length] = 0;
        strncpy(copy, dex_file_location, dex_file_location_string_length);
        LOGD("dex_file_location               = %s", copy);
        ptr += dex_file_location_string_length;

        uint32_t dex_file_checksum = *(uint32_t *) ptr;
        LOGD("dex_file_checksum               = %08x", dex_file_checksum);
        ptr += sizeof(dex_file_checksum);

        uint32_t dex_file_offset = *(const uint32_t*)ptr;
        LOGD("dex_file_offset                 = %08x", dex_file_offset);
        ptr += sizeof(dex_file_offset);

        const uint8_t* dex_file_pointer = (void*)hdr + dex_file_offset;
        const DexFileHeader* dex_header = (const DexFileHeader*)dex_file_pointer;

        log_dex_file_header_contents(dex_header);

        const uint32_t* class_def_offsets_pointer = (const uint32_t*)(ptr); // AOSP calls this (i believe erroneously) method_offsets_pointer
        ptr += (sizeof(*class_def_offsets_pointer) * dex_header->class_defs_size_);

        log_oat_dex_file_class_defs_contents(hdr, dex_header, class_def_offsets_pointer);

        /*
        uint32_t dex_file_offset = *(uint32_t *) ptr;
        LOGD("dex_file_offset                 = %08x", dex_file_offset);
        ptr += sizeof(dex_file_offset);

        const byte *dex_file_pointer = (byte *) hdr + dex_file_offset;
        const DexFileHeader *dex_header = (const DexFileHeader *) dex_file_pointer;

        LOGD("Dex file header:");
        hexdump_primitive(dex_header, 64, 16);
        //log_dex_file_header_contents(dex_header);

        const uint32_t *class_def_offsets_pointer = (const uint32_t *) ptr; // AOSP calls this (i believe erroneously) method_offsets_pointer

        ptr += (sizeof(*class_def_offsets_pointer) * dex_header->class_defs_size_);
        //log_oat_dex_file_class_defs_contents(hdr, dex_header, class_def_offsets_pointer);
         */
    }
}

void log_oat_dex_file_class_defs_contents(const OatHeader *oat_header, const DexFileHeader *hdr,
                                          const uint32_t *class_def_offsets_pointer)
{
    LOGD("OatDexFile Class Definitions:");
    for (size_t class_def_index = 0; class_def_index < hdr->class_defs_size_; class_def_index++)
    {
        LOGD("OatDexFile Class Definition #%zd:", class_def_index);

        uint32_t class_def_offset = class_def_offsets_pointer[class_def_index];
        const byte *oat_class_pointer = ((byte *) oat_header) + class_def_offset;

        const byte *status_pointer = oat_class_pointer;
        int16_t mirror_class_status = *((int16_t *) status_pointer);
        LOGD("Mirror Class Status: %hd (%s)", mirror_class_status,
             repr_mirror_Class_Status(mirror_class_status));

        const byte *type_pointer = status_pointer + sizeof(mirror_class_status);
        uint16_t oat_class_type = *((uint16_t *) type_pointer);
        LOGD("Oat Class Type: %hu (%s)", oat_class_type, repr_OatClassType(oat_class_type));

        if (oat_class_type == kOatClassSomeCompiled)
        {

        }
    }
}

void log_dex_file_header_contents(const DexFileHeader *hdr)
{
    LOGD("Dex Header Contents:");

    LOGD("Magic:");
    hexdump_primitive(hdr->magic_, 8, 8);

    LOGD("Checksum:        %08x", hdr->checksum_);

    LOGD("Signature:");
    hexdump_primitive(hdr->signature_, 20, 20);

    LOGD("File Size:       %d", hdr->file_size_);
    LOGD("Header Size:     %d", hdr->header_size_);
    LOGD("Endian Tag:      %x", hdr->endian_tag_);
    LOGD("Link Size:       %x", hdr->link_size_);
    LOGD("Link Offset:     %x", hdr->link_off_);
    LOGD("Map Offset:      %x", hdr->map_off_);

    LOGD("#StringId's:     %d", hdr->string_ids_size_);
    LOGD("StringId Offset: %x", hdr->string_ids_off_);
    LOGD("#TypeId's:       %d", hdr->type_ids_size_);
    LOGD("TypeId Offset:   %x", hdr->type_ids_off_);
    LOGD("#ProtoId's:      %d", hdr->proto_ids_size_);
    LOGD("ProtoId Offset:  %x", hdr->proto_ids_off_);
    LOGD("#FieldId's:      %d", hdr->field_ids_size_);
    LOGD("FieldId Offset:  %x", hdr->field_ids_off_);
    LOGD("#MethodId's:     %d", hdr->method_ids_size_);
    LOGD("MethodId Offset: %x", hdr->method_ids_off_);
    LOGD("#ClassDef's:     %d", hdr->class_defs_size_);
    LOGD("ClassDef Offset: %x", hdr->class_defs_off_);
    LOGD("Data Size:       %d", hdr->data_size_);
    LOGD("Data Offset:     %x", hdr->data_off_);

    log_dex_file_class_defs_contents(hdr);
}

void log_dex_file_class_defs_contents(const DexFileHeader *hdr)
{
    LOGD("Dex File Class Definitions: ");
    ClassDef *def = (void *) hdr + hdr->class_defs_off_;
    StringId *strings = (void *) hdr + hdr->string_ids_off_;
    for (uint32_t i = 0; i < hdr->class_defs_size_; i++)
    {
        ClassDef c = def[i];
        char *source_file_index = (void *) hdr + strings[c.source_file_idx_].string_data_off_;
        if(strstr(source_file_index, "Dex") != NULL)
        {
            LOGD("Class Index: %hu", c.class_idx_);
            LOGD("Access Flags: %08x", c.access_flags_);
            LOGD("SuperClass Index: %hu", c.superclass_idx_);
            LOGD("Interfaces Offset: %d", c.interfaces_off_);
            LOGD("Source File Index: %u", c.source_file_idx_);
            LOGD("-> String contents: %s", source_file_index);
        }
    }
}