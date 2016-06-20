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

#include "art_resolution.h"
#include "memory_map_lookup.h"
#include "logging.h"
#include "art/oat_dump.h"

/*
 *
 * This function looks up a java function's native implementation in memory.
 *
 * The function returns true if the lookup didn't encounter any errors.
 * In case the function returns false the contents of the result structure
 * are undefined, and cannot be assumed to contain correct values.
 *
 * If the function succeeds
 *
 */
bool android_FindLoadedClass(struct ArtClassContext* result, const char* class_name)
{
    struct MemoryMapView* mem_map = CreateMemoryMapView();
    if(mem_map == NULL)
    {
        return false;
    }

    struct MemoryMappedFile* f;
    list_for_each_entry(f, &mem_map->list_files, view_list_files_entry)
    {
        void* elf_start;
        void* elf_oat_start;
        void* elf_oat_end;
        if(!extractElfOatPointersFromFile(f, &elf_start, &elf_oat_start, &elf_oat_end))
        {
            continue;
        }
        if(!oat_Setup(&result->oat_file, elf_oat_start, elf_oat_end))
        {
            continue;
        }

        LOGD("Found oat file: ELF: "PRINT_PTR", OAT: "PRINT_PTR", END: "PRINT_PTR,
             (uintptr_t)elf_start, (uintptr_t)elf_oat_start, (uintptr_t)elf_oat_end);
        for(uint32_t i = 0; i < NumDexFiles(result->oat_file.header); i++)
        {
            if(!oat_GetOatDexFile(&result->oat_file, &result->oat_dex, i))
            {
                continue;
            }
            if(!oat_FindClassInDex(&result->oat_dex, &result->oat_class, class_name))
            {
                continue;
            }
            // Found the class, done here
            DestroyMemoryMapView(mem_map);
            return true;
        }
    }
    DestroyMemoryMapView(mem_map);
    return false;
}
bool android_FindLoadedMethod(struct ArtMethodContext* result, const char* class_name, const char* method_name, const char* method_proto)
{
    if(!android_FindLoadedClass(&result->clazz, class_name))
    {
        return false;
    }
    LOGI("Found class %s", class_name);
    const struct DexHeader* hdr = result->clazz.oat_dex.data.dex_file_pointer;
    uint16_t class_def_index = GetIndexForClassDef(hdr, result->clazz.oat_class.dex_class.class_def);
    log_dex_file_class_def_contents(hdr, class_def_index);
    if(oat_FindDirectMethod(&result->clazz.oat_class, &result->oat_method, method_name, method_proto))
    {
        LOGI("Found direct method %s", method_name);
        return true;
    }
    if(oat_FindVirtualMethod(&result->clazz.oat_class, &result->oat_method, method_name, method_proto))
    {
        LOGI("Found virtual method %s", method_name);
        return true;
    }
    LOGI("Could not find method %s", method_name);
    // Neither direct, nor virtual => not found
    return false;
}