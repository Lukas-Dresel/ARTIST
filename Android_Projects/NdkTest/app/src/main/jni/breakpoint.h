//
// Created by Lukas on 8/19/2015.
//

#ifndef NDKTEST_BREAKPOINT_H
#define NDKTEST_BREAKPOINT_H

#include "trappoint_interface.h"

struct BreakPointInfo;
typedef struct BreakPointInfo BreakPointInfo;

typedef TRAPPOINT_CALLBACK BREAKPOINT_CALLBACK;

typedef bool (*BREAKPOINT_PREDICATE)(BreakPointInfo *b, void *args);

void init_breakpoints();

void destroy_breakpoints();

BreakPointInfo *install_breakpoint(void *addr, uint32_t method, BREAKPOINT_CALLBACK handler,
                                   void *additionalArg);

void uninstall_breakpoint(BreakPointInfo *b);

BreakPointInfo *find_first_breakpoint_with_predicate(BREAKPOINT_PREDICATE p, void *args);

void dump_installed_trappoints_info();

bool validate_BreakPointInfo_contents(BreakPointInfo *b);


#endif //NDKTEST_BREAKPOINT_H
