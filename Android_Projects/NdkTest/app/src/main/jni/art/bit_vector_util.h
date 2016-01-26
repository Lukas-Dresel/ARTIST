//
// Created by Lukas on 9/25/2015.
//

#ifndef NDKTEST_BIT_VECTOR_UTIL_H
#define NDKTEST_BIT_VECTOR_UTIL_H

#include <stdint.h>

uint32_t    bit_vector_NumSetBits       ( const uint32_t* data, uint32_t end );
bool        bit_vector_IsBitSet         ( const uint32_t* data, uint32_t bit_index );

#endif //NDKTEST_BIT_VECTOR_UTIL_H
