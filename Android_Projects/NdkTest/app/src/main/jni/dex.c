//
// Created by Lukas on 8/20/2015.
//

#include "dex.h"
#include "logging.h"
#include "leb128.h"

String dex_file_GetStringDataByIndex(const DexFileHeader* hdr, uint32_t string_id)
{
    CHECK(hdr != NULL);
    CHECK(string_id < hdr->string_ids_size_);

    StringId* strings = (void*)hdr + hdr->string_ids_off_;
    String str;
    const byte* string_data_item = (void*) hdr + strings[string_id].string_data_off_;
    str.length = DecodeUnsignedLeb128(&string_data_item);
    str.content = (const char*)string_data_item;
    return str;
}
String      dex_file_GetClassDefName(const DexFileHeader* hdr, uint32_t class_def_index)
{
    CHECK(hdr != NULL);
    CHECK(class_def_index < hdr->class_defs_size_);

    ClassDef c = dex_file_GetClassDefinitionByIndex(hdr, class_def_index);
    return dex_file_GetTypeIdName(hdr, c.class_idx_);
}
String      dex_file_GetMethodIdName(const DexFileHeader* hdr, uint32_t method_id_index)
{
    CHECK(hdr != NULL);
    CHECK(method_id_index < hdr->method_ids_size_);

    MethodId m = dex_file_GetMethodDescriptorByIndex(hdr, method_id_index);
    return dex_file_GetStringDataByIndex(hdr, m.name_idx_);
}
String      dex_file_GetTypeIdName(const DexFileHeader* hdr, uint32_t type_id_index)
{
    CHECK(hdr != NULL);
    if(type_id_index == kDexNoIndex)
    {
        char* message = "[INVALID TYPE INDEX]";
        String s = {.content = message, .length = strlen(message)};
        return s;
    }
    CHECK(type_id_index < hdr->type_ids_size_);

    TypeId t = dex_file_GetTypeDescriptorByIndex(hdr, type_id_index);
    return dex_file_GetStringDataByIndex(hdr, t.descriptor_idx_);
}
String      dex_file_GetFieldIdName(const DexFileHeader* hdr, uint32_t field_id_index)
{
    CHECK(hdr != NULL);
    CHECK(field_id_index < hdr->field_ids_size_);

    FieldId f = dex_file_GetFieldDescriptorByIndex(hdr, field_id_index);
    return dex_file_GetStringDataByIndex(hdr, f.name_idx_);
}
uint32_t dex_file_FindSuperClassDefinitionIndex(const DexFileHeader* hdr, uint32_t original_class_def_index)
{
    CHECK(hdr != NULL);
    CHECK(original_class_def_index < hdr->class_defs_size_);

    // Dex specification requires superclasses to be defined prior to their subclasses,
    // so iterating up to our class should be sufficient.
    ClassDef original = dex_file_GetClassDefinitionByIndex(hdr, original_class_def_index);
    for(uint32_t i = 0; i < original_class_def_index; i++)
    {
        ClassDef current = dex_file_GetClassDefinitionByIndex(hdr, i);
        if(current.class_idx_ == original.superclass_idx_)
        {
            return i;
        }
    }
    return kDexNoIndex;
}
uint32_t dex_file_FindClassDefinitionIndicesByPredicate(const DexFileHeader *hdr,
                                                        CLASSDEF_PREDICATE p, void *args,
                                                        uint32_t *result, uint32_t maxResults)
{
    CHECK(hdr != NULL);
    CHECK(p != NULL);
    CHECK(result != NULL);

    if(maxResults == 0)
    {
        return 0;
    }

    uint32_t foundResults = 0;
    for(uint32_t i = 0; i < hdr->class_defs_size_; i++)
    {
        ClassDef current = dex_file_GetClassDefinitionByIndex(hdr, i);
        if(p(hdr, &current, args))
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
ClassDef dex_file_GetClassDefinitionByIndex(const DexFileHeader* hdr, uint32_t class_def_index)
{
    CHECK(hdr != NULL);
    CHECK(class_def_index < hdr->class_defs_size_);

    ClassDef* class_definitions = (void*)hdr + hdr->class_defs_off_;
    return class_definitions[class_def_index];
}
ProtoId dex_file_GetMethodPrototypeByIndex(const DexFileHeader* hdr, uint32_t proto_id)
{
    CHECK(hdr != NULL);
    CHECK(proto_id < hdr->proto_ids_size_);

    ProtoId* prototype_array = (void*)hdr + hdr->proto_ids_off_;
    return prototype_array[proto_id];
}
MethodId dex_file_GetMethodDescriptorByIndex(const DexFileHeader* hdr, uint32_t method_id)
{
    CHECK(hdr != NULL);
    CHECK(method_id < hdr->method_ids_size_);

    MethodId* methods = (void*)hdr + hdr->method_ids_off_;
    return methods[method_id];
}
uint32_t dex_file_FindMethodDescriptorIndicesByPredicate(const DexFileHeader* hdr, METHODID_PREDICATE p, void* args,
                                              uint32_t* result, uint32_t maxResults)
{
    CHECK(hdr != NULL);
    CHECK(p != NULL);
    CHECK(result != NULL);

    if(maxResults == 0)
    {
        return 0;
    }

    uint32_t foundResults = 0;
    for(uint32_t i = 0; i < hdr->method_ids_size_; i++)
    {
        MethodId current = dex_file_GetMethodDescriptorByIndex(hdr, i);
        if(p(hdr, &current, args))
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
TypeId dex_file_GetTypeDescriptorByIndex(const DexFileHeader* hdr, uint32_t type_id)
{
    CHECK(hdr != NULL);
    CHECK(type_id < hdr->type_ids_size_);

    TypeId* types = (void*)hdr + hdr->type_ids_off_;
    return types[type_id];
}
FieldId dex_file_GetFieldDescriptorByIndex(const DexFileHeader* hdr, uint32_t field_id)
{
    CHECK(hdr != NULL);
    CHECK(field_id < hdr->field_ids_size_);

    FieldId* fields = (void*)hdr + hdr->field_ids_off_;
    return fields[field_id];
}

