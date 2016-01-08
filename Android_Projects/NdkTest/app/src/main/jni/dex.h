//
// Created by Lukas on 8/20/2015.
//

#ifndef NDKTEST_DEX_H
#define NDKTEST_DEX_H

#include "dex_internal.h"

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
          struct DecodedMethod      decoded_method_data;
};

struct DexField
{
    const struct DexHeader *        dex_header;
    const struct DexClass *         containing_class;
    const struct FieldID *          field_id;
          struct DecodedField       decoded_field_data;
};

bool dex_FindClass(const struct DexHeader *hdr, struct DexClass *result, char *descriptor);

bool dex_FindVirtualMethod(const struct DexClass *clazz, struct DexMethod *result,
                           const char *descriptor, const char *signature);

bool dex_FindDirectMethod(const struct DexClass *clazz, struct DexMethod *result,
                          const char *descriptor, const char *signature);

#endif //NDKTEST_DEX_FILE_H
