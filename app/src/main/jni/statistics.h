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
 

#ifndef NDKTEST_STATISTICS_H
#define NDKTEST_STATISTICS_H

#include <stdint.h>
#include "art/oat.h"

bool stats_NumCompiledMethodsInOatDexClass(struct OatClass* oat_class, uint32_t* result_compiled, uint32_t* result_total);
bool stats_NumCompiledMethodsInOatDexFile(struct OatDexFile* oat_dex, uint32_t* result_compiled, uint32_t* result_total);
bool stats_NumCompiledMethodsInOatFile(struct OatFile* oat, uint32_t* result_compiled, uint32_t* result_total);

bool stats_logNumCompiledMethodsInOatClass(struct OatClass* oat_class, uint32_t* result_compiled, uint32_t* result_total);
bool stats_logNumCompiledMethodsInOatDexFile(struct OatDexFile* oat_dex, uint32_t* result_compiled, uint32_t* result_total);
bool stats_logNumCompiledMethodsInOatFile(struct OatFile* oat);

typedef void (*OATDEX_TO_OAT_MERGER)(void* oat, struct OatDexFile* current_oat_dex);
typedef void (*OATCLASS_TO_OATDEX_MERGER)(void* oat_dex, struct OatClass* current_oat_class);
typedef void (*OAT_TO_FINAL_MERGER)(void* final, struct OatFile* oat);

#endif //NDKTEST_STATISTICS_H
