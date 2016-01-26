//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_REFERENCE_TABLE_H
#define NDKTEST_ART_REFERENCE_TABLE_H

#include <string>
#include <vector>
#include <inttypes.h>
#include "../util/dirty_dirty_std_library_hacks.h"

struct ReferenceTable
{
    std::string name_;       // This is a std::string
    std::vector entries_;    // This is a std::vector
    size_t max_size_;
};

#endif //NDKTEST_ART_REFERENCE_TABLE_H
