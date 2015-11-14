//
// Created by Lukas on 11/14/2015.
//

#include "dex.h"

#ifndef NDKTEST_DEX_INTERNAL_H
#define NDKTEST_DEX_INTERNAL_H

const uint8_t kDexMagic[] = { 'd', 'e', 'x', '\n' };
const uint8_t kDexMagicVersion[] = { '0', '3', '5', '\0' };

bool IsMagicValid(const uint8_t* magic)
{
    return (memcmp(magic, kDexMagic, sizeof(kDexMagic)) == 0);
}

#endif //NDKTEST_DEX_1_H
