//
// Created by Lukas on 9/25/2015.
//

#ifndef NDKTEST_BIT_UTILS_H
#define NDKTEST_BIT_UTILS_H

#define COUNT_32BIT_SET_BITS(x) __builtin_popcount(x)
#define COUNT_64BIT_SET_BITS(x) __builtin_popcountll(x)

#define COUNT_32BIT_LEADING_ZEROS(x) __builtin_clz(x)
#define COUNT_64BIT_LEADING_ZEROS(x) __builtin_clzll(x)

#define COUNT_32BIT_TAILING_ZEROS(x) __builtin_ctz(x)
#define COUNT_64BIT_TAILING_ZEROS(x) __builtin_ctzll(x)

#endif //NDKTEST_BIT_UTILS_H
