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
bool stats_logNumCompiledMethodsInOatDexFile(struct OatDexFile* oat_dex, uint32_t* result_compiled, uint32_t* result_total);
bool stats_logNumCompiledMethodsInOatFile(struct OatFile* oat);

#endif //NDKTEST_STATISTICS_H
