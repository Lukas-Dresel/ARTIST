//
// Created by Lukas on 10/2/2015.
//

#ifndef NDKTEST_DEX_CLASS_DATA_H
#define NDKTEST_DEX_CLASS_DATA_H

#include <stdint.h>
// A dex file's class_data_item is leb128 encoded, this structure holds a decoded form of the
// header for a class_data_item
typedef struct DexClassDataHeader
{
    uint32_t static_fields_size_;  // the number of static fields
    uint32_t instance_fields_size_;  // the number of instance fields
    uint32_t direct_methods_size_;  // the number of direct methods
    uint32_t virtual_methods_size_;  // the number of virtual methods
} DexClassDataHeader;

typedef struct DexClassDataField
{
    uint32_t field_idx_delta_;  // delta of index into the field_ids array for FieldId
    uint32_t access_flags_;  // access flags for the field
} DexClassDataField;
// A decoded version of the method of a class_data_item
typedef struct DexClassDataMethod
{
    uint32_t method_idx_delta_;  // delta of index into the method_ids array for MethodId
    uint32_t access_flags_;
    uint32_t code_off_;
} DexClassDataMethod;

typedef struct DexClassData
{
    const uint8_t*          class_data_start;

    DexClassDataHeader      hdr;                // unpacked class_data_item header

    DexClassDataField *     static_fields;      // array of unpacked encoded_field entries of all static fields
    DexClassDataField *     instance_fields;    // array of unpacked encoded_field entries of all instance fields
    DexClassDataMethod *    direct_methods;     // array of unpacked encoded_method entries of all directly implemented methods
    DexClassDataMethod *    virtual_methods;    // array of unpacked encoded_method entries of all virtual methods

    uint32_t*               static_fields_indices;      // array of indices into the field_ids array for FieldId
    uint32_t*               instance_fields_indices;    // array of indices into the field_ids array for FieldId
    uint32_t*               direct_methods_indices;     // array of indices into the method_ids array for MethodId
    uint32_t*               virtual_methods_indices;    // array of indices into the method_ids array for MethodId

} DexClassData;

DexClassData*       dex_class_data_Initialize                       ( const uint8_t* class_data );
void                dex_class_data_Destroy                          ( DexClassData* self );

uint32_t            dex_class_data_GetNumberOfStaticFields          ( DexClassData* self );
uint32_t            dex_class_data_GetNumberOfInstanceFields        ( DexClassData* self );
uint32_t            dex_class_data_GetNumberOfDirectMethods         ( DexClassData* self );
uint32_t            dex_class_data_GetNumberOfVirtualMethods        ( DexClassData* self );

uint32_t            dex_class_data_GetFieldIdIndex_StaticField      ( DexClassData *self, uint32_t index );
uint32_t            dex_class_data_GetFieldIdIndex_InstanceField    ( DexClassData *self, uint32_t index );
uint32_t            dex_class_data_GetMethodIdIndex_DirectMethod    ( DexClassData *self, uint32_t index );
uint32_t            dex_class_data_GetMethodIdIndex_VirtualMethod   ( DexClassData *self, uint32_t index );

DexClassDataField   dex_class_data_GetEncodedField_StaticField      ( DexClassData *self, uint32_t index );
DexClassDataField   dex_class_data_GetEncodedField_InstanceField    ( DexClassData *self, uint32_t index );
DexClassDataMethod  dex_class_data_GetEncodedMethod_DirectMethod    ( DexClassData *self, uint32_t index );
DexClassDataMethod  dex_class_data_GetEncodedMethod_VirtualMethod   ( DexClassData *self, uint32_t index );

#endif //NDKTEST_DEX_CLASS_DATA_H
