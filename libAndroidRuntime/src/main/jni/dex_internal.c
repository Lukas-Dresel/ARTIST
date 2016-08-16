/*
 * Copyright 2016 Lukas Dresel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
//
// Created by Lukas on 11/14/2015.
//

/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Modeled after
 *     https://android.googlesource.com/platform/art/+/android-6.0.0_r5/runtime/dex_file.cc
 *     https://android.googlesource.com/platform/art/+/android-6.0.0_r5/runtime/dex_file.h
 *     https://android.googlesource.com/platform/art/+/android-6.0.0_r5/runtime/dex_file-inl.h
 * This file is heavily borrowed from the above sources.
 * It was written to port the parsing of dex files to c code.
 */

#include <stdlib.h>
#include "dex_internal.h"
#include <utility/logging.h>
#include "android_utf.h"
#include "leb128.h"

#define CHECK_RETURNNOINDEX(x) CHECK_RETURN((x), (kDexNoIndex))
#define CHECK_RETURNNOINDEX16(x) CHECK_RETURN((x), (kDexNoIndex16))


const size_t kSha1DigestSize = 20;
const uint32_t kDexEndianConstant = 0x12345678;

// The value of an invalid index.
const uint32_t kDexNoIndex = 0xFFFFFFFF;

// The value of an invalid index.
const uint16_t kDexNoIndex16 = 0xFFFF;

// The separator character in MultiDex locations.
const char kMultiDexSeparator = ':';

const uint8_t kDexMagic[] = {'d', 'e', 'x', '\n'};
const uint8_t kDexMagicVersion[] = {'0', '3', '5', '\0'};

bool IsMagicValid(const uint8_t *magic)
{
    CHECK(magic != NULL);
    return (memcmp(magic, kDexMagic, sizeof(kDexMagic)) == 0);
}

bool IsVersionValid(const uint8_t *magic)
{
    CHECK(magic != NULL);
    const uint8_t *version = &magic[sizeof(kDexMagic)];
    return (memcmp(version, kDexMagicVersion, sizeof(kDexMagicVersion)) == 0);
}

uint32_t GetVersion(struct DexHeader *hdr)
{
    CHECK(hdr != NULL);
    const char *versionString = (const char *) &(hdr->magic_[sizeof(kDexMagic)]);
    return (uint32_t) atoi(versionString);
}

bool IsMultiDexLocation(const char *location)
{
    CHECK(location != NULL);
    return strrchr(location, kMultiDexSeparator) != NULL;
}

uint32_t GetInvalidIndex()
{
    return kDexNoIndex;
}

uint16_t GetInvalidIndex16()
{
    return kDexNoIndex16;
}

bool IsValidIndex16(uint16_t index)
{
    return index != kDexNoIndex16;
}

bool IsValidIndex(uint32_t index)
{
    return index != kDexNoIndex;
}


/* Dex MemoryMappedFile Comparators
 *
 * These are comparators for some of the structures found in dex files. They
 * compare them based on orderings inherent in dex file structures.
 */

/*
 * Returns value <0  if a < b
 * Returns value 0   if a == b
 * Returns value >0  if a > b
 */

int CompareTypeIDsByDexOrdering(const struct DexHeader *hdr, const struct TypeID *a,
                                const struct TypeID *b)
{
    CHECK(hdr != NULL);
    CHECK(a != NULL);
    CHECK(b != NULL);
    return a->descriptor_idx_ - b->descriptor_idx_;
}

int CompareStringIDsByDexOrdering(const struct DexHeader *hdr, const struct StringID *a,
                                  const struct StringID *b)
{
    CHECK(hdr != NULL);
    CHECK(a != NULL);
    CHECK(b != NULL);
    const char *str_a = GetStringData(hdr, a);
    const char *str_b = GetStringData(hdr, b);
    return CompareModifiedUtf8ToModifiedUtf8AsUtf16CodePointValues(str_a, str_b);
}

int CompareProtoIDsByDexOrdering(const struct DexHeader *hdr, const struct ProtoID *a,
                                 const struct ProtoID *b)
{
    CHECK(hdr != NULL);
    CHECK(a != NULL);
    CHECK(b != NULL);

    const struct TypeList no_params = {.size_ = 0};

    const struct TypeList *params_a = (a->parameters_off_ == 0) ? &no_params : GetProtoParameters(hdr, a);
    const struct TypeList *params_b = (b->parameters_off_ == 0) ? &no_params : GetProtoParameters(hdr, b);
    if (params_a->size_ < params_b->size_)
    {
        return -1;
    }
    else if (params_b->size_ > params_b->size_)
    {
        return 1;
    }
    for (int i = 0; i < params_a->size_; i++)
    {
        int currdif = params_a->list_[i].type_id - params_b->list_[i].type_id;
        if (currdif == 0)
        {
            continue;
        }
        return currdif;
    }
    return 0;
}

