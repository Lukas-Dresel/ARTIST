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
 
#ifndef NDKTEST_ART_RESOLUTION_H
#define NDKTEST_ART_RESOLUTION_H

#include <stdbool.h>
#include <art/oat.h>

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
