//
// Created by Lukas on 8/6/2015.
//

#ifndef NDKTEST_TRAPPOINT_INTERFACE_H
#define NDKTEST_TRAPPOINT_INTERFACE_H

#include <ucontext.h>
#include <stdbool.h>

#define TRAP_METHOD_INSTR_BKPT 1
#define TRAP_METHOD_INSTR_KNOWN_ILLEGAL 2
#define TRAP_METHOD_SIG_TRAP 0x10000
#define TRAP_METHOD_SIG_ILL  0x20000

struct TrapPointInfo;
typedef struct TrapPointInfo TrapPointInfo;

typedef void (*CALLBACK)(void* addr, ucontext_t* ctx, void* additionalArg);

typedef bool (*PREDICATE)(TrapPointInfo* trap, void* args);

void init_trap_points();
void destroy_trap_points();

TrapPointInfo* install_trap_point(void* addr, uint32_t method, CALLBACK handler, void* additionalArg);
void uninstall_trap_point(TrapPointInfo* trap);

TrapPointInfo* find_first_trappoint_with_predicate(PREDICATE p, void* args);

void dump_installed_trappoints_info();
bool validate_TrapPointInfo_contents(TrapPointInfo * trap);


#endif //NDKTEST_TRAPPOINT_INTERFACE_H
