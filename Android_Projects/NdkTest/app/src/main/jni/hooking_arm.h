#ifndef _HOOKING_ARM_H
#define _HOOKING_ARM_H

#include <jni.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hooking.h"
#include "function_internals_helper_arm.h"
#include "list.h"


#define HOOK_CODE_SIZE 16
extern void hook_stub_arm();
extern void hook_stub_thumb();

typedef struct FunctionInfo
{
    const unsigned char*    name;
    void*                   address;
    void*                   memory_location;
    bool                    uses_thumb_mode;
} FunctionInfo;


typedef struct InlineFunctionHook
{
    JNIEnv *            env;

    FunctionInfo        target_function;
    FunctionInfo        hook_function;

    unsigned char       hook_code[HOOK_CODE_SIZE];
    unsigned char       preserved_code[HOOK_CODE_SIZE];
    int                 overwrite_size;

    struct list_head    installed_entry;
    struct list_head    enabled_entry;
    struct list_head    disabled_entry;

} InlineFunctionHook;

#endif






