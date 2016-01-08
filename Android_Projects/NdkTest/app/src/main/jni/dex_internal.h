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

#include <stdbool.h>
#include <stdint.h>

#ifndef NDKTEST_DEX_INTERNAL_H
#define NDKTEST_DEX_INTERNAL_H

struct DexHeader
{
    uint8_t     magic_[8];
    uint32_t    checksum_;         // See also location_checksum_
    uint8_t     signature_[20];    // [kSha1DigestSize]
    uint32_t    file_size_;        // size of entire file
    uint32_t    header_size_;      // offset to start of next section
    uint32_t    endian_tag_;
    uint32_t    link_size_;        // unused
    uint32_t    link_off_;         // unused
    uint32_t    map_off_;          // unused
    uint32_t    string_ids_size_;  // number of StringIds
    uint32_t    string_ids_off_;   // file offset of StringIds array
    uint32_t    type_ids_size_;    // number of TypeIds, we don't support more than 65535
    uint32_t    type_ids_off_;     // file offset of TypeIds array
    uint32_t    proto_ids_size_;   // number of ProtoIds, we don't support more than 65535
    uint32_t    proto_ids_off_;    // file offset of ProtoIds array
    uint32_t    field_ids_size_;   // number of FieldIds
    uint32_t    field_ids_off_;    // file offset of FieldIds array
    uint32_t    method_ids_size_;  // number of MethodIds
    uint32_t    method_ids_off_;   // file offset of MethodIds array
    uint32_t    class_defs_size_;  // number of ClassDefs
    uint32_t    class_defs_off_;   // file offset of DexClassDef array
    uint32_t    data_size_;        // unused
    uint32_t    data_off_;         // unused
};

// Raw class_def_item.
struct ClassDef
{
    uint16_t class_idx_;  // index into type_ids_ array for this class
    uint16_t pad1_;  // padding = 0
    uint32_t access_flags_;
    uint16_t superclass_idx_;  // index into type_ids_ array for superclass
    uint16_t pad2_;  // padding = 0
    uint32_t interfaces_off_;  // file offset to TypeList
    uint32_t source_file_idx_;  // index into string_ids_ for source file name
    uint32_t annotations_off_;  // file offset to annotations_directory_item
    uint32_t class_data_off_;  // file offset to class_data_item
    uint32_t static_values_off_;  // file offset to EncodedArray
};

// Raw string_id_item.
struct StringID
{
    uint32_t string_data_off_;  // offset in bytes from the base address
};

// Raw method_id_item.
struct MethodID
{
    uint16_t class_idx_;  // index into type_ids_ array for defining class
    uint16_t proto_idx_;  // index into proto_ids_ array for method prototype
    uint32_t name_idx_;  // index into string_ids_ array for method name
};

// Raw proto_id_item.
struct ProtoID
{
    uint32_t shorty_idx_;       // index into string_ids array for shorty descriptor
    uint16_t return_type_idx_;  // index into type_ids array for return type
    uint16_t pad_;             // padding = 0
    uint32_t parameters_off_;  // file offset to type_list for parameter types
};

// Raw type_id_item.
struct TypeID
{
    uint32_t descriptor_idx_;  // index into string_ids
} ;

// Raw field_id_item.
struct FieldID
{
    uint16_t class_idx_;  // index into type_ids_ array for defining class
    uint16_t type_idx_;  // index into type_ids_ array for field type
    uint32_t name_idx_;  // index into string_ids_ array for field name
};

struct TypeItem
{
    uint16_t type_id;
};
struct TypeList
{
    uint32_t        size_;
    struct TypeItem list_[1];
};
// Raw code_item.
struct CodeItem
{
    uint16_t registers_size_;
    uint16_t ins_size_;
    uint16_t outs_size_;
    uint16_t tries_size_;
    uint32_t debug_info_off_;  // file offset to debug info stream
    uint32_t insns_size_in_code_units_;  // size of the insns array, in 2 byte code units
    uint16_t insns_[1];
};

struct DecodedField
{
    void*       backing_memory_address;
    size_t      backing_memory_size;

    uint32_t    field_idx_diff;
    uint32_t    field_idx;
    uint32_t    access_flags;
};
struct DecodedMethod
{
    void*       backing_memory_address;
    size_t      backing_memory_size;

    uint32_t    method_idx_diff;
    uint32_t    method_idx;
    uint32_t    access_flags;
    uint32_t    code_off;
};
struct DecodedClassData
{
    void*       backing_memory_address;
    size_t      backing_memory_size;

    uint32_t    static_fields_size;
    uint32_t    instance_fields_size;
    uint32_t    direct_methods_size;
    uint32_t    virtual_methods_size;

    void*       static_fields_array;
    void*       instance_fields_array;
    void*       direct_methods_array;
    void*       virtual_methods_array;
};




bool        IsMagicValid(const uint8_t *magic);
bool        IsVersionValid(const uint8_t *magic);
uint32_t    GetVersion(struct DexHeader *hdr);

bool        IsMultiDexLocation(const char *location);

uint32_t    GetInvalidIndex();
uint16_t    GetInvalidIndex16();
bool        IsValidIndex16(uint16_t index);
bool        IsValidIndex(uint32_t index);


/* Dex File Comparators
 *
 * These are comparators for some of the structures found in dex files. They
 * compare them based on orderings inherent in dex file structures.
 *
 *
 * int (*)(const struct DexHeader* hdr, const T* a, const T* b)
 * {
 *     Returns value <0  if a < b
 *     Returns value 0   if a == b
 *     Returns value >0  if a > b
 * }
 *
 */

