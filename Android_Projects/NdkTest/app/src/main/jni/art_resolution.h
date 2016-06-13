//
// Created by Lukas on 3/9/2016.
//

#ifndef NDKTEST_ART_RESOLUTION_H
#define NDKTEST_ART_RESOLUTION_H

#include <stdbool.h>
#include "art/oat.h"

struct ArtClassContext
{
    struct OatFile          oat_file;
    struct OatDexFile       oat_dex;
    struct OatClass         oat_class;
};
struct ArtMethodContext
{
    struct ArtClassContext  clazz;
    struct OatMethod        oat_method;
};
bool android_FindLoadedClass(struct ArtClassContext* result, const char* class_name);
bool android_FindLoadedMethod(struct ArtMethodContext* result, const char* class_name,
                                                    const char* method_name, const char* method_proto);



#endif //NDKTEST_ART_H
