//
// Created by Lukas on 8/16/2015.
//

#include "oat_info.h"

#include "error.h"
#include "logging.h"
#include "mem_map.h"
#include "memory.h"
#include "bit_vector_util.h"
#include "oat.h"
#include "dex_class_data.h"
#include "dex.h"
#include "abi_interface.h"

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
    self->header = NULL;
    self->dex_file_storage_start = NULL;
    self->key_value_storage_start = NULL;
    self->dex_file_storage_info = NULL;
    return self;
}

OatHeader *oat_info_GetHeader(LazyOatInfo *self)
{
    CHECK(self != NULL)
    if (LIKELY(self->header != NULL))
    {
        return self->header;
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
    self->header = (OatHeader *) self->begin;
    return self->header;
}

uint8_t *oat_info_GetKeyValueStoragePointer(LazyOatInfo *self)
{
    CHECK(self != NULL);

    if (LIKELY(self->key_value_storage_start != NULL))
    {
        return self->key_value_storage_start;
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

    OatHeader *hdr = oat_info_GetHeader(self);
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
OatDexFileInfo* oat_info_GetOatDexFileByIndex(LazyOatInfo* self, uint32_t index)
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
        OatDexFileInfo* info = oat_dex_file_storage_GetOatDexFileInfo(s, i);
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
    while (oat_GetStoreKeyValuePairByIndex(hdr, index, &key, &value))
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

void log_oat_dex_file_method_offsets_content(const OatHeader* oat_header, const OatClass* oat_class, uint32_t method_index)
{
    CHECK_NE(oat_header, NULL);
    CHECK_NE(oat_class, NULL);

    size_t methods_pointer_index;
    switch(oat_class->oat_class_type)
    {
        case kOatClassNoneCompiled:
        {
            LOGD("Not available, this class has no compiled code.");
            return;
        }
        case kOatClassSomeCompiled:
        {
            const uint32_t* bitmap_data = (const uint32_t*)oat_class->bitmap_pointer;
            if(!bit_vector_IsBitSet(bitmap_data, method_index))
            {
                // This method has no compiled code
                LOGD("Method %d has no compiled coded.", method_index);
                return;
            }
            size_t num_set_bits = bit_vector_NumSetBits(bitmap_data, method_index);
            methods_pointer_index = num_set_bits;
            break;
        }
        case kOatClassAllCompiled:
        {
            methods_pointer_index = method_index;
            break;
        }
        default:
            return;
    }
    const OatMethodOffsets oat_method_offset = oat_class->methods_pointer[methods_pointer_index];

    LOGD("Method %d has OatMethodOffsets entry has code offset 0x%08x", method_index, oat_method_offset.code_offset_);

    const byte* code_pointer = entry_point_to_code_pointer((void*) oat_header + oat_method_offset.code_offset_);
    OatQuickMethodHeader* code_header = ((OatQuickMethodHeader*)code_pointer) - 1;
    LOGD("OatQuickMethodHeader: ");
    LOGD("Code Size: %d", code_header->code_size_);
    LOGD("Frame Size in bytes: %d", code_header->frame_info_.frame_size_in_bytes_);
    LOGD("Code: ");
    hexdump_primitive(code_pointer, code_header->code_size_, 8);
}

bool methodIdPredicate_ClassTypeIndex(DexFileHeader* hdr, MethodId* c, void* args)
{
    return ( c->class_idx_ == (uint32_t)args );
}
void log_oat_dex_file_class_def_contents(const uint8_t* oat_class_pointer)
{
    LOGD("Oat Class Pointer: "PRINT_PTR, (uintptr_t)oat_class_pointer);

    const byte *status_pointer = oat_class_pointer;
    int16_t mirror_class_status = *((int16_t *) status_pointer);
    LOGD("Mirror Class Status: %hd (%s)", mirror_class_status,
         repr_mirror_Class_Status(mirror_class_status));

    const byte *type_pointer = status_pointer + sizeof(mirror_class_status);
    uint16_t oat_class_type = *((uint16_t *) type_pointer);
    LOGD("Oat Class Type: %hu (%s)", oat_class_type, repr_OatClassType(oat_class_type));

    const byte* after_type_pointer = type_pointer + sizeof(uint16_t);

    uint32_t bitmap_size = 0;
    const byte* bitmap_pointer = NULL;
    const OatMethodOffsets* methods_pointer;
    if (oat_class_type == kOatClassSomeCompiled)
    {
        bitmap_size = *((uint32_t*)after_type_pointer);
        bitmap_pointer = after_type_pointer + sizeof(bitmap_size);
        methods_pointer = (void*)bitmap_pointer + bitmap_size;

        LOGD("Oat Class Compilation Bitmap Size: %d", bitmap_size);
        LOGD("Oat Class Compilation Bitmap: ");
        hexdump_primitive((void*)bitmap_pointer, bitmap_size, 8);
    }
    else
    {
        methods_pointer = (void*)after_type_pointer;
    }

    OatMethodOffsets* methods_pointer_ = (OatMethodOffsets*)methods_pointer;
    LOGD("Oat Class Methods Pointer: "PRINT_PTR, (uintptr_t)methods_pointer);
}
void log_oat_dex_file_class_defs_contents(const OatHeader *oat_header, const DexFileHeader *hdr,
                                          const uint32_t *class_def_offsets_pointer)
{
    LOGD("OatDexFile Class Definitions:");
    for (size_t class_def_index = 0; class_def_index < hdr->class_defs_size_; class_def_index++)
    {
        uint32_t class_def_offset = class_def_offsets_pointer[class_def_index];
        const byte *oat_class_pointer = ((byte *) oat_header) + class_def_offset;
        log_oat_dex_file_class_def_contents(oat_class_pointer);
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
    log_dex_file_method_id_array_contents(hdr);
}

void log_dex_file_string_id_contents(const DexFileHeader* hdr, uint32_t string_id_index)
{
    StringId* strings = (void*) hdr + hdr->string_ids_off_;
    LOGD("->String Value (%d): %s", string_id_index, (char*)((void*)hdr + strings[string_id_index].string_data_off_));
}
void log_dex_file_type_id_contents(const DexFileHeader* hdr, uint32_t type_id_index)
{
    TypeId* types = (void*)hdr + hdr->type_ids_off_;
    log_dex_file_string_id_contents(hdr, types[type_id_index].descriptor_idx_);
}
void log_dex_file_field_id_contents(const DexFileHeader* hdr, uint32_t field_id_index)
{
    if(field_id_index == kDexNoIndex)
    {
        return;
    }
    FieldId* fields = (void*)hdr + hdr->field_ids_off_;
    FieldId current = fields[field_id_index];
    LOGD("Class: [%d] %s", current.class_idx_, dex_file_GetClassDefName(hdr, current.class_idx_).content);
    log_dex_file_type_id_contents(hdr, current.class_idx_);
    LOGD("Type: [%d] %s", current.type_idx_, dex_file_GetTypeIdName(hdr, current.type_idx_).content);
    log_dex_file_type_id_contents(hdr, current.type_idx_);
    LOGD("Name: [%d] %s", current.name_idx_, dex_file_GetStringDataByIndex(hdr, current.name_idx_).content);
}
void log_dex_file_class_def_contents(const DexFileHeader* hdr, uint32_t class_def_index)
{
    if(class_def_index == kDexNoIndex)
    {
        return;
    }
    ClassDef *def = (void *) hdr + hdr->class_defs_off_;
    ClassDef c = def[class_def_index];
    LOGD("Class Index: [%hu] %s", c.class_idx_, dex_file_GetTypeIdName(hdr, c.class_idx_).content);
    LOGD("Access Flags: %08x", c.access_flags_);
    const char* super_class_name = (c.superclass_idx_ == kDexNoIndex) ? "[INVALID CLASS INDEX]" : dex_file_GetTypeIdName(hdr, c.superclass_idx_).content;
    LOGD("SuperClass Index: [%hu] %s", c.superclass_idx_, super_class_name);
    LOGD("Interfaces Offset: %d", c.interfaces_off_);
    LOGD("Source File Index: [%u] %s", c.source_file_idx_, dex_file_GetStringDataByIndex(hdr, c.source_file_idx_).content);
    log_dex_file_class_data_contents(hdr, class_def_index);
}
void log_dex_file_class_defs_contents(const DexFileHeader *hdr)
{
    LOGD("Dex File Class Definitions: ");

    for (uint32_t i = 0; i < hdr->class_defs_size_; i++)
    {
        log_dex_file_class_def_contents(hdr, i);
    }
}

void log_dex_file_proto_id_contents(const DexFileHeader* hdr, uint32_t proto_id)
{
    if(proto_id == kDexNoIndex)
    {
        return;
    }
    ProtoId* protos = (void*)hdr + hdr->proto_ids_off_;
    ProtoId p = protos[proto_id];
    LOGD("Shorty Name String Index: [%d] %s", p.shorty_idx_, dex_file_GetStringDataByIndex(hdr, p.shorty_idx_).content);
    LOGD("Return Type Index: [%d] %s", p.return_type_idx_, dex_file_GetTypeIdName(hdr, p.return_type_idx_).content);
    LOGD("Parameter Offset: %d", p.parameters_off_);
}
void log_dex_file_method_id_contents(const DexFileHeader *hdr, uint32_t method_index)
{
    if(method_index == kDexNoIndex)
    {
        return;
    }
    MethodId* def = (void*) hdr + hdr->method_ids_off_;
    MethodId m = def[method_index];
    LOGD("Dex File Method Definition: %d", method_index);
    LOGD("Class Index: [%d] %s", m.class_idx_, dex_file_GetTypeIdName(hdr, m.class_idx_).content);
    LOGD("Name Index: [%d] %s", m.name_idx_, dex_file_GetStringDataByIndex(hdr, m.name_idx_).content);
    LOGD("Prototype Index: %d", m.proto_idx_);
    log_dex_file_proto_id_contents(hdr, m.proto_idx_);
}
void log_dex_file_method_id_array_contents(const DexFileHeader *hdr)
{
    LOGD("Dex File Method Definitions: ");
    for(uint32_t i = 0; i < hdr->method_ids_size_; i++)
    {
        log_dex_file_method_id_contents(hdr, i);
    }
}
void log_dex_file_method_id_array_contents_by_class_def_index(const DexFileHeader *hdr, uint32_t class_def_index)
{
    ClassDef c = dex_file_GetClassDefinitionByIndex(hdr, class_def_index);

    const char* class_name = dex_file_GetStringDataByIndex(hdr, dex_file_GetTypeDescriptorByIndex(hdr, c.class_idx_).descriptor_idx_).content;
    LOGD("Dex File Method Definitions for class \"%s\": ", class_name);
    LOGD(" ");
    for(uint32_t i = 0; i < hdr->method_ids_size_; i++)
    {
        MethodId m = dex_file_GetMethodDescriptorByIndex(hdr, i);
        if(m.class_idx_ == c.class_idx_)
        {
            log_dex_file_method_id_contents(hdr, i);
            LOGD(" ");
        }
    }
}
void log_dex_file_class_data_contents(const DexFileHeader* hdr, uint32_t class_def_index)
{
    LOGD("Class Data: ");

    ClassDef c = dex_file_GetClassDefinitionByIndex(hdr, class_def_index);
    if(c.class_data_off_ == 0)
    {
        LOGD("This class has no class data.");
        return;
    }
    uint8_t* class_data_pointer = (void*)hdr + c.class_data_off_;
    DexClassData* class_data = dex_class_data_Initialize(class_data_pointer);
    if(UNLIKELY(class_data == NULL))
    {
        return;
    }
    uint32_t numStatic = dex_class_data_GetNumberOfStaticFields(class_data);
    uint32_t numInstance = dex_class_data_GetNumberOfInstanceFields(class_data);
    uint32_t numDirect = dex_class_data_GetNumberOfDirectMethods(class_data);
    uint32_t numVirtual = dex_class_data_GetNumberOfVirtualMethods(class_data);
    
    LOGD("Number of Static Fields:   %d", numStatic);
    LOGD("Number of Instance Fields: %d", numInstance);
    LOGD("Number of Direct Methods:  %d", numDirect);
    LOGD("Number of Virtual Methods: %d", numVirtual);

    LOGD("Static Fields: \n");
    for(uint32_t i = 0; i < numStatic; i++)
    {
        uint32_t field_id_index = dex_class_data_GetFieldIdIndex_StaticField(class_data, i);
        LOGD("Static Field:   [%d] %s", field_id_index, dex_file_GetFieldIdName(hdr, field_id_index).content);
        //log_dex_file_field_id_contents(hdr, field_id_index);
    }
    LOGD("\nInstance Fields: ");
    for(uint32_t i = 0; i < numInstance; i++)
    {
        uint32_t field_id_index = dex_class_data_GetFieldIdIndex_InstanceField(class_data, i);
        LOGD("Instance Field: [%d] %s", field_id_index, dex_file_GetFieldIdName(hdr, field_id_index).content);
        //log_dex_file_field_id_contents(hdr, field_id_index);
    }
    LOGD("\nDirect Methods: ");
    for(uint32_t i = 0; i < numDirect; i++)
    {
        uint32_t method_id_index = dex_class_data_GetMethodIdIndex_DirectMethod(class_data, i);
        LOGD("Direct Method:  [%d] %s", method_id_index, dex_file_GetMethodIdName(hdr, method_id_index).content);
        //log_dex_file_method_id_contents(hdr, method_id_index);
    }
    LOGD("\nVirtual Methods: ");
    for(uint32_t i = 0; i < numVirtual; i++)
    {
        uint32_t method_id_index = dex_class_data_GetMethodIdIndex_VirtualMethod(class_data, i);
        LOGD("Virtual Method: [%d] %s", method_id_index, dex_file_GetMethodIdName(hdr, method_id_index).content);
        //log_dex_file_method_id_contents(hdr, method_id_index);
    }
}