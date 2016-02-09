//
// Created by Lukas on 2/9/2016.
//

#ifndef NDKTEST_BREAKPOINT_H
#define NDKTEST_BREAKPOINT_H

#include <stdint.h>
#include <stdbool.h>
#include "hooking_common.h"

void*   Breakpoint_Install      (void *address, uint32_t trap_method, HOOKCALLBACK handler,
                                 void *arg);
bool    Breakpoint_Enable       (void *p);
bool    Breakpoint_Disable      (void *p);
void    Breakpoint_Uninstall    (void *p);

#endif //NDKTEST_BREAKPOINT_H
