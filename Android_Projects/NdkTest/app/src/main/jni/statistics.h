//
// Created by Lukas on 2/29/2016.
//

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
