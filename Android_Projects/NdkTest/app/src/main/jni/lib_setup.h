#ifndef _LIB_SETUP_H
#define _LIB_SETUP_H

#include <jni.h>

static inline void init()
{
    init_system_info();
    init_inline_function_hooking();
}

static inline void destroy()
{
    destroy_inline_function_hooking();
    destroy_system_info();
}

#endif






