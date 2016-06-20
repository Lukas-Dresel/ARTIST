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
 

#include <stdbool.h>
#include <stddef.h>
#include "statistics.h"
#include "logging.h"
#include "art/oat.h"
#include "art/bit_vector_util.h"

bool stats_NumCompiledMethodsInOatDexClass(struct OatClass* oat_class, uint32_t* result_compiled, uint32_t* result_total)
{
    uint32_t num_class_defs = oat_class->dex_class.decoded_class_data.direct_methods_size;
    num_class_defs += oat_class->dex_class.decoded_class_data.virtual_methods_size;

    *result_total = num_class_defs;

    const uint32_t* bitmap_ptr = (const uint32_t*)oat_class->oat_class_data.bitmap_pointer;

    switch(oat_class->oat_class_data.class_type)
    {
        case kOatClassAllCompiled:
            *result_compiled = num_class_defs;
            return true;
        case kOatClassSomeCompiled:
            *result_compiled = bit_vector_NumSetBits(bitmap_ptr, num_class_defs);
            return true;
        case kOatClassNoneCompiled:
            *result_compiled = 0;
            return true;
        default:
            LOGF("Unknown class type value: %x", oat_class->oat_class_data.class_type);
            return false;
    }
}
bool stats_NumCompiledMethodsInOatDexFile(struct OatDexFile* oat_dex, uint32_t* result_compiled, uint32_t* result_total)
{
    CHECK_RETURNFALSE(oat_dex != NULL);
    CHECK_RETURNFALSE(result_compiled != NULL);
    CHECK_RETURNFALSE(result_total != NULL);

    uint32_t dex_compiled = 0;
    uint32_t dex_total = 0;

    for(uint16_t class_idx = 0; class_idx < oat_dex->data.dex_file_pointer->class_defs_size_; class_idx ++)
    {
        struct OatClass current_class;
        if(!oat_GetClass(oat_dex, &current_class, class_idx))
        {
            return false;
        }
        uint32_t class_compiled;
        uint32_t class_total;
        if(!stats_NumCompiledMethodsInOatDexClass(&current_class, &class_compiled, &class_total))
        {
            return false;
        }
        dex_compiled += class_compiled;
        dex_total += class_total;
    }
    *result_compiled = dex_compiled;
    *result_total = dex_total;
    return true;
}

bool stats_NumCompiledMethodsInOatFile(struct OatFile* oat, uint32_t* result_compiled, uint32_t* result_total)
{
    CHECK_RETURNFALSE(oat != NULL);
    CHECK_RETURNFALSE(result_compiled != NULL);
    CHECK_RETURNFALSE(result_total != NULL);

    int dexFiles = NumDexFiles(oat->header);

    uint32_t oat_compiled = 0;
    uint32_t oat_total = 0;

    for(uint32_t i = 0; i < dexFiles; i++)
    {
        struct OatDexFile current_dex;
        if(!oat_GetOatDexFile(oat, &current_dex, i))
        {
            return false;
        }

        uint32_t dex_compiled;
        uint32_t dex_total;
        if(!stats_NumCompiledMethodsInOatDexFile(&current_dex, &dex_compiled, &dex_total))
        {
            return false;
        }
        oat_compiled += dex_compiled;
        oat_total += dex_total;
    }
    *result_compiled = oat_compiled;
    *result_total = oat_total;
    return true;
}

bool stats_logNumCompiledMethodsInOatClass(struct OatClass* oat_class, uint32_t* result_compiled, uint32_t* result_total)
{
    const struct DexHeader* hdr = oat_class->dex_class.dex_header;
    const char* class_name = GetTypeIDNameByIdx(hdr, oat_class->dex_class.class_def->class_idx_);
    uint32_t class_compiled = 0;
    uint32_t class_total = 0;
    if(!stats_NumCompiledMethodsInOatDexClass(oat_class, &class_compiled, &class_total))
    {
        return false;
    }
    LOGD("Class %s: %d Comp / %d Total === %f", class_name, class_compiled, class_total, (double)class_compiled/(double)class_total);
    return true;
}
bool stats_logNumCompiledMethodsInOatDexFile(struct OatDexFile* oat_dex, uint32_t* result_compiled, uint32_t* result_total)
{
    CHECK_RETURNFALSE(oat_dex != NULL);
    CHECK_RETURNFALSE(result_compiled != NULL);
    CHECK_RETURNFALSE(result_total != NULL);

    uint32_t dex_compiled = 0;
    uint32_t dex_total = 0;

    for(uint16_t class_idx = 0; class_idx < oat_dex->data.dex_file_pointer->class_defs_size_; class_idx ++)
    {
        struct OatClass current_class;
        if(!oat_GetClass(oat_dex, &current_class, class_idx))
        {
            return false;
        }
        uint32_t class_compiled;
        uint32_t class_total;
        if(!stats_NumCompiledMethodsInOatDexClass(&current_class, &class_compiled, &class_total))
        {
            return false;
        }
        LOGD("--> Class [%d]: %d Comp / %d Total === %f", class_idx, class_compiled, class_total, (double)class_compiled/(double)class_total);
        dex_compiled += class_compiled;
        dex_total += class_total;
    }
    *result_compiled = dex_compiled;
    *result_total = dex_total;
    return true;
}

bool stats_logNumCompiledMethodsInOatFile(struct OatFile* oat)
{
    CHECK_RETURNFALSE(oat != NULL);

    int dexFiles = NumDexFiles(oat->header);

    uint32_t oat_compiled = 0;
    uint32_t oat_total = 0;

    for(uint32_t i = 0; i < dexFiles; i++)
    {
        struct OatDexFile current_dex;
        if(!oat_GetOatDexFile(oat, &current_dex, i))
        {
            return false;
        }

        uint32_t dex_compiled;
        uint32_t dex_total;
        if(!stats_NumCompiledMethodsInOatDexFile(&current_dex, &dex_compiled, &dex_total))
        {
            return false;
        }
        uint32_t str_len = current_dex.data.location_string.length;
        char path[str_len + 1];
        strncpy(path, current_dex.data.location_string.content, str_len);
        path[str_len] = 0;
        LOGD("-> OatDexFile [%s]: %d Comp / %d Total === %f", path, dex_compiled, dex_total, (double)dex_compiled/(double)dex_total);
        oat_compiled += dex_compiled;
        oat_total += dex_total;
    }
    LOGD("Oat file: %d Comp / %d Total === %f", oat_compiled, oat_total, (double)oat_compiled/(double)oat_total);
    return true;
}