int CompareFieldIDsByDexOrdering(const struct DexHeader *hdr, const struct FieldID* a, const struct FieldID* b)
{
    CHECK(hdr != NULL);
    CHECK(a != NULL);
    CHECK(b != NULL);

    int class_idx_diff = a->class_idx_ - b->class_idx_;
    int name_idx_diff = a->name_idx_ - b->name_idx_;
    int type_idx_diff = a->type_idx_ - b->type_idx_;

    if(class_idx_diff != 0)
    {
        return class_idx_diff;
    }
    else if (name_idx_diff != 0)
    {
        return name_idx_diff;
    }
    else if (type_idx_diff != 0)
    {
        return type_idx_diff;
    }
    else
    {
        return 0;
    }
}

int CompareMethodIDsByDexOrdering(const struct DexHeader *hdr, const struct MethodID *a,
                                  const struct MethodID *b)
{
    CHECK(hdr != NULL);
    CHECK(a != NULL);
    CHECK(b != NULL);
    int class_idx_diff = a->class_idx_ - b->class_idx_;
    int name_idx_diff = a->name_idx_ - b->name_idx_;
    int proto_idx_diff = a->proto_idx_ - b->proto_idx_;

    if (class_idx_diff != 0)
    {
        return class_idx_diff;
    }
    else if (name_idx_diff != 0)
    {
        return name_idx_diff;
    }
    else if (proto_idx_diff != 0)
    {
        return proto_idx_diff;
    }
    else
    {
        return 0;
    }
}

/*
 * Dex MemoryMappedFile Utils
 */

void* DexOffsetToPointer(const struct DexHeader* hdr, uint32_t offset)
{
    CHECK_RETURNNULL(hdr != NULL);
    if(offset == 0)
    {
        return NULL;
    }
    return (void*)hdr + offset;
}

/* Dex MemoryMappedFile Structure Getters
 * Eventhough we might modify them, we return them as const, to make it clear that that is not
 * the standard use case. This is especially relevant with changes to structures that are variable
 * size encoded, e.g. uleb128 encoded values in the class_data_item structure
 */


const struct TypeID *GetTypeIDArray(const struct DexHeader *hdr)
{
    return DexOffsetToPointer(hdr, hdr->type_ids_off_);
}

const struct TypeID *GetTypeID(const struct DexHeader *hdr, uint16_t type_id_index)
{
    CHECK_RETURNNULL(hdr != NULL);
    if(type_id_index == kDexNoIndex16)
    {
        return NULL;
    }
    CHECK_RETURNNULL(type_id_index < hdr->type_ids_size_);
    struct TypeID *ids = DexOffsetToPointer(hdr, hdr->type_ids_off_);
    if(ids == NULL)
    {
        // The dex file does not have type ids, this could be legit
        return NULL;
    }
    return &ids[type_id_index];
}

uint16_t GetIndexForTypeID(const struct DexHeader *hdr, const struct TypeID *type_id)
{
    CHECK_RETURNNOINDEX16(hdr != NULL);
    CHECK_RETURNNOINDEX16(type_id != NULL);
    const struct TypeID *ids = GetTypeIDArray(hdr);
    if(ids == NULL)
    {
        return kDexNoIndex16;
    }
    CHECK_RETURNNOINDEX16(type_id >= ids);
    CHECK_RETURNNOINDEX16(type_id < &ids[hdr->type_ids_size_]);
    size_t result = type_id - ids;
    CHECK_RETURNNOINDEX16(result < 65536U);
    return (uint16_t) result;
}
const char* GetTypeIDName(const struct DexHeader* hdr, const struct TypeID* type_id)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(type_id != NULL);
    return GetStringDataByIdx(hdr, type_id->descriptor_idx_);
}
const char* GetTypeIDNameByIdx(const struct DexHeader* hdr, uint16_t type_idx)
{
    // Sanity checks done by GetTypeID
    const struct TypeID* type_id = GetTypeID(hdr, type_idx);
    return GetTypeIDName(hdr, type_id);
}

const struct StringID *GetStringIDArray(const struct DexHeader *hdr)
{
    CHECK_RETURNNULL(hdr != NULL);
    if(hdr->string_ids_off_ == 0)
    {
        return NULL;
    }
    return (void *) hdr + hdr->string_ids_off_;
}

