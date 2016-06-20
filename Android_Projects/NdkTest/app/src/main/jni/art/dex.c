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
// Created by Lukas on 8/20/2015.
//

#include <stdint.h>

#include "dex.h"
#include "../logging.h"

uint32_t dex_NumberOfStrings  ( const struct DexHeader* hdr )
{
    CHECK_RETURN(hdr != NULL, 0);
    return hdr->string_ids_size_;
}
uint32_t dex_NumberOfMethods  ( const struct DexHeader* hdr )
{
    CHECK_RETURN(hdr != NULL, 0);
    return hdr->method_ids_size_;
}
uint32_t dex_NumberOfFields   ( const struct DexHeader* hdr )
{
    CHECK_RETURN(hdr != NULL, 0);
    return hdr->field_ids_size_;
}
uint32_t dex_NumberOfClassDefs  ( const struct DexHeader* hdr )
{
    CHECK_RETURN(hdr != NULL, 0);
    return hdr->class_defs_size_;
}

bool dex_FindClass(const struct DexHeader *hdr, struct DexClass *result, char *mutf8_descriptor)
{
    CHECK_RETURNFALSE(hdr != NULL);
    CHECK_RETURNFALSE(result != NULL);
    CHECK_RETURNFALSE(mutf8_descriptor != NULL);

    const struct ClassDef *found_class = FindClassDefByDescriptor(hdr, mutf8_descriptor);
    if (found_class == NULL)
    {
        return false;
    }
    result->dex_header = hdr;
    result->class_def = found_class;
    memset(&result->decoded_class_data, 0, sizeof(result->decoded_class_data));
    if (result->class_def->class_data_off_ != 0)
    {
        const uint8_t *class_data_pointer = (void *) hdr + found_class->class_data_off_;
        DecodeEncodedClassDataItem(hdr, &result->decoded_class_data, &class_data_pointer);
    }
    return true;
}
bool dex_GetClass(const struct DexHeader* hdr, struct DexClass* result, uint16_t class_def_index)
{
    CHECK_RETURNFALSE(hdr != NULL);
    CHECK_RETURNFALSE(result != NULL);
    CHECK_RETURNFALSE(class_def_index < dex_NumberOfClassDefs(hdr));

    const struct ClassDef *found_class = GetClassDef(hdr, class_def_index);
    if (found_class == NULL)
    {
        return false;
    }
    result->dex_header = hdr;
    result->class_def = found_class;
    memset(&result->decoded_class_data, 0, sizeof(result->decoded_class_data));
    if (result->class_def->class_data_off_ != 0)
    {
        const uint8_t *class_data_pointer = (void *) hdr + found_class->class_data_off_;
        DecodeEncodedClassDataItem(hdr, &result->decoded_class_data, &class_data_pointer);
    }
    return true;
}
static bool FindMethod(const struct DexClass* dex_class, struct DexMethod* result,
                       const char* mutf8_descriptor, const char* mutf8_signature, bool direct)
{
    CHECK_RETURNFALSE(dex_class != NULL);
    CHECK_RETURNFALSE(mutf8_descriptor != NULL);
    CHECK_RETURNFALSE(mutf8_signature != NULL);
    struct DexMethod ignored;
    if(result == NULL)
    {
        // This could be a legitimate use-case to simply figure out if a method is found.
        result = &ignored;
    }

    LOGD("Looking up %s method %s %s", direct ? "direct" : "virtual", mutf8_descriptor, mutf8_signature);

    LOGD("Looking up the StringID* for method name %s", mutf8_descriptor);
    const struct StringID* wanted_name = FindStringIDByModifiedUTF8StringValue(dex_class->dex_header, mutf8_descriptor);
    if(wanted_name == NULL)
    {
        LOGD("Could not find class name as string.");
        return false;
    }
    LOGD("Found StringId for %s at "PRINT_PTR, mutf8_descriptor, (uintptr_t)wanted_name);

    LOGD("Looking up the ProtoID* for method signature %s", mutf8_signature);
    const struct ProtoID* wanted_prototype = FindProtoIDBySignatureString(dex_class->dex_header, mutf8_signature);
    if(wanted_prototype == NULL)
    {
        LOGD("Could not find prototype.");
        return false;
    }
    LOGD("Found ProtoID for %s at "PRINT_PTR, mutf8_signature, (uintptr_t)wanted_prototype);


    uint32_t num_methods =  dex_class->decoded_class_data.direct_methods_size;
    const uint8_t* p =      dex_class->decoded_class_data.direct_methods_array;
    uint32_t class_start_idx = 0;
    if(!direct)
    {
        // If the method is virtual it lies after all of the direct methods
        class_start_idx =   dex_class->decoded_class_data.direct_methods_size;
        num_methods =       dex_class->decoded_class_data.virtual_methods_size;
        p =                 dex_class->decoded_class_data.virtual_methods_array;
    }

    LOGD("Looking for method with matching name and prototype.");
    uint32_t method_idx = 0;
    struct DecodedMethod current;
    for(uint32_t i = 0; i < num_methods; i++)
    {
        DecodeEncodedMethod(dex_class->dex_header, &current, &p, &method_idx);

        const struct MethodID* method_id = GetMethodID(dex_class->dex_header, current.method_idx);
        const struct StringID* current_name = GetStringID(dex_class->dex_header, method_id->name_idx_);
        const struct ProtoID* current_proto = GetProtoID(dex_class->dex_header, method_id->proto_idx_);

        LOGD("MethodID: "PRINT_PTR", StringID: "PRINT_PTR", ProtoID: "PRINT_PTR, method_id, current_name, current_proto);

        LOGD("Comparing method name.");
        if(CompareStringIDsByDexOrdering(dex_class->dex_header, wanted_name, current_name) != 0)
        {
            continue;
        }
        LOGD("Comparing method proto.");
        if(CompareProtoIDsByDexOrdering(dex_class->dex_header, wanted_prototype, current_proto) != 0)
        {
            continue;
        }

        LOGD("Found method.");

        // Found the desired Method.
        result->dex_header = dex_class->dex_header;
        result->containing_class = dex_class;
        result->method_id = method_id;
        result->class_method_idx = class_start_idx + i;
        result->is_direct = direct;

        // Simply redecode by resetting the method_idx
        method_idx -= current.method_idx_diff;
        p = current.backing_memory_address;
        DecodeEncodedMethod(dex_class->dex_header, &result->decoded_method_data, &p, &method_idx);
        return true;
    }
    return false;
}
bool dex_FindVirtualMethod(const struct DexClass* class, struct DexMethod* result, const char* mutf8_descriptor, const char* mutf8_signature)
{
    return FindMethod(class, result, mutf8_descriptor, mutf8_signature, false);
}
bool dex_FindDirectMethod(const struct DexClass* class, struct DexMethod* result, const char* mutf8_descriptor, const char* mutf8_signature)
{
    return FindMethod(class, result, mutf8_descriptor, mutf8_signature, true);
}


