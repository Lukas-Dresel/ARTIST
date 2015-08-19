//
// Created by Lukas on 8/17/2015.
//

#ifndef NDKTEST_MACROS_H
#define NDKTEST_MACROS_H


#define QUOTE(x) #x
#define STRINGIFY(x) QUOTE(x)

#define __LINE_STRING__ STRINGIFY(__LINE__)

#define LIKELY(x)       __builtin_expect((x), true)
#define UNLIKELY(x)     __builtin_expect((x), false)

#endif //NDKTEST_MACROS_H
