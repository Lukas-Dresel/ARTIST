//
// Created by Lukas on 10/3/2015.
//

#ifndef NDKTEST_OAT_CLASS_H
#define NDKTEST_OAT_CLASS_H

#include "oat.h"
#include "oat_info.h"

struct OatClass*               oat_class_Parse(struct OatClass* result, const uint8_t* oat_class_pointer);

struct OatClass*               oat_class_Extract(struct OatClass* result, LazyOatInfo* oat_info,
                                          uint32_t dex_file_index, uint32_t class_def_index);

void*                   oat_class_GetMethodCodePointer(const struct OatHeader* oat_header,
                                                       const struct OatClass* oat_class,
                                                       uint32_t method_index);
OatQuickMethodHeader*   oat_class_GetQuickMethodHeader(const struct OatHeader* oat_header,
                                                       const struct OatClass* oat_class,
                                                       uint32_t method_index);


#endif //NDKTEST_OAT_CLASS_H
