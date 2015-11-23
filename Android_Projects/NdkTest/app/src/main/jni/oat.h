//
// Created by Lukas on 8/16/2015.
//

#ifndef NDKTEST_OAT_H
#define NDKTEST_OAT_H

#include "oat_internal.h"
#include "dex.h"

struct OatFile
{
    void*               begin;
    void*               end;

    struct OatHeader*   header;
    void*               key_value_storage_start;
    void*               dex_file_storage_start;
};

struct OatClass
{
    struct OatFile*     oat_file;
    struct DexClass     dex_class;
};

#endif //NDKTEST_OAT_H