const struct StringID *GetStringID(const struct DexHeader *hdr, uint32_t string_id_index)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(string_id_index < hdr->string_ids_size_);
    CHECK_RETURNNULL(hdr->string_ids_off_ != 0);
    struct StringID *ids = DexOffsetToPointer(hdr, hdr->string_ids_off_);
    return &ids[string_id_index];
}

uint32_t GetIndexForStringID(const struct DexHeader *hdr, const struct StringID *string_id)
{
    CHECK_RETURNNOINDEX(hdr != NULL);
    CHECK_RETURNNOINDEX(string_id != NULL);
    const struct StringID *ids = GetStringIDArray(hdr);
    if(ids == NULL)
    {
        return kDexNoIndex;
    }
    CHECK_RETURNNOINDEX(string_id >= ids);
    CHECK_RETURNNOINDEX(string_id < &ids[hdr->string_ids_size_]);
    return string_id - ids;
}


const struct ProtoID *GetProtoIDArray(const struct DexHeader *hdr)
{
    CHECK_RETURNNULL(hdr != NULL);
    if(hdr->proto_ids_off_ == 0)
    {
        return NULL;
    }
    return (void *) hdr + hdr->proto_ids_off_;
}

const struct ProtoID *GetProtoID(const struct DexHeader *hdr, uint16_t proto_id_index)
{
    CHECK_RETURNNULL(hdr != NULL);
    if(proto_id_index == kDexNoIndex16)
    {
        return NULL;
    }
    CHECK_RETURNNULL(proto_id_index < hdr->proto_ids_size_);
    const struct ProtoID *ids = GetProtoIDArray(hdr);
    return &ids[proto_id_index];
}

uint16_t GetIndexForProtoID(const struct DexHeader *hdr, const struct ProtoID *proto_id)
{
    CHECK_RETURNNOINDEX16(hdr != NULL);
    CHECK_RETURNNOINDEX16(proto_id != NULL);
    const struct ProtoID *ids = GetProtoIDArray(hdr);
    if(ids == NULL)
    {
        return kDexNoIndex16;
    }
    CHECK_RETURNNOINDEX16(proto_id >= ids);
    CHECK_RETURNNOINDEX16(proto_id < &ids[hdr->proto_ids_size_]);
    size_t result = proto_id - ids;
    CHECK_RETURNNOINDEX16(result < 65536U);
    return (uint16_t) result;
}

// Returns the short form method descriptor for the given prototype.
const char* GetProtoShortyByIndex(const struct DexHeader* hdr, uint16_t proto_idx)
{
    // Sanity checks done by GetProtoID
    const struct ProtoID* proto_id = GetProtoID(hdr, proto_idx);
    return GetStringDataByIdx(hdr, proto_id->shorty_idx_);
}

const struct TypeList* GetProtoParameters(const struct DexHeader* hdr, const struct ProtoID* proto_id)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(proto_id != NULL);

    return (const struct TypeList*)DexOffsetToPointer(hdr, proto_id->parameters_off_);
}

const struct MethodID *GetMethodIDArray(const struct DexHeader *hdr)
{
    CHECK_RETURNNULL(hdr != NULL);
    if(hdr->method_ids_off_ == 0)
    {
        return NULL;
    }
    return (void *) hdr + hdr->method_ids_off_;
}

const struct MethodID *GetMethodID(const struct DexHeader *hdr, uint32_t method_id_index)
{
    CHECK_RETURNNULL(hdr != NULL);
    if(method_id_index == kDexNoIndex)
    {
        return NULL;
    }
    CHECK_RETURNNULL(method_id_index < hdr->method_ids_size_);
    const struct MethodID *ids = GetMethodIDArray(hdr);
    return &ids[method_id_index];
}

uint32_t GetIndexForMethodID(const struct DexHeader *hdr, const struct MethodID *method_id)
{
    CHECK_RETURNNOINDEX(hdr != NULL);
    CHECK_RETURNNOINDEX(method_id != NULL);
    const struct MethodID *ids = GetMethodIDArray(hdr);
    if(ids == NULL)
    {
        return kDexNoIndex;
    }
    CHECK_RETURNNOINDEX(method_id >= ids);
    CHECK_RETURNNOINDEX(method_id < &ids[hdr->method_ids_size_]);
    return method_id - ids;
}
const char* GetMethodIDName(const struct DexHeader* hdr, const struct MethodID* method_id)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(method_id != NULL);
    return GetStringDataByIdx(hdr, method_id->name_idx_);
}
const char* GetMethodIDNameByIdx(const struct DexHeader* hdr, uint32_t method_idx)
{
    const struct MethodID* method_id = GetMethodID(hdr, method_idx);
    return GetMethodIDName(hdr, method_id);
}


