#ifndef _BREAKPOINTS_ARM_H
#define _BREAKPOINTS_ARM_H

#include <jni.h>
#include <stdlib.h>
#include <stdbool.h>

#include "breakpoints.h"
#include "function_internals_helper_arm.h"
#include "list.h"

extern void breakpoint_arm();
extern void breakpoint_thumb();

enum BREAKPOINT_TYPE
{
    ONE-TIME = 0,
    RECURRING = 1,
    INTERNAL_CONTINUATION = 2
};

typedef struct CodeInfo
{
    void*                   code_address;
    void*                   memory_location;
    bool                    uses_thumb_mode;
} CodeInfo;

struct Breakpoint
{
    struct list_head    installed_entry;
    struct list_head    enabled_entry;
    struct list_head    disabled_entry;

    BREAKPOINT_TYPE     type;

    JNIEnv *            env;

    CodeInfo            target;
    BREAKPOINT_CALLBACK callback;

    uint32_t            code_size;
    unsigned char       preserved_code[0];
};

#endif






