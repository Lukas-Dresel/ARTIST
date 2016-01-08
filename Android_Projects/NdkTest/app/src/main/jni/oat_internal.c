//
// Created by Lukas on 11/17/2015.
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
 *     https://android.googlesource.com/platform/art/+/android-6.0.0_r5/runtime/oat.cc
 *     https://android.googlesource.com/platform/art/+/android-6.0.0_r5/runtime/oat.h
 *     https://android.googlesource.com/platform/art/+/android-6.0.0_r5/runtime/oat_file.cc
 *     https://android.googlesource.com/platform/art/+/android-6.0.0_r5/runtime/oat_file.h
 * This file is heavily borrowed from the above sources.
 * It was written to port the parsing of oat files to c code.
 */

#include "oat_internal.h"
#include "logging.h"
#include "system_info.h"
#include "dex_internal.h"
#include "bit_vector_util.h"
#include "abi_interface.h"

static const uint8_t        kOatMagic[] = { 'o', 'a', 't', '\n' };
static const uint8_t        kOatVersion[] = { '0', '6', '4', '\0' };
static const const char*    kImageLocationKey = "image-location";
static const const char*    kDex2OatCmdLineKey = "dex2oat-cmdline";
static const const char*    kDex2OatHostKey = "dex2oat-host";
static const const char*    kPicKey = "pic";
static const const char*    kDebuggableKey = "debuggable";
static const const char*    kClassPathKey = "classpath";
static const const char     kTrueValue[] = "true";
static const const char     kFalseValue[] = "false";

#define CASE_ENUM_REPR(x) case (x): \
                              return #x

char *GetOatClassTypeRepresentation(uint16_t t)
{
    switch (t)
    {
        CASE_ENUM_REPR(kOatClassAllCompiled);
        CASE_ENUM_REPR(kOatClassSomeCompiled);
        CASE_ENUM_REPR(kOatClassNoneCompiled);
        CASE_ENUM_REPR(kOatClassMax);
        default:
        {
            LOGF("Unknown OatClassType encountered: %hu", t);
            return "UNKNOWN OatClassType";
        }
    }
}

char *GetOatClassStatusRepresentation(int16_t status)
{
    switch (status)
    {
        CASE_ENUM_REPR(kStatusRetired);
        CASE_ENUM_REPR(kStatusError);
        CASE_ENUM_REPR(kStatusNotReady);
        CASE_ENUM_REPR(kStatusIdx);
        CASE_ENUM_REPR(kStatusLoaded);
        CASE_ENUM_REPR(kStatusResolving);
        CASE_ENUM_REPR(kStatusResolved);
        CASE_ENUM_REPR(kStatusVerifying);
        CASE_ENUM_REPR(kStatusRetryVerificationAtRuntime);
        CASE_ENUM_REPR(kStatusVerifyingAtRuntime);
        CASE_ENUM_REPR(kStatusVerified);
        CASE_ENUM_REPR(kStatusInitializing);
        CASE_ENUM_REPR(kStatusInitialized);
        CASE_ENUM_REPR(kStatusMax);
        default:
        {
            LOGF("Unknown mirror::Class::Status encountered: %hd", status);
            return "UNKNOWN mirror::Class::Status";
        }
    }
}

char *GetInstructionSetRepresentation(enum InstructionSet set)
{
    switch (set)
    {
        CASE_ENUM_REPR(kNone);
        CASE_ENUM_REPR(kArm);
        CASE_ENUM_REPR(kArm64);
        CASE_ENUM_REPR(kThumb2);
        CASE_ENUM_REPR(kX86);
        CASE_ENUM_REPR(kX86_64);
        CASE_ENUM_REPR(kMips);
        CASE_ENUM_REPR(kMips64);
        default:
        {
            LOGF("Unknown InstructionSet encountered: %d", set);
            return "UNKNOWN InstructionSet";
        }
    }
}

