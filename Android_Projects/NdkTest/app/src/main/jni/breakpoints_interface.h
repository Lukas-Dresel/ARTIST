#ifndef _BREAKPOINTS_INTERFACE_H
#define _BREAKPOINTS_INTERFACE_H

#include <jni.h>
#include <stdbool.h>

struct Breakpoint;
typedef struct Breakpoint Breakpoint;

typedef void (*BREAKPOINT_CALLBACK)(ucontext_t*);

void                    init_breakpoints            ();
void                    destroy_breakpoints         ();

Breakpoint*             install_breakpoint          ( JNIEnv*, void*, BREAKPOINT_CALLBACK);
bool                    enable_breakpoint           ( Breakpoint * );
bool                    disable_breakpoint          ( Breakpoint * );
bool                    uninstall_breakpoint        ( Breakpoint * );

void                    print_full_breakpoint_info  (JNIEnv* env);

#endif