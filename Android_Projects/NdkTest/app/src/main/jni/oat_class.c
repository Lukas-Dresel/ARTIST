//
// Created by Lukas on 10/3/2015.
//

#include "oat_class.h"
#include "logging.h"
#include "bit_vector_util.h"
#include "abi_interface.h"

OatClass* oat_class_Parse(OatClass* result, const uint8_t* oat_class_pointer)
{
    CHECK(result != NULL);
    CHECK(oat_class_pointer != NULL);

    const byte *status_pointer = oat_class_pointer;

    result->mirror_class_status = *((int16_t *) status_pointer);

    const byte *type_pointer = status_pointer + sizeof(result->mirror_class_status);
    result->oat_class_type = *((uint16_t *) type_pointer);

    const byte* after_type_pointer = type_pointer + sizeof(result->oat_class_type);

    result->bitmap_size = 0;
    result->bitmap_pointer = NULL;
    const OatMethodOffsets* methods_pointer;
    if (result->oat_class_type == kOatClassSomeCompiled)
    {
        result->bitmap_size = *((uint32_t*)after_type_pointer);
        result->bitmap_pointer = after_type_pointer + sizeof(result->bitmap_size);
        methods_pointer = (void*)result->bitmap_pointer + result->bitmap_size;
    }
    else
    {
        methods_pointer = (void*)after_type_pointer;
    }
    result->methods_pointer = (OatMethodOffsets*)methods_pointer;
    return result;
}
OatClass* oat_class_Extract(OatClass* result, LazyOatInfo* oat_info, uint32_t dex_file_index, uint32_t class_def_index)
{
    OatDexFileInfo* oat_dex = oat_dex_file_storage_GetOatDexFileInfo(oat_info->dex_file_storage_info, dex_file_index);
    uint32_t current_oat_class_def_offset = oat_dex->class_definition_offsets[class_def_index];
    void* oat_class_def_pointer = (void*)oat_info->header + current_oat_class_def_offset;
    return oat_class_Parse(result, oat_class_def_pointer);
}

static uint32_t GetEffectiveMethodIndex(uint16_t class_type, uint32_t* bitmap_data, uint32_t method_index)
{
    CHECK(class_type < kOatClassMax);
    CHECK(bitmap_data != NULL);
    CHECK(method_index != kDexNoIndex);

    if(class_type == kOatClassAllCompiled)
    {
        return method_index;
    }
    if(class_type == kOatClassNoneCompiled || !bit_vector_IsBitSet(bitmap_data, method_index))
    {
        // This method has no compiled code
        return kDexNoIndex;
    }
    return bit_vector_NumSetBits(bitmap_data, method_index);
}
void* oat_class_GetMethodCodePointer(const OatHeader* oat_header, const OatClass* oat_class, uint32_t method_index)
{
    CHECK_NE(oat_header, NULL);
    CHECK_NE(oat_class, NULL);

    uint32_t methods_pointer_index = GetEffectiveMethodIndex(oat_class->oat_class_type, oat_class->bitmap_pointer, method_index);
    if(methods_pointer_index == kDexNoIndex)
    {
        // This method has no compiled code.
        return NULL;
    }
    const OatMethodOffsets oat_method_offset = oat_class->methods_pointer[methods_pointer_index];
    return (void*)oat_header + oat_method_offset.code_offset_;
}
OatQuickMethodHeader* oat_class_GetQuickMethodHeader(const OatHeader* oat_header, const OatClass* oat_class, uint32_t method_index)
{
    OatQuickMethodHeader* hdr = entry_point_to_code_pointer(oat_class_GetMethodCodePointer(oat_header, oat_class, method_index));
    return (hdr == NULL) ? NULL : hdr - 1; // The OatQuickMethodHeader lies right before the code.
}