const struct FieldID *GetFieldIDArray(const struct DexHeader *hdr)
{
    CHECK_RETURNNULL(hdr != NULL);
    if(hdr->field_ids_off_ == 0)
    {
        return NULL;
    }
    return (void *) hdr + hdr->field_ids_off_;
}

const struct FieldID *GetFieldID(const struct DexHeader *hdr, uint32_t field_id_index)
{
    CHECK_RETURNNULL(hdr != NULL);
    if(field_id_index == kDexNoIndex)
    {
        return NULL;
    }
    CHECK_RETURNNULL(field_id_index < hdr->field_ids_size_);
    const struct FieldID *ids = GetFieldIDArray(hdr);
    return &ids[field_id_index];
}

uint32_t GetIndexForFieldID(const struct DexHeader *hdr, const struct FieldID *field_id)
{
    CHECK_RETURNNOINDEX(hdr != NULL);
    CHECK_RETURNNOINDEX(field_id != NULL);
    const struct FieldID *ids = GetFieldIDArray(hdr);
    if(ids == NULL)
    {
        return kDexNoIndex;
    }
    CHECK_RETURNNOINDEX(field_id >= ids);
    CHECK_RETURNNOINDEX(field_id < &ids[hdr->field_ids_size_]);
    return field_id - ids;
}
const char* GetFieldIDName(const struct DexHeader* hdr, const struct FieldID* field_id)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(field_id != NULL);
    return GetStringDataByIdx(hdr, field_id->name_idx_);
}
const char* GetFieldIDNameByIdx(const struct DexHeader* hdr, uint32_t method_idx)
{
    const struct FieldID* field_id = GetFieldID(hdr, method_idx);
    return GetFieldIDName(hdr, field_id);
}



const struct ClassDef *GetClassDefArray(const struct DexHeader *hdr)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(hdr->class_defs_off_ != 0);
    return (void *) hdr + hdr->class_defs_off_;
}

const struct ClassDef *GetClassDef(const struct DexHeader *hdr, uint16_t class_def_index)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(class_def_index < hdr->class_defs_size_ );
    CHECK_RETURNNULL(hdr->class_defs_off_ != 0);
    struct ClassDef *defs = (void *) hdr + hdr->class_defs_off_;
    return &defs[class_def_index];
}
uint16_t GetIndexForClassDef(const struct DexHeader *hdr, const struct ClassDef *class_def)
{
    CHECK_RETURNNOINDEX16(hdr != NULL);
    CHECK_RETURNNOINDEX16(class_def != NULL);
    const struct ClassDef *defs = GetClassDefArray(hdr);
    CHECK_RETURNNOINDEX16(defs != NULL);
    CHECK_RETURNNOINDEX16(class_def >= defs);
    CHECK_RETURNNOINDEX16(class_def < &defs[hdr->class_defs_size_]);
    size_t result = class_def - defs;
    CHECK_RETURNNOINDEX16(result < 65536U);
    return (uint16_t) result;
}
const char* GetClassDefName(const struct DexHeader* hdr, const struct ClassDef* c)
{
    if(hdr == NULL || c == NULL)
    {
        return NULL;
    }

    const struct TypeID* type_id = GetTypeID(hdr, c->class_idx_);
    if(NULL == type_id)
    {
        return NULL;
    }
    return GetStringDataByIdx(hdr, type_id->descriptor_idx_);
}

const char* GetClassDefNameByIndex(const struct DexHeader* hdr, uint16_t index)
{
    if(hdr == NULL || !IsValidIndex16(index) || index >= hdr->class_defs_size_)
    {
        return NULL;
    }
    const struct ClassDef* c = GetClassDef(hdr, index);
    return GetClassDefName(hdr, c);
}


int32_t GetStringLength(struct DexHeader *hdr, const struct StringID *string_id)
{
    CHECK(hdr != NULL);
    CHECK(string_id != NULL);
    const uint8_t *ptr = (void *) hdr + string_id->string_data_off_;
    return DecodeUnsignedLeb128(&ptr);
}

const char *GetStringDataAndUtf16Length(const struct DexHeader *hdr,
                                               const struct StringID *string_id,
                                               uint32_t *utf16_length)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(string_id != NULL);
    CHECK_RETURNNULL(utf16_length != NULL);
    const uint8_t *ptr = (void *) hdr + string_id->string_data_off_;
    *utf16_length = DecodeUnsignedLeb128(&ptr);
    return (const char *) (ptr);
}

