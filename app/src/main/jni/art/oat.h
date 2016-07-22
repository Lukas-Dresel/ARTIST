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
// Created by Lukas on 8/16/2015.
//

#ifndef NDKTEST_OAT_H
#define NDKTEST_OAT_H

#include <stdint.h>
#include "oat_internal.h"
#include "dex.h"

struct OatFile
{
    void*                       begin;
    void*                       end;

    struct OatHeader*           header;
    void*                       key_value_storage_start;
    void*                       dex_file_storage_start;
};

struct OatDexFile
{
    const struct OatFile*       oat_file;

    uint32_t                    index;
    struct OatDexFileData       data;
};

struct OatClass
{
    const struct OatDexFile*        oat_dex_file;
    struct DexClass                 dex_class;
    struct OatClassData             oat_class_data;
};
struct OatMethod
{
    const struct OatClass*          oat_class;

    struct DexMethod                dex_method;
    const struct OatMethodOffsets*  oat_method_offsets;
};

void*   oat_PointerFromFileOffset(const struct OatFile* oat_file, uint32_t offset);

bool    oat_Setup(struct OatFile* result, void *mem_begin, void *mem_end);

bool    oat_FindDexFile(struct OatFile* oat_file, struct OatDexFile* result, const char* location);
bool    oat_GetOatDexFile(struct OatFile* oat_file, struct OatDexFile* result, uint32_t index);

bool    oat_FindClass(struct OatFile* oat, struct OatDexFile* result_oat_dex_file,
                   struct OatClass *result_clazz, char *descriptor);
bool    oat_FindClassInDex(const struct OatDexFile *oat_dex_file, struct OatClass *clazz,
                           char *descriptor);
bool    oat_GetClass(const struct OatDexFile* oat_dex_file, struct OatClass* clazz, uint16_t class_def_index);

bool    oat_FindDirectMethod(const struct OatClass * oat_class, struct OatMethod* result, const char* descriptor, const char* signature);
bool    oat_FindVirtualMethod(const struct OatClass * oat_class, struct OatMethod* result, const char* descriptor, const char* signature);
bool    oat_FindMethod(const struct OatClass* oat_class, struct OatMethod* result, const char* descriptor, const char* signature);

bool    oat_HasQuickCompiledCode(const struct OatMethod* m);
void*   oat_GetQuickCompiledEntryPoint(const struct OatMethod* m);
void*   oat_GetQuickCompiledMemoryPointer(const struct OatMethod* m);

#endif //NDKTEST_OAT_H
