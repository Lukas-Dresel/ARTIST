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
#include "../../main/jni/error.h"
#include "../../main/jni/signal_handling_helper.h"

#include "generate_trap_instruction.h"

typedef struct TargetInfo
{
    void*       call_addr;
    void*       mem_addr;
    bool        thumb;
} TargetInfo;

typedef struct ThumbCodeInfo
{
    uint16_t preserved;
    uint16_t trap_instruction;
} ThumbCodeInfo;

typedef struct ArmCodeInfo
{
    uint32_t preserved;
    uint32_t trap_instruction;
} ArmCodeInfo;

struct TrapPointInfo
{
    CALLBACK                handler;
    void*                   handler_args;

    TargetInfo              target;

    uint32_t                instr_size;

    uint32_t                trapping_method;
    union
    {
        ThumbCodeInfo       thumbCode;
        ArmCodeInfo         armCode;
    };

    struct list_head        installed;
};

#endif //NDKTEST_TRAPPOINT_H
