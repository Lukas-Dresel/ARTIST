//
// Created by Lukas on 3/9/2016.
//

#include "art_resolution.h"
#include "memory_map_lookup.h"

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
bool android_FindLoadedClass(struct ArtClassContext* result, char* class_name)
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
bool android_FindLoadedMethod(struct ArtMethodContext* result, char* class_name, char* method_name, char* method_proto)
{
    if(!android_FindLoadedClass(&result->clazz, class_name))
    {
        return false;
    }
    if(oat_FindDirectMethod(&result->clazz.oat_class, &result->oat_method, method_name, method_proto))
    {
        return true;
    }
    if(oat_FindVirtualMethod(&result->clazz.oat_class, &result->oat_method, method_name, method_proto))
    {
        return true;
    }
    // Neither direct, nor virtual => not found
    return false;
}