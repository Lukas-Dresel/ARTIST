//
// Created by Lukas on 8/20/2015.
//

#ifndef NDKTEST_DEX_H
#define NDKTEST_DEX_H

#include "dex_internal.h"

/*
 * Dex Type index sizes:
 *
 * StringId -   uint32_t
 * MethodId -   uint32_t
 * FieldId  -   uint32_t
 *
 * TypeId   -   uint16_t
 * ProtoId  -   uint16_t
 * ClassDef -   uint16_t
 *
 */

struct DexClass
{
    const struct DexHeader *        dex_header;
    const struct ClassDef *         class_def;
          struct DecodedClassData   decoded_class_data;
};

struct DexMethod
{
    const struct DexHeader *        dex_header;
    const struct DexClass *         containing_class;
    const struct MethodID *         method_id;
                 uint32_t           class_method_idx;
          struct DecodedMethod      decoded_method_data;
};

struct DexField
{
    const struct DexHeader *        dex_header;
    const struct DexClass *         containing_class;
    const struct FieldID *          field_id;
                 uint32_t           class_field_idx;
          struct DecodedField       decoded_field_data;
};

uint32_t dex_NumberOfStrings    ( const struct DexHeader* hdr );
uint32_t dex_NumberOfMethods    ( const struct DexHeader* hdr );
uint32_t dex_NumberOfFields     ( const struct DexHeader* hdr );
uint32_t dex_NumberOfClassDefs  ( const struct DexHeader* hdr );

bool dex_FindClass(const struct DexHeader *hdr, struct DexClass *result, char *descriptor);
bool dex_GetClass(const struct DexHeader* hdr, struct DexClass* result, uint16_t index);

bool dex_FindVirtualMethod(const struct DexClass *clazz, struct DexMethod *result, const char *mutf8_descriptor, const char *mutf8_Signature);
bool dex_FindDirectMethod(const struct DexClass *clazz, struct DexMethod *result, const char *descriptor, const char *signature);

#endif //NDKTEST_DEX_FILE_H