const char *GetStringData(const struct DexHeader *hdr, const struct StringID *string_id)
{
    uint32_t ignored;
    return GetStringDataAndUtf16Length(hdr, string_id, &ignored);
}
// Index version of GetStringDataAndUtf16Length.
const char *GetStringDataAndUtf16LengthByIdx(const struct DexHeader *hdr, uint32_t idx, uint32_t *utf16_length)
{
    if (idx == kDexNoIndex)
    {
        *utf16_length = 0;
        return NULL;
    }
    const struct StringID *string_id = GetStringID(hdr, idx);
    return GetStringDataAndUtf16Length(hdr, string_id, utf16_length);
}

const char *GetStringDataByIdx(const struct DexHeader *hdr, uint32_t idx)
{
    CHECK_RETURNNULL(hdr != NULL);

    uint32_t unicode_length;
    return GetStringDataAndUtf16LengthByIdx(hdr, idx, &unicode_length);
}


const struct StringID *FindStringIDByModifiedUTF8StringValue(const struct DexHeader *hdr,
                                                             const char *mutf8_string)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(mutf8_string != NULL);

    int32_t lo = 0;
    int32_t hi = hdr->string_ids_size_ - 1;

    while (hi >= lo)
    {
        int32_t mid = (hi + lo) / 2;
        const struct StringID *string_id = GetStringID(hdr, mid);
        const char *str = GetStringData(hdr, string_id);
        int compare = CompareModifiedUtf8ToModifiedUtf8AsUtf16CodePointValues(mutf8_string, str);
        if (compare > 0)
        {
            lo = mid + 1;
        }
        else if (compare < 0)
        {
            hi = mid - 1;
        }
        else
        {
            return string_id;
        }
    }
    return NULL;
}

const struct StringID *FindStringIDByUTF16Value(const struct DexHeader *hdr,
                                                const uint16_t *string,
                                                size_t length)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(string != NULL);
    int32_t lo = 0;
    int32_t hi = hdr->string_ids_size_ - 1;
    while (hi >= lo)
    {
        int32_t mid = (hi + lo) / 2;
        const struct StringID *str_id = GetStringID(hdr, mid);
        const char *str = GetStringData(hdr, str_id);
        int compare = CompareModifiedUtf8ToUtf16AsCodePointValues(str, string, length);
        if (compare > 0)
        {
            lo = mid + 1;
        }
        else if (compare < 0)
        {
            hi = mid - 1;
        }
        else
        {
            return str_id;
        }
    }
    return NULL;
}

const struct TypeID *FindTypeIDByStringIndex(const struct DexHeader *hdr, uint32_t string_index)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(string_index < hdr->string_ids_size_);

    int32_t lo = 0;
    int32_t hi = hdr->type_ids_size_ - 1;
    while (hi >= lo)
    {
        int32_t mid = (hi + lo) / 2;
        const struct TypeID *type_id = GetTypeID(hdr, mid);
        if (string_index > type_id->descriptor_idx_)
        {
            lo = mid + 1;
        }
        else if (string_index < type_id->descriptor_idx_)
        {
            hi = mid - 1;
        }
        else
        {
            return type_id;
        }
    }
    return NULL;
}