int CompareTypeIDsByDexOrdering(const struct DexHeader* hdr, const struct TypeID* a, const struct TypeID* b);
int CompareStringIDsByDexOrdering(const struct DexHeader* hdr, const struct StringID* a, const struct StringID* b);
int CompareProtoIDsByDexOrdering(const struct DexHeader* hdr, const struct ProtoID* a, const struct ProtoID* b);
int CompareMethodIDsByDexOrdering(const struct DexHeader* hdr, const struct MethodID* a, const struct MethodID* b);


/* Dex File Structure Getters
 * Eventhough we might modify them, we return them as const, to make it clear that that is not
 * the standard use case. This is especially relevant with changes to structures that are variable
 * size encoded, e.g. uleb128 encoded values in the class_data_item structure
 */


inline const struct TypeID *    GetTypeIDArray(const struct DexHeader *hdr);
const struct TypeID *           GetTypeID(const struct DexHeader *hdr, uint32_t type_id_index);
uint16_t                        GetIndexForTypeID(const struct DexHeader *hdr, const struct TypeID *type_id);


inline const struct StringID *  GetStringIDArray(const struct DexHeader *hdr);
const struct StringID *         GetStringID(const struct DexHeader *hdr, uint32_t string_id_index);
uint32_t                        GetIndexForStringID(const struct DexHeader *hdr, const struct StringID *string_id);


inline const struct ProtoID *   GetProtoIDArray(const struct DexHeader *hdr);
const struct ProtoID *          GetProtoID(const struct DexHeader *hdr, uint16_t proto_id_index);
uint16_t                        GetIndexForProtoID(const struct DexHeader *hdr, const struct ProtoID *proto_id);
const char*                     GetProtoShortyByIndex(const struct DexHeader* hdr, uint32_t proto_idx);
const struct TypeList*          GetProtoParameters(const struct DexHeader* hdr, const struct ProtoID* proto_id);

inline const struct MethodID *  GetMethodIDArray(const struct DexHeader *hdr);
const struct MethodID *         GetMethodID(const struct DexHeader *hdr, uint32_t method_id_index);
uint32_t                        GetIndexForMethodID(const struct DexHeader *hdr, const struct MethodID *method_id);


inline const struct FieldID *   GetFieldIDArray(const struct DexHeader *hdr);
const struct FieldID *          GetFieldID(const struct DexHeader *hdr, uint32_t field_id_index);
uint32_t                        GetIndexForFieldID(const struct DexHeader *hdr, const struct FieldID *field_id);


inline const struct ClassDef *  GetClassDefArray(const struct DexHeader *hdr);
const struct ClassDef *         GetClassDef(const struct DexHeader *hdr, uint16_t class_def_index);
const char*                     GetClassDefName(const struct DexHeader* hdr, const struct ClassDef* c);
uint16_t                        GetIndexForClassDef(const struct DexHeader *hdr, const struct ClassDef *class_def);

inline int32_t                  GetStringLength(struct DexHeader *hdr, const struct StringID *string_id);
inline const char *             GetStringDataAndUtf16Length(const struct DexHeader *hdr, const struct StringID *string_id, uint32_t *utf16_length);
const char *                    GetStringData(const struct DexHeader *hdr, const struct StringID *string_id);
const char *                    StringDataAndUtf16LengthByIdx(const struct DexHeader *hdr, uint32_t idx, uint32_t *utf16_length);
const char *                    StringDataByIdx(const struct DexHeader *hdr, uint32_t idx);


const struct StringID *         FindStringIDByModifiedUTF8StringValue(const struct DexHeader *hdr, const char *string);
const struct StringID *         FindStringIDByUTF16Value(const struct DexHeader *hdr, const uint16_t *s, size_t length);

const struct TypeID *           FindTypeIDByStringIndex(const struct DexHeader *hdr, uint32_t string_index);

bool                            CreateTypeListFromStringSignature(const struct DexHeader *hdr,
                                                                  const struct String *signature,
                                                                  uint16_t *return_type_idx, uint16_t *
                                                                  param_type_idxs,
                                                                  uint32_t max_num_writable_parameters,
                                                                  uint32_t *num_written_parameters);

const struct ProtoID*           FindProtoIDBySignatureString(const struct DexHeader* hdr, const char* signature);

const struct ClassDef *         FindClassDefByTypeIndex(const struct DexHeader *hdr, uint16_t type_index);
const struct ClassDef *         FindClassDefByDescriptor(const struct DexHeader* hdr, const char* descriptor);
const struct ClassDef *         FindSuperClassDef(const struct DexHeader* hdr, const struct ClassDef* class_def);

const struct MethodID *         FindMethodID(const struct DexHeader *hdr, const struct TypeID *declaring_klass, const struct StringID *name, const struct ProtoID *signature);

const struct DecodedMethod*     DecodeEncodedMethod(const struct DexHeader* hdr, struct DecodedMethod* result, const uint8_t** data, uint32_t* prev_method_idx);
const struct DecodedField*      DecodeEncodedField(const struct DexHeader* hdr, struct DecodedField* result, const uint8_t** data, uint32_t* prev_field_idx);
const struct DecodedClassData*  DecodeEncodedClassDataItem(const struct DexHeader* hdr, struct DecodedClassData * result, const uint8_t** data);


#endif //NDKTEST_DEX_1_H
