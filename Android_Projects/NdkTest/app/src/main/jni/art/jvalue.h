//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_JVALUE_H
#define NDKTEST_ART_JVALUE_H

#include <stdint.h>
#include "../util/macros.h"

union PACKED(4) JValue
{
    uint8_t z;
    int8_t b;
    uint16_t c;
    int16_t s;
    int32_t i;
    int64_t j;
    float f;
    double d;
    void* l;
};

#endif //NDKTEST_ART_JVALUE_H