static int CompareProtoParams(uint32_t sig_type_length, const uint16_t *signature_type_idxs,const struct TypeList* proto_params)
{
    // Sanity checks
    CHECK(signature_type_idxs != NULL);
    CHECK(proto_params != NULL);

    LOGD("Comparing proto params of length %d", sig_type_length);
    int compare = 0;
    for (int i = 0; (compare == 0) && (i < proto_params->size_) && (i < sig_type_length); i++)
    {
        compare = signature_type_idxs[i] - proto_params->list_[i].type_id;
    }
    if(compare != 0)
    {
        // If the parameters didn't match, exit
        return compare;
    }
    // Caution: Always make sure that this length check only occurs AFTER the type comparison.
    // The sorting on differing lists returns the difference of the first non-matching types as
    // comparator-value. The lengths are only considered if one list is a sublist of the other
    if (sig_type_length < proto_params->size_)
    {
        compare = -1;
    }
    else if (sig_type_length > proto_params->size_)
    {
        compare = 1;
    }
    return compare;
}
const struct ProtoID *FindProtoID(const struct DexHeader *hdr, uint16_t ret_type_idx,
                                  const uint16_t *signature_type_idxs, uint32_t sig_type_length)
{
    CHECK_RETURNNULL(hdr != NULL);
    // If we are given a nonzero-length we need the array pointer to not be null.
    CHECK_RETURNNULL(sig_type_length == 0 || signature_type_idxs != NULL);

    LOGD("Looking up proto id with return_type_idx %d", ret_type_idx);


    struct TypeList no_params = { .size_ = 0 };

    int32_t lo = 0;
    int32_t hi = hdr->proto_ids_size_ - 1;
    while (hi >= lo)
    {
        int32_t mid = (hi + lo) / 2;
        const struct ProtoID *proto_id = GetProtoID(hdr, mid);
        CHECK(proto_id != NULL);


        const struct TypeList *proto_params = GetProtoParameters(hdr, proto_id);
        if(proto_params == NULL)
        {
            if(proto_id->parameters_off_ != 0)
            {
                LOGE("Could not get prototype parameters for prototype #%d", mid);
                return NULL;
            }
            // If we simply have no parameters we use an empty list
            proto_params = &no_params;
        }
        int compare = ret_type_idx - proto_id->return_type_idx_;
        if (compare == 0)
        {
            compare = CompareProtoParams(sig_type_length, signature_type_idxs, proto_params);
            if (compare > 0)
            {
                lo = mid + 1;
            }
            else if (compare < 0)
            {
                hi = mid - 1;
            }
            else
            {
                LOGD("Found prototype.");
                return proto_id;
            }
        }
        else if (compare > 0)
        {
            lo = mid + 1;
        }
        else
        {
            hi = mid - 1;
        }
    }
    LOGD("Did not find prototype.");
    return NULL;
}


const struct ClassDef *FindClassDefByDescriptor(const struct DexHeader *hdr, const char *mutf8_descriptor)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(mutf8_descriptor != NULL);
    uint32_t num_class_defs = hdr->class_defs_size_;
    if (num_class_defs == 0)
    {
        return NULL;
    }
    const struct StringID *string_id = FindStringIDByModifiedUTF8StringValue(hdr, mutf8_descriptor);
    if (string_id != NULL)
    {
        uint32_t string_index = GetIndexForStringID(hdr, string_id);
        const struct TypeID *type_id = FindTypeIDByStringIndex(hdr, string_index);
        if (type_id != NULL)
        {
            uint16_t type_index = GetIndexForTypeID(hdr, type_id);
            const struct ClassDef *class_def = FindClassDefByTypeIndex(hdr, type_index);
            if (class_def != NULL)
            {
                return class_def;
            }
        }
    }
    return NULL;

}

const struct ClassDef *FindClassDefByTypeIndex(const struct DexHeader *hdr, uint16_t type_index)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(type_index < hdr->type_ids_size_);

    size_t num_class_defs = hdr->class_defs_size_;
    for (uint16_t i = 0; i < num_class_defs; i++)
    {
        const struct ClassDef *class_def = GetClassDef(hdr, i);
        if (class_def->class_idx_ == type_index)
        {
            return class_def;
        }
    }
    return NULL;
}

const struct ClassDef *FindSuperClassDef(const struct DexHeader *hdr,
                                         const struct ClassDef *class_def)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(class_def != NULL);
    return FindClassDefByTypeIndex(hdr, class_def->superclass_idx_);
}

const struct FieldID *FindFieldID(const struct DexHeader *hdr,
                                  const struct TypeID *declaring_klass,
                                  const struct StringID *name,
                                  const struct TypeID *type)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(declaring_klass != NULL);
    CHECK_RETURNNULL(name != NULL);
    CHECK_RETURNNULL(type != NULL);

    // Binary search MethodIds knowing that they are sorted by class_idx, name_idx then proto_idx
    const uint16_t class_idx = GetIndexForTypeID(hdr, declaring_klass);
    const uint32_t name_idx = GetIndexForStringID(hdr, name);
    const uint16_t type_idx = GetIndexForTypeID(hdr, type);
    int32_t lo = 0;
    int32_t hi = hdr->field_ids_size_ - 1;
    while (hi >= lo)
    {
        int32_t mid = (hi + lo) / 2;
        const struct FieldID *field = GetFieldID(hdr, mid);
        if (class_idx > field->class_idx_)
        {
            lo = mid + 1;
        }
        else if (class_idx < field->class_idx_)
        {
            hi = mid - 1;
        }
        else
        {
            if (name_idx > field->name_idx_)
            {
                lo = mid + 1;
            }
            else if (name_idx < field->name_idx_)
            {
                hi = mid - 1;
            }
            else
            {
                if (type_idx > field->type_idx_)
                {
                    lo = mid + 1;
                }
                else if (type_idx < field->type_idx_)
                {
                    hi = mid - 1;
                }
                else
                {
                    return field;
                }
            }
        }
    }
    return NULL;
}

