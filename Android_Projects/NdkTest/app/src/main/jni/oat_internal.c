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
 * This file is heavily borrowed from the above sources.
 * It was written to port the parsing of oat files to c code.
 */

#include "oat_internal.h"
#include "logging.h"
#include "system_info.h"
#include "dex_internal.h"

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

char *GetInstructionSetRepresentation(InstructionSet set)
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

InstructionSet GetInstructionSet(const struct OatHeader* hdr)
{
    CHECK(IsValid(hdr));
    return  hdr->instruction_set_;
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