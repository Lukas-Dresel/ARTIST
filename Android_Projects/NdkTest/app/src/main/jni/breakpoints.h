#ifndef _BREAKPOINTS_H
#define _BREAKPOINTS_H

#include "breakpoints_interface.h"

#if defined (TARGET_ARCHITECTURE_ARM)
#include "breakpoints_arm.h"
#elif defined (TARGET_ARCHITECTURE_X86)
#include "breakpoints_x86.h"
#endif

#endif