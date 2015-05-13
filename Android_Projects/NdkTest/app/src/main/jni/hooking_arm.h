#ifndef _HOOKING_ARM_H
#define _HOOKING_ARM_H

#include <jni.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hooking.h"
#include "hooking_arm_helper.h"
#include "list.h"


#define HOOK_CODE_SIZE 12
#define HOOK_CODE_ARM   "\x00\xf0\x9f\xe5" "1338" "\xDE\xAD\xBA\xBE"
#define HOOK_CODE_THUMB "\x01\xa0\x00\x68\x87\x46\x87\x46" "AAAA"

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