const struct MethodID *FindMethodID(const struct DexHeader *hdr,
                                    const struct TypeID *declaring_klass,
                                    const struct StringID *name,
                                    const struct ProtoID *signature)
{
    CHECK_RETURNNULL(hdr);

    CHECK_RETURNNULL(declaring_klass != NULL);
    CHECK_RETURNNULL(name != NULL);
    CHECK_RETURNNULL(signature != NULL);

    const uint16_t class_idx = GetIndexForTypeID(hdr, declaring_klass);
    const uint32_t name_idx = GetIndexForStringID(hdr, name);
    const uint16_t proto_idx = GetIndexForProtoID(hdr, signature);

    if (UNLIKELY(class_idx == kDexNoIndex16 ||
                 name_idx == kDexNoIndex16 ||
                 proto_idx == kDexNoIndex16))
    {
        return NULL;
    }
    int32_t lo = 0;
    int32_t hi = hdr->method_ids_size_ - 1;
    int compare = 0;
    while (hi >= lo)
    {
        int32_t mid = (hi + lo) / 2;
        const struct MethodID *method = GetMethodID(hdr, mid);
        if (class_idx > method->class_idx_)
        {
            compare = 1;
        }
        else if (class_idx < method->class_idx_)
        {
            compare = -1;
        }
        else
        {
            if (name_idx > method->name_idx_)
            {
                compare = 1;
            }
            else if (name_idx < method->name_idx_)
            {
                compare = -1;
            }
            else
            {
                if (proto_idx > method->proto_idx_)
                {
                    compare = 1;
                }
                else if (proto_idx < method->proto_idx_)
                {
                    compare = -1;
                }
                else
                {
                    return method;
                }
            }
        }
        if (compare > 0)
        {
            lo = mid + 1;
        }
        else
        {
            hi = mid - 1;
        }
    }
    return NULL;
}

bool CreateTypeListFromStringSignature( const struct DexHeader *hdr,
                                        const char *mutf8_signature,
                                        uint16_t *return_type_idx,
                                        uint16_t *param_type_idxs,
                                        uint32_t max_num_writable_parameters,
                                        uint32_t *num_written_parameters )
{
    CHECK_RETURNFALSE(hdr != NULL);

    CHECK_RETURNFALSE(mutf8_signature != NULL);
    size_t sig_length = strlen(mutf8_signature);

    CHECK_RETURNFALSE(return_type_idx != NULL);
    CHECK_RETURNFALSE(param_type_idxs != NULL);
    CHECK_RETURNFALSE(num_written_parameters != NULL);
    LOGD("Creating type list from signature %s", mutf8_signature);

    if (mutf8_signature[0] != '(')
    {
        return false;
    }

    *num_written_parameters = 0;

    size_t offset = 1;
    size_t end = sig_length;
    bool process_return = false;
    while (offset < end)
    {
        size_t start_offset = offset;
        char c = mutf8_signature[offset];
        offset++;
        if (c == ')')
        {
            process_return = true;
            continue;
        }
        while (c == '[')
        {  // process array prefix
            if (offset >= end)
            {  // expect some descriptor following [
                return false;
            }
            c = mutf8_signature[offset];
            offset++;
        }
        if (c == 'L')
        {  // process type descriptors
            do
            {
                if (offset >= end)
                {  // unexpected early termination of descriptor
                    return false;
                }
                c = mutf8_signature[offset];
                offset++;
            } while (c != ';');
        }

        char descriptor[offset - start_offset + 1];
        strncpy(descriptor, mutf8_signature + start_offset, offset - start_offset);
        descriptor[offset - start_offset] = 0;
        const struct StringID *string_id = FindStringIDByModifiedUTF8StringValue(hdr, descriptor);
        if (string_id == NULL)
        {
            return false;
        }
        const struct TypeID *type_id = FindTypeIDByStringIndex(hdr,
                                                               GetIndexForStringID(hdr, string_id));
        if (type_id == NULL)
        {
            return false;
        }
        uint16_t type_idx = GetIndexForTypeID(hdr, type_id);
        if (!process_return)
        {
            if (*num_written_parameters > max_num_writable_parameters)
            {
                LOGF("The number of found parameters exceeded the passed maximum. TypeList parsing failed.");
                LOGF("If you want to be on the safe side, have the param_type_idxs array be mutf8_signature->length long.");
                LOGF("This might seem wasteful, but should succeed in all cases.");
                return false;
            }
            param_type_idxs[(*num_written_parameters)] = type_idx;
            *num_written_parameters += 1;
        }
        else
        {
            *return_type_idx = type_idx;
            return offset == end;  // return true if the mutf8_signature had reached a sensible end
        }
    }
    return false;  // failed to correctly parse return type
}

