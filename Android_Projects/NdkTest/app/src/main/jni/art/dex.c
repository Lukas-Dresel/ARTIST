//
// Created by Lukas on 8/20/2015.
//

#include <stdint.h>

#include "dex.h"
#include "../logging.h"


bool dex_FindClass(const struct DexHeader *hdr, struct DexClass *result, char *descriptor)
{
    CHECK_RETURNFALSE(hdr != NULL);
    CHECK_RETURNFALSE(result != NULL);
    CHECK_RETURNFALSE(descriptor != NULL);

    const struct ClassDef *found_class = FindClassDefByDescriptor(hdr, descriptor);
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
    if(result == NULL)
    {
        // This could be a legitimate use-case to simply figure out if a method is found.
        return false;
    }

    const struct StringID* wanted_name = FindStringIDByModifiedUTF8StringValue(dex_class->dex_header, mutf8_descriptor);
    if(wanted_name == NULL)
    {
        return false;
    }

    const struct ProtoID* wanted_prototype = FindProtoIDBySignatureString(dex_class->dex_header, mutf8_signature);
    if(wanted_prototype == NULL)
    {
        return false;
    }

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

    uint32_t method_idx = 0;
    struct DecodedMethod current;
    for(uint32_t i = 0; i < num_methods; i++)
    {
        DecodeEncodedMethod(dex_class->dex_header, &current, &p, &method_idx);

        const struct MethodID* method_id = GetMethodID(dex_class->dex_header, current.method_idx);
        const struct StringID* current_name = GetStringID(dex_class->dex_header, method_id->name_idx_);
        const struct ProtoID* current_proto = GetProtoID(dex_class->dex_header, method_id->proto_idx_);

        if(CompareStringIDsByDexOrdering(dex_class->dex_header, wanted_name, current_name) != 0)
        {
            continue;
        }
        if(CompareProtoIDsByDexOrdering(dex_class->dex_header, wanted_prototype, current_proto) != 0)
        {
            continue;
        }

        // Found the desired Method.
        result->dex_header = dex_class->dex_header;
        result->containing_class = dex_class;
        result->method_id = method_id;
        result->class_method_idx = class_start_idx + i;

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


