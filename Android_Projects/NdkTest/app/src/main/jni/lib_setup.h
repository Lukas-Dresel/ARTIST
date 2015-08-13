#ifndef _LIB_SETUP_H
#define _LIB_SETUP_H

#include <jni.h>
#include "trappoint_interface.h"
#include "system_info.h"

static inline void init()
{
    init_system_info();
    init_trap_points();
}

static inline void destroy()
{
    destroy_trap_points();
    destroy_system_info();
}

#endif






