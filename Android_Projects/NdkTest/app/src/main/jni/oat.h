//
// Created by Lukas on 8/16/2015.
//

#ifndef NDKTEST_OAT_H
#define NDKTEST_OAT_H

#include <stdint.h>
#include <stdbool.h>

#define INVALID_OFF_T (-1)

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


typedef struct OatHeader
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
} OatHeader;

char *repr_OatClassType(uint16_t t);

char *repr_InstructionSetFeatures(uint32_t f);

char *repr_InstructionSet(InstructionSet i);

char *repr_mirror_Class_Status(int16_t s);

const char *oat_header_GetStoreValueByKey(OatHeader *this, const char *key);

bool oat_header_GetStoreKeyValuePairByIndex(OatHeader *this, size_t index, const char **key,
                                            const char **value);

size_t oat_header_GetHeaderSize(OatHeader *this);

#endif //NDKTEST_OAT_H
