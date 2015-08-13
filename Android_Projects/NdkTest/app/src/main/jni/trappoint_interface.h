//
// Created by Lukas on 8/6/2015.
//

#ifndef NDKTEST_TRAPPOINT_INTERFACE_H
#define NDKTEST_TRAPPOINT_INTERFACE_H

#include <jni.h>

struct TrapPointInfo;

typedef struct TrapPointInfo TrapPointInfo;

typedef void (*CALLBACK)(void* addr, struct ucontext_t* ctx);

void init_trap_points();

TrapPointInfo* install_trap_point(void* addr, CALLBACK handler);

void uninstall_trap_point(TrapPointInfo* trap);

void run_trap_point_test(JNIEnv *);

void destroy_trap_points();

#endif //NDKTEST_TRAPPOINT_INTERFACE_H
