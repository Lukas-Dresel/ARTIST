#ifndef _LIB_SETUP_H
#define _LIB_SETUP_H

#include <jni.h>

static inline void init()
{
    init_system_info();
}

static inline void destroy()
{
    destroy_system_info();
}

#endif






