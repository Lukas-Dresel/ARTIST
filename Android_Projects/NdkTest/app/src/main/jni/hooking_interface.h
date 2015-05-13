#ifndef _HOOKING_INTERFACE_H
#define _HOOKING_INTERFACE_H

#include <jni.h>
#include <stdbool.h>

#include "config.h"

#if defined (TARGET_ARCHITECTURE_ARM)
#include "hooking_arm.h"
#elif defined (TARGET_ARCHITECTURE_X86)
#include "hooking_x86.h"
#endif



struct InlineFunctionHook;
typedef struct InlineFunctionHook InlineFunctionHook;

void                    init_inline_function_hooking            ();

InlineFunctionHook*     install_inline_function_hook            ( JNIEnv* env, const unsigned char* targetName, void* targetAddress, const unsigned char* hookName, void* hookAddress);
bool                    enable_inline_function_hook             ( InlineFunctionHook * hook );
bool                    disable_inline_function_hook            ( InlineFunctionHook * hook );
bool                    uninstall_inline_function_hook          ( InlineFunctionHook * hook );

bool                    patch_original_code                     ( InlineFunctionHook * hook, void* new_code, int size);

void                    destroy_inline_function_hooking         ();

void                    print_full_inline_function_hook_info    (JNIEnv* env);

#endif