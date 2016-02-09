//
// Created by Lukas on 1/31/2016.
//

#ifndef NDKTEST_HOOKING_COMMON_H
#define NDKTEST_HOOKING_COMMON_H

#include <sys/ucontext.h>

typedef void (*HOOKCALLBACK)(void *addr, ucontext_t *ctx, void *additionalArg);

#endif //NDKTEST_HOOKING_COMMON_H
