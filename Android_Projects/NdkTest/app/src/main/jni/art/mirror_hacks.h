//
// Created by Lukas on 3/11/2016.
//

#ifndef NDKTEST_MIRROR_HACKS_H
#define NDKTEST_MIRROR_HACKS_H

#include <stdint.h>

struct MirrorHackStringContent
{
    char unknown[12];
    uint16_t chars[0];
};
struct MirrorHackString
{
    void* some_ref;
    void* null;
    struct MirrorHackStringContent* str_content;
    uint32_t str_len;
    uint32_t hash;
};


#endif //NDKTEST_MIRROR_HACKS_H