const struct ProtoID *FindProtoIDBySignatureString(const struct DexHeader *hdr,
                                                   const char *signature)
{
    CHECK_RETURNNULL(hdr != NULL);
    CHECK_RETURNNULL(signature != NULL);
    size_t sig_length = strlen(signature);
    uint16_t return_type_index;
    uint16_t param_type_indices[sig_length + 10 - 1];
    uint32_t num_params;

    bool success = CreateTypeListFromStringSignature(hdr,
                                                     signature,
                                                     &return_type_index,
                                                     &param_type_indices[0],
                                                     sig_length - 1,
                                                     &num_params);
    if (!success)
    {
        LOGD("Could not create type list from prototype.");
        return false;
    }
    LOGD("Created type list from prototype %s", signature);
    LOGD("Number of found parameters: %d", num_params);
    const struct ProtoID *proto_id = FindProtoID(hdr, return_type_index, &param_type_indices[0],
                                                 num_params);
    // no need to check for null, as that is exactly what we want to return if it occurs.
    return proto_id;

}

const struct DecodedMethod *DecodeEncodedMethod(const struct DexHeader *hdr,
                                                struct DecodedMethod *result, const uint8_t **data,
                                                uint32_t *prev_method_idx)
{
    CHECK(hdr != NULL);
    CHECK(result != NULL);
    CHECK(data != NULL);
    result->backing_memory_address = (void *) *data;
    result->method_idx_diff = DecodeUnsignedLeb128(data);
    *prev_method_idx += result->method_idx_diff;
    result->method_idx = *prev_method_idx;
    result->access_flags = DecodeUnsignedLeb128(data);
    result->code_off = DecodeUnsignedLeb128(data);
    result->backing_memory_size = (void *) (*data) - result->backing_memory_address;
    return result;
}

const struct DecodedField *DecodeEncodedField(const struct DexHeader *hdr,
                                              struct DecodedField *result, const uint8_t **data,
                                              uint32_t *prev_field_idx)
{
    CHECK(hdr != NULL);
    CHECK(result != NULL);
    CHECK(data != NULL);
    result->backing_memory_address = (void *) *data;
    result->field_idx_diff = DecodeUnsignedLeb128(data);
    *prev_field_idx += result->field_idx_diff;
    result->field_idx = *prev_field_idx;
    result->access_flags = DecodeUnsignedLeb128(data);
    result->backing_memory_size = (void *) (*data) - result->backing_memory_address;
    return result;
}

const struct DecodedClassData *DecodeEncodedClassDataItem(const struct DexHeader *hdr,
                                                              struct DecodedClassData *result,
                                                              const uint8_t **data)
{
    CHECK(hdr != NULL);
    CHECK(result != NULL);
    CHECK(data != NULL);

    result->backing_memory_address = (void *) *data;

    result->static_fields_size = DecodeUnsignedLeb128(data);
    result->instance_fields_size = DecodeUnsignedLeb128(data);
    result->direct_methods_size = DecodeUnsignedLeb128(data);
    result->virtual_methods_size = DecodeUnsignedLeb128(data);

    struct DecodedField ignored_field;
    struct DecodedMethod ignored_method;

    result->static_fields_array = (void *) *data;
    for (uint32_t i = 0, field_idx = 0; i < result->static_fields_size; i++)
    {
        DecodeEncodedField(hdr, &ignored_field, data, &field_idx);
    }

    result->instance_fields_array = (void *) *data;
    for (uint32_t i = 0, field_idx = 0; i < result->instance_fields_size; i++)
    {
        DecodeEncodedField(hdr, &ignored_field, data, &field_idx);
    }

    result->direct_methods_array = (void *) *data;
    for (uint32_t i = 0, method_idx = 0; i < result->direct_methods_size; i++)
    {
        DecodeEncodedMethod(hdr, &ignored_method, data, &method_idx);
    }

    result->virtual_methods_array = (void *) *data;
    for (uint32_t i = 0, method_idx = 0; i < result->virtual_methods_size; i++)
    {
        DecodeEncodedMethod(hdr, &ignored_method, data, &method_idx);
    }

    result->backing_memory_size = (void *) (*data) - result->backing_memory_address;
    return result;
}