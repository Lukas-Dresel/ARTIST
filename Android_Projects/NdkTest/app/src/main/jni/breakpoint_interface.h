//
// Created by Lukas on 8/6/2015.
//

#ifndef NDKTEST_BREAKPOINT_INTERFACE_H
#define NDKTEST_BREAKPOINT_INTERFACE_H

#include <jni.h>

void init_breakpoints();

void run_breakpoint_test(JNIEnv*);

void destroy_breakpoints();

#endif //NDKTEST_BREAKPOINT_INTERFACE_H
