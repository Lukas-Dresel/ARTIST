//
// Created by Lukas on 9/28/2015.
//

#ifndef NDKTEST_TYPEDEFS_H
#define NDKTEST_TYPEDEFS_H

typedef uint8_t byte;

typedef struct String
{
    uint32_t length;
    const char* content;
} String;

#endif //NDKTEST_TYPEDEFS_H