char *GetInstructionSetFeaturesRepresentation(uint32_t f)
{
    switch (f)
    {
        case 0:
            return "None";
        case kHwDiv:
            // Supports hardware divide.
            return "Hardware Division Support";
        case kHwLpae:
            // Supports Large Physical Address Extension.
            return "Large PhysicalAddress Extension";
        case kHwDiv | kHwLpae:
            return "Hardware Division Support | Large Physical Address Extension";
        default:
        {
            LOGF("Unknown InstructionSetFeatures value %x", f);
            return "Illegal InstructionSetFeatures value";
        }
    }
}
#undef CASE_ENUM_REPR

static const void* GetPointerFromOffset(const void* base, uint32_t off)
{
    if(base == NULL || off == 0)
    {
        return NULL;
    }
    return base + off;
}

static bool IsValid(const struct OatHeader* hdr)
{
    CHECK_RETURNFALSE(hdr != NULL);
    if(memcmp(hdr->magic_, kOatMagic, sizeof(kOatMagic)) != 0)
    {
        return false;
    }
    if(memcmp(hdr->version_, kOatVersion, sizeof(kOatVersion)) != 0)
    {
        return false;
    }
    if(hdr->executable_offset_ % getSystemPageSize() != 0) // Check for page alignment
    {
        return false;
    }
    if(hdr->image_patch_delta_ & getSystemPageSize() != 0)
    {
        return false;
    }
    return true;
}
const char* GetMagic(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return (char*)hdr->magic_;
}
uint32_t GetChecksum(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return hdr->adler32_checksum_;
}

enum InstructionSet GetInstructionSet(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return hdr->instruction_set_;
}
uint32_t GetInstructionSetFeaturesBitmap(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return hdr->instruction_set_features_;
}

uint32_t GetExecutableOffset(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    CHECK(hdr->executable_offset_ > sizeof(struct OatHeader));
    return hdr->executable_offset_;
}

uint32_t GetJniDlsymLookupOffset(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    CHECK(hdr->jni_dlsym_lookup_offset_ >+ hdr->interpreter_to_compiled_code_bridge_offset_);
    return hdr->jni_dlsym_lookup_offset_;
}
const void* GetJniDlSymLookup(const struct OatHeader* hdr)
{
    return (uint8_t*)hdr + GetJniDlsymLookupOffset(hdr);
}

uint32_t GetQuickGenericJniTrampolineOffset(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    CHECK_GE(hdr->quick_generic_jni_trampoline_offset_, hdr->jni_dlsym_lookup_offset_);
    return hdr->quick_generic_jni_trampoline_offset_;
}
const void* GetQuickGenericJniTrampoline(const struct OatHeader* hdr)
{
    return (const uint8_t*)(hdr) + GetQuickGenericJniTrampolineOffset(hdr);
}

uint32_t GetQuickToInterpreterBridgeOffset(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    CHECK_GE(hdr->quick_to_interpreter_bridge_offset_, hdr->quick_resolution_trampoline_offset_);
    return hdr->quick_to_interpreter_bridge_offset_;
}
const void* GetQuickToInterpreterBridge(const struct OatHeader* hdr)
{
    return (const uint8_t*)(hdr) + GetQuickToInterpreterBridgeOffset(hdr);
}

int32_t GetImagePatchDelta(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return hdr->image_patch_delta_;
}
uint32_t GetImageFileLocationOatChecksum(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return hdr->image_file_location_oat_checksum_;
}
uint32_t GetImageFileLocationOatDataBegin(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return hdr->image_file_location_oat_data_begin_;
}

uint32_t GetKeyValueStoreSize(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return hdr->key_value_store_size_;
}
const uint8_t* GetKeyValueStore(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return hdr->key_value_store_;
}

const void* GetDexFileStoragePointer(const struct OatHeader* hdr)
{
    CHECK_RETURNNULL(hdr != NULL);
    return (void*)hdr + GetHeaderSize(hdr);
}

static const char *ParseString(const char *start, const char *end)
{
    while (start < end && *start != 0)
    {
        start++;
    }
    return start;
}

