//
// Created by Lukas on 8/6/2015.
//

#ifndef NDKTEST_TRAPPOINT_H
#define NDKTEST_TRAPPOINT_H


#include <stdint.h>

#include "../../main/jni/hooking/trappoint_interface.h"
#include "../../main/jni/util/list.h"
#include "abi.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
    TRAPPOINT_CALLBACK handler;
    void*                   handler_args;

    struct InstructionInfo  target;

    uint32_t                instr_size;

    uint32_t                trapping_method;
    union
    {
        ThumbCodeInfo       thumbCode;
        ArmCodeInfo         armCode;
    };

    struct list_head        installed;
};

#ifdef __cplusplus
}
#endif

#endif //NDKTEST_TRAPPOINT_H
