//
// Created by Lukas on 8/6/2015.
//

#ifndef NDKTEST_BREAKPOINT_H
#define NDKTEST_BREAKPOINT_H

#include <jni.h>

#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include <stdlib.h>

#include <stdio.h>

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>

#include <ucontext.h>

#include "../../main/jni/debug_util.h"
#include "../../main/jni/breakpoint_interface.h"
#include "../../main/jni/memory.h"
#include "../../main/jni/util.h"

#include "cpsr_util.h"

static struct sigaction old_sigtrap_action;

#endif //NDKTEST_BREAKPOINT_H
