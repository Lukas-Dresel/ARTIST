//
// Created by Lukas on 10/2/2015.
//

#include "dex_class_data.h"
#include "leb128.h"
#include "logging.h"
#include "memory.h"

static DexClassDataHeader* ReadClassDataHeader(DexClassDataHeader* result, const uint8_t** iterator)
{
    result->static_fields_size_      = DecodeUnsignedLeb128(iterator);
    result->instance_fields_size_    = DecodeUnsignedLeb128(iterator);
    result->direct_methods_size_     = DecodeUnsignedLeb128(iterator);
    result->virtual_methods_size_    = DecodeUnsignedLeb128(iterator);
    return result;
}
static DexClassDataField* ReadClassDataField(DexClassDataField* result, const uint8_t** iterator)
{
    result->field_idx_delta_ = DecodeUnsignedLeb128(iterator);
    result->access_flags_    = DecodeUnsignedLeb128(iterator);
    return result;
}
static DexClassDataMethod* ReadClassDataMethod(DexClassDataMethod* result, const uint8_t** iterator)
{
    result->method_idx_delta_    = DecodeUnsignedLeb128(iterator);
    result->access_flags_        = DecodeUnsignedLeb128(iterator);
    result->code_off_            = DecodeUnsignedLeb128(iterator);
    return result;
}


static DexClassData* dex_class_data_Create(const uint8_t* class_data)
{
    const uint8_t* iterator = class_data;
    DexClassDataHeader hdr;
    ReadClassDataHeader(&hdr, &iterator);

    size_t data_arrays_size = 0;
    data_arrays_size += sizeof(DexClassDataField) * hdr.static_fields_size_;
    data_arrays_size += sizeof(DexClassDataField) * hdr.instance_fields_size_;
    data_arrays_size += sizeof(DexClassDataMethod) * hdr.direct_methods_size_;
    data_arrays_size += sizeof(DexClassDataMethod) * hdr.virtual_methods_size_;

    size_t index_arrays_size = 0;
    data_arrays_size += sizeof(uint32_t) * hdr.static_fields_size_;
    data_arrays_size += sizeof(uint32_t) * hdr.instance_fields_size_;
    data_arrays_size += sizeof(uint32_t) * hdr.direct_methods_size_;
    data_arrays_size += sizeof(uint32_t) * hdr.virtual_methods_size_;

    size_t needed = sizeof(DexClassData) + data_arrays_size + index_arrays_size;

    DexClassData* self = allocate_memory_chunk(needed);
    if(UNLIKELY(self == NULL))
    {
        LOGF("Failed to allocate memory for DexClassData struct.");
        return NULL;
    }
    self->class_data_start = class_data;
    self->hdr = hdr;
    self->static_fields     = (void*)(self + 1); // Skip the header, then place the arrays right after it.
    self->instance_fields   = (void*)(self->static_fields + hdr.static_fields_size_);
    self->direct_methods    = (void*)(self->instance_fields + hdr.instance_fields_size_);
    self->virtual_methods   = (void*)(self->direct_methods + hdr.direct_methods_size_);

    self->static_fields_indices = (void*)(self->virtual_methods + hdr.virtual_methods_size_);
    self->instance_fields_indices = (void*)(self->static_fields_indices + hdr.static_fields_size_);
    self->direct_methods_indices = (void*)(self->instance_fields_indices + hdr.instance_fields_size_);
    self->virtual_methods_indices = (void*)(self->direct_methods_indices + hdr.direct_methods_size_);

    return self;
}
DexClassData* dex_class_data_Initialize(const uint8_t* class_data)
{
    CHECK(class_data != NULL);

    DexClassData* self = dex_class_data_Create(class_data);
    if(UNLIKELY(self == NULL))
    {
        return NULL;
    }

    const uint8_t* iterator = class_data;
    uint32_t current_index;

    // Read in the header information. Eventhough we already did this in dex_class_data_Create,
    // this sets up the iterator to start at the right position
    ReadClassDataHeader(&self->hdr, &iterator);

    // Extract all the information from the arrays.
    current_index = 0;
    for(uint32_t i = 0; i < self->hdr.static_fields_size_; i++)
    {
        ReadClassDataField(&self->static_fields[i], &iterator);
        current_index += self->static_fields[i].field_idx_delta_;
        self->static_fields_indices[i] = current_index;
    }
    current_index = 0;
    for(uint32_t i = 0; i < self->hdr.instance_fields_size_; i++)
    {
        ReadClassDataField(&self->instance_fields[i], &iterator);
        current_index += self->instance_fields[i].field_idx_delta_;
        self->instance_fields_indices[i] = current_index;
    }
    current_index = 0;
    for(uint32_t i = 0; i < self->hdr.direct_methods_size_; i++)
    {
        ReadClassDataMethod(&self->direct_methods[i], &iterator);
        current_index += self->direct_methods[i].method_idx_delta_;
        self->direct_methods_indices[i] = current_index;
    }
    current_index = 0;
    for(uint32_t i = 0; i < self->hdr.virtual_methods_size_; i++)
    {
        ReadClassDataMethod(&self->virtual_methods[i], &iterator);
        current_index += self->virtual_methods[i].method_idx_delta_;
        self->virtual_methods_indices[i] = current_index;
    }
    return self;
}
void dex_class_data_Destroy(DexClassData* self)
{
    CHECK(self != NULL);
    free(self);
}
uint32_t dex_class_data_GetNumberOfStaticFields(DexClassData* self)
{
    CHECK(self != NULL);
    return self->hdr.static_fields_size_;
}
uint32_t dex_class_data_GetNumberOfInstanceFields(DexClassData* self)
{
    CHECK(self != NULL);
    return self->hdr.instance_fields_size_;
}
uint32_t dex_class_data_GetNumberOfDirectMethods(DexClassData* self)
{
    CHECK(self != NULL);
    return self->hdr.direct_methods_size_;
}
uint32_t dex_class_data_GetNumberOfVirtualMethods(DexClassData* self)
{
    CHECK(self != NULL);
    return self->hdr.virtual_methods_size_;
}

uint32_t dex_class_data_GetFieldIdIndex_StaticField(DexClassData *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(index < self->hdr.static_fields_size_);
    return self->static_fields_indices[index];
}
uint32_t dex_class_data_GetFieldIdIndex_InstanceField(DexClassData *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(index < self->hdr.instance_fields_size_);
    return self->instance_fields_indices[index];
}
uint32_t dex_class_data_GetMethodIdIndex_DirectMethod(DexClassData *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(index < self->hdr.direct_methods_size_);
    return self->direct_methods_indices[index];
}
uint32_t dex_class_data_GetMethodIdIndex_VirtualMethod(DexClassData *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(index < self->hdr.virtual_methods_size_);
    return self->virtual_methods_indices[index];
}

DexClassDataField dex_class_data_GetEncodedField_StaticField(DexClassData *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(index < self->hdr.static_fields_size_);
    return self->static_fields[index];
}
DexClassDataField dex_class_data_GetEncodedField_InstanceField(DexClassData *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(index < self->hdr.instance_fields_size_);
    return self->instance_fields[index];
}
DexClassDataMethod dex_class_data_GetEncodedMethod_DirectMethod(DexClassData *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(index < self->hdr.direct_methods_size_);
    return self->direct_methods[index];
}
DexClassDataMethod dex_class_data_GetEncodedMethod_VirtualMethod(DexClassData *self, uint32_t index)
{
    CHECK(self != NULL);
    CHECK(index < self->hdr.virtual_methods_size_);
    return self->virtual_methods[index];
}