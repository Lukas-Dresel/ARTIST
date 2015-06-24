#ifndef _HOOKING_ARM_H
#define _HOOKING_ARM_H

#include <jni.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hooking.h"
#include "function_internals_helper_arm.h"
#include "list.h"


#define HOOK_CODE_SIZE 16

//      @REFERENCE: asm.s:hook_stub_arm
//                      push    {r0, r1, r2, r3}
//                      pop     {r1, r2, r3}
//                      ldr     pc, [pc, #-4]
//                      Address to jump to
//#define HOOK_CODE_ARM   "\x0f\x00\x2d\xe9" "\x0e\x00\xbd\xe8" "\x0e\xf0\x1f\xe5" "BABE"

//      @REFERENCE: asm.s:hook_stub_thumb
//                      push    {r0, r1, r2, r3}
//                      pop     {r1, r2, r3}
//                      add r0, pc, #2
//                      ldr r0, [r0, #0]
//                      mov pc, r0
//                      mov pc, r0
//                      Address to jump to
//
extern void hook_stub_arm();

//#define HOOK_CODE_THUMB "\x0f\xb4" \
                        "\x0e\xbc" \
                        "\x01\xa0\x00\x68\x87\x46\x87\x46" "AAAA"
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






