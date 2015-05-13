#ifndef _HOOKING_H
#define _HOOKING_H

#include "hooking_interface.h"

#if defined (TARGET_ARCHITECTURE_ARM)
#include "hooking_arm.h"
#elif defined (TARGET_ARCHITECTURE_X86)
#include "hooking_x86.h"
#endif

#endif