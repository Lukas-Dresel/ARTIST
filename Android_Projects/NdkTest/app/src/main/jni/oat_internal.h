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

#ifndef NDKTEST_OAT_INTERNAL_H
#define NDKTEST_OAT_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>
#include "typedefs.h"

typedef enum OatClassType
{
    kOatClassAllCompiled = 0,   // OatClass is followed by an OatMethodOffsets for each method.
    kOatClassSomeCompiled = 1,  // A bitmap of which OatMethodOffsets are present follows the OatClass.
    kOatClassNoneCompiled = 2,  // All methods are interpretted so no OatMethodOffsets are necessary.
    kOatClassMax = 3
} OatClassType;

enum mirror_Class_Status
{
    kStatusRetired = -2,
    kStatusError = -1,
    kStatusNotReady = 0,
    kStatusIdx = 1,  // Loaded, DEX idx in super_class_type_idx_ and interfaces_type_idx_.
    kStatusLoaded = 2,  // DEX idx values resolved.
    kStatusResolving = 3,  // Just cloned from temporary class object.
    kStatusResolved = 4,  // Part of linking.
    kStatusVerifying = 5,  // In the process of being verified.
    kStatusRetryVerificationAtRuntime = 6,  // Compile time verification failed, retry at runtime.
    kStatusVerifyingAtRuntime = 7,  // Retrying verification at runtime.
    kStatusVerified = 8,  // Logically part of linking; done pre-init.
    kStatusInitializing = 9,  // Class init in progress.
    kStatusInitialized = 10,  // Ready to go.
    kStatusMax = 11,
};

typedef enum InstructionSet
{
    kNone,
    kArm,
    kArm64,
    kThumb2,
    kX86,
    kX86_64,
    kMips,
    kMips64
} InstructionSet;

typedef enum InstructionSetFeatures
{
    kHwDiv = 0x1,              // Supports hardware divide.
    kHwLpae = 0x2,              // Supports Large Physical Address Extension.
} InstructionSetFeatures;

struct OatHeader
{
    uint8_t magic_[4];
    uint8_t version_[4];
    uint32_t adler32_checksum_;
    InstructionSet instruction_set_;
    InstructionSetFeatures instruction_set_features_;
    uint32_t dex_file_count_;
    uint32_t executable_offset_;
    uint32_t interpreter_to_interpreter_bridge_offset_;
    uint32_t interpreter_to_compiled_code_bridge_offset_;
    uint32_t jni_dlsym_lookup_offset_;
    uint32_t portable_imt_conflict_trampoline_offset_;
    uint32_t portable_resolution_trampoline_offset_;
    uint32_t portable_to_interpreter_bridge_offset_;
    uint32_t quick_generic_jni_trampoline_offset_;
    uint32_t quick_imt_conflict_trampoline_offset_;
    uint32_t quick_resolution_trampoline_offset_;
    uint32_t quick_to_interpreter_bridge_offset_;
    // The amount that the image this oat is associated with has been patched.
    int32_t image_patch_delta_;
    uint32_t image_file_location_oat_checksum_;
    uint32_t image_file_location_oat_data_begin_;
    uint32_t key_value_store_size_;
    uint8_t key_value_store_[0];  // note variable width data at end
};

struct OatClass
{
    int16_t                     mirror_class_status;
    uint16_t                    oat_class_type;
    uint32_t                    bitmap_size;
    const uint8_t*              bitmap_pointer;
    struct OatMethodOffsets*    methods_pointer;
};
struct OatMethodOffsets
{
    // This code offset points to the actual code.
    // There is a struct OatQuickMethodHeader that lies directly before this point.
    // This contains all the information about the code.
    uint32_t code_offset_;
};

struct QuickMethodFrameInfo
{
    uint32_t frame_size_in_bytes_;
    uint32_t core_spill_mask_;
    uint32_t fp_spill_mask;
};

typedef struct OatQuickMethodHeader
{
    uint32_t                    mapping_table_offset_;
    uint32_t                    vmap_table_offset_;
    uint32_t                    gc_map_offset_;
    struct QuickMethodFrameInfo frame_info_;
    uint32_t                    code_size_;
};

struct DecodedOatDexFile
{
    //Content
    void*                   backing_memory_address;
    uint32_t                backing_memory_size;

    String                  location_string;
    uint32_t                checksum;
    uint32_t                dex_file_offset;
    uint32_t                number_of_defined_classes;
    uint32_t*               class_definition_offsets;
};


char *          GetOatClassTypeRepresentation(uint16_t t);
char *          GetOatClassStatusRepresentation(int16_t status);
char *          GetInstructionSetRepresentation(InstructionSet set);
char *          GetInstructionSetFeaturesRepresentation(uint32_t f);

const char*     GetMagic(const struct OatHeader* hdr);
uint32_t        GetChecksum(const struct OatHeader* hdr);

size_t          GetHeaderSize(const struct OatHeader* hdr);

uint32_t        NumDexFiles(const struct OatHeader* hdr);

uint32_t        GetHeaderSize(const struct OatHeader* hdr);

InstructionSet  GetInstructionSet(const struct OatHeader* hdr);
uint32_t        GetInstructionSetFeaturesBitmap(const struct OatHeader* hdr);

uint32_t        GetExecutableOffset(const struct OatHeader* hdr);

uint32_t        GetJniDlsymLookupOffset(const struct OatHeader* hdr);
const void*     GetJniDlSymLookup(const struct OatHeader* hdr);

uint32_t        GetQuickGenericJniTrampolineOffset(const struct OatHeader* hdr);
const void*     GetQuickGenericJniTrampoline(const struct OatHeader* hdr);

uint32_t        GetQuickToInterpreterBridgeOffset(const struct OatHeader* hdr);
const void*     GetQuickToInterpreterBridge(const struct OatHeader* hdr);

int32_t         GetImagePatchDelta(const struct OatHeader* hdr);
uint32_t        GetImageFileLocationOatChecksum(const struct OatHeader* hdr);
uint32_t        GetImageFileLocationOatDataBegin(const struct OatHeader* hdr);

uint32_t        GetKeyValueStoreSize(const struct OatHeader* hdr);
const uint8_t*  GetKeyValueStore(const struct OatHeader* hdr);

const char *    GetStoreValueByKey(const struct OatHeader *this, const char *key);

bool            GetStoreKeyValuePairByIndex(const struct OatHeader *this, size_t index,
                                            const char **key, const char **value);

bool            IsKeyEnabled(const struct OatHeader* hdr, const char* key);
bool            IsPic(const struct OatHeader* hdr);
bool            IsDebuggable(const struct OatHeader* hdr);

#endif //NDKTEST_OAT_INTERNAL_H