const char *GetStoreValueByKey(const struct OatHeader *this, const char *key)
{
    const char *ptr = (const char *) (&this->key_value_store_);
    const char *end = ptr + this->key_value_store_size_;
    while (ptr < end)
    {
        // Scan for a closing zero.
        const char *str_end = ParseString(ptr, end);
        if (str_end < end)
        {
            if (strcmp(key, ptr) == 0)
            {
                // Same as key. Check if value is OK.
                if (ParseString(str_end + 1, end) < end)
                {
                    return str_end + 1;
                }
            }
            else
            {
                // Different from key. Advance over the value.
                ptr = ParseString(str_end + 1, end) + 1;
            }
        }
        else
        {
            break;
        }
    }
    // Not found.
    return NULL;
}

bool GetStoreKeyValuePairByIndex(const struct OatHeader *this, size_t index, const char **key,
                                     const char **value)
{
    const char *ptr = (const char *) (&this->key_value_store_);
    const char *end = ptr + this->key_value_store_size_;
    ssize_t counter = (ssize_t) (index);
    while (ptr < end && counter >= 0)
    {
        // Scan for a closing zero.
        const char *str_end = ParseString(ptr, end);
        if (str_end < end)
        {
            const char *maybe_key = ptr;
            ptr = ParseString(str_end + 1, end) + 1;
            if (ptr <= end)
            {
                if (counter == 0)
                {
                    *key = maybe_key;
                    *value = str_end + 1;
                    return true;
                }
                else
                {
                    counter--;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            break;
        }
    }
    // Not found.
    return false;
}

size_t GetHeaderSize(const struct OatHeader* hdr)
{
    return sizeof(struct OatHeader) + hdr->key_value_store_size_;
}

uint32_t NumDexFiles(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return hdr->dex_file_count_;
}

bool IsKeyEnabled(const struct OatHeader* hdr, const char* key)
{
    CHECK_RETURNFALSE(IsValid(hdr));
    CHECK_RETURNFALSE(key != NULL);
    const char* key_value = GetStoreValueByKey(hdr, key);
    return (key_value != NULL && strncmp(key_value, kTrueValue, sizeof(kTrueValue)) == 0);
}

bool IsPic(const struct OatHeader* hdr)
{
    return IsKeyEnabled(hdr, kPicKey);
}
bool IsDebuggable(const struct OatHeader* hdr)
{
    return IsKeyEnabled(hdr, kDebuggableKey);
}

bool ReadOatValue_UInt32(const void **data, void* end,  uint32_t *result)
{
    CHECK_RETURNFALSE(data != NULL);
    CHECK_RETURNFALSE(*data != NULL);
    CHECK_RETURNFALSE(end != NULL);
    CHECK_RETURNFALSE(end > *data);
    CHECK_RETURNFALSE(result != NULL);

    if(UNLIKELY((*data + sizeof(uint32_t)) >= end))
    {
        return false;
    }
    *result = *(uint32_t*)*data;
    *data += sizeof(uint32_t);
    return true;
}
bool ReadOatValue_String(const void **data, void* end, struct String *result)
{
    CHECK_RETURNFALSE(data != NULL);
    CHECK_RETURNFALSE(*data != NULL);
    CHECK_RETURNFALSE(end != NULL);
    CHECK_RETURNFALSE(end > *data);
    CHECK_RETURNFALSE(result != NULL);

    const void* p = *data;
    uint32_t length;
    if(!ReadOatValue_UInt32(&p, end, &length))
    {
        LOGF("Found truncated string length.");
        return false;
    }
    CHECK(result->length > 0u);
    const char* content = p;
    p += result->length;
    if(UNLIKELY(p >= end))
    {
        LOGF("Found truncated string content.");
        return false;
    }
    result->length = length;
    result->content = content;
    return true;
}
bool ReadOatValue_Array(const void **data, void* end, const void **result,
                               uint32_t num_elements, size_t element_size)
{
    CHECK_RETURNFALSE(data != NULL);
    CHECK_RETURNFALSE(*data != NULL);
    CHECK_RETURNFALSE(end != NULL);
    CHECK_RETURNFALSE(end > *data);
    CHECK_RETURNFALSE(result != NULL);
    CHECK_RETURNFALSE(element_size > 0 || num_elements == 0);
    const void* p_after = *data + num_elements * element_size;
    if(UNLIKELY(p_after >= end))
    {
        LOGF("Found truncated array.");
        return false;
    }
    *result = *data;
    *data = p_after;
    return true;
}

bool ReadOatDexFileData(const void** data, void* end, struct OatDexFileData* result,
                               const struct OatHeader* oat_hdr)
{
    CHECK_RETURNFALSE(data != NULL);
    CHECK_RETURNFALSE(*data != NULL);
    CHECK_RETURNFALSE(end != NULL);
    CHECK_RETURNFALSE(end > *data);
    CHECK_RETURNFALSE(result != NULL);

    const void* p = *data;

    struct String location;
    if(UNLIKELY(!ReadOatValue_String(&p, end, &location)))
    {
        LOGF("Found truncated oat dex file location.");
        return false;
    }

    uint32_t oat_dex_file_checksum;
    if(UNLIKELY(!ReadOatValue_UInt32(&p, end, &oat_dex_file_checksum)))
    {
        LOGF("Found truncated oat dex file checksum.");
        return false;
    }

    uint32_t dex_file_offset;
    if(UNLIKELY(!ReadOatValue_UInt32(&p, end, &dex_file_offset)))
    {
        LOGF("Found truncated dex file offset.");
        return false;
    }

    const struct DexHeader *dex_header = (void*)oat_hdr + dex_file_offset;
    if (UNLIKELY(dex_header > end))
    {
        LOGF("Dex File Pointer points outside the valid memory range.");
        return false;
    }
    if (UNLIKELY((dex_header + 1) > end))
    {
        LOGF("Found truncated DexFile header");
        return false;
    }
    if(UNLIKELY((void*)dex_header + dex_header->file_size_ > end))
    {
        LOGF("Found truncated DexFile");
        return false;
    }

    uint32_t num_classes = dex_header->class_defs_size_;
    const uint32_t* class_def_offsets = (uint32_t*)data;
    if(UNLIKELY(!ReadOatValue_Array(&p, end, (const void**)&class_def_offsets, num_classes, sizeof(uint32_t))))
    {
        LOGF("Found truncated oat class definition offsets array.");
    }


    result->backing_memory_address = *data;
    *data = p;
    result->location_string = location;
    result->checksum = oat_dex_file_checksum;
    result->dex_file_offset = dex_file_offset;
    result->class_definition_offsets = class_def_offsets;
    result->num_defined_classes = num_classes;
    result->dex_file_pointer = dex_header;
    return true;
}
bool DecodeOatClassData(void* oat_class_pointer, void* end, struct OatClassData * result)
{
    CHECK_RETURNFALSE(oat_class_pointer != NULL);
    CHECK_RETURNFALSE(end != NULL);
    CHECK_RETURNFALSE(end > oat_class_pointer);
    CHECK_RETURNFALSE(result != NULL);

    const byte* status_pointer = oat_class_pointer;
    const byte* type_pointer = status_pointer + sizeof(result->mirror_class_status);
    const byte* after_type_pointer = type_pointer + sizeof(result->oat_class_type);

    if (UNLIKELY(type_pointer > end))
    {
        LOGF("Found truncated oat class status.");
        return false;
    }
    if (UNLIKELY(after_type_pointer > end))
    {
        LOGF("Found truncated oat class compilation type.");
        return false;
    }

    // Status and type did not exceed the boundaries
    int16_t     class_status    = *(int16_t*)  status_pointer;
    uint16_t    class_type      = *(uint16_t*) type_pointer;


    uint32_t    bitmap_size = 0;
    const byte* bitmap_pointer = NULL;

    const struct OatMethodOffsets * methods_pointer = NULL;

    if (class_type == kOatClassSomeCompiled)
    {
        bitmap_size = *((uint32_t*)after_type_pointer);
        bitmap_pointer = after_type_pointer + sizeof(result->bitmap_size);
        if(UNLIKELY(bitmap_pointer > end))
        {
            LOGF("Found truncated oat method compilation bitmap size");
            return false;
        }
        methods_pointer = (void*)result->bitmap_pointer + result->bitmap_size;
        if(UNLIKELY(methods_pointer > end))
        {
            LOGF("Found truncated oat method compilation bitmap");
            return false;
        }
    }
    else if (class_type == kOatClassAllCompiled)
    {
        // No need to check this as the after_type_pointer's integrity was already verified above.
        methods_pointer = (void*)after_type_pointer;
    }
    // If class_type == kOatClassNoneCompiled leave methods_pointer as NULL


    // Everything verifiable checked out, we're good to go.
    result->backing_memory_address = oat_class_pointer;
    result->mirror_class_status = class_status;
    result->oat_class_type = class_type;
    result->bitmap_size = bitmap_size;
    result->bitmap_pointer = bitmap_pointer;
    result->methods_pointer = (struct OatMethodOffsets *)methods_pointer;
    return true;
}

const struct OatMethodOffsets* GetOatMethodOffsets(const struct OatClassData* clazz, uint32_t method_index)
{
    // Sadly no checks on the method_index are possible.
    CHECK_RETURNNULL(clazz != NULL);
    if(clazz->methods_pointer == NULL)
    {
        CHECK_EQ(kOatClassNoneCompiled, clazz->oat_class_type);
        return NULL;
    }

    size_t methods_pointer_idx;
    if(clazz->bitmap_pointer == NULL)
    {
        CHECK_EQ(kOatClassAllCompiled, clazz->oat_class_type);
        return NULL;
    }
    else
    {
        CHECK_EQ(kOatClassSomeCompiled, clazz->oat_class_type);
        const uint32_t* bitmap_ = (uint32_t*)clazz->bitmap_pointer;
        if(!bit_vector_IsBitSet(bitmap_, method_index))
        {
            return NULL;
        }
        size_t num_set_bits = bit_vector_NumSetBits(bitmap_, method_index);
        methods_pointer_idx = num_set_bits;
    }
    const struct OatMethodOffsets * oat_method_offsets = &clazz->methods_pointer[methods_pointer_idx];
    return oat_method_offsets;
}


const void* GetQuickCode(void* base, const struct OatMethodOffsets* off)
{
    CHECK_RETURNNULL(base != NULL);
    CHECK_RETURNNULL(off != NULL);
    return GetPointerFromOffset(base, off->code_offset_);
}
const struct OatQuickMethodHeader* GetOatQuickMethodHeader(void* base, const struct OatMethodOffsets* off)
{
    // No need for sanity checks, as they are performed in GetQuickCode
    const void* code_ptr = EntryPointToCodePointer(GetQuickCode(base, off));
    if(code_ptr == NULL)
    {
        return NULL;
    }
    return ((struct OatQuickMethodHeader*)code_ptr) - 1;
}
uint32_t GetOatQuickMethodHeaderOffset(void* base, const struct OatMethodOffsets* off)
{
    // No need for sanity checks, as they are performed in GetOatQuickMethodHeader
    const struct OatQuickMethodHeader* method_header = GetOatQuickMethodHeader(base, off);
    if (method_header == NULL)
    {
        return 0u;
    }
    return (const void*)method_header - base;
}

uint32_t GetQuickCodeSize(void* base, const struct OatMethodOffsets* off)
{
    // No need for sanity checks, as they are performed in GetOatQuickMethodHeader
    const struct OatQuickMethodHeader* m_hdr = GetOatQuickMethodHeader(base, off);
    if (m_hdr == NULL)
    {
        return 0u;
    }
    return m_hdr->code_size_;
}
uint32_t GetQuickCodeSizeOffset(void* base, const struct OatMethodOffsets* off)
{
    // No need for sanity checks, as they are performed in GetOatQuickMethodHeader
    const struct OatQuickMethodHeader* method_header = GetOatQuickMethodHeader(base, off);
    if (method_header == NULL)
    {
        return 0u;
    }
    return (const void*)&method_header->code_size_ - base;
}