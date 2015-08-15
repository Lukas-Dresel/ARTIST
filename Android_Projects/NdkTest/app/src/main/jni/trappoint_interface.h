//
// Created by Lukas on 8/6/2015.
//

#ifndef NDKTEST_TRAPPOINT_INTERFACE_H
#define NDKTEST_TRAPPOINT_INTERFACE_H

#include <ucontext.h>
#include <stdbool.h>

#define TRAP_METHOD_SIGTRAP_BKPT 1
#define TRAP_METHOD_SIGILL_KNOWN_ILLEGAL_INSTR 2

struct TrapPointInfo;
typedef struct TrapPointInfo TrapPointInfo;

typedef void (*CALLBACK)(void* addr, ucontext_t* ctx, void* additionalArg);

void init_trap_points();
void destroy_trap_points();

TrapPointInfo* install_trap_point(void* addr, uint32_t method, CALLBACK handler, void* additionalArg);
void uninstall_trap_point(TrapPointInfo* trap);

void dump_installed_trappoints_info();
bool validate_TrapPointInfo_contents(TrapPointInfo * trap);


#endif //NDKTEST_TRAPPOINT_INTERFACE_H
