#ifndef _LIB_SETUP_H
#define _LIB_SETUP_H

#include <jni.h>
#include "breakpoint_interface.h"
#include "system_info.h"

static inline void init()
{
    init_system_info();
    init_breakpoints();
}

static inline void destroy()
{
    destroy_breakpoints();
    destroy_system_info();
}

#endif






