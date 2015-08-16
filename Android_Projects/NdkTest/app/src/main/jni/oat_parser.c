//
// Created by Lukas on 8/16/2015.
//

#include "oat_parser.h"

#include "error.h"
#include "logging.h"

OatHeader* parse_oat_file(void* address)
{
    OatHeader* header = (OatHeader*)address;
    return header;
}

char* get_instruction_set_representation(InstructionSet set)
{
    switch(set)
    {
        case kNone:
            return "None";
        case kArm:
            return "Arm";
        case kArm64:
            return "Arm64";
        case kThumb2:
            return "Thumb2";
        case kX86:
            return "x86";
        case kX86_64:
            return "x86_64";
        case kMips:
            return "MIPS";
        case kMips64:
            return "MIPS64";
        default:
            set_last_error("Unknown Instruction Set in Oat Header.");
            return "UNKNOWN INSTRUCTION SET";
    }
}
static const char* parse_string(const char* start, const char* end)
{
    while (start < end && *start != 0) {
        start++;
    }
    return start;
}
void log_oat_header_info(OatHeader* hdr)
{
    LOGD("Oat Header Information:");

    LOGD("Magic:                                         %08x | %s", *((uint32_t*)&hdr->magic_), hdr->magic_);
    LOGD("Version:                                       %08x | %s", *((uint32_t*)&hdr->version_), hdr->version_);
    LOGD("Checksum:                                      %08x", hdr->adler32_checksum_);

    clear_last_error();
    LOGD("InstructionSet:                                %d (%s)", hdr->instruction_set_, get_instruction_set_representation(hdr->instruction_set_));
    if(error_occurred())
    {
        LOGE("Found invalid instruction set in Oat Header at "PRINT_PTR, (uintptr_t)hdr);
    }
    LOGD("InstructionSetFeatures:                        %x", hdr->instruction_set_features_);
    LOGD("Dex File Count:                                %d", hdr->dex_file_count_);
    LOGD("Executable Offset:                             %x", hdr->executable_offset_);

    LOGD("Interp2Interp Bridge Offset:                   %x", hdr->interpreter_to_interpreter_bridge_offset_);
    LOGD("Interp2Compiled Bridge Offset:                 %x", hdr->interpreter_to_compiled_code_bridge_offset_);
    LOGD("JNI DLSym lookup offset:                       %x", hdr->jni_dlsym_lookup_offset_);

    LOGD("Portable IMT Conflict Trampoline Offset:       %x", hdr->portable_imt_conflict_trampoline_offset_);
    LOGD("Portable Resolution Trampoline Offset:         %x", hdr->portable_resolution_trampoline_offset_);
    LOGD("Portable to Interpreter Bridge Offset:         %x", hdr->portable_to_interpreter_bridge_offset_);

    LOGD("Quick Generic JNI Trampoline Offset:           %x", hdr->quick_generic_jni_trampoline_offset_);
    LOGD("Quick IMT Conflict Trampoline Offset:          %x", hdr->quick_imt_conflict_trampoline_offset_);
    LOGD("Quick Resolution Trampoline Offset:            %x", hdr->quick_resolution_trampoline_offset_);
    LOGD("Quick to Interpreter Bridge Offset:            %x", hdr->quick_to_interpreter_bridge_offset_);

    LOGD("Image Patch Delta:                             %x", hdr->image_patch_delta_);
    LOGD("Image File Location Oat Checksum:              %x", hdr->image_file_location_oat_checksum_);
    LOGD("Image File Location Oat Data Begin:            %x", hdr->image_file_location_oat_data_begin_);

    LOGD("Key-Value Store Size:                          %d", hdr->key_value_store_size_);

    log_key_value_store_info(hdr);
}
void log_key_value_store_info(OatHeader* hdr)
{
    // TODO implement
    return;
}