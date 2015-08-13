//
// Created by Lukas on 8/6/2015.
//

#ifndef NDKTEST_TRAPPOINT_H
#define NDKTEST_TRAPPOINT_H

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
#include "../../main/jni/trappoint_interface.h"
#include "../../main/jni/memory.h"
#include "../../main/jni/util.h"
#include "../../main/jni/list.h"

#include "bkpt.h"
#include "illegal_instruction.h"

#include "cpsr_util.h"

typedef struct TargetInfo
{
    void*       call_addr;
    void*       mem_addr;
    bool        thumb;
} TargetInfo;
typedef struct ThumbWriteInfo
{
    uint16_t preserved;
    uint16_t bkpt;
} ThumbWriteInfo;
typedef struct ArmWriteInfo
{
    uint32_t preserved;
    uint32_t bkpt;
} ArmWriteInfo;

static struct sigaction old_sigtrap_action;
static struct list_head installed_trap_points;


struct TrapPointInfo
{
    CALLBACK            handler;

    TargetInfo          target;

    uint32_t            instr_size;

    union
    {
        ThumbWriteInfo      thumbCode;
        ArmWriteInfo        armCode;
    };

    struct list_head    installed;
};

#endif //NDKTEST_TRAPPOINT_H
